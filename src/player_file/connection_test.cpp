#include "connection.h"

#include <cstddef>
#include <algorithm>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include <boost/asio.hpp>
#include <gtest/gtest.h>

using boost::asio::ip::tcp;
using boost::asio::buffer;
using teamstyle16::Connection;

namespace {

}  // namespace

class ConnectionTest : public ::testing::Test
{
 protected:
    ConnectionTest()
            : iostream_(),
              acceptor_(iostream_.rdbuf()->get_io_service(),
                        tcp::endpoint(tcp::v4(), port)),
              generator_(std::random_device()())
    {
        std::thread acceptor_thread(&ConnectionTest::AcceptAI, this);
        Connection::Instance()->Connect("localhost", std::to_string(port));
        acceptor_thread.join();
    }

    void AcceptAI()
    {
        acceptor_.accept(*iostream_.rdbuf());
    }

    std::string ReadTeamName()
    {
        std::string team_name;
        std::getline(iostream_, team_name);
        return team_name;
    }

    std::string ReadUntil(char ch)
    {
        std::string command;
        std::getline(iostream_, command, ch);
        return std::move(command);
    }

    void SendMessage(const ::google::protobuf::Message &message)
    {
        std::string message_str;
        message.SerializeToString(&message_str);
        iostream_ << message_str.size() << '\n' << message_str;
    }

    void RandomizeElement(teamstyle16::communicate::Element &element)
    {
        element.set_index(generator_() % 10000000);
        element.set_type(generator_() % 100);

        element.mutable_pos()->set_x(generator_() % 100);
        element.mutable_pos()->set_y(generator_() % 100);
        element.mutable_pos()->set_z(generator_() % 100);

        element.mutable_size()->set_x(generator_() % 4 + 1);
        element.mutable_size()->set_y(generator_() % 4 + 1);

        element.set_visible(generator_() % 2);
        element.set_team(generator_() % 3);
        element.set_health(generator_() % 1000);
        element.set_fuel(generator_() % 1000);
        element.set_ammo(generator_() % 1000);
        element.set_metal(generator_() % 1000);

        element.mutable_dest()->set_x(generator_() % 100);
        element.mutable_dest()->set_y(generator_() % 100);
        element.mutable_dest()->set_z(generator_() % 100);
    }

    const static int port = 8098;

    tcp::iostream iostream_;
    tcp::acceptor acceptor_;
    std::mt19937 generator_;
};

TEST_F(ConnectionTest, GetTeamName)
{
    EXPECT_EQ("0xFFFFFF", ReadTeamName());
}

TEST_F(ConnectionTest, StableInfoReceived)
{
    using teamstyle16::Info;
    using teamstyle16::Map;
    using teamstyle16::communicate::StableInfo;

    std::cerr << "Connected to " << ReadTeamName() << std::endl;
    // send
    teamstyle16::communicate::StableInfo stable_info;
    stable_info.set_team_num(1);
    stable_info.set_weather(3);
    stable_info.set_population_limit(500);
    stable_info.set_round_limit(1000);
    stable_info.set_time_per_round(0.5f);

    teamstyle16::communicate::Map &map = *stable_info.mutable_map();
    map.set_x_max(100);
    map.set_y_max(200);
    int map_size = map.x_max() * map.y_max();
    for (int i = 0; i < map_size; i++)
    {
        map.add_terrain(generator_() % 2);
    }

    ASSERT_TRUE(stable_info.IsInitialized());
    SendMessage(stable_info);

    // receive
    Connection::Instance()->PrepareWork();

    EXPECT_EQ(stable_info.team_num(), Info()->team_num);
    EXPECT_EQ(stable_info.weather(), Info()->weather);
    EXPECT_EQ(stable_info.population_limit(), Info()->population_limit);
    EXPECT_EQ(stable_info.round_limit(), Info()->round_limit);
    EXPECT_EQ(stable_info.time_per_round(), Info()->time_per_round);

    EXPECT_EQ(map.x_max(), Info()->x_max);
    EXPECT_EQ(map.y_max(), Info()->y_max);
    for (int x = 0; x < game_info_.x_max; x++)
        for (int y = 0; y < game_info_.y_max; y++)
        {
            EXPECT_EQ(map.terrain(y * map.x_max() + x), Map(x, y));
        }
}

