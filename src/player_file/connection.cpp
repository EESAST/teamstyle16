#include <cstdint>
#include "connection.h"

namespace teamstyle16 {

typedef std::uint8_t Coordinate;
typedef std::uint8_t Population;
typedef std::int16_t Round;

struct StableHeader
{
    Coordinate x_max;
    Coordinate y_max;
    std::int8_t weather;
    std::int8_t team_num;
    Population max_population;
    Round max_round;
    float time_per_round;
};

struct RoundHeader
{
    Population population;
    std::uint8_t production_list_size;
    Round round;
    std::int32_t score[2];
};

struct ProductionListEntry
{
    
};

struct ElementState
{
    std::int32_t index;
    Coordinate x;
    Coordinate y;
    Coordinate z;
    Type type;
    std::int8_t team;
    bool visible;
    std::uint16_t health;
    std::uint16_t fuel;
    std::uint16_t ammo;
    std::uint16_t metal;
    Coordinate dest_x;
    Coordinate dest_y;
};

void Connection::Connect(const std::string &host, unsigned short port)
{
    using boost::asio::ip::tcp;

    tcp::resolver resolver(io_service_);
    auto endpoint_iterator = resolver.resolve({host, std::to_string(port)});

    boost::asio::async_connect(socket_, endpoint_iterator,
        [this](boost::system::error_code ec, tcp::resolver::iterator)
        {
            if (!ec)
                ReadStableHeader();
        });

    std::thread t([this](){ io_service_.run(); });
}

void Connection::Send(const std::string &message)
{
    boost::asio::async_write(socket_, boost::asio::buffer(message),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (ec)
                socket_.close();
        });
}

Connection * Connection::Instance()
{
    static Connection connection;
    return &connection;
}

Connection::Connection()
        : io_service_(),
          socket_(io_service_),
          io_service_thread_(),
          header_buffer_(std::max(sizeof(StableHeader),
                                  sizeof(RoundHeader))),
          body_buffer_(),
          game_info_(),
          map_()
{
}

void Connection::ReadStableHeader()
{
    boost::asio::async_read(socket_,
        boost::asio::buffer(header_buffer_, sizeof(StableHeader)),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            DecodeStableHeader();
            ReadStableBody();
          }
          else
          {
            socket_.close();
          }
        });
}

void Connection::DecodeStableHeader()
{
    auto header = reinterpret_cast<const StableHeader *>(header_buffer_.data());
    
    game_info_.x_max = header->x_max;
    game_info_.y_max = header->y_max;
    game_info_.weather = header->weather;
    game_info_.team_num = header->team_num;
    game_info_.max_population = header->max_population;
    game_info_.max_round = header->max_round;
    game_info_.time_per_round = header->time_per_round;
}

void Connection::ReadStableBody()
{
    map_.resize(game_info_.x_max * game_info_.y_max);
    boost::asio::async_read(socket_, boost::asio::buffer(map_, map_.size()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (ec)
                socket_.close();
        });
}

void Connection::ReadRoundHeader()
{
    boost::asio::async_read(socket_,
        boost::asio::buffer(header_buffer_, sizeof(RoundHeader)),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            DecodeRoundHeader();
            ReadRoundBody();
          }
          else
          {
            socket_.close();
          }
        });
}

void Connection::DecodeRoundHeader()
{
    
}




Connection::~Connection()
{
    io_service_.post([this]() { socket_.close(); });
}

}  // namespace teamstyle16
