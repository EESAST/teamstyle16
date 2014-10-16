#include <cstdlib>
#include <iostream>
#include "connection.h"

void AIMain();

int main(int argc, char const *argv[])
{
    using teamstyle16::Connection;

    Connection *connection = Connection::Instance();

    if (argc == 1)
        connection->Connect();
    else if (argc == 2)
        connection->Connect(argv[1]);
    else if (argc == 3)
        connection->Connect(argv[1], argv[2]);
    else
    {
        std::cerr << "Usage: " << argv[0] << " [host] [port]\n";
        std::exit(EXIT_FAILURE);
    }

    try
    {
        connection->PrepareWork();
        while (true)
        {
            connection->Update();
            // If AIMain() returns, we assume the player want to update infos.
            AIMain();
        }
    }
    catch (const boost::system::system_error &e)
    {
        std::clog << "Connection lost: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    return 0;
}