TEST_F(ConnectionTest, RoundInfoReceived)
{
    using teamstyle16::Info;
    using teamstyle16::ProductionEntry;
    using teamstyle16::State;
    using teamstyle16::TryUpdate;
    using teamstyle16::Update;
    using teamstyle16::kElementTypes;
    using teamstyle16::communicate::Element;
    using teamstyle16::communicate::RoundInfo;

    EXPECT_EQ(0, TryUpdate());

    int info_num = 0;
    RoundInfo round_info;
    for (int i = 0; i < 100; i++)
    {
        round_info.set_round(i);

        round_info.clear_score();
        round_info.add_score(generator_() % 10000000);
        round_info.add_score(generator_() % 10000000);

        round_info.set_population(generator_() % 1000);

        round_info.clear_element();
        int element_num = generator_() % 100;
        for (int i = 0; i < element_num; i++)
        {
            RandomizeElement(*round_info.add_element());
        }

        round_info.clear_production_list();
        int production_num = generator_() % 50;
        for (int i = 0; i < production_num; i++)
        {
            RoundInfo::ProductionEntry *entry =
                round_info.add_production_list();
            entry->set_type(generator_() % kElementTypes);
            entry->set_round_left(generator_() % 20);
        }

        // send
        ASSERT_TRUE(round_info.IsInitialized());
        SendMessage(round_info);
        info_num++;

        // receive
        int choice = generator_() % 9;
        if (choice < 3)
        {
            int round_passed = Update();
            EXPECT_GE(info_num, round_passed);
            info_num -= round_passed;
        }
        else if (choice < 6)
        {
            int round_passed = TryUpdate();
            EXPECT_GE(info_num, round_passed);
            info_num -= round_passed;
        }

        if (info_num != 0)  // some data haven't been transferred
            continue;

        EXPECT_EQ(round_info.round(), Info()->round);
        EXPECT_EQ(round_info.score(0), Info()->scores[0]);
        EXPECT_EQ(round_info.score(1), Info()->scores[1]);
        EXPECT_EQ(round_info.population(), Info()->population);
        EXPECT_EQ(round_info.element_size(), Info()->element_num);
        EXPECT_EQ(round_info.production_list_size(), Info()->production_num);

        // Compare elements
        for (int i = 0; i < Info()->element_num; i++)
        {
            const Element &expected = round_info.element(i);
            const State *state = teamstyle16::GetState(Info()->elements[i]);
            ASSERT_TRUE(state);  // shouldn't be nullptr
            const State &actual = *state;

            EXPECT_EQ(expected.index(), actual.index);
            EXPECT_EQ(expected.type(), actual.type);

            EXPECT_EQ(expected.pos().x(), actual.pos.x);
            EXPECT_EQ(expected.pos().y(), actual.pos.y);
            EXPECT_EQ(expected.pos().z(), actual.pos.z);

            EXPECT_EQ(expected.size().x(), actual.size.x_length);
            EXPECT_EQ(expected.size().y(), actual.size.y_length);

            EXPECT_EQ(expected.visible(), actual.visible);
            EXPECT_EQ(expected.team(), actual.team);
            EXPECT_EQ(expected.health(), actual.health);
            EXPECT_EQ(expected.fuel(), actual.fuel);
            EXPECT_EQ(expected.ammo(), actual.ammo);
            EXPECT_EQ(expected.metal(), actual.metal);

            EXPECT_EQ(expected.dest().x(), actual.destination.x);
            EXPECT_EQ(expected.dest().y(), actual.destination.y);
            EXPECT_EQ(expected.dest().z(), actual.destination.z);
        }

        // Compare production list
        for (int i = 0; i < Info()->production_num; i++)
        {
            const RoundInfo::ProductionEntry &expected =
                round_info.production_list(i);
            const ProductionEntry &actual = Info()->production_list[i];

            EXPECT_EQ(expected.type(), actual.unit_type);
            EXPECT_EQ(expected.round_left(), actual.round_left);
        }
    }
}

TEST_F(ConnectionTest, CommandSentCorrectly)
{
    ReadTeamName();

    teamstyle16::AttackPos(122432, {3, 56, 8});
    EXPECT_EQ("ap122432 3 56 8", ReadUntil(','));

    teamstyle16::AttackUnit(24353, 72458);
    EXPECT_EQ("au24353 72458", ReadUntil(','));

    teamstyle16::ChangeDest(99589, {54, 21, 66});
    EXPECT_EQ("cd99589 54 21 66", ReadUntil(','));

    teamstyle16::Fix(77777, 45678);
    EXPECT_EQ("fx77777 45678", ReadUntil(','));

    teamstyle16::Produce(3);
    EXPECT_EQ("pd3", ReadUntil(','));

    teamstyle16::Supply(50702, 49810, 500, -1, 450);
    EXPECT_EQ("sp50702 49810 500 -1 450", ReadUntil(','));

    teamstyle16::Cancel(386);
    EXPECT_EQ("cc386", ReadUntil(','));
}

