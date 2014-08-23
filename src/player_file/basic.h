#ifndef  BASIC_H
#define  BASIC_H

// *********************** 游戏常量 ***********************
const int kMaxRound = 300;      // 最大回合数
const int kMaxMapSize = 80;     // 地图最大边长
const int kMaxElementNum = 200;     // 最大元素个数
const int kMaxProductionNum = 200;  // 生产列表大小

const int kFortScore = 1;       // 占领据点每回合奖励的积分
const int kDamageScore = 1;     // 造成一点伤害奖励的积分
const int kCollectScore = 1;    // 收集一单位资源奖励的积分

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
    CRUISER,     // 巡洋舰
    BATTLESHIP,  // 战舰
    CARRIER,     // 航母
    CARGO,       // 运输舰
    FORMATION,   // 机群（飞机编队）
    kElementTypes
};

enum PlaneType  // 机种
{
    SCOUT,      // 侦察机
    TORPEDOER,  // 鱼雷机
    BOMBER,     // 轰炸机
    FIGHTER,    // 战斗机
    kMaxPlaneTypes
};


// *********************** 相关类型 ***********************
typedef int Formation[kMaxPlaneTypes];  // 表示机群内各机种的数目

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

struct State  // 状态
{
    int index;  // every element has a unique index

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
    int weather;   // 天气影响所有单位的视野，数值为单位视野的改变量

    int population;  // 当前人口
    int max_polulation;  // 人口上限
    
    int x_max;
    int y_max;

    int element_num;
    State elements[kMaxElementNum];

    int production_num;
    ProductionEntry production_list[kMaxProductionNum];
};


// *********************** 游戏数据 ***********************

const Property kElementInfo[kElementTypes] = {};
const Property kPlaneInfo[4] = {};


const int ScoutSightRange[3] = {1, 3, 4};        // 编队内有侦察机时的视野范围
const int NonScoutSightRange[3] = {0, 1, 2};     // 编队内无侦察机时的视野范围


const GameInfo * info();  // 获取游戏信息

MapType GetMapType(int x, int y, int z);
int GetElement(int x, int y, int z);


void Attack(int operand, int target);
void ChangeDest(int operand, const Position *dest);
void Collect(int operand, int target);
void Explode(int operand);
void Fix(int operand, int target);
void Produce(int operand, int kind);
void Supply(int operand, int target, int fuel, int metal, int ammo);
void Cancel(int operand);


// *********************** 辅助函数 ***********************

// if kind is a basic type (is ElementType), 
//    return a pointer pointing into kElementInfo
// Else if kind can be parsed into Formation, generate the property of it.
//     The returned value points to an internal object whose validity or value
//     may be altered by any subsequent call to GetProperty.
// Else, return NULL
const Property * GetProperty(int type);

// Parse formation from kind number
bool KindToFormation(int kind, Formation *formation);
// Make kind number from struct Formation
int FormationToKind(const Formation *Formation);

#endif
