#include "basic.h"
#include <iostream>
#include <cmath>
#include <map>
#include <cstring>
#include <cstdlib>
#include <vector>


// Remove this line if you konw C++, and don't want a dirty namespace
using namespace teamstyle16;
using std::vector;
using std::cout;
using std::endl;
using std::abs;
using std::vector;
using std::map;
using std::cerr;

const int INF = 1000;
const int MAXN = 3605;
const int TOTAL_ELEMENTTYPE = 10;

const char * GetTeamName()
{
    return "The Flow of Mini China";  // Name of your team
}

enum CommandType
{
	//Produce
	PRODUCE,
	//Suppply
	SUPPLYFORT,
	SUPPLYBASE,
	SUPPLYUNIT,
	//Attack
	ATTACKUNIT,
	ATTACKFORT,
	ATTACKBASE,
	//Fix
    FIX,
	//ChangeDest
	FORWARD,
	CARGOSUPPLY,
	RETURNBASE,
	//Cancel
	CANCEL,
    kCommandTypes
};

void init();//初始化定义各种量的函数
void enemy_init();//根据Info()初始化当时的敌人
void Supply_Repair(int i);/////该补充燃料或者弹药的时候，或者血量太少需要回基地维修
Position minus(Position pos1,Position pos2,int fire_range);//返回pos1距pos2恰好相距fire_range的一个位置
int if_in(int i,int *a,int len);//判断某个数是否在数组中
int GetBase(int team);//获得己方或对方基地的索引
int distance(Position pos1, Position pos2);//两点之间的距离
int DisToBase(Position pos1, Position pos2);  ////返回到基地的距离， pos2表示基地左上角
int if_alive(int operand);//判断某单位是否仍存活
int max(int x, int y);//最大值
int min(int x, int y);//最小值
int GetNear(Position pos, ElementType type);//获得距该位置最近的某类型单位的索引
int if_command(int i,CommandType type,ElementType tar  = kElementTypes);//判断对该成员的该命令是否可以下达（如果已经下达更高级的指令则不可）
void Cargo_Supply(int index);//对于回合开始时相距为1的单位进行补给
void MoveCargo(int i);//对于程序最后仍然没有命令的运输船，去资源点收集，或者回基地补给，或者呆在前线
void Carrier_Supply(int i);//航母对身边单位的补给
void Attack(int index);//寻找射程内的基地或据点，或选择血量最少的单位攻击,攻击后运动到距敌人恰好射程的地方
void BaseAct(int index);//基地的维修，生产
void BaseProduce(int index);///基地的生产
void Forward(int index);////寻找最近的敌军前进，如果是运输船
void DefineUnit();
Position findOcean(Position pos);//找到pos最近的不临大陆的地方
int AwayToLand(Position pos);//判断某个位置是不不临大陆
const State *getState(int index);
int changePosToNumber(Position p);
Position genPosition(int x, int y, int z);
void moveToBase(const State *s, const State *t);
Position randPos();

const GameInfo *INFO = Info();
int enemy_num;
State *enemy_element;
int (*command)[2];  //本回合发出的指令，一个效果指令，一个移动指令
double parameter[4];

int baseMineCargo = -2;  //三个补给基地的     初始化为-2，-1表示在生产
int baseFuelCargoNum = 1;
int baseFuelCargo[2] = {-2, -2};
int baseDenfendNum = 0;
int baseDefender[6] = {-2, -2, -2, -2, -2, -2};      //留守基地的驱逐舰，初始化为 -2，-1表示在生产
int baseSubmarineNum = 0;
int baseSubmarine[6] = {-2, -2, -2, -2, -2, -2};
int baseFighterNum = 0;
int baseFighter[6] = {-2, -2, -2, -2, -2, -2};

vector<int>Enemy_Indexes;
map<int, Position> lastPos;

int myBaseFuel;
int ProduceState = -1;
int DISTANCE_NEAR_BASE = 30;
int BEGINGAME = 0;
Position BaseDefendPos;//放置守卫基地的驱逐舰的位置
//Position PassWay;  //双方基地间的重要通道

int oilNum = 0;
int mineNum = 0;
vector<int> oilLeft, mineLeft;
map<int, int> oilMap;
map<int, int> mineMap;
int unitUnderConstruction[TOTAL_ELEMENTTYPE];

void readProductionList()
{
	const GameInfo *game = Info();
	int ttype;
	memset(unitUnderConstruction, 0, sizeof(unitUnderConstruction));
	for(int i = 0; i < game->production_num; i++)
	{
		ttype = game->production_list[i].unit_type;
		unitUnderConstruction[ttype]++;
	}
}

inline int calcTotalFuleOfProductionList() {
	int totalFuel = 0;
	for (int i = 4; i < TOTAL_ELEMENTTYPE; ++i) {
		totalFuel += unitUnderConstruction[i] * (kProperty[i].fuel_max / ((i == 7) + 1));
	}
	return totalFuel;
}

void AIMain()
{  
	TryUpdate();
	//回合开始时的初始化
	init();
	DefineUnit();


	readProductionList();
	const State *myBase = INFO->elements[GetBase(INFO->team_num)];
	myBaseFuel = myBase->fuel - calcTotalFuleOfProductionList();

	for(int i = 0; i < INFO->element_num; i++)
		if(INFO->elements[i]->team == INFO->team_num)
		{
			const State *Element = INFO->elements[i];
			Supply_Repair(i);//该补给或维修就去补给，维修
	
			//TryUpdate();
			//enemy_init();
			if(Element->type != BASE)
				Attack(i);        //攻击

			if(Element->type == BASE)
				BaseAct(i);
			else if(Element->type == CARGO)
			{
				Cargo_Supply(i);   //运输舰补给
				MoveCargo(i);      //运输舰运动
			}	
			else if(Element->type == CARRIER)
			{
				Carrier_Supply(i);
			}
			else ;

			Forward(i);       //前进
		}
	//Update();
	
	for (int i = 0; i < INFO->element_num; ++i) {
		const State *p = INFO->elements[i];
		if (p->team != INFO->team_num)
			continue;
		if (lastPos[p->index].x == p->pos.x && lastPos[p->index].y == p->pos.y && (if_command(i, FORWARD) || p->type == CARGO) && (INFO->round % 2 == 0)) {
			ChangeDest(p->index, findOcean(p->pos));
			//cerr << p->pos.x << " " << p->pos.y << " " << p->pos.z << endl;
			//cerr << findOcean(p->pos).x << " " << findOcean(p->pos).y << " " << findOcean(p->pos).z << endl;
			//cerr << endl;
		}
		lastPos[p->index] = p->pos;
	}

	delete []enemy_element;
	delete []command;
}

