#ifndef  BASIC_H
#define  BASIC_H

// 地图参数
const int kMaxRound = 300;           // 最大回合数
const int kMaxMapSize = 80;          // 地图最大边长
const int kMaxProductionNum = 10;

//
const int kMaxFortNum = 6;            // 据点最大数
const int kMaxBuildingNum = 2 + kMaxFortNum;

const int kMaxMineNum = 8;            // 矿场最大数
const int kMaxOilfieldNum = 8;        // 油田最大数
const int kMaxResourceNum = kMaxMineNum + kMaxOilfieldNum;

const int kMaxUnitFood = 200;           // 每方最大人口数

const int kMaxElementNum = kMaxBuildingNum + kMaxResourceNum + kMaxUnitFood;
//

const int kFortScore = 1;       // 占领据点每回合奖励的积分
const int kDamageScore = 1;     // 造成一点伤害奖励的积分
const int kCollectScore = 1;    // 收集一单位资源奖励的积分

enum { NO_TEAM = 2 };
enum Unitlevel { UNDERWATER, SURFACE, AIR };  // 单位类型
enum AttackType { FIRE, TORPEDO };  // 攻击类型
// 潜艇只能造成和接受鱼雷伤害
// 陆地建筑只能造成和接受火力伤害
// 飞机不能接受鱼雷伤害
// Attack[0],[1]分别代表火力攻击和鱼雷攻击，
// Defence[0],[1]分别代表对火力防御和对鱼雷防御
enum MapType { OCEAN, LAND };       // 地形：海洋，陆地
enum ElementType
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
    CRUISER,     // 巡洋舰
    BATTLESHIP,  // 战舰
    CARRIER,     // 航母
    CARGO,       // 运输舰
    FORMATION,   // 机群（飞机编队）
    kElementTypes
};
const int kBuildingTypes = 2;
const int kResourceTypes = 2;
const int kUnitTypes = 7;

enum PlaneType  // 机种
{
    SCOUT,      // 侦察机
    TORPEDOER,  // 鱼雷机
    BOMBER,     // 轰炸机
    FIGHTER,    // 战斗机
    kMaxPlaneType
};

struct Formation
{
    int num[kMaxPlaneType];
};

// Parse formation from type number
bool KindToFormation(int type, Formation *formation);
// Make type number from struct Formation
int FormationToKind(const Formation *Formation);


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

struct Property
{
    Unitlevel level;
    Size size;

    int sight_range[3];
    int fire_range[3];

    int health_max;
    int fuel_max;
    int ammo_max;
    int ammo_once;  // 单次攻击消耗弹药
    int metal_max;

    int attack[2];
    int defence[2];
    int speed;

    int cost;  //消耗钢铁
    int build_round;  // 生产所需回合数
	int population;   //人口占有
};

const Property kElementInfo[kElementTypes] = {};
const Property kPlaneInfo[4] = {};

// if type is a basic type (is ElementType but is not Formation), 
//    return a pointer pointing into kElementInfo
// Else if type can be parsed into Formation, generate the property of it.
//     The returned value points to an internal object whose validity or value
//     may be altered by any subsequent call to GetProperty.
// Else, return NULL
const Property * GetProperty(int type);

const int ScoutSightRange[3] = {1, 3, 4};        // 编队内有侦察机时的视野范围
const int NonScoutSightRange[3] = {0, 1, 2};     // 编队内无侦察机时的视野范围


struct State
{
    int element_index;  // every element has a unique index

    Position pos;  // Position of the upper-left corner
    int type;
    int team;
    bool visible;

    int health;
    int fuel;
    int ammo;
    int metal;

    Position destination;
};

struct Map
{
    int row;
    int col;

    MapType type[kMaxMapSize][kMaxMapSize];  // 地形

    // 存储地图各处元素索引号，包含建筑，资源，单位
    // 若该处不存在元素，则为-1
    int element[kMaxMapSize][kMaxMapSize][3];
};

struct ProductionEntry
{
    int unit_type;
    int round_left;
};

struct GameInfo  // 游戏信息结构体，每回合选手从中获取必要的信息
{
    int team_num;  // 队伍号(0或1)
    int score[2];  // 两队当前积分
    int round;     // 当前总回合数
	int weather;   //天气影响所有单位的视野，数值为单位视野的改变量
	
    Map map;

    int element_num;
    State elements[kMaxElementNum];

    int building_num;
    State *building;  // point into elements

    int resource_num;
    State *resource;  // point into elements

    int unit_num[2];
    State *unit;  // point into elements

    int production_num;
    ProductionEntry production_list[kMaxProductionNum];
};

enum OrderType
{
    PASS,
    CHANGE_DESTINATION,  // 更改目的地为target
    ATTACK,   // 攻击索引号为value的单位
    SUPPLY,   // 补给索引号为value的单位
    COLLECT,  // 收集索引号为value的单位
    FIX,      // 维修索引号为value的单位
    EXPLODE,  // 自爆
    PRODUCE   // 生产value类型的单位
};


struct Command
{
    int element_index;  // 操作对象索引号
    int order;

    Position target;
    int value;          // 各项指令的value
};


#endif
