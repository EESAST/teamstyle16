#include "basic.h"

#include <string>

#include <gtest/gtest.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::json_parser::read_json;
using boost::property_tree::json_parser::write_json;
using boost::property_tree::ptree;

class BasicTest : public ::testing::Test
{
 protected:
    BasicTest() { read_json("../game_constants.json", pt_); }
    void CheckProperty(int type, const std::string &name);

    ptree pt_;
};

TEST_F(BasicTest, CheckBasicConstants)
{
    using namespace teamstyle16;

    EXPECT_EQ(pt_.get<int>("max_map_size"), kMaxMapSize);
    EXPECT_EQ(pt_.get<int>("max_round_limit"), kMaxRoundLimit);
    EXPECT_EQ(pt_.get<int>("max_population_limit"), kMaxPopulationLimit);
    EXPECT_EQ(pt_.get<int>("fort_score"), kFortScore);
    EXPECT_EQ(pt_.get<int>("damage_score"), kDamageScore);
    EXPECT_EQ(pt_.get<int>("collect_score"), kCollectScore);
}

TEST_F(BasicTest, CheckProperties)
{
    using namespace teamstyle16;

    CheckProperty(BASE, "base");
    CheckProperty(FORT, "fort");
    CheckProperty(MINE, "mine");
    CheckProperty(OILFIELD, "oilfield");
    CheckProperty(SUBMARINE, "submarine");
    CheckProperty(DESTROYER, "destroyer");
    CheckProperty(CARRIER, "carrier");
    CheckProperty(CARGO, "cargo");
    CheckProperty(FIGHTER, "cruiser");
    CheckProperty(SCOUT, "battleship");
}

void BasicTest::CheckProperty(int type, const std::string &name)
{
    ptree element_pt = pt_.get_child("property." + name);
    const teamstyle16::Property &element_info = teamstyle16::kProperty[type];

    EXPECT_EQ(element_pt.get("level", -1),
              element_info.level) << "Element type: " << name;
    EXPECT_EQ(element_pt.get("health_max", -1),
              element_info.health_max) << "Element type: " << name;
    EXPECT_EQ(element_pt.get("fuel_max", -1),
              element_info.fuel_max) << "Element type: " << name;
    EXPECT_EQ(element_pt.get("ammo_max", -1),
              element_info.ammo_max) << "Element type: " << name;
    EXPECT_EQ(element_pt.get("ammo_once", -1),
              element_info.ammo_once) << "Element type: " << name;
    EXPECT_EQ(element_pt.get("metal_max", -1),
              element_info.metal_max) << "Element type: " << name;
    EXPECT_EQ(element_pt.get("speed", -1),
              element_info.speed) << "Element type: " << name;
    EXPECT_EQ(element_pt.get("cost", -1),
              element_info.cost) << "Element type: " << name;
    EXPECT_EQ(element_pt.get("build_round", -1),
              element_info.build_round) << "Element type: " << name;
    EXPECT_EQ(element_pt.get("population", -1),
              element_info.population) << "Element type: " << name;
}
