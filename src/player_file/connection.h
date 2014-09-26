#ifndef TEAMSTYLE16_CONNECTION_H_
#define TEAMSTYLE16_CONNECTION_H_

#include <string>
#include <thread>

#include <boost/asio.hpp>
#include <boost/utility.hpp>

#include "basic.h"

namespace teamstyle16 {

const std::string default_host("localhost");
const unsigned short default_port = 8067;

class Connection : boost::noncopyable
{
 public:
    void Connect(const std::string &host = default_host,
                 unsigned short port = default_port);


    void Send(const std::string &message);

    static Connection * Instance();

 private:
    Connection();
    ~Connection();

    void ReadStableHeader();
    void DecodeStableHeader();
    void ReadStableBody();

    void ReadRoundHeader();
    void DecodeRoundHeader();
    void ReadRoundBody();
    void DecodeRoundBody();

    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::socket socket_;
    std::thread io_service_thread_;

    std::vector<char> header_buffer_;
    std::vector<char> body_buffer_;

    GameInfo game_info_;
    std::vector<int> map_;
};

}  // namespace teamstyle16

#endif  // TEAMSTYLE16_CONNECTION_H_
