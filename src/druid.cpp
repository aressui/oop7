#include "druid.h"
#include "squirrel.h"
#include "werewolf.h"

Druid::Druid(int x, int y, const std::string& name) 
    : NPC(DruidType, x, y, name) {}

Druid::Druid(std::istream &is) : NPC(DruidType, is) {}

void Druid::print() {
    std::cout << *this;
}

void Druid::save(std::ostream &os) {
    os << DruidType << std::endl;
    NPC::save(os);
}

bool Druid::accept(std::shared_ptr<NPCVisitor> visitor) {
    return visitor->visit(std::static_pointer_cast<Druid>(shared_from_this()));
}

bool Druid::fight(std::shared_ptr<Squirrel> other) {
    fight_notify(other, false);
    return false;
}

bool Druid::fight(std::shared_ptr<Werewolf> other) {
    fight_notify(other, false);
    return false;
}

bool Druid::fight(std::shared_ptr<Druid> other) {
    fight_notify(other, false);
    return false;
}

int Druid::get_move_distance() const {
    return 10;
}

int Druid::get_kill_distance() const {
    return 10;
}

std::ostream &operator<<(std::ostream &os, Druid &druid) {
    os << "Druid: " << *static_cast<NPC *>(&druid) << std::endl;
    return os;
}