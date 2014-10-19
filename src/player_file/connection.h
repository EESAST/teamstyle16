#ifndef TEAMSTYLE16_CONNECTION_H_
#define TEAMSTYLE16_CONNECTION_H_

#include <string>
#include <vector>

#include <boost/asio.hpp>

// boost::noncopyable has been moved since Boost 1.56
#if (Boost_VERSION_MAJOR >= 1 && Boost_VERSION_MINOR >= 56)
#include <boost/core/noncopyable.hpp>
#else
#include <boost/utility.hpp>
#endif

#include "basic.h"

namespace teamstyle16 {

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