Position operator + (const Position &A, const Position &B) {
	return genPosition(A.x + B.x, A.y + B.y, A.z + B.z);
}

Position randPos() {
	return genPosition(rand() % 5 - 2, rand() % 5 - 2, 0);
}

const State *getState(int index) {
	for (int i = 0; i < INFO->element_num; ++i) {
		if (INFO->elements[i]->index == index)
			return INFO->elements[i];
	}
	return NULL;
}

//初始化定义各种量的函数
void init()
{
	if(BEGINGAME == 0)
	{
		// parameter ???
		parameter[0] = 0.3; //血量
		parameter[1] = 0.3; //金属
		parameter[2] = 0.3; //燃料
		parameter[3] = 0.4; //弹药
	
		const State *Base = INFO->elements[GetBase(INFO->team_num)];
		const State *EnemyBase = INFO->elements[GetBase(INFO->team_num ^ 1)];
		
		// what the meaning of DefendPos???
		BaseDefendPos.x = Base->pos.x+6*(Base->pos.x < EnemyBase->pos.x?1:-1);
		BaseDefendPos.y = Base->pos.y+5*(Base->pos.y < INFO->y_max/2?1:-1);
		BaseDefendPos.z = 1;
		BaseDefendPos = findOcean(BaseDefendPos);
		//BaseDefendPos.x += rand() % 3 - 1;
		//BaseDefendPos.y += rand() % 3 - 1;

		//PassWay.x = (Base->pos.x + EnemyBase->pos.x)/2;
		//PassWay.y = (Base->pos.y + EnemyBase->pos.y)/2;
		//PassWay.z = 1;

		for (int i = 0; i < INFO->element_num; ++i) {
			const State *p = INFO->elements[i];
			if (p->type == OILFIELD) {
				oilMap[p->index] = oilNum++;
				oilLeft.push_back(1000);
			}
			if (p->type == MINE) {
				mineMap[p->index] = mineNum++;
				mineLeft.push_back(500);
			}
		}

		Produce(DESTROYER);
		Produce(DESTROYER);
		Produce(DESTROYER);

		BEGINGAME = 1;
	}
	/*if(BEGINGAME == 1)
		BEGINGAME == 2;*/
	command = new int[INFO->element_num][2];    //命令列表初始化
	for(int i = 0; i<INFO->element_num; i++)
		for(int j = 0; j < 2; j++)
			command[i][j] = -1; 
	enemy_init();

	for (int i = 0; i < INFO->element_num; ++i) {
		const State *p = INFO->elements[i];
		if (p->type == OILFIELD && p->visible) {
			oilLeft[oilMap[p->index]] = p->fuel;
		}
		if (p->type == MINE && p->visible) {
			mineLeft[mineMap[p->index]] = p->metal;
		}
	}

	if (INFO->round > 15) 
    	baseDenfendNum = 1;
    if (INFO->round > 30)
    	baseDenfendNum = 2;
    if (INFO->round > 50)
    	baseDenfendNum = 3;

  	/*if (INFO->round > 15)
  		baseFighter = 1;
  	if (INFO->round > 30)
  		baseFighter = 2;
  	if (INFO->round > 50)
  		baseFighter = 3;*/

    int enemyFighterNum[2] = {0, 0};
    int myBase = GetBase(INFO->team_num);
    for (int i = 0; i < enemy_num; ++i) {
    	const State p = enemy_element[i];
    	if (p.type != FIGHTER)
    		continue;
    	if (DisToBase(p.pos, INFO->elements[myBase]->pos) <= 30)
    		++enemyFighterNum[1];
    	if (DisToBase(p.pos, INFO->elements[myBase]->pos) <= 10)
    		++enemyFighterNum[0];
    }
    if (enemyFighterNum[1] > 5) {
    	baseDenfendNum = min(enemyFighterNum[1], 6);
    	baseFighterNum = 2;
    }
    if (enemyFighterNum[0] > 5) {
    	//Produce(FIGHTER);
    	Produce(DESTROYER);
    }

    if (INFO->round > 30)
    	baseFuelCargoNum = 2;
	
	baseSubmarineNum = 0;
    if (INFO->round > 15)
    	baseSubmarineNum = 1;
    if (INFO->round > 30)
    	baseSubmarineNum = 2;
    if (INFO->round > 50)
    	baseSubmarineNum = 3;
    if (INFO->round > 80)
    	baseSubmarineNum = 4;

	//baseSubmarineNum = min(INFO->round / 10 + 1, 6);
	
}

//根据Info()初始化当时的敌人
void enemy_init()
{
	//敌方单位构建包括敌方单位和无主据点，不包括矿场和油田
	enemy_num = 0;
	for(int i=0;i<Info()->element_num;i++)
		if(Info()->elements[i]->team != Info()->team_num
			&& Info()->elements[i]->type != MINE && Info()->elements[i]->type != OILFIELD)
		{
			enemy_num += 1;
			Enemy_Indexes.push_back(i);
		}	
	enemy_element = new State[enemy_num];
	for(int i=0; i<enemy_num; i++)
		{
			enemy_element[i] = *(Info()->elements[Enemy_Indexes[i]]);
		}
	Enemy_Indexes.clear();
}

//找到pos最近的不临大陆的地方 // waiting for modify??
Position findOcean(Position pos)
{
	Position sea;
	if(AwayToLand(pos))
		return pos;
	for(int i = 0; i < 9; i++)
	{
		sea.x = pos.x + i % 3;
		sea.y = pos.y + i / 3;
		sea.z = pos.z;
		if(AwayToLand(sea))
			return sea;
	}
	return pos;
}

