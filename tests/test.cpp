#include <gtest/gtest.h>
#include "npc.h"
#include "squirrel.h"
#include "werewolf.h"
#include "druid.h"
#include "visitor.h"
#include "factory.h"
#include <sstream>
#include <memory>

class MockObserver : public IFightObserver {
public:
    bool fight_observed = false;
    std::shared_ptr<NPC> last_attacker;
    std::shared_ptr<NPC> last_defender;
    bool last_win = false;

    void on_fight(const std::shared_ptr<NPC> attacker, 
                 const std::shared_ptr<NPC> defender, 
                 bool win) override {
        fight_observed = true;
        last_attacker = attacker;
        last_defender = defender;
        last_win = win;
    }
};

class NPCTest : public ::testing::Test {
protected:
    void SetUp() override {
        observer = std::make_shared<MockObserver>();
    }
    std::shared_ptr<MockObserver> observer;
};

TEST_F(NPCTest, NPCCreation) {
    auto squirrel = std::make_shared<Squirrel>(100, 100, "TestSquirrel");
    EXPECT_EQ(squirrel->get_type(), SquirrelType);
    EXPECT_EQ(squirrel->get_x(), 100);
    EXPECT_EQ(squirrel->get_y(), 100);
    EXPECT_EQ(squirrel->get_name(), "TestSquirrel");
    EXPECT_TRUE(squirrel->is_alive());
}

TEST_F(NPCTest, CoordinatesValidation) {
    EXPECT_THROW(std::make_shared<Squirrel>(-1, 100, "BadSquirrel"), std::runtime_error);
    EXPECT_THROW(std::make_shared<Squirrel>(100, 600, "BadSquirrel"), std::runtime_error);
}

TEST_F(NPCTest, DistanceCalculation) {
    auto npc1 = std::make_shared<Squirrel>(100, 100, "Squirrel1");
    auto npc2 = std::make_shared<Werewolf>(103, 104, "Werewolf1");

    EXPECT_TRUE(npc1->is_close(npc2, 5));
    EXPECT_FALSE(npc1->is_close(npc2, 4));
}

TEST_F(NPCTest, MovementDistances) {
    auto druid = std::make_shared<Druid>(100, 100, "Druid1");
    auto squirrel = std::make_shared<Squirrel>(100, 100, "Squirrel1");
    auto werewolf = std::make_shared<Werewolf>(100, 100, "Werewolf1");

    EXPECT_EQ(druid->get_move_distance(), 10);
    EXPECT_EQ(druid->get_kill_distance(), 10);
    
    EXPECT_EQ(squirrel->get_move_distance(), 0);
    EXPECT_EQ(squirrel->get_kill_distance(), 0);
    
    EXPECT_EQ(werewolf->get_move_distance(), 0);
    EXPECT_EQ(werewolf->get_kill_distance(), 0);
}

TEST_F(NPCTest, AliveState) {
    auto squirrel = std::make_shared<Squirrel>(100, 100, "Squirrel1");
    EXPECT_TRUE(squirrel->is_alive());
    squirrel->make_dead();
    EXPECT_FALSE(squirrel->is_alive());
}

TEST_F(NPCTest, PositionSetting) {
    auto squirrel = std::make_shared<Squirrel>(100, 100, "Squirrel1");
    squirrel->set_position(200, 300);
    EXPECT_EQ(squirrel->get_x(), 200);
    EXPECT_EQ(squirrel->get_y(), 300);
}

TEST_F(NPCTest, VisitorPatternCorrectness) {
    auto squirrel = std::make_shared<Squirrel>(100, 100, "Squirrel1");
    auto werewolf = std::make_shared<Werewolf>(100, 100, "Werewolf1");
    
    squirrel->subscribe(observer);
    
    auto visitor = std::make_shared<FightVisitor>(squirrel);
    bool result = werewolf->accept(visitor);
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(observer->fight_observed);
    EXPECT_TRUE(observer->last_win);
}

