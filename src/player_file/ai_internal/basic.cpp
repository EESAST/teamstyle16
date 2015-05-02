#include <cstddef>
#include <sstream>

#include "basic.h"
#include "connection.h"

namespace teamstyle16 {

extern const std::size_t kMaxTeamNameSize = 32;

extern const int kMaxMapSize = 80;
extern const int kMaxRoundLimit = 200;
extern const int kMaxPopulationLimit = 60;

extern const int kCaptureScore = 100;   // 攻下据点一次性奖励积分
extern const int kFortScore = 10;       // 占领据点每回合奖励积分
extern const int kDamageScore = 1;      // 每造成一点伤害奖励积分
extern const int kCollectScore = 10;    // 有效采集一次资源奖励积分

extern const int kHealthPerMetal = 5;

extern const Property kProperty[kElementTypes] = 
{
	{SURFACE,    {4, 10, 8},  {0, 8, 6}, 1000, 1000,  -1,  6, 200,  {40, 0},  {15, -1}, 0,  0,  0, 0},
	{SURFACE,    {3, 9, 7},   {0, 8, 6}, 300,  200,   300, 4, 200,  {36, 0},  {12, -1}, 0,  0,  0, 0},
	{SURFACE,    {0, 0, 0},   {0, 0, 0}, -1,   0,     0,   0, 500,  {0, 0},   {-1, -1}, 0,  0,  0, 0},
	{SURFACE,    {0, 0, 0},   {0, 0, 0}, -1,   1000,  0,   0, 0,    {0, 0},   {-1, -1}, 0,  0,  0, 0},
	{UNDERWATER, {6, 5, 3},   {5, 5, 0}, 35,   120,   20,  2, 0,    {0, 40},  {-1, 7},  6,  7,  2, 2},
	{SURFACE,    {5, 8, 6},   {4, 8, 6}, 70,   150,   40,  4, 0,    {25, 25}, {13, 10}, 7,  14, 3, 3},
	{SURFACE,    {3, 8, 8},   {0, 8, 6}, 120,  200,   70,  2, 0,    {32, 0},  {16, 10}, 5,  30, 5, 4},
	{SURFACE,    {2, 6, 5},   {0, 0, 0}, 80,   300,   120, 0, 50,   {0, 0},   {15, 8},  11, 16, 3, 1},
	{AIR,		 {1, 7, 8},   {1, 3, 4}, 50,   100,   21,  3, 0,    {30, 20}, {10, -1}, 9,  14, 3, 3},
	{AIR,		 {2, 12, 16}, {2, 3, 4}, 50,   120,   5,   1, 0,    {18, 7},  {7, -1},  12, 10, 2, 1},
};


const GameInfo * Info()
{
    return Connection::Instance()->game_info();
}

MapType Map(int x, int y)
{
    return Connection::Instance()->map(x, y);
}

const State * GetState(int index)
{
    return Connection::Instance()->GetState(index);
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

void Produce(int type)
{
    std::ostringstream oss;
    oss << "pd" << type << ',';

    Connection::Instance()->Send(oss.str());
}

void Supply(int operand, int target, int fuel, int ammo, int metal)
{
    std::ostringstream oss;
    oss << "sp" << operand << ' '
                << target << ' '
                << fuel << ' '
                << ammo << ' '
                << metal << ',';

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
        return &kProperty[type];

    // may need to calculate property for planes

    return NULL;
}

}  // namespace teamstyle16