//判断某个位置是不不临大陆
int AwayToLand(Position pos)
{
	return 1-(Map(pos.x-1, pos.y-1)||Map(pos.x-1, pos.y)||Map(pos.x-1, pos.y+1)||Map(pos.x, pos.y-1)
			||Map(pos.x, pos.y)||Map(pos.x, pos.y+1)||Map(pos.x+1, pos.y-1)||Map(pos.x+1, pos.y)||Map(pos.x+1, pos.y+1));
}
/////该补充燃料或者弹药的时候，或者血量太少需要回基地维修
void Supply_Repair(int i)
{
	const State *Element = INFO->elements[i];
	if(Element->health < parameter[0] * kProperty[Element->type].health_max
		&& Element->health != -1
		&& if_command(i, RETURNBASE))
	{
		moveToBase(Element, INFO->elements[GetBase(INFO->team_num)]);
		//ChangeDest(Element->index, INFO->elements[GetBase(INFO->team_num)]->pos);
		command[i][1] = RETURNBASE;
		return;
	}
	else if(Element->type == BASE)
	{
		if(Element->metal < 0.3 * kProperty[BASE].metal_max && if_alive(baseMineCargo))
		{
			if(getState(baseMineCargo)->metal > (int)(0.4 * kProperty[CARGO].metal_max)) {
				moveToBase(getState(baseMineCargo), Element);
				//ChangeDest(baseMineCargo, Element->pos);
			}
			else
			{
				/*int mine = GetNear(Element->pos, MINE);
				if(mine < 0)
					return;*/
				ChangeDest(baseMineCargo, INFO->elements[GetNear(Element->pos, MINE)]->pos);
			}
			for(int index=0; index<INFO->element_num; index++)
				if(INFO->elements[index]->index == baseMineCargo)
				{
					command[index][1] = RETURNBASE;
					return;
				}
		}
		for (int i = 0; i < baseFuelCargoNum; ++i) {
			if(Element->fuel < 0.3 * kProperty[BASE].fuel_max && if_alive(baseFuelCargo[i]))
			{
				if(getState(baseFuelCargo[i])->fuel > 0.6 * kProperty[CARGO].fuel_max) {
					moveToBase(getState(baseFuelCargo[i]), Element);
					//ChangeDest(baseFuelCargo[i], Element->pos);
				}
				else
				{
					/*int fuel = GetNear(Element->pos, OILFIELD);
					if(fuel <= 0)
						return;*/
					ChangeDest(baseFuelCargo[i], INFO->elements[GetNear(Element->pos, OILFIELD)]->pos);
				}
				for(int index = 0; index < INFO->element_num; index++)
					if(INFO->elements[index]->index == baseFuelCargo[i])
					{
						command[index][1] = RETURNBASE;
						return;
					}
			}
		}
	}
	else if(Element->type == CARRIER && ((Element->ammo <  parameter[1]*kProperty[Element->type].ammo_max && Element->ammo != -1)
			|| (Element->fuel < distance(Element->pos, INFO->elements[GetBase(INFO->team_num)]->pos) + 9)))
	{
		//回基地
		int base_index = GetBase(INFO->team_num);
		if(DisToBase(Element->pos, INFO->elements[base_index]->pos) < DISTANCE_NEAR_BASE)
			if(if_command(i, RETURNBASE))
			{
				moveToBase(Element, INFO->elements[base_index]);
				//ChangeDest(INFO->elements[i]->index, INFO->elements[base_index]->pos);
				command[i][1] = RETURNBASE;	
				return;
			}

		int cargo_index = GetNear(Element->pos, CARGO);
		if(cargo_index >= 0  && cargo_index != baseMineCargo && cargo_index != baseFuelCargo[0] && cargo_index != baseFuelCargo[1])
		{
			if(if_command(cargo_index, CARGOSUPPLY))
			{
				ChangeDest(INFO->elements[cargo_index]->index, Element->pos);
				command[cargo_index][1] = CARGOSUPPLY;	
				return;
			}
			else
			{
				ChangeDest(Element->index, INFO->elements[cargo_index]->pos);
				command[i][1] = CARGOSUPPLY;
				return;
			}
		}
	}
	else if(Element->type > FORT 
			&& ((Element->ammo <  parameter[1]*kProperty[Element->type].ammo_max && Element->ammo != -1)
			|| (Element->fuel < distance(Element->pos, INFO->elements[GetBase(INFO->team_num)]->pos) + 9))
			&& Element->type != CARGO)        //燃料不足
	{
		//回基地
		int base_index = GetBase(INFO->team_num);
		if(DisToBase(Element->pos, INFO->elements[base_index]->pos) < DISTANCE_NEAR_BASE)
			if(if_command(i, RETURNBASE))
			{
				moveToBase(Element, INFO->elements[base_index]);
				//ChangeDest(INFO->elements[i]->index, INFO->elements[base_index]->pos);
				command[i][1] = RETURNBASE;	
				return;
			}

		int cargo_index = GetNear(Element->pos, CARGO);
		int carrier_index = GetNear(Element->pos, CARRIER);
		int index;

		if (Element->type == FIGHTER || Element->type == SCOUT)
			cargo_index = -1;

		if(cargo_index <= 0 && carrier_index <= 0)return; 
		else if(carrier_index <= 0) index = cargo_index;
		else if(cargo_index <= 0) index = carrier_index;
		else 
			index = distance(Element->pos, INFO->elements[cargo_index]->pos) > distance(Element->pos, INFO->elements[carrier_index]->pos) ? carrier_index : cargo_index;

		if(//distance(Element->pos, INFO->elements[cargo_index]->pos) > distance(Element->pos, INFO->elements[index]->pos) &&
			(INFO->elements[index]->ammo > kProperty[Element->type].ammo_max - Element->ammo
				|| INFO->elements[index]->fuel > 0.7 * kProperty[Element->type].fuel_max - Element->fuel)
			&& INFO->elements[index]->index != baseMineCargo 
			&& INFO->elements[index]->index != baseFuelCargo[0] 
			&& INFO->elements[index]->index != baseFuelCargo[1])
		{
			if(cargo_index == index && if_command(cargo_index, CARGOSUPPLY))
			{
				ChangeDest(INFO->elements[index]->index, Element->pos);
				command[index][1] = CARGOSUPPLY;	
				return;
			}
			else
			{
				ChangeDest(Element->index, INFO->elements[index]->pos);
				command[i][1] = CARGOSUPPLY;
				return;
			}
		}
	}
	//else if(Element->type > FORT 
	//		&& Element->ammo <  parameter[1]*kProperty[Element->type].ammo_max
	//		&& Element->ammo != -1)
	//{
	//
	//	int cargo_index = GetNear(Element->pos, CARGO);
	//	if(if_command(i, CARGOSUPPLY)
	//		&& DisToBase(Element->pos, INFO->elements[GetBase(1 - INFO->team_num)]->pos) < kProperty[BASE].fire_ranges[kProperty[Element->type].level])
	//	{
	//		Position Pos =  minus(Element->pos, INFO->elements[GetBase(1 - INFO->team_num)]->pos, 
	//							kProperty[BASE].fire_ranges[kProperty[Element->type].level]+1);
	//		ChangeDest(Element->index, Pos);
	//		command[i][1] = CARGOSUPPLY;
	//		if(if_command(cargo_index, CARGOSUPPLY))
	//		{
	//			ChangeDest(INFO->elements[cargo_index]->index, Pos);
	//			command[cargo_index][1] = CARGOSUPPLY;	
	//			return;
	//		}
	//	}

	//	if(cargo_index >= 0  && cargo_index != baseMineCargo && cargo_index != baseFuelCargo)
	//	{
	//		if(if_command(cargo_index, CARGOSUPPLY))
	//		{
	//			ChangeDest(INFO->elements[cargo_index]->index, Element->pos);
	//			command[cargo_index][1] = CARGOSUPPLY;	
	//			return;
	//		}
	//		else
	//		{
	//			ChangeDest(Element->index, INFO->elements[cargo_index]->pos);
	//			command[i][1] = CARGOSUPPLY;
	//			return;
	//		}
	//	}
	//}
	else;
}

