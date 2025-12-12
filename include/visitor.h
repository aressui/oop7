#ifndef VISITOR_H
#define VISITOR_H

#include <memory>

class Squirrel;
class Werewolf;
class Druid;

class NPCVisitor {
public:
    virtual ~NPCVisitor() = default;
    virtual bool visit(std::shared_ptr<Squirrel> squirrel) = 0;
    virtual bool visit(std::shared_ptr<Werewolf> werewolf) = 0;
    virtual bool visit(std::shared_ptr<Druid> druid) = 0;
};

class FightVisitor : public NPCVisitor {
private:
    std::shared_ptr<class NPC> attacker;
    
public:
    FightVisitor(std::shared_ptr<class NPC> attacker);
    bool visit(std::shared_ptr<Squirrel> squirrel) override;
    bool visit(std::shared_ptr<Werewolf> werewolf) override;
    bool visit(std::shared_ptr<Druid> druid) override;
};

#endif