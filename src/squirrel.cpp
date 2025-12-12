#include "squirrel.h"
#include "werewolf.h"
#include "druid.h"

Squirrel::Squirrel(int x, int y, const std::string& name) 
    : NPC(SquirrelType, x, y, name) {}

Squirrel::Squirrel(std::istream &is) : NPC(SquirrelType, is) {}

void Squirrel::print() {
    std::cout << *this;
}

void Squirrel::save(std::ostream &os) {
    os << SquirrelType << std::endl;
    NPC::save(os);
}

bool Squirrel::accept(std::shared_ptr<NPCVisitor> visitor) {
    return visitor->visit(std::static_pointer_cast<Squirrel>(shared_from_this()));
}

bool Squirrel::fight(std::shared_ptr<Squirrel> other) {
    fight_notify(other, false);
    return false;
}

bool Squirrel::fight(std::shared_ptr<Werewolf> other) {
    fight_notify(other, true);
    return true;
}

bool Squirrel::fight(std::shared_ptr<Druid> other) {
    fight_notify(other, false);
    return false;
}

int Squirrel::get_move_distance() const {
    return 0;
}

int Squirrel::get_kill_distance() const {
    return 0;
}

std::ostream &operator<<(std::ostream &os, Squirrel &squirrel) {
    os << "Squirrel: " << *static_cast<NPC *>(&squirrel) << std::endl;
    return os;
}