//返回pos1距pos2恰好相距fire_range的一个位置
Position minus(Position pos1,Position pos2,int fire_range)
{
	Position target;
	target.x = pos1.x > pos2.x ? pos1.x+(fire_range - distance(pos1,pos2))/2 : pos1.x -(fire_range - distance(pos1,pos2))/2;
	target.y = pos1.y > pos2.y ? pos1.y+((fire_range - distance(pos1,pos2))+1)/2 : pos1.y -((fire_range - distance(pos1,pos2))+1)/2;
	target.z = pos1.z;
	return target;

}

//判断某个数是否在数组中
int if_in(int i, int *a, int len)
{
	for(int j = 0; j < len; j++)
		if(i == a[j])
			return 1;
	return 0;
}

//获得己方或对方基地的索引
int GetBase(int team)
{
	for(int i = 0; i < INFO->element_num; i++)
		if(INFO->elements[i]->type == BASE && INFO->elements[i]->team == team)
			return i;
	return -1;
}

int changePosToNumber(Position p) {
	return p.x * INFO->y_max + p.y;
}

//两点之间的距离
int distance(Position pos1, Position pos2)
{
	return abs(pos1.x - pos2.x) + abs(pos1.y - pos2.y);
}

const int BASE_DELTA_X[12] = {-2, -2, -2, 2, 2, 2, -1, 0, 1, -1, 0, 1};
const int BASE_DELTA_Y[12] = {-1, 0, 1, -1, 0, 1, -2, -2, -2, 2, 2, 2};

//返回到基地的距离
int DisToBase(Position pos1, Position pos2)  //pos2表示基地中心
{
	/*int min = 1000;
	if(pos1.x < pos2.x)//分为九块
	{
		if(pos1.y > pos2.y)
			return abs(pos1.x - pos2.x) + abs(pos1.y - pos2.y);
		else if(pos1.y < pos2.y - 2)
			return abs(pos1.x - pos2.x) + abs(pos1.y - pos2.y + 2);
		else 
			return abs(pos1.x - pos2.x);
	}
	else if(pos1.x > pos2.x +2)
	{
		if(pos1.y > pos2.y)
			return abs(pos1.x - pos2.x - 2) + abs(pos1.y - pos2.y);
		else if(pos1.y < pos2.y - 2)
			return abs(pos1.x - pos2.x - 2) + abs(pos1.y - pos2.y + 2);
		else 
			return abs(pos1.x - pos2.x - 2);
	}
	else
	{
		if(pos1.y > pos2.y)
			return abs(pos1.y - pos2.y);
		else if(pos1.y < pos2.y -2)
			return abs(pos1.y - pos2.y +2);
		else 
			return 0;
	}*/
	int minDis = 1000;
	Position curPos;
	/*for (int i = 0; i < 12; ++i) {
		curPos.x = pos2.x + BASE_DELTA_X[i];
		curPos.y = pos2.y + BASE_DELTA_Y[i];
		if (curPos.x < 0 || curPos.x > INFO->x_max || curPos.y < 0 || curPos.y > INFO->y_max)
			continue;
		if (distance(pos1, curPos) < minDis)
			minDis = distance(pos1, curPos);
	}*/
	for (int i = -1; i <= 1; ++i)
		for (int j = -1; j <= 1; ++j) {
			curPos.x = pos2.x + i;
			curPos.y = pos2.y + j;
			if (curPos.x < 0 || curPos.x > INFO->x_max || curPos.y < 0 || curPos.y > INFO->y_max)
				continue;
			if (distance(pos1, curPos) < minDis)
				minDis = distance(pos1, curPos);
		}
	return minDis;
}

//判断某单位是否仍存活
int if_alive(int operand)
{
	for(int i = 0; i < INFO->element_num; i++)
		if(INFO->elements[i]->index == operand)
			return 1;
	return 0;
}

//最大值
int max(int x, int y)
{
	return x>y?x:y;
}

//最小值
int min(int x, int y)
{
	return x<y?x:y;
}

//获得距该位置最近的某类型单位的索引
int GetNear(Position pos, ElementType type)
{
	int i,near_index = -1,min_distance = 1000;
	for(i = 0; i < INFO->element_num; i++)
	{
		const State *target = INFO->elements[i];
		if(target->type == type &&  distance(target->pos, pos)<min_distance)
		{
			if( type == OILFIELD && 
				distance(target->pos, INFO->elements[GetBase(1 - INFO->team_num)]->pos) > kProperty[BASE].sight_ranges[SURFACE]
				&& oilLeft[oilMap[target->index]] > 0 ) //油田还有石油
			{
				near_index = i;
				min_distance = distance(target->pos, pos);
			}
			else if(type == MINE  && 
				distance(target->pos, INFO->elements[GetBase(1 - INFO->team_num)]->pos) > kProperty[BASE].sight_ranges[SURFACE]
				&& mineLeft[mineMap[target->index]] > 0 )//保证矿场还有金属
			{
				near_index = i;
				min_distance = distance(target->pos, pos);
			}
			else if(target->team == INFO->team_num && type != OILFIELD && type!=MINE)
			{
				near_index = i;
				min_distance = distance(target->pos, pos);
			}
			else ;
		}
	}
	return near_index;
}