TEST_F(NPCTest, SquirrelFightLogic) {
    auto squirrel = std::make_shared<Squirrel>(100, 100, "Squirrel1");
    auto werewolf = std::make_shared<Werewolf>(100, 100, "Werewolf1");
    auto druid = std::make_shared<Druid>(100, 100, "Druid1");

    squirrel->subscribe(observer);

    auto visitor1 = std::make_shared<FightVisitor>(squirrel);
    EXPECT_TRUE(werewolf->accept(visitor1));
    EXPECT_TRUE(observer->fight_observed);
    EXPECT_TRUE(observer->last_win);
    
    observer->fight_observed = false;

    auto visitor2 = std::make_shared<FightVisitor>(squirrel);
    EXPECT_FALSE(druid->accept(visitor2));
    EXPECT_TRUE(observer->fight_observed);
    EXPECT_FALSE(observer->last_win);
}

TEST_F(NPCTest, WerewolfFightLogic) {
    auto werewolf = std::make_shared<Werewolf>(100, 100, "Werewolf1");
    auto druid = std::make_shared<Druid>(100, 100, "Druid1");
    auto squirrel = std::make_shared<Squirrel>(100, 100, "Squirrel1");

    werewolf->subscribe(observer);

    auto visitor1 = std::make_shared<FightVisitor>(werewolf);
    EXPECT_TRUE(druid->accept(visitor1));
    EXPECT_TRUE(observer->fight_observed);
    EXPECT_TRUE(observer->last_win);
    
    auto visitor2 = std::make_shared<FightVisitor>(werewolf);
    EXPECT_FALSE(squirrel->accept(visitor2));
    EXPECT_TRUE(observer->fight_observed);
    EXPECT_FALSE(observer->last_win);
}

TEST_F(NPCTest, DruidFightLogic) {
    auto druid = std::make_shared<Druid>(100, 100, "Druid1");
    auto squirrel = std::make_shared<Squirrel>(100, 100, "Squirrel1");
    auto werewolf = std::make_shared<Werewolf>(100, 100, "Werewolf1");

    druid->subscribe(observer);
    
    auto visitor1 = std::make_shared<FightVisitor>(druid);
    EXPECT_FALSE(squirrel->accept(visitor1));
    EXPECT_TRUE(observer->fight_observed);
    EXPECT_FALSE(observer->last_win);
    
    observer->fight_observed = false;
    
    auto visitor2 = std::make_shared<FightVisitor>(druid);
    EXPECT_FALSE(werewolf->accept(visitor2));
    EXPECT_TRUE(observer->fight_observed);
    EXPECT_FALSE(observer->last_win);
}

TEST_F(NPCTest, SaveAndLoad) {
    auto original_squirrel = std::make_shared<Squirrel>(123, 456, "TestSquirrel");
    auto original_werewolf = std::make_shared<Werewolf>(321, 254, "TestWerewolf");

    std::stringstream ss;
    original_squirrel->save(ss);
    original_werewolf->save(ss);

    ss.seekg(0);

    int type;
    ss >> type;
    auto loaded_squirrel = std::make_shared<Squirrel>(ss);

    ss >> type;
    auto loaded_werewolf = std::make_shared<Werewolf>(ss);
    
    EXPECT_EQ(loaded_squirrel->get_type(), SquirrelType);
    EXPECT_EQ(loaded_squirrel->get_x(), 123);
    EXPECT_EQ(loaded_squirrel->get_y(), 456);
    EXPECT_EQ(loaded_squirrel->get_name(), "TestSquirrel");
    
    EXPECT_EQ(loaded_werewolf->get_type(), WerewolfType);
    EXPECT_EQ(loaded_werewolf->get_x(), 321);
    EXPECT_EQ(loaded_werewolf->get_y(), 254);
    EXPECT_EQ(loaded_werewolf->get_name(), "TestWerewolf");
}

TEST_F(NPCTest, FactoryFunction) {
    auto squirrel = factory(SquirrelType, 100, 200, "FactorySquirrel");
    EXPECT_NE(squirrel, nullptr);
    EXPECT_EQ(squirrel->get_type(), SquirrelType);
    EXPECT_EQ(squirrel->get_name(), "FactorySquirrel");
    
    auto werewolf = factory(WerewolfType, 300, 400, "FactoryWerewolf");
    EXPECT_NE(werewolf, nullptr);
    EXPECT_EQ(werewolf->get_type(), WerewolfType);
    
    auto druid = factory(DruidType, 500, 100, "FactoryDruid");
    EXPECT_NE(druid, nullptr);
    EXPECT_EQ(druid->get_type(), DruidType);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}