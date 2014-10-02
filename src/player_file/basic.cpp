#include <cstdlib>

#include "basic.h"
#include "connection.h"

namespace teamstyle16 {

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
    using std::to_string;

    Connection::Instance()->Send("ap" + to_string(operand) + ' ' +
                                        to_string(target.x) + ' ' +
                                        to_string(target.y) + ' ' +
                                        to_string(target.z) + ',');
}

void AttackUnit(int operand, int target)
{
    using std::to_string;

    Connection::Instance()->Send("au" + to_string(operand) + ' ' +
                                        to_string(target) + ',');
}

void ChangeDest(int operand, Position dest)
{
    using std::to_string;

    Connection::Instance()->Send("cd" + to_string(operand) + ' ' +
                                        to_string(dest.x) + ' ' +
                                        to_string(dest.y) + ' ' +
                                        to_string(dest.z) + ',');
}

void Fix(int operand, int target)
{
    using std::to_string;

    Connection::Instance()->Send("fx" + to_string(operand) + ' ' +
                                        to_string(target) + ',');
}

void Produce(int operand, int type)
{
    using std::to_string;

    Connection::Instance()->Send("pd" + to_string(operand) + ' ' +
                                        to_string(type) + ',');
}

void Supply(int operand, int target, int fuel, int metal, int ammo)
{
    using std::to_string;

    Connection::Instance()->Send("sp" + to_string(operand) + ' ' +
                                        to_string(target) + ' ' +
                                        to_string(fuel) + ' ' +
                                        to_string(metal) + ' ' +
                                        to_string(ammo) + ',');
}

void Cancel()
{
    Connection::Instance()->Send("cc,");
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
