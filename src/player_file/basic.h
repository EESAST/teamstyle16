#ifndef  BASIC_H
#define  BASIC_H

// 地图参数
const int kMaxRound = 1000;           // 最大回合数
const int kMaxMapSize = 200;          // 地图最大边长
const int kMaxFortNum = 6;            // 据点最大数
const int kMaxProductionNum = 10;

const int kMaxBuildingNum = 2 + kMaxFortNum;

const int kMaxMineNum = 8;            // 矿场最大数
const int kMaxOilfieldNum = 8;        // 油田最大数
const int kMaxResourceNum = kMaxMineNum + kMaxOilfieldNum;

const int kMaxUnitNum = 100;          // 每方最大单位数

enum Level { UNDERWATER, SURFACE, AIR };  // 层次
enum AttackType { FIRE, TORPEDO };  // 攻击类型
// 潜艇只能造成和接受鱼雷伤害
// 陆地建筑只能造成和接受火力伤害
// 飞机不能接受鱼雷伤害
// Attack[0],[1]分别代表火力攻击和鱼雷攻击，
// Defence[0],[1]分别代表对火力防御和对鱼雷防御
enum MapType { OCEAN, LAND };       // 地形：海洋，陆地
enum BuildingType { BASE, FORT, kMaxBuildingType };  // 建筑
enum ResourceType { MINE, OILFIELD, kMaxResourceType };   // 资源
enum UnitType  // 单位
{
    SUBMARINE,   // 潜艇
    DESTROYER,   // 驱逐舰
    CRUISER,     // 巡洋舰
    BATTLESHIP,  // 战舰
    CARRIER,     // 航母
    CARGO,       // 运输舰
    FORMATION,   // 机群（飞机编队）
    kMaxUnitType
};
enum PlaneType  // 机种
{
    FIGHTER,    // 战斗机
    TORPEDOER,  // 鱼雷机
    BOMBER,     // 轰炸机
    SCOUT,      // 侦察机
    kMaxPlaneType
};


struct Size
{
    int x_length;
    int y_length;
};


struct Property
{
    Level level;
    Size size;

    int sight_range[3];
    int fire_range[3];

    int health_max;
    int fuel_max;
    int ammo_max;
    int metal_max;

    int attack[2];
    int defence[2];
    int speed;

    int cost;
};
const Property kBuildingInfo[kMaxBuildingType] = {};
const Property kUnitInfo[kMaxUnitType] = {};
const Property kPlaneInfo[4] = {};  // 各机种参数
// 飞机的部分属性不是常量

const int ScoutSightRange[3] = {1, 3, 4};        // 编队内有侦察机时的视野范围
const int NonScoutSightRange[3] = {0, 1, 2};     // 编队内无侦察机时的视野范围



const int IslandScoreTime = 5;   // 连续占有岛屿触发积分奖励的回合数
                                 // @Vone 每回合奖励的分数？



struct Position
{
    int x;
    int y;
};

struct State
{
    int index;

    Position pos;
    int type;

    int health;
    int fuel;
    int ammo;
    int metal;

    Position destination;

    const Property *info;
};

struct ProductionEntry
{
    int unit_type;
    int round_left;
};

struct GameInfo  // 游戏信息结构体，每回合选手从中获取必要的信息
{
    int team_num;                                // 队伍号(0或1)
    int score[2];                                   // 两队当前积分
    int round;                                      // 当前总回合数

    int resource_num;
    State resources[kMaxResourceNum]; // 资源点

    int unit_num[2];
    State units[2 * kMaxUnitNum];            // 己方视野范围内的所有单位的数组

    int building_num;
    State buildings[kMaxBuildingNum];

    ProductionEntry production_list[kMaxProductionNum];
};

enum Order { ATTACK, SUPPLY, FIX, BUILD，EXPLODE };   // 攻击，补给，维修，生产，自爆

struct Command
{
    Position destination;  // 要移动的目的地   移动目的可以为原地，即原地等待
    Order order;                // 攻击，补给，维修，生产，自爆
    Position order_target;      // order操作目标的坐标
};


#endif