//判断对该成员的该命令是否可以下达（如果已经下达更高级的指令则不可）
int if_command(int i, CommandType type, ElementType target)
{
	const State *element = INFO->elements[i];
	if(type < FORWARD && type > command[i][0])  //非移动指令
	{
		if(type == PRODUCE || type == FIX)     //生产和维修操作
		{
			if(element->type == BASE) return 1;
			return 0;
		}
		else if(type <= SUPPLYUNIT)      //补给操作
		{
			if(element->type >= FIGHTER 
				|| element->type == DESTROYER 
				|| element->type == SUBMARINE
				|| target == CARGO)
				return 0;
			return 1;
		}
		else if(target == kElementTypes)
			return 1;
		else if((kProperty[element->type].attacks[0] <= kProperty[target].defences[0] 
					|| kProperty[(int)target].defences[0] == -1)
			&& (kProperty[element->type].attacks[1] <= kProperty[target].defences[1]
					|| kProperty[(int)target].defences[1] == -1))
			return 0;
		else ;
		return 1;
	}
	if(type >= FORWARD && type > command[i][1])  //移动指令
	{   
		if(element->type <= FORT)
			return 0;//基地和据点不能移动
		return 1;
	}
	return 0;
}


//对于回合开始时相距为1的单位进行补给
void Cargo_Supply(int index)
{
	const State *element, *target;
	element = INFO->elements[index];
	if(!if_command(index, SUPPLYUNIT))
		return;
	for(int j = 0; j < INFO->element_num; j++)
	{
		target = INFO->elements[j];
		if(target->type == BASE && DisToBase(element->pos, target->pos) <= 1
			&& target->team == INFO->team_num)
		{
			/*if((target->fuel + 0.5*element->fuel < kProperty[BASE].fuel_max
				&& element->fuel >= 0.4*kProperty[element->type].fuel_max)
			|| (target->metal + 0.7*element->metal < kProperty[BASE].metal_max
				&& element->metal >= 0.4*kProperty[element->type].metal_max))*/
			//{
				//Supply(INFO->elements[index], target->index, 0, element->fuel, element->fuel);
				command[index][0] = SUPPLYBASE;
				Supply(element->index, target->index, max(element->fuel - 100, 0), 0, element->metal);
			//}
				if(element->index == baseMineCargo)
				{	
					/*int mine = GetNear(element->pos, MINE);
					if(mine < 0)
						return;*/
					ChangeDest(element->index, INFO->elements[GetNear(element->pos, MINE)]->pos);
					command[index][1] = CARGOSUPPLY;
					return;
				}

				if(element->index == baseFuelCargo[0] || element->index == baseFuelCargo[1])
				{
					ChangeDest(element->index, INFO->elements[GetNear(element->pos,OILFIELD)]->pos);
					command[index][1] = CARGOSUPPLY;
					return;
				}
				
				int MinFuel = 1000, UnitIndex = -1;
				for(int i=0; i<INFO->element_num; i++)
				{
					const State *unit = INFO->elements[i];
					if(unit->type > OILFIELD && unit->type != CARGO && unit->team == INFO->team_num
						&& unit->fuel < MinFuel)
					{
						MinFuel = unit->fuel;
						UnitIndex = i;
					}
				}
				if(UnitIndex != -1 && if_command(index, CARGOSUPPLY))
				{
					ChangeDest(element->index, INFO->elements[UnitIndex]->pos);
					command[index][1] = CARGOSUPPLY;
				}
				ChangeDest(element->index, INFO->elements[GetNear(element->pos, MINE)]->pos);
				command[index][1] = CARGOSUPPLY;
				return;
		}
		else if(target->type != CARGO
			&& target->type != FORT
			&& target->type != FIGHTER
			&& target->type != SCOUT
			&& distance(element->pos,target->pos) <= 1
			&& target->team == INFO->team_num)
		{
			int supplyFuel = 0;
			if (element->fuel >= 0.6 * kProperty[element->type].fuel_max
				&& target->fuel < 0.3 * kProperty[target->type].fuel_max) {
				supplyFuel = (int)(element->fuel * 0.3);
			}
			if (supplyFuel > 0
			|| (target->ammo < 0.3 * kProperty[target->type].ammo_max
				&& element->ammo >= 0.2 * kProperty[element->type].ammo_max)) {
				command[index][0] = SUPPLYUNIT;
				Supply(element->index, target->index, supplyFuel, element->ammo, 0);
				return;
			}
			
			/*if((target->fuel + 0.3*element->fuel < kProperty[target->type].fuel_max
				&& element->fuel >= 0.6*kProperty[element->type].fuel_max)
			|| (target->ammo + 0.3*element->ammo < kProperty[BASE].ammo_max
				&& element->ammo >= 0.3*kProperty[element->type].ammo_max))
			{
				command[index][0] = SUPPLYUNIT;
				Supply(INFO->elements[index]->index, INFO->elements[j]->index, kProperty[target->type].fuel_max - target->fuel,
						kProperty[target->type].ammo_max - target->ammo, 0);
				return;
			}*/
			//else if(target->type == FORT)command[index][0] = SUPPLYFORT;
			//else continue;
		}
	}	
}

void Carrier_Supply(int index)
{
	const State *element, *target;
	element = INFO->elements[index];
	if(!if_command(index, SUPPLYUNIT))
		return;
	for(int j = 0; j < INFO->element_num; j++)
	{
		target = INFO->elements[j];
		if(target->type > OILFIELD && target->type != CARGO &&
			distance(element->pos,target->pos) <= 1
			&& target->team == INFO->team_num)
		{
			int supplyFuel = 0;
			if (element->fuel >= 0.5 * kProperty[element->type].fuel_max
				&& target->fuel < 0.3 * kProperty[target->type].fuel_max) {
				supplyFuel = (int)(element->fuel * 0.3);
			}
			if (supplyFuel > 0 ||
				(target->ammo < 0.3 * kProperty[target->type].ammo_max
				&& element->ammo >= 0.4 * kProperty[element->type].ammo_max)) {
				command[index][0] = SUPPLYUNIT;
				Supply(INFO->elements[index]->index, INFO->elements[j]->index, supplyFuel, element->ammo / 2, 0);
				return;
			}
			/*if((target->fuel + 0.3*element->fuel < kProperty[target->type].fuel_max
				&& element->fuel >= 0.4*kProperty[element->type].fuel_max)
			|| (target->ammo + 0.3*element->ammo < kProperty[BASE].ammo_max
				&& element->ammo >= 0.3*kProperty[element->type].ammo_max))
			{
				command[index][0] = SUPPLYUNIT;
				Supply(INFO->elements[index]->index, INFO->elements[j]->index, kProperty[target->type].fuel_max - target->fuel,
						kProperty[target->type].ammo_max - target->ammo, 0);
				return;
			}
			//else if(target->type == FORT)command[index][0] = SUPPLYFORT;
			else continue;*/
		}		
	}
}

