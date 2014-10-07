#ifndef TEAMSTYLE16_CONNECTION_H_
#define TEAMSTYLE16_CONNECTION_H_

#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/core/noncopyable.hpp>

#include "basic.h"

namespace teamstyle16 {

using boost::asio::ip::tcp;

const std::string default_host("localhost");
const std::string default_port("8067");

class Connection : boost::noncopyable
{
 public:
    void Connect(const std::string &host = default_host,
                 const std::string &port = default_port);
    void Send(const std::string &message);

    void PrepareWork();
    int Update();  // update to latest, return rounds passed
    int TryUpdate();  // the same, does not block

    // accessors
    boost::asio::io_service & io_service() { return io_service_; }
    const GameInfo * game_info() { return &game_info_; }
    MapType map(int x, int y) { return map_[y * game_info_.x_max + x]; }

    static Connection * Instance();

 private:
    Connection();
    ~Connection() {};

    void ReadStableInfo();
    void ReadRoundInfo();

    boost::asio::io_service io_service_;
    boost::asio::ip::tcp::socket socket_;

    GameInfo game_info_;
    std::vector<MapType> map_;
};

}  // namespace teamstyle16

#endif  // TEAMSTYLE16_CONNECTION_H_
