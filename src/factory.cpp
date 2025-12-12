#include "factory.h"
#include "squirrel.h"
#include "werewolf.h"
#include "druid.h"
#include "visitor.h"
#include <fstream>
#include <cstring>
#include <random>
#include <iostream>

std::string NpcTypeToString(NpcType type) {
    switch (type) {
        case SquirrelType: return "Squirrel";
        case WerewolfType: return "Werewolf";
        case DruidType: return "Druid";
        default: return "Unknown";
    }
}

class TextObserver : public IFightObserver {
private:
    TextObserver(){};
public:
    static std::shared_ptr<IFightObserver> get() {
        static TextObserver instance;
        return std::shared_ptr<IFightObserver>(&instance, [](IFightObserver *) {});
    }

    void on_fight(const std::shared_ptr<NPC> attacker, 
                 const std::shared_ptr<NPC> defender, 
                 bool win) override {
        if (win) {
            std::cout << std::endl << "Murder --------" << std::endl;
            std::cout << "Killer: ";
            attacker->print();
            std::cout << "Victim: ";
            defender->print();
        }
    }
};

class FileObserver : public IFightObserver {
private:
    std::ofstream file;
    FileObserver() {
        file.open("log.txt", std::ios::app);
    }
public:
    static std::shared_ptr<IFightObserver> get() {
        static FileObserver instance;
        return std::shared_ptr<IFightObserver>(&instance, [](IFightObserver *) {});
    }

    void on_fight(const std::shared_ptr<NPC> attacker, 
                 const std::shared_ptr<NPC> defender, 
                 bool win) override {
        if (win) {
            file << std::endl << "Murder ---" << std::endl;
            file << "Killer: ";
            file << NpcTypeToString(attacker->get_type());
            file << " \"" << attacker->get_name() << "\" {x:" 
                 << attacker->get_x() << ", y:" << attacker->get_y() << "}" << std::endl;
            file << "Victim: ";
            file << NpcTypeToString(defender->get_type());
            file << " \"" << defender->get_name() << "\" {x:" 
                 << defender->get_x() << ", y:" << defender->get_y() << "}" << std::endl;
        }
    }
};

std::shared_ptr<NPC> factory(std::istream &is) {
    std::shared_ptr<NPC> result;
    int type{0};
    
    if (is >> type) {
        try {
            switch (type) {
                case SquirrelType:
                    result = std::make_shared<Squirrel>(is);
                    break;
                case WerewolfType:
                    result = std::make_shared<Werewolf>(is);
                    break;
                case DruidType:
                    result = std::make_shared<Druid>(is);
                    break;
                default:
                    std::cerr << "Unexpected NPC type: " << type << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error loading NPC: " << e.what() << std::endl;
        }
    }

    if (result) {
        result->subscribe(TextObserver::get());
        result->subscribe(FileObserver::get());
    }

    return result;
}

std::shared_ptr<NPC> factory(NpcType type, int x, int y, const std::string& name) {
    std::shared_ptr<NPC> result;
    try {
        switch (type) {
            case SquirrelType:
                result = std::make_shared<Squirrel>(x, y, name);
                break;
            case WerewolfType:
                result = std::make_shared<Werewolf>(x, y, name);
                break;
            case DruidType:
                result = std::make_shared<Druid>(x, y, name);
                break;
            default:
                std::cerr << "Unknown NPC type: " << type << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error creating NPC: " << e.what() << std::endl;
    }
    
    if (result) {
        result->subscribe(TextObserver::get());
        result->subscribe(FileObserver::get());
    }

    return result;
}

void save(const set_t &array, const std::string &filename) {
    std::ofstream fs(filename);
    fs << array.size() << std::endl;
    for (auto &n : array) {
        n->save(fs);
    }
    fs.flush();
    fs.close();
}

set_t load(const std::string &filename) {
    set_t result;
    std::ifstream is(filename);
    
    if (is.good() && is.is_open()) {
        int count;
        is >> count;
        for (int i = 0; i < count; ++i) {
            result.insert(factory(is));
        }
        is.close();
    } else {
        std::cerr << "Error: " << std::strerror(errno) << std::endl;
    }
    return result;
}

set_t fight(const set_t &array, size_t distance) {
    set_t dead_list;

    for (const auto &attacker : array) {
        for (const auto &defender : array) {
            if ((attacker != defender) && 
                (attacker->is_close(defender, distance)) &&
                !dead_list.count(defender)) {
            
                auto visitor = std::make_shared<FightVisitor>(attacker);
                bool success = defender->accept(visitor);
                
                if (success) {
                    dead_list.insert(defender);
                }
            }
        }
    }

    return dead_list;
}

std::string generate_name() {
    static std::vector<std::string> names = {
        "Swift", "Brave", "Smart", "Agile", "Red", "Forest", 
        "Night", "Gray", "Strong", "Wise", "Old", "Quiet"
    };
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, names.size() - 1);
    return names[dis(gen)] + "_" + std::to_string(rd() % 1000);
}