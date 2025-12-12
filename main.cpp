#include "factory.h"
#include "game.h"
#include <iostream>
#include <random>

std::ostream &operator<<(std::ostream &os, const set_t &array) {
    for (auto &n : array) {
        n->print();
    }
    return os;
}

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "lab7") {
        set_t array;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> coord_dis(0, 500);
        std::uniform_int_distribution<> type_dis(1, 3);
        
        for (size_t i = 0; i < 10; ++i) {
            array.insert(factory(
                static_cast<NpcType>(type_dis(gen)),
                coord_dis(gen),
                coord_dis(gen),
                generate_name()
            ));
        }

        save(array, "npcs.txt");
        
        array = load("npcs.txt");
        
        std::cout << array;

        for (size_t distance = 20; (distance <= 100) && !array.empty(); distance += 20) {
            auto dead_list = fight(array, distance);
            for (auto &d : dead_list) {
                array.erase(d);
            }
        }

        if (array.empty()) {
            std::cout << "No survivors left" << std::endl;
        } else {
            std::cout << array;
        }
    } else {
        Game game;
        game.run();
    }
    
    return 0;
}