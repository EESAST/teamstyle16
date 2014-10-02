#ifndef TEAMSTYLE16_BASIC_H
#define TEAMSTYLE16_BASIC_H

#include <cstdint>
#include <cstddef>

namespace teamstyle16
{

const int kMaxElementNum = 200;     // 最大元素个数
const int kMaxProductionNum = 200;  // 生产列表大小

enum { NO_TEAM = 2 };
enum Level { UNDERWATER, SURFACE, AIR };  // 层次
enum AttackType { FIRE, TORPEDO };  // 攻击类型
enum MapType { OCEAN, LAND };       // 地形

enum ElementType  // 元素类型
{
    // Buildings
    BASE,
    FORT,
    // Resources
    MINE,
    OILFIELD,
    // Units
    SUBMARINE,   // 潜艇
    DESTROYER,   // 驱逐舰
    CARRIER,     // 航母
    CARGO,       // 运输舰
    FIGHTER,     // 战斗机
    SCOUT,       // 侦察机
    kElementTypes
};


struct Position
{
    int x;
    int y;
    int z;
};

struct Size
{
    int x_length;
    int y_length;
};

struct Property  // 属性
{
    Level level;
    Size size;

    int sight_ranges[3];
    int fire_ranges[3];

    int health_max;
    int fuel_max;
    int ammo_max;
    int ammo_once;  // 单次攻击消耗弹药
    int metal_max;

    int attacks[2];
    int defences[2];
    int speed;

    int cost;  //消耗钢铁
    int build_round;  // 生产所需回合数
	int population;   //人口占有
};

struct State  // 状态
{
    int index;  // 每个元素都有唯一的索引号

    Position pos;  // 元素左上角的位置
    int type;
    int team;
    bool visible;

    int health;
    int fuel;
    int ammo;
    int metal;

    Position destination;
};

struct ProductionEntry
{
    int unit_type;
    int round_left;
};

struct GameInfo  // 游戏信息结构体，每回合选手从中获取必要的信息
{
    int x_max;
    int y_max;

    int population_limit;  // 人口上限
    int round_limit;
    float time_per_round;
    int weather;  // 天气影响所有单位的视野，数值为单位视野的改变量

    int team_num;  // 队伍号(0或1)
    int scores[2];  // 两队当前积分
    int round;     // 当前总回合数

    int population;  // 当前人口

    int element_num;
    State elements[kMaxElementNum];

    int production_num;
    ProductionEntry production_list[kMaxProductionNum];
};

// *********************** 游戏常量 ***********************

extern const std::size_t kMaxTeamNameSize;

extern const int kMaxMapSize;          // 地图最大边长
extern const int kMaxRoundLimit;       // 最大回合数上限
extern const int kMaxPopulationLimit;  // 最大人口上限

extern const int kFortScore;       // 占领据点每回合奖励的积分
extern const int kDamageScore;     // 造成一点伤害奖励的积分
extern const int kCollectScore;    // 收集一单位资源奖励的积分

extern const Property kElementInfos[kElementTypes];


const GameInfo * Info();  // 获取游戏信息
MapType Map(int x, int y);

// Move to the next round, or the latest round if possible
// Return rounds actually passed
int Update();

// Trying to move to the latest round, won't block
// Return rounds actually passed
int TryUpdate();

// Commands
void AttackPos(int operand, Position target);
void AttackUnit(int operand, int target);
void ChangeDest(int operand, Position dest);
void Fix(int operand, int target);
void Produce(int operand, int type);
void Supply(int operand, int target, int fuel, int metal, int ammo);
void Cancel(int operand);  // 取消回合内此前下达的 Produce 外的所有指令


// *********************** 辅助函数 ***********************

// 如果 type 是基本类型，即取值为 [0, kElementTypes)
//     返回指向 ElementType 内部的指针
// 否则，如果 type 可以被解析成机群，生成该机群的属性
//     返回值指向一个内部对象，其有效性或值可能因随后对 GetProperty 的调用而改变
// 否则，type 取值无效，返回 NULL

const Property * GetProperty(int type);

}  // namespace teamstyle16

#endif  // TEAMSTYLE16_BASIC_H
