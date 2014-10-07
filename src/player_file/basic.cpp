#include <cstdlib>
#include <sstream>

#include "basic.h"
#include "connection.h"

namespace teamstyle16 {

extern const std::size_t kMaxTeamNameSize = 32;

extern const int kMaxMapSize = 80;
extern const int kMaxRoundLimit = 300;
extern const int kMaxPopulationLimit = 300;

extern const int kFortScore = 1;
extern const int kDamageScore = 1;
extern const int kCollectScore = 1;

extern const Property kElementInfos[kElementTypes] = {};


const GameInfo * Info()
{
    return Connection::Instance()->game_info();
}

MapType Map(int x, int y)
{
    return Connection::Instance()->map(x, y);
}

int Update()
{
    return Connection::Instance()->Update();
}

int TryUpdate()
{
    return Connection::Instance()->TryUpdate();
}

void AttackPos(int operand, Position target)
{
    std::ostringstream oss;
    oss << "ap" << operand << ' '
                << target.x << ' '
                << target.y << ' '
                << target.z << ',';

    Connection::Instance()->Send(oss.str());
}

void AttackUnit(int operand, int target)
{
    std::ostringstream oss;
    oss << "au" << operand << ' ' << target << ',';

    Connection::Instance()->Send(oss.str());
}

void ChangeDest(int operand, Position dest)
{
    std::ostringstream oss;
    oss << "cd" << operand << ' '
                << dest.x << ' '
                << dest.y << ' '
                << dest.z << ',';

    Connection::Instance()->Send(oss.str());
}

void Fix(int operand, int target)
{
    std::ostringstream oss;
    oss << "fx" << operand << ' ' << target << ',';

    Connection::Instance()->Send(oss.str());
}

void Produce(int operand, int type)
{
    std::ostringstream oss;
    oss << "pd" << operand << ' ' << type << ',';

    Connection::Instance()->Send(oss.str());
}

void Supply(int operand, int target, int fuel, int metal, int ammo)
{
    std::ostringstream oss;
    oss << "sp" << operand << ' '
                << target << ' '
                << fuel << ' '
                << metal << ' '
                << ammo << ',';

    Connection::Instance()->Send(oss.str());
}

void Cancel(int operand)
{
    std::ostringstream oss;
    oss << "cc" << operand << ',';

    Connection::Instance()->Send(oss.str());
}

const Property * GetProperty(int type)
{
    if (type < 0)
        return NULL;
    if (type < kElementTypes)
        return &kElementInfos[type];

    // may need to calculate property for planes

    return NULL;
}

}  // namespace teamstyle16
