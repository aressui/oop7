#include "game.h"
#include "npc.h"
#include "druid.h"
#include "squirrel.h"
#include "werewolf.h"
#include "visitor.h"
#include <iostream>
#include <random>
#include <chrono>
#include <algorithm>

using namespace std::chrono_literals;

Game::Game() : running(true) {
    create_npcs();
}

Game::~Game() {
    running = false;
    queue_cv.notify_all();
    
    if (move_thread.joinable()) move_thread.join();
    if (fight_thread.joinable()) fight_thread.join();
}

void Game::create_npcs() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> pos_dist(0, MAP_SIZE - 1);
    std::uniform_int_distribution<> type_dist(0, 2);
    
    std::unique_lock lock(npcs_mutex);
    for (int i = 0; i < NPC_COUNT; ++i) {
        int x = pos_dist(gen);
        int y = pos_dist(gen);
        NpcType type = static_cast<NpcType>(type_dist(gen));
        
        auto name = generate_name();
        auto npc = factory(type, x, y, name);
        if (npc) {
            npcs.insert(npc);
        }
    }
}

void Game::move_npc(std::shared_ptr<NPC> npc) {
    if (!npc->is_alive()) return;
    
    int dist = npc->get_move_distance();
    if (dist == 0) return;
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dir_dist(-1, 1);
    
    int dx = dir_dist(gen);
    int dy = dir_dist(gen);
    
    int new_x = npc->get_x() + dx * dist;
    int new_y = npc->get_y() + dy * dist;
    
    new_x = std::max(0, std::min(MAP_SIZE - 1, new_x));
    new_y = std::max(0, std::min(MAP_SIZE - 1, new_y));
    
    npc->set_position(new_x, new_y);
}

bool Game::process_fight(std::shared_ptr<NPC> attacker, std::shared_ptr<NPC> defender) {
    if (!attacker->is_alive() || !defender->is_alive()) {
        return false;
    }
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dice(1, 6);
    
    int attack = dice(gen);
    int defense = dice(gen);
    
    auto visitor = std::make_shared<FightVisitor>(attacker);
    bool can_kill = defender->accept(visitor);
    
    if (can_kill && attack > defense) {
        defender->make_dead();
        attacker->fight_notify(defender, true);
        
        {
            std::lock_guard lock(cout_mutex);
            std::cout << attacker->get_name() << " killed " << defender->get_name() << std::endl;
        }
        
        return true;
    } else {
        attacker->fight_notify(defender, false);
        return false;
    }
}

void Game::move_worker() {
    while (running) {
        {
            std::shared_lock lock(npcs_mutex);
            
            for (auto& npc : npcs) {
                if (!npc->is_alive()) continue;
                
                move_npc(npc);
                
                int kill_dist = npc->get_kill_distance();
                if (kill_dist > 0) {
                    for (auto& other : npcs) {
                        if (npc == other || !other->is_alive()) continue;
                        
                        if (npc->is_close(other, kill_dist)) {
                            std::lock_guard qlock(queue_mutex);
                            fight_queue.push({npc, other});
                            queue_cv.notify_one();
                        }
                    }
                }
            }
        }
        
        std::this_thread::sleep_for(50ms);
    }
}

void Game::fight_worker() {
    while (running) {
        Fight fight;
        bool has_fight = false;
        
        {
            std::unique_lock lock(queue_mutex);
            queue_cv.wait_for(lock, 100ms, [this]() {
                return !fight_queue.empty() || !running;
            });
            
            if (!fight_queue.empty()) {
                fight = fight_queue.front();
                fight_queue.pop();
                has_fight = true;
            }
        }
        
        if (has_fight) {
            process_fight(fight.attacker, fight.defender);
        }
    }
}

void Game::print_map() {
    const int CELL = 10;
    std::vector<std::vector<char>> grid(MAP_SIZE / CELL, 
                                        std::vector<char>(MAP_SIZE / CELL, '.'));
    
    {
        std::shared_lock lock(npcs_mutex);
        
        for (const auto& npc : npcs) {
            if (!npc->is_alive()) continue;
            
            int gx = npc->get_x() / CELL;
            int gy = npc->get_y() / CELL;
            
            if (gx >= 0 && gx < grid[0].size() && gy >= 0 && gy < grid.size()) {
                char symbol = '.';
                switch (npc->get_type()) {
                    case DruidType: symbol = 'D'; break;
                    case SquirrelType: symbol = 'S'; break;
                    case WerewolfType: symbol = 'W'; break;
                }
                grid[gy][gx] = symbol;
            }
        }
    }
    
    {
        std::lock_guard lock(cout_mutex);
        
        int alive = 0;
        {
            std::shared_lock lock(npcs_mutex);
            for (const auto& npc : npcs) {
                if (npc->is_alive()) alive++;
            }
        }
        
        std::cout << "\nAlive: " << alive << std::endl;
        
        for (const auto& row : grid) {
            for (char cell : row) {
                std::cout << cell;
            }
            std::cout << std::endl;
        }
    }
}

void Game::print_survivors() {
    std::lock_guard lock(cout_mutex);
    
    std::cout << "\nSurvivors:" << std::endl;
    
    std::shared_lock npc_lock(npcs_mutex);
    int count = 0;
    
    for (const auto& npc : npcs) {
        if (npc->is_alive()) {
            std::string type;
            switch (npc->get_type()) {
                case DruidType: type = "Druid"; break;
                case SquirrelType: type = "Squirrel"; break;
                case WerewolfType: type = "Werewolf"; break;
            }
            std::cout << type << " " << npc->get_name() 
                      << " (" << npc->get_x() << ", " << npc->get_y() << ")" << std::endl;
            count++;
        }
    }
    
    std::cout << "Total: " << count << std::endl;
}

void Game::run() {
    move_thread = std::thread(&Game::move_worker, this);
    fight_thread = std::thread(&Game::fight_worker, this);
    
    auto start = std::chrono::steady_clock::now();
    
    while (running) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);
        
        if (elapsed.count() >= GAME_TIME) {
            running = false;
            break;
        }
        
        print_map();
        std::this_thread::sleep_for(1s);
    }
    
    queue_cv.notify_all();
    
    if (move_thread.joinable()) move_thread.join();
    if (fight_thread.joinable()) fight_thread.join();
    
    print_survivors();
}