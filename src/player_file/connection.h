#ifndef TEAMSTYLE16_CONNECTION_H_
#define TEAMSTYLE16_CONNECTION_H_

#include <string>

#include <boost/asio.hpp>
#include <boost/utility.hpp>

#include "basic.h"

namespace teamstyle16 {

const std::string default_host("localhost");
const unsigned short default_port = 8067;

class Connection : boost::noncopyable
{
 public:
    bool Connect(const std::string &host = default_host,
                 unsigned short port = default_port);

    void Send(const std::string &message);

    static Connection * Instance();

 private:
    Connection();

    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::socket socket_;
    GameInfo game_info_;
};

}  // namespace teamstyle16

#endif  // TEAMSTYLE16_CONNECTION_H_
