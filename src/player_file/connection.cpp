#include <cstdlib>
#include <iostream>
#include "connection.h"

const char * GetTeamName();

namespace teamstyle16 {

using boost::asio::ip::tcp;

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

void Connection::Connect(const std::string &host, const std::string &port)
{
    try
    {
        std::clog << "Connecting to " << host << ':' << port << std::endl;
        iosteam_.connect(tcp::resolver::query(host, port));
    }
    catch (const boost::system::system_error &e)
    {
        std::clog << "Connection failed: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
    // send back team name
    std::string team_name = GetTeamName();
    if (team_name.size() > kMaxTeamNameSize)
        team_name.resize(kMaxTeamNameSize);

    iosteam_ << team_name;
    std::clog << "Connection established\n";
}


void Connection::Send(const std::string &message)
{
    std::clog << "Sending message (" + message + ") to host\n";
    iosteam_ << message;
    std::clog << "Message sent\n";
}

void Connection::PrepareWork()
{
    ReadStableInfo();
}

int Connection::Update()
{
    std::clog << "Force update\n";
    int round_passed = 0;
    do
    {
        ReadRoundInfo();
        round_passed++;
    } while (iosteam_.rdbuf()->available());

    std::clog << round_passed << " round(s) passed\n";
    return round_passed;
}

int Connection::TryUpdate()
{
    std::clog << "Try update\n";

    int round_passed = 0;
    while (iosteam_.rdbuf()->available())
    {
        ReadRoundInfo();
        round_passed++;
    }

    std::clog << round_passed << " round(s) passed\n";
    return round_passed;
}

void Connection::ReadStableInfo()
{
    std::clog << "Reading stable infomation from host\n";
    // read header
    std::vector<char> buffer(sizeof(StableHeader));
    std::clog << "Reading stable header from host\n";
    iosteam_.read(buffer.data(), buffer.size());
    std::clog << "Stable header read\n";

    const StableHeader * header =
        reinterpret_cast<const StableHeader *>(buffer.data());

    std::clog << "Decoding stable info header\n";
    game_info_.x_max = header->x_max;
    game_info_.y_max = header->y_max;
    game_info_.weather = header->weather;
    game_info_.team_num = header->team_num;
    game_info_.population_limit = header->population_limit;
    game_info_.round_limit = header->round_limit;
    game_info_.time_per_round = header->time_per_round;
    std::clog << "Decode completed\n";

    // read body (map)
    map_.resize(game_info_.x_max * game_info_.y_max);
    std::clog << "Reading map (" << game_info_.x_max << " * "
                                 << game_info_.y_max << ")\n";
    iosteam_.read(reinterpret_cast<char *>(map_.data()),
                  map_.size() * sizeof(map_[0]));
    std::clog << "Map read\n";
    std::clog << "Stable infomation read\n";
}

void Connection::ReadRoundInfo()
{
    std::clog << "Reading round infomation from host\n";
    // read header
    std::vector<char> buffer(sizeof(RoundHeader));
    std::clog << "Reading round header from host\n";
    iosteam_.read(buffer.data(), buffer.size());
    std::clog << "Round header read\n";

    const RoundHeader * header =
        reinterpret_cast<const RoundHeader *>(buffer.data());

    std::clog << "Decoding stable info header\n";
    game_info_.round = header->round;
    game_info_.element_num = header->element_num;
    game_info_.population = header->population;
    game_info_.production_num = header->production_num;
    game_info_.scores[0] = header->scores[0];
    game_info_.scores[1] = header->scores[1];
    std::clog << "Decode completed\n";

    // read body
    // production list
    std::clog << "Reading production list from host\n";
    iosteam_.read(reinterpret_cast<char *>(game_info_.production_list),
                  game_info_.production_num * sizeof(ProductionEntry));
    std::clog << "Production list read (" << game_info_.production_num
              << " entry(s))\n";

    // states
    std::clog << "Reading states from host\n";
    iosteam_.read(reinterpret_cast<char *>(game_info_.elements),
                  game_info_.element_num * sizeof(State));
    std::clog << "States read (" << game_info_.element_num << " entry(s))\n";

    std::clog << "Round infomation read\n";
}

Connection * Connection::Instance()
{
    static Connection connection;
    return &connection;
}

Connection::Connection()
        : iosteam_(),
          game_info_(),
          map_()
{
}

}  // namespace teamstyle16
