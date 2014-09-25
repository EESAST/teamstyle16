#include "connection.h"

namespace teamstyle16 {

bool Connection::Connect(const std::string &host, unsigned short port)
{
    using boost::asio::ip::tcp;
    using boost::asio::ip::address_v4;

    tcp::endpoint endpoint(address_v4::from_string(host), port);
}

void Connection::Send(const std::string &message)
{
    boost::asio::async_write(socket_,
        boost::asio::buffer(message),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
            }
            else
            {
                socket_.close();
            }
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
          game_info_()
{
}


}  // namespace teamstyle16
