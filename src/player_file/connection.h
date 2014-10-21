#ifndef TEAMSTYLE16_CONNECTION_H_
#define TEAMSTYLE16_CONNECTION_H_

#include <map>
#include <string>

#include <boost/asio.hpp>

// boost::noncopyable has been moved since Boost 1.56
#if (Boost_VERSION_MAJOR >= 1 && Boost_VERSION_MINOR >= 56)
#include <boost/core/noncopyable.hpp>
#else
#include <boost/utility.hpp>
#endif

#include "basic.h"
#include "communicate.pb.h"

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
    const GameInfo * game_info() { return &game_info_; }
    MapType map(int x, int y)
    {
        return static_cast<MapType>(
            stable_info_.map().terrain(y * game_info_.x_max + x));
    }
    const State * GetState(int index)
    {
        std::map<int, State>::const_iterator iter = elements_.find(index);
        return iter == elements_.end() ? NULL : &iter->second;
    }
    static Connection * Instance();

 private:
    Connection();
    ~Connection() {};

    void ReadStableInfo();
    void ReadRoundInfo();

    void ReadMessage();

    boost::asio::ip::tcp::iostream iosteam_;

    communicate::StableInfo stable_info_;
    communicate::RoundInfo round_info_;
    std::string message_;

    std::map<int, State> elements_;
    GameInfo game_info_;
};

}  // namespace teamstyle16

#endif  // TEAMSTYLE16_CONNECTION_H_
