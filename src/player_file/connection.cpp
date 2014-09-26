#include <cstdint>
#include <boost/log/trivial.hpp>
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
    BOOST_LOG_TRIVIAL(info) << "Connecting to " << host << ':' << port;

    tcp::resolver resolver(io_service_);
    auto endpoint_iterator = resolver.resolve({host, port});
    boost::asio::connect(socket_, endpoint_iterator);
    // send back team name
    boost::asio::write(socket_,
                       boost::asio::buffer(std::string(GetTeamName())));
    BOOST_LOG_TRIVIAL(info) << "Connection established";
}


void Connection::Send(const std::string &message)
{
    BOOST_LOG_TRIVIAL(info) << "Sending message (" + message + ") to host";
    boost::asio::write(socket_, boost::asio::buffer(message));
    BOOST_LOG_TRIVIAL(info) << "Message sent";
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
    BOOST_LOG_TRIVIAL(info) << "Reading stable infomation from host";
    // read header
    std::vector<char> buffer(sizeof(StableHeader));
    BOOST_LOG_TRIVIAL(debug) << "Reading stable header from host";
    boost::asio::read(socket_, boost::asio::buffer(buffer));
    BOOST_LOG_TRIVIAL(debug) << "Stable header read";
    
    const StableHeader * header =
        reinterpret_cast<const StableHeader *>(buffer.data());

    BOOST_LOG_TRIVIAL(debug) << "Decoding stable info header";
    game_info_.x_max = header->x_max;
    game_info_.y_max = header->y_max;
    game_info_.weather = header->weather;
    game_info_.team_num = header->team_num;
    game_info_.max_population = header->max_population;
    game_info_.max_round = header->max_round;
    game_info_.time_per_round = header->time_per_round;
    BOOST_LOG_TRIVIAL(debug) << "Decode completed";

    // read body (map)
    map_.resize(game_info_.x_max * game_info_.y_max);
    BOOST_LOG_TRIVIAL(debug) << "Reading map";
    boost::asio::read(socket_, boost::asio::buffer(map_));
    BOOST_LOG_TRIVIAL(debug) << "Map read";
    BOOST_LOG_TRIVIAL(info) << "Stable infomation read";
}

void Connection::ReadRoundInfo()
{
    BOOST_LOG_TRIVIAL(info) << "Reading round infomation from host";
    // read header
    std::vector<char> buffer(sizeof(RoundHeader));
    BOOST_LOG_TRIVIAL(debug) << "Reading round header from host";
    boost::asio::read(socket_, boost::asio::buffer(buffer));
    BOOST_LOG_TRIVIAL(debug) << "Round header read";

    const RoundHeader * header =
        reinterpret_cast<const RoundHeader *>(buffer.data());

    BOOST_LOG_TRIVIAL(debug) << "Decoding stable info header";
    game_info_.round = header->round;
    game_info_.element_num = header->element_num;
    game_info_.population = header->population;
    game_info_.production_num = header->production_num;
    game_info_.score[0] = header->score[0];
    game_info_.score[1] = header->score[1];
    BOOST_LOG_TRIVIAL(debug) << "Decode completed";

    // read body
    // production list
    BOOST_LOG_TRIVIAL(debug) << "Reading production list from host";
    boost::asio::read(
        socket_,
        boost::asio::buffer(game_info_.production_list,
                            game_info_.production_num * sizeof(ProductionEntry)));
    BOOST_LOG_TRIVIAL(debug) << "Production list read (" +
                                std::to_string(game_info_.production_num) +
                                " entry(s))";
    
    // states
    BOOST_LOG_TRIVIAL(debug) << "Reading states from host";
    boost::asio::read(
        socket_,
        boost::asio::buffer(game_info_.elements,
                            game_info_.element_num * sizeof(State)));
    BOOST_LOG_TRIVIAL(debug) << "States read (" +
                                std::to_string(game_info_.element_num) +
                                " entry(s))";
    
    BOOST_LOG_TRIVIAL(info) << "Round infomation read";
}

Connection * Connection::Instance()
{
    static Connection connection;
    return &connection;
}

Connection::Connection()
        : io_service_(),
          socket_(io_service_),
          game_info_({0}),
          map_({0})
{
}

}  // namespace teamstyle16
