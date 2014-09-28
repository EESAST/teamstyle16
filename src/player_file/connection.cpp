#include <iostream>
#include <cstdint>
#include "connection.h"

const char * GetTeamName();

namespace teamstyle16 {

struct StableHeader
{
    int x_max;
    int y_max;
    int weather;
    int team_num;
    int max_population;
    int max_round;
    float time_per_round;
};

struct RoundHeader
{
    int round;
    int element_num;
    int population;
    int production_num;
    int score[2];
};

void Connection::Connect(const std::string &host, const std::string &port)
{
    std::clog << "Connecting to " << host << ':' << port << std::endl;

    tcp::resolver resolver(io_service_);
    auto endpoint_iterator = resolver.resolve({host, port});
    boost::asio::connect(socket_, endpoint_iterator);
    // send back team name
    boost::asio::write(socket_,
                       boost::asio::buffer(std::string(GetTeamName())));
    std::clog << "Connection established\n";
}


void Connection::Send(const std::string &message)
{
    std::clog << "Sending message (" + message + ") to host\n";
    boost::asio::write(socket_, boost::asio::buffer(message));
    std::clog << "Message sent\n";
}

void Connection::FirstUpdate()
{
    ReadStableInfo();
    ReadRoundInfo();
}

int Connection::Update()
{
    int round_passed = 0;
    do
    {
        ReadRoundInfo();
        round_passed++;
    } while (socket_.available());

    return round_passed;
}

int Connection::TryUpdate()
{
    int round_passed = 0;
    while (socket_.available())
    {
        ReadRoundInfo();
        round_passed++;
    }
    return round_passed;
}

void Connection::ReadStableInfo()
{
    std::clog << "Reading stable infomation from host\n";
    // read header
    std::vector<char> buffer(sizeof(StableHeader));
    std::clog << "Reading stable header from host\n";
    boost::asio::read(socket_, boost::asio::buffer(buffer));
    std::clog << "Stable header read\n";

    const StableHeader * header =
        reinterpret_cast<const StableHeader *>(buffer.data());

    std::clog << "Decoding stable info header\n";
    game_info_.x_max = header->x_max;
    game_info_.y_max = header->y_max;
    game_info_.weather = header->weather;
    game_info_.team_num = header->team_num;
    game_info_.max_population = header->max_population;
    game_info_.max_round = header->max_round;
    game_info_.time_per_round = header->time_per_round;
    std::clog << "Decode completed\n";

    // read body (map)
    map_.resize(game_info_.x_max * game_info_.y_max);
    std::clog << "Reading map\n";
    boost::asio::read(socket_, boost::asio::buffer(map_));
    std::clog << "Map read\n";
    std::clog << "Stable infomation read\n";
}

void Connection::ReadRoundInfo()
{
    std::clog << "Reading round infomation from host\n";
    // read header
    std::vector<char> buffer(sizeof(RoundHeader));
    std::clog << "Reading round header from host\n";
    boost::asio::read(socket_, boost::asio::buffer(buffer));
    std::clog << "Round header read\n";

    const RoundHeader * header =
        reinterpret_cast<const RoundHeader *>(buffer.data());

    std::clog << "Decoding stable info header\n";
    game_info_.round = header->round;
    game_info_.element_num = header->element_num;
    game_info_.population = header->population;
    game_info_.production_num = header->production_num;
    game_info_.score[0] = header->score[0];
    game_info_.score[1] = header->score[1];
    std::clog << "Decode completed\n";

    // read body
    // production list
    std::clog << "Reading production list from host\n";
    boost::asio::read(
        socket_,
        boost::asio::buffer(game_info_.production_list,
                            game_info_.production_num * sizeof(ProductionEntry)));
    std::clog << "Production list read (" +
                 std::to_string(game_info_.production_num) +
                 " entry(s))\n";

    // states
    std::clog << "Reading states from host\n";
    boost::asio::read(
        socket_,
        boost::asio::buffer(game_info_.elements,
                            game_info_.element_num * sizeof(State)));
    std::clog << "States read (" +
                 std::to_string(game_info_.element_num) +
                 " entry(s))\n";

    std::clog << "Round infomation read\n";
}

Connection * Connection::Instance()
{
    static Connection connection;
    return &connection;
}

Connection::Connection()
        : io_service_(),
          socket_(io_service_),
          game_info_({0, 0, 0, 0, 0.0f, 0, 0, {0, 0}, 0, 0, 0, {}, 0, {}}),
          map_()
{
}

}  // namespace teamstyle16
