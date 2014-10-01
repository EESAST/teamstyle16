#include "connection.h"

#include <cstring>
#include <algorithm>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <gtest/gtest.h>

using boost::asio::ip::tcp;
using boost::asio::buffer;
using teamstyle16::Connection;

namespace {

struct StableHeader
{
    int x_max;
    int y_max;
    int weather;
    int team_num;
    int population_limit;
    int round_limit;
    float time_per_round;
};

struct RoundHeader
{
    int round;
    int element_num;
    int population;
    int production_num;
    int scores[2];
};

}  // namespace
class ConnectionTest : public ::testing::Test
{
 protected:
    ConnectionTest()
            : io_service_(),
              acceptor_(io_service_, tcp::endpoint(tcp::v4(), 8098)),
              socket_(io_service_),
              generator_((std::random_device()())),
              buffer_(256, 0)
    {
        std::thread acceptor_thread(&ConnectionTest::AcceptAI, this);
        Connection::Instance()->Connect("localhost", std::to_string(port));
        acceptor_thread.join();
    }

    void AcceptAI()
    {
        acceptor_.accept(socket_);
    }

    std::string ReadTeamName()
    {
        size_t size = teamstyle16::kMaxTeamNameSize;
        boost::asio::read(socket_, buffer(buffer_, size));

        return boost::trim_right_copy(
            std::string(buffer_.begin(), buffer_.begin() + size));
    }

    std::string ReadUntil(boost::asio::streambuf &buf,
                          std::istream &is,
                          char ch)
    {
        boost::asio::read_until(socket_, buf, ch);

        std::string command;
        std::getline(is, command, ch);
        return std::move(command);
    }

    void RandomizeState(teamstyle16::State &state)
    {
        state.index = generator_() % 10000000;
        state.pos.x = generator_() % 100;
        state.pos.y = generator_() % 100;
        state.pos.z = generator_() % 100;
        state.type = generator_() % 100;
        state.team = generator_() % 3;
        state.visible = generator_() % 2;
        state.health = generator_() % 1000;
        state.fuel = generator_() % 1000;
        state.ammo = generator_() % 1000;
        state.metal = generator_() % 1000;
        state.destination.x = generator_() % 100;
        state.destination.y = generator_() % 100;
        state.destination.z = generator_() % 100;
    }

    const static int port = 8098;

    boost::asio::io_service io_service_;
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    std::mt19937 generator_;

    std::vector<char> buffer_;
};

TEST_F(ConnectionTest, GetTeamName)
{
    EXPECT_EQ("0xFFFFFF", ReadTeamName());
}

TEST_F(ConnectionTest, StableInfoReceived)
{
    using teamstyle16::Info;
    using teamstyle16::Map;
    using teamstyle16::MapType;

    // send
    // Header
    StableHeader header = {
        100, 200, 3, 1, 500, 1000, 0.5
    };
    boost::asio::write(socket_, buffer(&header, sizeof(header)));
    // Body (map)
    std::vector<MapType> map(header.x_max * header.y_max);
    for (MapType &map_type : map)
    {
        map_type = static_cast<MapType>(generator_() % 2);
    }
    boost::asio::write(socket_, buffer(map));

    // receive
    Connection::Instance()->PrepareWork();

    EXPECT_EQ(header.x_max, Info()->x_max);
    EXPECT_EQ(header.y_max, Info()->y_max);
    EXPECT_EQ(header.weather, Info()->weather);
    EXPECT_EQ(header.team_num, Info()->team_num);
    EXPECT_EQ(header.population_limit, Info()->population_limit);
    EXPECT_EQ(header.round_limit, Info()->round_limit);
    EXPECT_EQ(header.time_per_round, Info()->time_per_round);


    for (int x = 0; x < header.x_max; x++)
        for (int y = 0; y < header.y_max; y++)
        {
            EXPECT_EQ(map[y * header.x_max + x], Map(x, y));
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

    EXPECT_EQ(0, TryUpdate());

    int info_num = 0;
    RoundHeader header;
    for (int i = 0; i < 100; i++)
    {
        header.round = i;
        header.element_num = generator_() % 100;
        header.population = generator_() % 1000;
        header.production_num = generator_() % 50;
        header.scores[0] = generator_() % 10000000;
        header.scores[1] = generator_() % 10000000;

        std::vector<ProductionEntry> production_list(header.production_num);
        for (auto &entry : production_list)
        {
            entry.unit_type = generator_() % kElementTypes;
            entry.round_left = generator_() % 20;
        }

        std::vector<State> states(header.element_num);
        for (auto &state : states)
        {
            RandomizeState(state);
        }

        // send
        boost::asio::write(socket_, buffer(&header, sizeof(header)));
        boost::asio::write(socket_, buffer(production_list));
        boost::asio::write(socket_, buffer(states));
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

        EXPECT_EQ(header.round, Info()->round);
        EXPECT_EQ(header.element_num, Info()->element_num);
        EXPECT_EQ(header.population, Info()->population);
        EXPECT_EQ(header.production_num, Info()->production_num);
        EXPECT_EQ(header.scores[0], Info()->scores[0]);
        EXPECT_EQ(header.scores[1], Info()->scores[1]);
        EXPECT_EQ(0, std::memcmp(production_list.data(),
                                 Info()->production_list,
                                 sizeof(ProductionEntry) * header.production_num));
        EXPECT_EQ(0, std::memcmp(states.data(),
                                 Info()->elements,
                                 sizeof(State) * header.element_num));
    }
}

TEST_F(ConnectionTest, CommandSentCorrectly)
{
    boost::asio::streambuf buf;
    std::istream is(&buf);
    std::string command;

    ReadTeamName();

    teamstyle16::AttackPos(122432, {3, 56, 8});
    EXPECT_EQ("ap122432 3 56 8", ReadUntil(buf, is, ','));

    teamstyle16::AttackUnit(24353, 72458);
    EXPECT_EQ("au24353 72458", ReadUntil(buf, is, ','));

    teamstyle16::ChangeDest(99589, {54, 21, 66});
    EXPECT_EQ("cd99589 54 21 66", ReadUntil(buf, is, ','));

    teamstyle16::Fix(77777, 45678);
    EXPECT_EQ("fx77777 45678", ReadUntil(buf, is, ','));

    teamstyle16::Produce(7438, 3);
    EXPECT_EQ("pd7438 3", ReadUntil(buf, is, ','));

    teamstyle16::Supply(50702, 49810, 500, -1, 450);
    EXPECT_EQ("sp50702 49810 500 -1 450", ReadUntil(buf, is, ','));

    teamstyle16::Cancel();
    EXPECT_EQ("cc", ReadUntil(buf, is, ','));
}

