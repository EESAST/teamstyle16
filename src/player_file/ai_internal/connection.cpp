#include <cstdlib>
#include <iostream>
#include "connection.h"

const char * GetTeamName();

namespace {

void ParseToState(const teamstyle16::communicate::Element &element,
                  teamstyle16::State &state)
{
    state.index = element.index();

    state.pos.x = element.pos().x();
    state.pos.y = element.pos().y();
    state.pos.z = element.pos().z();

    state.size.x_length = element.size().x();
    state.size.y_length = element.size().y();

    state.type = teamstyle16::ElementType(element.type());
    state.team = element.team();
    state.visible = element.visible();
    state.health = element.health();
    state.fuel = element.fuel();
    state.ammo = element.ammo();
    state.metal = element.metal();
    state.ammo = element.ammo();

    state.destination.x = element.dest().x();
    state.destination.y = element.dest().y();
    state.destination.z = element.dest().z();
}

}  // namespace

namespace teamstyle16 {

using boost::asio::ip::tcp;

void Connection::Connect(const std::string &host, const std::string &port)
{
    std::clog << "Connecting to " << host << ':' << port << std::endl;
    iostream_.connect(host, port);
    if (!iostream_)
    {
        std::clog << "Connection failed: " << iostream_.error().message() << std::endl;
        std::exit(EXIT_FAILURE);
    }
    // send back team name
    std::string team_name = GetTeamName();
    if (team_name.size() > kMaxTeamNameSize)
        team_name.resize(kMaxTeamNameSize);

    // TODO: check whether '\n' in team_name
    iostream_ << team_name << std::endl;
    std::clog << "Connection established\n";
}


void Connection::Send(const std::string &message)
{
    std::clog << "Sending message (" + message + ") to host\n";
    iostream_ << message;
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
    } while (iostream_.rdbuf()->available());

    std::clog << round_passed << " round(s) passed\n";
    return round_passed;
}

int Connection::TryUpdate()
{
    std::clog << "Try update\n";

    int round_passed = 0;
    while (iostream_.rdbuf()->available())
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
    ReadMessage();
    stable_info_.ParseFromString(message_);

    std::clog << "Decoding stable info\n";
    game_info_.x_max = stable_info_.map().x_max();
    game_info_.y_max = stable_info_.map().y_max();
    game_info_.team_num = stable_info_.team_num();
    game_info_.weather = stable_info_.weather();
    game_info_.population_limit = stable_info_.population_limit();
    game_info_.round_limit = stable_info_.round_limit();
    game_info_.time_per_round = stable_info_.time_per_round();
    std::clog << "Decode completed\n";

    std::clog << "Stable infomation read\n";
}

void Connection::ReadRoundInfo()
{
    std::clog << "Reading round infomation from host\n";
    ReadMessage();
    round_info_.ParseFromString(message_);

    std::clog << "Decoding round info\n";
    game_info_.round = round_info_.round();
    game_info_.scores[0] = round_info_.score(0);
    game_info_.scores[1] = round_info_.score(1);
    game_info_.population = round_info_.population();
    game_info_.element_num = round_info_.element_size();
    game_info_.production_num = round_info_.production_list_size();
    std::clog << "Decode completed\n";

    // Set elements
    elements_.clear();
    for (int i = 0; i < game_info_.element_num; i++)
    {
        const communicate::Element &element = round_info_.element(i);
        State &local_element = elements_[element.index()];

        ParseToState(element, local_element);
        game_info_.elements[i] = &local_element;
    }
    std::clog << "States read (" << game_info_.element_num << " entry(s))\n";

    // Set prodection list
    for (int i = 0; i < game_info_.production_num; i++)
    {
        const communicate::RoundInfo::ProductionEntry &this_entry =
            round_info_.production_list(i);
        game_info_.production_list[i].unit_type = this_entry.type();
        game_info_.production_list[i].round_left = this_entry.round_left();
    }

    std::clog << "Round infomation read\n";
}

void Connection::ReadMessage()
{
    std::size_t message_size;
    if (!(iostream_ >> message_size))
        throw std::runtime_error("Failed to read message size from host, "
                                 "connection seems lost");

    iostream_.ignore(1);  // ignore the following '\n'

    message_.resize(message_size);
    iostream_.read(&message_[0], message_size);
    if (!iostream_)  // failed to read
        throw std::runtime_error("Failed to read messages from host, "
                                 "connection seems lost");
}

Connection * Connection::Instance()
{
    static Connection connection;
    return &connection;
}

Connection::Connection()
        : iostream_(),
          stable_info_(),
          round_info_(),
          message_(),
          elements_(),
          game_info_()
{
}

}  // namespace teamstyle16
