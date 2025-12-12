#ifndef SQUIRREL_H
#define SQUIRREL_H

#include "npc.h"
#include "visitor.h"

class Squirrel : public NPC {
public:
    Squirrel(int x, int y, const std::string& name);
    Squirrel(std::istream &is);

    void print() override;
    void save(std::ostream &os) override;
    
    bool accept(std::shared_ptr<NPCVisitor> visitor) override;
    
    bool fight(std::shared_ptr<Squirrel> other) override;
    bool fight(std::shared_ptr<Werewolf> other) override;
    bool fight(std::shared_ptr<Druid> other) override;

    int get_move_distance() const override;
    int get_kill_distance() const override;

    friend std::ostream &operator<<(std::ostream &os, Squirrel &squirrel);
};

#endif