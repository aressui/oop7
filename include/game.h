#ifndef GAME_H
#define GAME_H

#include "factory.h"
#include <vector>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <thread>
#include <queue>
#include <condition_variable>

class Game {
private:
    static const int MAP_SIZE = 100;
    static const int NPC_COUNT = 50;
    static const int GAME_TIME = 30;

    set_t npcs;
    std::shared_mutex npcs_mutex;
    
    std::atomic<bool> running;
    std::thread move_thread;
    std::thread fight_thread;
    
    struct Fight {
        std::shared_ptr<NPC> attacker;
        std::shared_ptr<NPC> defender;
    };
    std::queue<Fight> fight_queue;
    std::mutex queue_mutex;
    std::condition_variable queue_cv;
    
    std::mutex cout_mutex;

    void move_worker();
    void fight_worker();
    
    void create_npcs();
    void move_npc(std::shared_ptr<NPC> npc);
    bool process_fight(std::shared_ptr<NPC> attacker, std::shared_ptr<NPC> defender);
    void print_map();
    void print_survivors();

public:
    Game();
    ~Game();
    
    void run();
};

#endif