Position genPosition(int x, int y, int z) {
	Position p;
	p.x = x, p.y = y, p.z = z;
	return p;
}


void moveToBase(const State *s, const State *t) {
	if (s->type == FIGHTER || s->type == SCOUT) {
		ChangeDest(s->index, t->pos);
		return;
	}

	std::vector<Position> V;
	V.clear();
	for (int i = 0; i < 12; ++i) {
		int nx = t->pos.x + BASE_DELTA_X[i];
		int ny = t->pos.y + BASE_DELTA_Y[i];
		if (nx < 0 || ny < 0 || nx >= INFO->x_max || ny >= INFO->y_max || Map(nx, ny) == LAND)
			continue;
		V.push_back(genPosition(nx, ny, s->pos.z));
	}
	for (int i = 0; i < V.size(); ++i)
		for (int j = i + 1; j < V.size(); ++j)
			if (distance(s->pos, V[i]) > distance(s->pos, V[j]))
				std::swap(V[i], V[j]);
	if (V.size() > 3) {
		for (int i = 3; i < (int)V.size(); ++i) {
			V.erase(V.begin() + i);
			--i;
		}
	}
	int r = rand() % V.size();
	ChangeDest(s->index, V[r]);
}

//对于程序最后仍然没有命令的运输船，去资源点收集，或者回基地补给，或者呆在前线
void MoveCargo(int i)
{
	const State *Element = INFO->elements[i];
	const State *base = INFO->elements[GetBase(INFO->team_num)];
	if(Element->type == CARGO && if_command(i, RETURNBASE))
	{
		if(Element->fuel < kProperty[CARGO].fuel_max / 2)          //燃料不足
		{
			int fuel = GetNear(Element->pos,OILFIELD);
			if(fuel < 0)
				return;
			ChangeDest(Element->index, INFO->elements[fuel]->pos);
			command[i][1] = CARGOSUPPLY;
			return;
		}
		else if(Element->metal < 0.5 * kProperty[CARGO].metal_max)     //金属不足
		{	
			if(Element->index == baseFuelCargo[0] || Element->index == baseFuelCargo[1])
			{
				if(Element->fuel > 0.5 * kProperty[CARGO].fuel_max)
				{
					int mine = GetNear(Element->pos, MINE);
					if(mine < 0)
						return;
					ChangeDest(Element->index, INFO->elements[mine]->pos);
					command[i][1] = CARGOSUPPLY;
					return;
				}
				else
				{
					int fuel = GetNear(Element->pos, OILFIELD);
					if(fuel < 0)
						return;
					ChangeDest(Element->index, INFO->elements[fuel]->pos);
					command[i][1] = CARGOSUPPLY;
					return;
				}
			}
			int mine = GetNear(Element->pos, MINE);
			if(mine < 0)
				return;
			ChangeDest(Element->index, INFO->elements[mine]->pos);
			command[i][1] = CARGOSUPPLY;
			return;
		}
		else if((Element->index == baseMineCargo || Element->index == baseFuelCargo[0] || Element->index == baseFuelCargo[1])
			&& ((base->metal + 0.8*Element->metal < kProperty[BASE].metal_max
				&& Element->metal > 0.4*kProperty[Element->type].metal_max)
			|| (base->fuel + (int)(0.7*Element->fuel) < kProperty[BASE].fuel_max
				&& Element->fuel >= kProperty[Element->type].fuel_max / 2)))
		{
			moveToBase(Element, base);
			//ChangeDest(Element->index, base->pos);
			command[i][1] = CARGOSUPPLY;
			return;
		}
		else ;
	}

	if(if_command(i, FORWARD))                   //是补给前线的船
	{
        int MinAmmo = 1000;
        Position target;
        for(int j = 0; j < INFO->element_num; j++)
			if(INFO->elements[j]->ammo != -1
				&& INFO->elements[j]->ammo < MinAmmo 
                && INFO->elements[j]->type != CARGO
                && INFO->elements[j]->team == INFO->team_num
                && INFO->elements[j]->type != FIGHTER
                && INFO->elements[j]->type != SCOUT)
            {
                MinAmmo = INFO->elements[j]->ammo;
                target = INFO->elements[j]->pos;
            }
        if(MinAmmo != 1000)
        {
            ChangeDest(Element->index, target);   
            command[i][1] = FORWARD;
			return;
        }
    }
}

