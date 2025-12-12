#include "werewolf.h"
#include "squirrel.h"
#include "druid.h"

Werewolf::Werewolf(int x, int y, const std::string& name) 
    : NPC(WerewolfType, x, y, name) {}

Werewolf::Werewolf(std::istream &is) : NPC(WerewolfType, is) {}

void Werewolf::print() {
    std::cout << *this;
}

void Werewolf::save(std::ostream &os) {
    os << WerewolfType << std::endl;
    NPC::save(os);
}

bool Werewolf::accept(std::shared_ptr<NPCVisitor> visitor) {
    return visitor->visit(std::static_pointer_cast<Werewolf>(shared_from_this()));
}

bool Werewolf::fight(std::shared_ptr<Squirrel> other) {
    fight_notify(other, false);
    return false;
}

bool Werewolf::fight(std::shared_ptr<Werewolf> other) {
    fight_notify(other, false);
    return false;
}

bool Werewolf::fight(std::shared_ptr<Druid> other) {
    fight_notify(other, true);
    return true;
}

int Werewolf::get_move_distance() const {
    return 0;
}

int Werewolf::get_kill_distance() const {
    return 0;
}

std::ostream &operator<<(std::ostream &os, Werewolf &werewolf) {
    os << "Werewolf: " << *static_cast<NPC *>(&werewolf) << std::endl;
    return os;
}