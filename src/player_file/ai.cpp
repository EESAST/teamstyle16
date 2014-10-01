#include <chrono>
#include <thread>
#include "basic.h"

// Remove this line if you konw C++, and don't want a dirty namespace
using namespace teamstyle16;

const char * GetTeamName()
{
    return "0xFFFFFF";  // 队伍名
}

void AIMain()
{
    // AI代码写在这里
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // AttackPos(1, {2, 3, 4});
        AttackUnit(5, 6);
        ChangeDest(7, {8, 9, 10});
        Fix(11, 12);
        Produce(13, 14);
        Supply(15, 16, -1, -1, -1);
        Cancel(1);
    }
}