//寻找射程内的基地或据点，或选择血量最少的单位攻击
//如果是基地、据点；或者攻击范围小于敌人就向前走
//如果攻击范围大于敌人就向后走
void Attack(int index)
{
	const State	*Element = INFO->elements[index];
	int health = 1000, enemy_index = -1;
	int FORTindex = -1;
	if(!if_command(index,ATTACKBASE))
		return;
	State *enemyBase = NULL;
	for(int i = 0; i < enemy_num; i++)
	{
		State *enemy = &enemy_element[i];
		if(enemy->type == BASE && enemy->team == 1 - INFO->team_num
			&& DisToBase(Element->pos, enemy->pos) <= kProperty[Element->type].fire_ranges[SURFACE]
			&& if_command(index, ATTACKBASE, BASE))
		{												//敌方基地
			enemyBase = enemy;
			/*AttackUnit(Element->index,enemy->index);
			command[index][0] = ATTACKBASE;
			if(if_command(index, FORWARD))
			{
				ChangeDest(Element->index, minus(Element->pos,enemy->pos,
					kProperty[Element->type].fire_ranges[kProperty[enemy->type].level]));
				command[index][1] = FORWARD;
			}
			return;*/
		}
		if(distance(Element->pos, enemy->pos) <= kProperty[Element->type].fire_ranges[kProperty[enemy->type].level])
		{	
			if(enemy->type == FORT
				&& enemy->team == 1 - INFO->team_num
				&& if_command(index, ATTACKFORT, FORT))                        //据点
			{
				AttackUnit(Element->index, enemy->index);
				command[index][0] = ATTACKFORT;
				if(if_command(index,FORWARD))
				{
					ChangeDest(Element->index, enemy->pos);
					command[index][1] = FORWARD;
				}
				return;
			}
			if(enemy->type == FORT 
				&& enemy->team != 1 - INFO->team_num
				&& if_command(index,ATTACKFORT,FORT))
			{
				FORTindex = i;
			}
			if(enemy->type != FORT
				&&health > enemy->health
				&& if_command(index, ATTACKUNIT, ElementType(enemy->type)))
			{
				health = enemy->health;					//寻找血量最少的敌方单位
				enemy_index = i;
			}
		}
	}
	if(enemy_index != -1)
	{
		AttackUnit(Element->index, enemy_element[enemy_index].index);
		command[index][0] = ATTACKUNIT;
		if(if_command(index, FORWARD)
			&& !if_in(INFO->elements[index]->index, baseDefender, baseDenfendNum))
		{
			//射程远于敌方
			if(kProperty[Element->type].fire_ranges[kProperty[enemy_element[enemy_index].type].level]
				> kProperty[enemy_element[enemy_index].type].fire_ranges[kProperty[Element->type].level])
				ChangeDest(Element->index, minus(Element->pos,enemy_element[enemy_index].pos,
					kProperty[Element->type].fire_ranges[kProperty[enemy_element[enemy_index].type].level]));
			//射程近于敌方
			else 
				ChangeDest(Element->index, enemy_element[enemy_index].pos);
			command[index][1] = FORWARD;
		}
		else if(if_command(index,FORWARD)
			&& if_in(INFO->elements[index]->index, baseDefender, baseDenfendNum))
		{
			ChangeDest(Element->index, BaseDefendPos + randPos());
			command[index][1] = FORWARD;
		}
		return;
	}

	if (enemyBase != NULL) {
		AttackUnit(Element->index,enemyBase->index);
		command[index][0] = ATTACKBASE;
		if(if_command(index, FORWARD))
		{
			ChangeDest(Element->index, minus(Element->pos,enemyBase->pos,
				kProperty[Element->type].fire_ranges[kProperty[enemyBase->type].level]));
			command[index][1] = FORWARD;
		}
		return;
	}

	if(FORTindex != -1)
	{
		AttackUnit(Element->index, enemy_element[FORTindex].index);
		command[index][0] = ATTACKUNIT;
		if(if_command(index,FORWARD))
		{
			if(!if_in(INFO->elements[index]->index, baseDefender, baseDenfendNum))
				ChangeDest(Element->index, enemy_element[FORTindex].pos);
			else
				ChangeDest(Element->index, BaseDefendPos + randPos());
			command[index][1] = FORWARD;
		}
	}
	
}

//基地的维修，生产
void BaseAct(int index)
{
	const State *target;
	if(baseMineCargo != -1 && !if_alive(baseMineCargo))
	{
		Produce(CARGO);
		command[index][0] = PRODUCE;
		baseMineCargo = -1;
		//return;
	}
	for (int i = 0; i < baseFuelCargoNum; ++i) {
		if(baseFuelCargo[i] != -1 && !if_alive(baseFuelCargo[i]))
		{
			Produce(CARGO);
			command[index][0] = PRODUCE;
			baseFuelCargo[i] = -1;
			//return;
		}
	}

	//FIX
	for(int i = 0; i < INFO->element_num; i++)
	{
		target = INFO->elements[i];
		if(DisToBase(target->pos, INFO->elements[index]->pos) <= 1 
			&& target->index != INFO->elements[index]->index
			&& target->team == INFO->team_num)
		{
			if(target->health < 0.4*kProperty[target->type].health_max
				&& target->health != -1)
			{
				if(if_command(index, FIX))
				{	
					Fix(INFO->elements[index]->index, INFO->elements[i]->index);
					command[index][0] = FIX;
					return;
				}
			}
		}
	}

	Attack(index);
	//SUPPLY
	for(int i = 0; i < INFO->element_num; i++)
	{
		target = INFO->elements[i];
		if(DisToBase(target->pos, INFO->elements[index]->pos) <= 1 
			&& target->index != INFO->elements[index]->index
			//&& target->type != CARGO
			&& target->team == INFO->team_num)
		{
			int supplyFuel = kProperty[target->type].fuel_max;
			if (target->type == CARGO)
				supplyFuel /= 3;
			if(target->fuel < parameter[2]*kProperty[target->type].fuel_max
					|| target->ammo < parameter[3]*kProperty[target->type].ammo_max)
			{
				if(if_command(index, SUPPLYUNIT))
				{	
					Supply(INFO->elements[index]->index, INFO->elements[i]->index, max(supplyFuel - target->fuel, 0), 
                                                        kProperty[target->type].ammo_max-target->ammo, 0);
					command[index][0] = SUPPLYUNIT;
					return;
				}
			}
		}
	}
	BaseProduce(index);
}

///基地的生产
void BaseProduce(int index)
{
	if(!if_command(index, PRODUCE))return;

	for (int i = 0; i < baseFighterNum; ++i)
		if (baseFighter[i] != -1 && !if_alive(baseFighter[i])) {
			Produce(FIGHTER);
			command[index][0] = PRODUCE;
			baseFighter[i] = -1;
		}

    for(int i = 0; i < baseDenfendNum; i++)
		if(baseDefender[i] != -1 && !if_alive(baseDefender[i]))
		{
			Produce(DESTROYER);
			command[index][0] = PRODUCE;
			baseDefender[i] = -1;
			//return;
		}
	

	for (int i = 0; i < baseSubmarineNum; ++i)
		if (baseSubmarine[i] != -1 && !if_alive(baseSubmarine[i])) {
			Produce(SUBMARINE);
			command[index][0] = PRODUCE;
			baseSubmarine[i] = -1;
		}

	if (ProduceState == 0) {
		Produce(DESTROYER);
		Produce(DESTROYER);
		Produce(SUBMARINE);
		//Produce(DESTROYER);
		command[index][0] = PRODUCE;
	}

	if (myBaseFuel > 400) {
		Produce(CARRIER);
	}else if (myBaseFuel > 300) {
		if ((rand() & 1) || INFO->round > 40)
			Produce(DESTROYER);
		else
			Produce(SUBMARINE);
		command[index][0] = PRODUCE;
	}

	ProduceState += 1;
	//Produce(SUBMARINE);
	
	if(ProduceState % 5 == 0)
	{
		if (ProduceState < 16)
			return;
		Produce(SUBMARINE);
		command[index][0] = PRODUCE;
		return;
	}
	else if(ProduceState % 5 == 1)
	{
		if (ProduceState < 5)
			return;
		Produce(FIGHTER);
		command[index][0] = PRODUCE;
		return;
	}
	else if(ProduceState % 5 == 2 || ProduceState % 10 == 3 || ProduceState % 20 == 18)
	{
		Produce(DESTROYER);
		command[index][0] = PRODUCE;
		return;
	}
	else if(ProduceState % 10 == 3)
	{
		//Produce(SCOUT);
		//command[index][0] = PRODUCE;
		return;
	}
	else if(ProduceState % 20 == 8)
	{
		Produce(CARGO);
		command[index][0] = PRODUCE;
		return;
	}
	else if(ProduceState % 5 == 4)
	{
		Produce(CARRIER);
		command[index][0] = PRODUCE;
		return;
	}
	else;
}

