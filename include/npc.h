#ifndef NPC_H
#define NPC_H

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <set>
#include <cmath>
#include <stdexcept>

class Squirrel;
class Werewolf;
class Druid;
class IFightObserver;
class NPCVisitor;

using set_t = std::set<std::shared_ptr<class NPC>>;

enum NpcType {
    Unknown = 0,
    SquirrelType = 1,
    WerewolfType = 2,
    DruidType = 3
};

class IFightObserver {
public:
    virtual ~IFightObserver() = default;
    virtual void on_fight(const std::shared_ptr<class NPC> attacker, 
                         const std::shared_ptr<class NPC> defender, 
                         bool win) = 0;
};

class NPC : public std::enable_shared_from_this<NPC> {
protected:
    NpcType type;
    int x{0};
    int y{0};
    std::string name;
    bool alive;
    std::vector<std::shared_ptr<IFightObserver>> observers;

public:
    NPC(NpcType t, int _x, int _y, const std::string& _name);
    NPC(NpcType t, std::istream &is);
    virtual ~NPC() = default;

    void subscribe(std::shared_ptr<IFightObserver> observer);
    void fight_notify(const std::shared_ptr<NPC> defender, bool win);

    virtual bool accept(std::shared_ptr<NPCVisitor> visitor) = 0;
    
    virtual bool fight(std::shared_ptr<Squirrel> other) = 0;
    virtual bool fight(std::shared_ptr<Werewolf> other) = 0;
    virtual bool fight(std::shared_ptr<Druid> other) = 0;

    bool is_close(const std::shared_ptr<NPC> &other, size_t distance) const;
    
    virtual int get_move_distance() const = 0;
    virtual int get_kill_distance() const = 0;
    
    void set_position(int new_x, int new_y);
    bool is_alive() const;
    void make_dead();
    
    NpcType get_type() const;
    int get_x() const;
    int get_y() const;
    std::string get_name() const;
    
    virtual void print() = 0;
    virtual void save(std::ostream &os);

    friend std::ostream &operator<<(std::ostream &os, NPC &npc);
};

#endif