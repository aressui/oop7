#ifndef FACTORY_H
#define FACTORY_H

#include "npc.h"
#include "visitor.h"
#include <memory>
#include <random>

std::shared_ptr<NPC> factory(NpcType type, int x, int y, const std::string& name);
std::shared_ptr<NPC> factory(std::istream &is);
void save(const set_t &array, const std::string &filename);
set_t load(const std::string &filename);
set_t fight(const set_t &array, size_t distance);
std::string generate_name();

#endif