void DefineUnit() //flag = 0,1,2对应baseMineCargo, baseFuelCargo, baseDefender
 //i对应baseDefender的标号
{
    const State *Element, *base; 
    base = INFO->elements[GetBase(INFO->team_num)];

	for(int i = 0; i < baseDenfendNum; i++)
		if(baseDefender[i] <= 0 || !if_alive(baseDefender[i]))
		{
			int Distance = 1000, index = -1;
			for(int j = 0; j < INFO->element_num; j++)
			{
				Element = INFO->elements[j];
				if(Element->team == INFO->team_num && Distance > distance(Element->pos, base->pos) 
					&& Element->type == DESTROYER && !if_in(Element->index, baseDefender, baseDenfendNum))
				{
					Distance = distance(Element->pos, base->pos);
					index = INFO->elements[j]->index;
				}
			 }
			if(Distance != 1000)
				baseDefender[i] = index;
		}

	for (int i = 0; i < baseSubmarineNum; ++i)
		if (baseSubmarine[i] <= 0 || !if_alive(baseSubmarine[i])) {
			int Distance = 1000, index = -1;
			for (int j = 0; j < INFO->element_num; ++j) {
				Element = INFO->elements[j];
				if (Element->team == INFO->team_num && Distance > distance(Element->pos, base->pos)
					&& Element->type == SUBMARINE && !if_in(Element->index, baseSubmarine, baseSubmarineNum))
				{
					Distance = distance(Element->pos, base->pos);
					index = INFO->elements[j]->index;
				}
			}
			if (Distance != 1000)
				baseSubmarine[i] = index;
		}

	for (int i = 0; i < baseFighterNum; ++i)
		if (baseFighter[i] <= 0 || !if_alive(baseFighter[i])) {
			int Distance = 1000, index = -1;
			for (int j = 0; j < INFO->element_num; ++j) {
				Element = INFO->elements[j];
				if (Element->team == INFO->team_num && Distance > distance(Element->pos, base->pos)
					&& Element->type == FIGHTER && !if_in(Element->index, baseFighter, baseFighterNum))
				{
					Distance = distance(Element->pos, base->pos);
					index = INFO->elements[j]->index;
				}
			}
			if (Distance != 1000)
				baseFighter[i] = index;
		}

	if(baseMineCargo <= 0 || !if_alive(baseMineCargo))
	{
		int Distance = 1000, index = -1;
		for(int j=0; j<INFO->element_num; j++)
		{
			Element = INFO->elements[j];
			if(Element->team == INFO->team_num && Element->type == CARGO 
				&& Distance > distance(Element->pos, base->pos) && Element->index != baseFuelCargo[0] && Element->index != baseFuelCargo[1])
			{
				Distance = distance(Element->pos, base->pos);
				index = INFO->elements[j]->index;
			}
		}
		if(Distance != 1000)
			baseMineCargo = index;
	}
	for (int i = 0; i < baseFuelCargoNum; ++i) {
		if(baseFuelCargo[i] <= 0 || !if_alive(baseFuelCargo[i]))
		{
			int Distance = 1000, index = -1;
			for(int j = 0; j < INFO->element_num; j++)
			{
				Element = INFO->elements[j];
				if(Element->team == INFO->team_num && Element->type == CARGO
					&& Distance > distance(Element->pos, base->pos) && Element->index != baseMineCargo)
				{
					Distance = distance(Element->pos, base->pos);
					index = INFO->elements[j]->index;
				}
				}
			if(Distance != 1000)
				baseFuelCargo[i] = index;
		}
	}
}
////寻找最近的敌军前进，如果是运输船就打
void Forward(int index)
{
	const State *Element = INFO->elements[index];
	int Distance=1000; 
	Position target;
	if(!if_command(index, FORWARD))return;
	if(Element->type == BASE || Element->type == FORT 
        || Element->type == CARGO)
		return;
	else if(if_in(INFO->elements[index]->index, baseDefender, baseDenfendNum)
		&& if_command(index, FORWARD))
	{
		ChangeDest(Element->index, BaseDefendPos + randPos());
		command[index][1] = FORWARD;
		return;
	}else if (if_in(INFO->elements[index]->index, baseSubmarine, baseSubmarineNum)
		&& if_command(index, FORWARD))
	{
		ChangeDest(Element->index, BaseDefendPos);
		command[index][1] = FORWARD;
		return;
	}else if (if_in(INFO->elements[index]->index, baseFighter, baseFighterNum)
		&& if_command(index, FORWARD))
	{
		ChangeDest(Element->index, INFO->elements[GetBase(INFO->team_num)]->pos);
		command[index][1] = FORWARD;
		return;
	}

	else
	{
		for(int i = 0; i < enemy_num; i++) {
			if (INFO->round <= 30 && enemy_element[i].type == BASE)
				continue;
			if(distance(Element->pos, enemy_element[i].pos) < Distance
				&& if_command(index, ATTACKUNIT, ElementType(enemy_element[i].type)))
			{
				Distance = distance(Element->pos,enemy_element[i].pos);
				target = enemy_element[i].pos;
			}
		}
	}
	if(Distance != 1000)
	{
		ChangeDest(Element->index, target);
		command[index][1] = FORWARD;
		return;
	}
	
	Position enemyBase = INFO->elements[GetBase(1-INFO->team_num)]->pos;
	ChangeDest(Element->index, enemyBase);
	command[index][1] = FORWARD;
}
