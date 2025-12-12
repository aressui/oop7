#include "npc.h"
#include "squirrel.h"
#include "werewolf.h"
#include "druid.h"

NPC::NPC(NpcType t, int _x, int _y, const std::string& _name) 
    : type(t), x(_x), y(_y), name(_name), alive(true) 
{
    if (x < 0 || x > 500 || y < 0 || y > 500) {
        throw std::runtime_error("Coordinates must be in range 0-500");
    }
}

NPC::NPC(NpcType t, std::istream &is) : type(t), alive(true) {
    is >> x;
    is >> y;
    is >> name;
    
    if (x < 0 || x > 500 || y < 0 || y > 500) {
        throw std::runtime_error("Coordinates must be in range 0-500");
    }
}

void NPC::subscribe(std::shared_ptr<IFightObserver> observer) {
    observers.push_back(observer);
}

void NPC::fight_notify(const std::shared_ptr<NPC> defender, bool win) {
    for (auto &o : observers) {
        o->on_fight(shared_from_this(), defender, win);
    }
}

bool NPC::is_close(const std::shared_ptr<NPC> &other, size_t distance) const {
    int dx = x - other->x;
    int dy = y - other->y;
    return (dx*dx + dy*dy) <= (distance * distance);
}

void NPC::set_position(int new_x, int new_y) {
    x = new_x;
    y = new_y;
}

bool NPC::is_alive() const {
    return alive;
}

void NPC::make_dead() {
    alive = false;
}

NpcType NPC::get_type() const {
    return type;
}

int NPC::get_x() const {
    return x;
}

int NPC::get_y() const {
    return y;
}

std::string NPC::get_name() const {
    return name;
}

void NPC::save(std::ostream &os) {
    os << x << std::endl;
    os << y << std::endl;
    os << name << std::endl;
}

std::ostream &operator<<(std::ostream &os, NPC &npc) {
    os << "{name: \"" << npc.name << "\", x:" << npc.x << ", y:" << npc.y << "}";
    return os;
}