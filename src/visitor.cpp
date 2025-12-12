#include "visitor.h"
#include "squirrel.h"
#include "werewolf.h"
#include "druid.h"

FightVisitor::FightVisitor(std::shared_ptr<NPC> attacker) 
    : attacker(attacker) {}

bool FightVisitor::visit(std::shared_ptr<Squirrel> squirrel) {
    return attacker->fight(squirrel);
}

bool FightVisitor::visit(std::shared_ptr<Werewolf> werewolf) {
    return attacker->fight(werewolf);
}

bool FightVisitor::visit(std::shared_ptr<Druid> druid) {
    return attacker->fight(druid);
}