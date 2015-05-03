#include <cstdlib>
//This is written by haldak.
#include "basic.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdlib>

using namespace teamstyle16;
using std::vector;

const char * GetTeamName()
{
    return "We do have allodoxaphobia!";  // Name of your team
}

const GameInfo *INFO = Info();//获取Game Info

struct Help
{
	int index;
	int helper;
	Help(int index):index(index), helper(0){}
	bool operator == (const Help& b)
	{
		if (b.index == index)
			return true;
		return false;
	}
};

vector<Help> HelpList;

enum HELP //作战单位请求帮助
{
	help_fix,
	help_supply
};
struct need_help//将作战单位的类型，index和帮助指令（维修或者补给）传给Need_HELP数组
{
	ElementType type;
	int index;
	HELP help_command;
};
vector<need_help>Need_HELP;
int help_num;
//bool fix_sign;
//bool supply_sign;

struct BaseCommand//基地状态
{
	bool attack;
	int attack_index;
	bool fix;
	int fix_index;
	bool supply;
	int supply_index;
}BaseC;

struct Source
{
	int index;
	int source;
	Source(int index):index(index), source(10000){}
};

vector<Source> SourceList;//记录各资源点的资源量

void Quick_Attack();//女神的快攻！（Written by CYY）
void start_game();//开局设置
void remove_v(vector<int>& v,int val);//删除vector<int>v中值为val的元素
void attack_target();//操作攻击序列攻击指定目标
int hurt_by_distance( ElementType typeA , ElementType typeB , int distance, int def = 1);//def:是否计算防御
void base_defend();//基地防御
void base_fix();//基地维修函数
void resort(int &type,vector<int>&fix_type);//维修的同级优化
void NeedSupply(int index);//标记index需要运输舰补给
void scout_movement();//scout逻辑
void fighter_movement();//fighter逻辑
vector<int> cargo_movement(vector<int>Cargo_last,int last_num);//运输舰逻辑，并不会补给军队
void destroyer_movement();//destroyer逻辑
void submarine_movement();//submarine逻辑
void carrier_movement();//carrier逻辑
void fort_defend();//据点防御函数
void fort_supply();//据点补给（油）
void production();//统筹生产函数
void ChangeBase(int oprend,int base);//移动效率最大化函数
void ChangeDefend(int oprend,int target);//防御点计算并移动函数
void distance_to_go(int target,int distance,MapType type);//判断离某个单位距离为distance的地图

int distance_cal(Position pos1, Position pos2);//计算直角距离
int distance(int index1,int index2);//按规则返回距离最小值
int GetNear(Position pos, ElementType type,int team=3);//获得最近的某元素位置
int GetFireRange(ElementType type);//返回火力范围0-水下，1-水面，2-天空
int if_alive(int operand);//判断元素是否存活
int a_near_b_near_c(int operand_index,ElementType eletype,int destination_index);//寻找a经b到c，使路径最短的b

int enemy_base_index;//敌方基地index
vector<int>Enemy_Indexes;//敌方index
void enemy_init();//根据Info()初始化当时的敌人
int enemy_num;//该回合敌人的总数
State *enemy_element;//每个敌人状态都存在这里

int our_base_index;//我方基地index
vector<int>Our_Indexes;//我方index
void our_init();//根据Info()初始化我方状况
int our_num;//我方单位的总数
State *our_element;//我方单位的状态存在这里

vector<int>Cargo_Temp;//cargo上回合序列
int cargo_temp_num=0;//cargo上回合数量

vector<int>Defend_Index;//防御单位列表
vector<int>Defend_Fighter;//防御单位列表
vector<Position>Defend_Go;//防御单位移动目的地
int def_go_num=0;//打算移到相应目的地的防御单位数量

int turn=1;//开局条件

vector<int>Target_Indexes;//cargo打算采资源的地点index，避免一堆cargo跑到同一个地方
int target_num;//Target_Indexes的元素数量
int pos_time;
vector<int>Attack_Indexes;//攻击序列
int attack_num;//攻击序列元素数量
bool base_ask_for_help;//基地求援符号
int base_help_index;//基地指定的敌方目标index
int enemy_count;//危险的敌人数目
bool find_submarine;//基地发现submarine符号
int submarine_target_index;//基地发现的对方submarine index
int fort;
vector<Position>Distance_Go;
int dis_go_num;

struct AttackCommand//作战单位已有攻击指令的结构体
{
	int index;
	bool attack_by_command;
};

//#define FIG_1//这个宏定义使得production函数里面先造飞机
//#define QA//这个宏定义使得开局打快攻，一波飞机轰敌方基地
#ifdef QA
const int kuaigong=2;//快攻生产所需回合数
#else
const int kuaigong=0;
#endif

void AIMain()//in while loop
{
	//回合初始化
	TryUpdate();
	enemy_init();
	our_init();

	//开局条件
#ifdef QA
	Quick_Attack();
#else
	if(turn==1)
	{
		start_game();
		turn+=1;
	}
#endif

	//回合执行
	base_defend();
	fort_supply();
	fort_defend();
	if(INFO->round > kuaigong+3)
		production();
	scout_movement();
	fighter_movement();
	destroyer_movement();
	submarine_movement();
	carrier_movement();
	attack_target();
	Cargo_Temp=cargo_movement(Cargo_Temp,cargo_temp_num);
	base_fix();

	//回合末尾
	delete []enemy_element;
	delete []our_element;
	Need_HELP.clear();
	Target_Indexes.clear();
	Attack_Indexes.clear();
	Distance_Go.clear();
	Defend_Index.clear();
}

//删除vector的元素
void remove_v(vector<int>& v,int val)
{
    v.erase(remove(v.begin(),v.end(),val),v.end());
}

//来自女神的快攻
void Quick_Attack()
{
	attack_num=0;
	//获取基地index
	int counter=0;
	for(int i=0;i<INFO->element_num;i++)
	{
		if( (INFO->elements[i]->type == BASE) && (INFO->elements[i]->team == INFO->team_num) )//我方基地index
		{
			our_base_index=INFO->elements[i]->index;
			counter+=1;
		}
		if( (INFO->elements[i]->type == BASE) && (INFO->elements[i]->team != INFO->team_num) )//敌方基地index
		{
			enemy_base_index=INFO->elements[i]->index;
			counter+=1;
		}
		if(counter>=2)
			break;
	}
	if(INFO->round == 0)
	{
		Produce(FIGHTER);
		Produce(FIGHTER);
		Produce(FIGHTER);
		Produce(FIGHTER);
		Produce(DESTROYER);
		Produce(FIGHTER);
		Produce(CARGO);
		Produce(CARGO);
		Produce(FIGHTER);
		Produce(FIGHTER);;
	}
}

//开局条件
void start_game()
{
	attack_num=0;
	//获取基地index及资源点情况
	for(int i=0;i<INFO->element_num;i++)
	{
		if( (INFO->elements[i]->type == BASE) && (INFO->elements[i]->team == INFO->team_num) )//我方基地index
		{
			our_base_index=INFO->elements[i]->index;
		}
		if( (INFO->elements[i]->type == BASE) && (INFO->elements[i]->team != INFO->team_num) )//敌方基地index
		{
			enemy_base_index=INFO->elements[i]->index;
		}
		if( INFO->elements[i]->type == MINE || INFO->elements[i]->type == OILFIELD)//资源点
		{
			SourceList.push_back(*new Source(INFO->elements[i]->index));
		}
	}

	for(int i=0;i<our_num;i++)
	{
		if(our_element[i].type == CARGO)
		{
			Cargo_Temp.push_back(our_element[i].index);
			cargo_temp_num+=1;
		}
	}
	//Produce(DESTROYER);
	//Produce(DESTROYER);
	//Produce(DESTROYER);
	//Produce(FIGHTER);
	Produce(FIGHTER);
	Produce(FIGHTER);
	Produce(FIGHTER);
	Produce(FIGHTER);
	Produce(SCOUT);
	Produce(CARGO);
	Produce(CARGO);
	Produce(CARGO);
	//Produce(CARGO);
}

//操作攻击序列，攻击指定目标
void attack_target()
{
	vector<int>destroyer_attack;
	int des_att_num=0;
	vector<int>fighter_attack;
	int fig_att_num=0;
	vector<int>submarine_attack;
	int sub_att_num=0;
	vector<int>carrier_attack;
	int car_att_num=0;
	vector<int>attack_base;
	int att_base_num=0;
	vector<int>attack_fort;
	int att_fort_num=0;

	vector<AttackCommand>AC;

	for(int i=0;i<attack_num;i++)
	{
		AttackCommand node={Attack_Indexes[i],false};
		AC.push_back(node);
		ElementType etype=GetState(Attack_Indexes[i])->type;
		if(etype == DESTROYER)
		{
			destroyer_attack.push_back(Attack_Indexes[i]);
			des_att_num+=1;
		}
		else if(etype == FIGHTER)
		{
			fighter_attack.push_back(Attack_Indexes[i]);
			fig_att_num+=1;
		}
		else if(etype == SUBMARINE)
		{
			submarine_attack.push_back(Attack_Indexes[i]);
			sub_att_num+=1;
		}
		else if(etype == CARRIER)
		{
			carrier_attack.push_back(Attack_Indexes[i]);
			car_att_num+=1;
		}
	}
	//不是攻击序列中的单位也可以攻击
	for(int i=0;i<help_num;i++)
	{
		if (GetState(Need_HELP[i].index)->type != CARGO)
		{
			AttackCommand node={Need_HELP[i].index,false};
			AC.push_back(node);
			attack_num++;
		}
	}
	
	for (unsigned int i = 0; i < HelpList.size(); i++)//确认已经被派去补给的船是否存活
	{
		if (if_alive(HelpList[i].index) && GetState(HelpList[i].index)->type != CARGO)
		{
			AttackCommand node={HelpList[i].index,false};
			AC.push_back(node);
			attack_num++;
		}
	}

	for(unsigned int i=0;i<Defend_Index.size();i++)
	{
		AttackCommand node={Defend_Index[i],false};
		AC.push_back(node);
		attack_num++;
	}

	//fighter,全部保家
	int count_fight = 0, fort_fight_num = 0;
	for(int i = 0; i < INFO->element_num; i++)
		if(INFO -> elements[i] -> team == INFO -> team_num && INFO -> elements[i] ->type == FIGHTER)
			count_fight++;
	/*
	if(count_fight > 5)
	{
		for(int i = 0; i < fig_att_num; i++)
		{
			attack_fort.push_back(fighter_attack[i]);
			att_fort_num += 1;
		}
	}
	*/
	if(count_fight == 7)
	{
		int fighter_temp = 0;
		if(fig_att_num == 1)
			fighter_temp = 1;
		else
			fighter_temp = 0;
		for(int i = 0; i < fighter_temp; i++)
		{
			attack_fort.push_back(fighter_attack[i]);
			att_fort_num += 1;
		}
	}
	else if(count_fight == 8)
	{
		int fighter_temp = 0;
		if(fig_att_num == 2)
			fighter_temp = 2;
		else if(fig_att_num == 1)
			fighter_temp = 1;
		else
			fighter_temp = 0;
		for(int i = 0; i < fighter_temp; i++)
		{
			attack_fort.push_back(fighter_attack[i]);
			att_fort_num += 1;
		}
	}
	else if(count_fight == 9)
	{
		int fighter_temp = 0;
		if(fighter_temp == 3)
			fighter_temp = 3;
		else if(fig_att_num == 2)
			fighter_temp = 2;
		else if(fig_att_num == 1)
			fighter_temp = 1;
		else
			fighter_temp = 0;
		for(int i = 0; i < fighter_temp; i++)
		{
			attack_fort.push_back(fighter_attack[i]);
			att_fort_num += 1;
		}
	}
	/*
	if(count_fight == 6)
	{
		int fighter_temp = 0;
		if(fig_att_num >= 1)
			fighter_temp = 1;
		else
			fighter_temp = 0;
		for(int i = 0; i < fighter_temp; i++)
		{
			attack_fort.push_back(fighter_attack[i]);
			att_fort_num += 1;
		}
	}
	else if(count_fight == 7)
	{
		int fighter_temp = 0;
		if(fig_att_num >= 2)
			fighter_temp = 2;
		else if(fig_att_num == 1)
			fighter_temp = 1;
		else
			fighter_temp = 0;
		for(int i = 0; i < fighter_temp; i++)
		{
			attack_fort.push_back(fighter_attack[i]);
			att_fort_num += 1;
		}
	}
	else if(count_fight == 8)
	{
		int fighter_temp = 0;
		if(fig_att_num >= 3)
			fighter_temp = 3;
		else if(fig_att_num == 2)
			fighter_temp = 2;
		else if(fig_att_num == 1)
			fighter_temp = 1;
		else
			fighter_temp = 0;
		for(int i = 0; i < fighter_temp; i++)
		{
			attack_fort.push_back(fighter_attack[i]);
			att_fort_num += 1;
		}
	}
	else if(count_fight == 9)
	{
		int fighter_temp = 0;
		if(fig_att_num >= 4)
			fighter_temp = 4;
		else if(fighter_temp == 3)
			fighter_temp = 3;
		else if(fig_att_num == 2)
			fighter_temp = 2;
		else if(fig_att_num == 1)
			fighter_temp = 1;
		else
			fighter_temp = 0;
		for(int i = 0; i < fighter_temp; i++)
		{
			attack_fort.push_back(fighter_attack[i]);
			att_fort_num += 1;
		}
	}
	*/
	
	//SUBMARINE,守据点
	for(int i = 0; i < sub_att_num; i++)
	{
		int oil_attack = GetNear(GetState(enemy_base_index) -> pos,OILFIELD);
		int metal_attack = GetNear(GetState(enemy_base_index) -> pos,MINE);
		if(sub_att_num == 1)
		{
			if(distance(submarine_attack[i],oil_attack) > 2)
				ChangeDest(submarine_attack[i],GetState(oil_attack)->pos);
		}
		else if(sub_att_num == 2)
		{
			if(distance(submarine_attack[0],oil_attack) > 2)
				ChangeDest(submarine_attack[0],GetState(oil_attack) -> pos);
			if(distance(submarine_attack[1],metal_attack) > 2)
				ChangeDest(submarine_attack[1],GetState(metal_attack) -> pos);
		}
		else
		{
			if(distance(submarine_attack[0],oil_attack) > 2)
				ChangeDest(submarine_attack[0],GetState(oil_attack) -> pos);
			if(distance(submarine_attack[1],metal_attack) > 2)
				ChangeDest(submarine_attack[1],GetState(metal_attack) -> pos);
		}
	}

	//CARRIER,主力
	  //carrier_attack[i]
	  //car_att_num
	if(car_att_num >= 3 && des_att_num >= 8)
	{
		distance_to_go(enemy_base_index,7,OCEAN);
		for(int i = 0; i < car_att_num; i++)
		{
			if(distance(carrier_attack[i],enemy_base_index) > kProperty[CARRIER].fire_ranges[1])
			{
				if(dis_go_num >= car_att_num)
					ChangeDest(carrier_attack[i],Distance_Go[i]);
				else
					ChangeDest(carrier_attack[i],Distance_Go[i % dis_go_num]);
			}
			else
			{
				AttackUnit(carrier_attack[i],enemy_base_index);
				for(int j = 0;j < attack_num; j++)	
				{
					if(AC[j].index == carrier_attack[i])	
					{	
						AC[j].attack_by_command = true;
						break;	
					}
				}
			}
		}
		for(int i = 0; i < des_att_num; i++)
		{
			if(distance(destroyer_attack[i],enemy_base_index) > kProperty[DESTROYER].fire_ranges[1])
				ChangeBase(destroyer_attack[i],enemy_base_index);
			else
			{
				AttackUnit(destroyer_attack[i],enemy_base_index);
				for(int j = 0;j < attack_num; j++)	
				{
					if(AC[j].index == destroyer_attack[i])	
					{	
						AC[j].attack_by_command = true;
						break;	
					}
				}
			}
		}
	}
	else
	{
		for(int i = 0; i < car_att_num; i++)
		{
			attack_fort.push_back(carrier_attack[i]);
			att_fort_num += 1;
		}
		for(int i = 0; i < des_att_num; i++)	
		{
			attack_fort.push_back(destroyer_attack[i]);
			att_fort_num += 1;
		}
	}

	//保家序列，搜寻家最近的据点，防基地防潜艇干据点
	//保护基地，最高优先级
	int dis1 = 1000, dis_temp1 = 1000, target_submarine = enemy_base_index;
	for(int i = 0; i < enemy_num; i++)
	{
		if(enemy_element[i].type == SUBMARINE)
		{
			if((dis1 = distance(our_base_index,enemy_element[i].index)) < dis_temp1)
			{
				dis_temp1 = dis1;
				target_submarine = enemy_element[i].index;
			}
		}
	}
	if(base_ask_for_help)
	{
		int nearest = 26;
		for(int i=0;i<enemy_num;i++)
		{
			if( distance(our_base_index,enemy_element[i].index) < nearest)
			{
				base_help_index=enemy_element[i].index;
				nearest = distance(our_base_index,enemy_element[i].index);
			}
			else if (distance(our_base_index,enemy_element[i].index) == nearest)
			{
				Position pos = enemy_element[i].pos, my = GetState(our_base_index)->pos, emy = GetState(enemy_base_index)->pos;
				if ((pos.x-my.x)*(emy.x-pos.x) >=0 && (pos.y-my.y)*(emy.y-pos.y))
					base_help_index=enemy_element[i].index;
			}
			if(distance(our_base_index,enemy_element[i].index) <= 20)
				enemy_count++;
		}
		for(unsigned int i=0;i<Defend_Index.size();i++)
		{
			ChangeDefend(Defend_Index[i],base_help_index);
		}
		if (enemy_count >= 5)
		{
			for(int i=0;i<att_fort_num;i++)
			{
				if (distance(our_base_index,attack_fort[i]) < distance(enemy_base_index,attack_fort[i]) / 2
					|| distance(our_base_index,attack_fort[i]) < 15)
				{
					ChangeDefend(attack_fort[i],base_help_index);
					Defend_Index.push_back(attack_fort[i]);
				}
			}
		}
		for(unsigned int i=0;i<Defend_Index.size();i++)
		{
			for(int j = 0;j < attack_num; j++)	
			{
				if(AC[j].index == Defend_Index[i])	
				{	
					AC[j].attack_by_command = true;
					break;	
				}
			}
		}
	}
	//保护cargo，次要优先级。
	if(target_submarine != enemy_base_index)
	{
		for(int i=0;i<att_fort_num;i++)
		{
			if(GetState(attack_fort[i])->type == CARRIER)
				continue;

			if(if_alive(target_submarine))
			{
				for(int j = 0;j < attack_num; j++)	
				{
					if(AC[j].index == attack_fort[i])	
					{	
						if (AC[j].attack_by_command == false)
						{
							if( distance(attack_fort[i],target_submarine) <= kProperty[GetState(attack_fort[i])->type].fire_ranges[0])
							{
								AttackUnit(attack_fort[i],target_submarine);
								AC[j].attack_by_command = true;
							}
							else
								ChangeBase(attack_fort[i],target_submarine);
						}
					}
				}
			}
		}
	}
    //打据点
	{
		int dis_temple, dis = 1000, index, target_fort = enemy_base_index;
		for(int i=0;i<INFO->element_num;i++)//找离我方基地最近的据点
		{
			if(INFO -> elements[i] -> type == FORT && INFO->elements[i]->team != INFO -> team_num)
			{
				index = INFO -> elements[i] -> index;
				if( (dis_temple = distance(our_base_index,index)) < dis)
				{
					target_fort = index;
					dis = dis_temple;
				}
			}
		}
		fort = target_fort;
		
		if (dis < 10 && count_fight > 5)
		{
			for(unsigned int i=0;i<Defend_Index.size();i++)
			{
				attack_fort.push_back(Defend_Index[i]);
				att_fort_num += 1;
			}
		}
		//自动攻击
		for(int i = 0; i<att_fort_num; i++)
		{
			int target=fort,dis_temp=1000,dis;
			bool command = true;
			for(int j = 0;j < attack_num; j++)	
			{
				if(AC[j].index == attack_fort[i])	
				{	
					command = AC[j].attack_by_command;
					break;	
				}
			}
			if(command == false)
			{
				for(int j = 0; j < enemy_num; j++)//找火力范围之内的敌军,并没有优化敌军攻击顺序
				{ 
					if(GetState(attack_fort[i])->type == CARRIER && enemy_element[j].type == SUBMARINE)
					continue;
					if(GetState(attack_fort[i])->type == SUBMARINE && (enemy_element[j].type == FIGHTER || enemy_element[j].type == SCOUT))
					continue;
					if((enemy_element[j].health > 0) && (dis = distance(attack_fort[i],enemy_element[j].index)) 
					<= kProperty[GetState(attack_fort[i])->type].fire_ranges[GetFireRange(enemy_element[j].type)])
					{
						if(dis < dis_temp)	
						{	
							target = j;
							dis_temp = dis;
						}
					}
				}
				//if( distance(attack_fort[i],enemy_base_index) <= kProperty[GetState(attack_fort[i]) -> type].fire_ranges[1])
					//target = enemy_base_index;
				if(target != fort)
				{	
					AttackUnit(AC[i].index,enemy_element[target].index);
					if (enemy_element[target].health != GetState(enemy_element[target].index)->health)
					{
						enemy_element[target].health -= hurt_by_distance(GetState(AC[i].index)->type, enemy_element[target].type, dis, 0);
					}
					else
						enemy_element[target].health -= hurt_by_distance(GetState(AC[i].index)->type, enemy_element[target].type, dis, 1);
					for(int j = 0;j < attack_num; j++)	
					{
						if(AC[j].index == attack_fort[i])	
						{	
							AC[j].attack_by_command = true;
							break;	
						}
					}
				}
			}
		}
		
		//抢据点
		for(int i = 0; i < INFO -> element_num; i++)
		{ 
			if(INFO -> elements[i] -> type == FORT && (INFO -> elements[i] -> team != INFO -> team_num) )
			{
				if(INFO -> elements[i] -> health <= 50)
				{
					for(int j = 0;j < att_fort_num; j++)
					{
						bool command = true;
						for(int k = 0;k < attack_num; k++)	
						{
							if(AC[k].index == attack_fort[j])	
							{	
								command = AC[k].attack_by_command;
								break;	
							}
						}
						if(command == false)
						{
							if(distance(attack_fort[j],INFO -> elements[i] -> index) > kProperty[GetState(attack_fort[j]) -> type].fire_ranges[1] )
							{
								ChangeBase(attack_fort[j],INFO -> elements[i]->index);
							}
							else
							{
								AttackUnit(attack_fort[j],INFO -> elements[i] -> index);
								for(int m = 0; m < attack_num;m++)
								{
									if(AC[m].index == attack_fort[j])
									{
										AC[m].attack_by_command = true;
										break;
									}
								}
							}
						}
					}
					break;
				}
			}
		}
		
		//打最近的据点
		for(int i = 0; i < att_fort_num; i++)
		{
			bool command = true;
			for(int j = 0;j < attack_num; j++)	
			{
				if(AC[j].index == attack_fort[i])	
				{	
					command = AC[j].attack_by_command;
					break;	
				}
			}
			if(command == false)
			{
				if( distance(attack_fort[i],fort) <= kProperty[GetState(attack_fort[i])->type].fire_ranges[1] )
				{
					AttackUnit(attack_fort[i],fort);
					ChangeBase(attack_fort[i],fort);
					for(int j = 0; j < attack_num; j++)
					{
						if(AC[j].index == attack_fort[i])
						{
							AC[j].attack_by_command = true;
							break;
						}
					}
				}
				else
					ChangeBase(attack_fort[i],fort);
			}
		}
	}

	for(unsigned int i=0;i<Defend_Index.size();i++)
	{
		for(int j = 0;j < attack_num; j++)	
		{
			if(AC[j].index == Defend_Index[i])	
			{	
				AC[j].attack_by_command = false;
				break;	
			}
		}
	}

	//给我方所有没有攻击指令的战斗序列下达自动攻击指令,小心天气！（视野小于firerange）
	for(int i = 0; i < attack_num; i++)
	{
		int target = fort,dis_temp = 1000, dis;
		if(AC[i].attack_by_command == false)
		{
			for(int j = 0;j < enemy_num; j++)//找火力范围之内的敌军,并没有优化敌军攻击顺序
			{
				if(GetState(AC[i].index)->type == CARRIER && enemy_element[j].type == SUBMARINE)
					continue;
				if(GetState(AC[i].index)->type == SUBMARINE && (enemy_element[j].type == FIGHTER || enemy_element[j].type == SCOUT))
					continue;
				if( (enemy_element[j].health > 0) && (dis = distance(AC[i].index,enemy_element[j].index)) 
					<= kProperty[GetState(AC[i].index) -> type].fire_ranges[GetFireRange(enemy_element[j].type)])
				{
					if(dis < dis_temp)
					{
						target = j;
						dis_temp = dis;
					}
				}
			}
			/*if(GetState(AC[i].index)->type != SUBMARINE
				&& distance(AC[i].index,enemy_base_index) <= kProperty[GetState(AC[i].index) -> type].fire_ranges[1])
				target = enemy_base_index;*/
			if (target == fort)
				AttackUnit(AC[i].index,target);
			else
			{
				AttackUnit(AC[i].index,enemy_element[target].index);
				if (enemy_element[target].health != GetState(enemy_element[target].index)->health)
				{
					enemy_element[target].health -= hurt_by_distance(GetState(AC[i].index)->type, enemy_element[target].type, dis, 0);
				}
				else
					enemy_element[target].health -= hurt_by_distance(GetState(AC[i].index)->type, enemy_element[target].type, dis, 1);
			}
		}
	}

	destroyer_attack.clear();
	fighter_attack.clear();
	submarine_attack.clear();
	carrier_attack.clear();
	attack_base.clear();
	attack_fort.clear();
	AC.clear();
}

//统筹生产函数
void production()
{
	//int des=0,fig=20;
	int cargo_alive=0,submarine_alive=0,fighter_alive=0,scout_alive=0,destroyer_alive=0,carrier_alive=0;
	int cargo_pd=0,submarine_pd=0,fighter_pd=0,scout_pd=0,destroyer_pd=0,carrier_pd=0;
	for(int i=0;i<our_num;i++)//遍历我方元素，得到每种单位的数量
	{
		if(GetState(our_element[i].index)->type == CARGO)
			cargo_alive++;
		else if(GetState(our_element[i].index)->type == SUBMARINE)
			submarine_alive++;
		else if(GetState(our_element[i].index)->type == FIGHTER)
			fighter_alive++;
		else if(GetState(our_element[i].index)->type == SCOUT)
			scout_alive++;
		else if(GetState(our_element[i].index)->type == DESTROYER)
			destroyer_alive++;
		else if(GetState(our_element[i].index)->type == CARRIER)
			carrier_alive++;
	}
	for(int i=0;i<INFO->production_num;i++)//遍历生产列表，得到我方每种单位在生产列表中的数量
	{
		if(INFO->production_list[i].unit_type == CARGO)
			cargo_pd++;
		else if(INFO->production_list[i].unit_type == SUBMARINE)
			submarine_pd++;
		else if(INFO->production_list[i].unit_type == FIGHTER)
			fighter_pd++;
		else if(INFO->production_list[i].unit_type == SCOUT)
			scout_pd++;
		else if(INFO->production_list[i].unit_type == DESTROYER)
			destroyer_pd++;
		else if(INFO->production_list[i].unit_type == CARRIER)
			carrier_pd++;
	}
	int cargo_total=cargo_alive+cargo_pd;
	int submarine_total=submarine_alive+submarine_pd;
	int fighter_total=fighter_alive+fighter_pd;
	int scout_total=scout_alive+scout_pd;
	int destroyer_total=destroyer_alive+destroyer_pd;
	int carrier_total=carrier_alive+carrier_pd;

	//补充cargo
	int cargo_num = 2 + (INFO->round / 6 < 4 ? INFO->round / 6 : 4);//2~6动态增长
	cargo_num = cargo_num > HelpList.size() + 2 ? cargo_num : HelpList.size() + 2;//保证正在补给中的船数最大值
	if(cargo_total<cargo_num)
	{
		int fuel=GetState(our_base_index)->fuel
			-cargo_pd*kProperty[CARGO].fuel_max/2
			-submarine_pd*kProperty[SUBMARINE].fuel_max
			-fighter_pd*kProperty[FIGHTER].fuel_max
			-scout_pd*kProperty[SCOUT].fuel_max
			-destroyer_pd*kProperty[DESTROYER].fuel_max
			-carrier_pd*kProperty[CARRIER].fuel_max;
		int metal=GetState(our_base_index)->metal;
		for(int i=0;i<cargo_num-cargo_total;i++)
			if(fuel >= 100 && metal >= kProperty[CARGO].cost)
			{
				Produce(CARGO);
				fuel-=kProperty[CARGO].fuel_max/2;
				metal-=kProperty[CARGO].cost;
				cargo_pd++;
			}
	}

	//守家
	if (base_ask_for_help)
	{
		int metal=GetState(our_base_index)->metal;
		if (Defend_Index.size() < 5 && metal > 50)
		{
			for (int i = 0; i < 5 - (int)Defend_Index.size() && metal - i * 14 >= 50; i++)
			{
				Produce(DESTROYER);
				destroyer_pd++;
			}
		}
	}

	//补充scout
	if(scout_total < 1)
	{
		int fuel=GetState(our_base_index)->fuel
			-cargo_pd*150
			-submarine_pd*kProperty[SUBMARINE].fuel_max
			-fighter_pd*kProperty[FIGHTER].fuel_max
			-scout_pd*kProperty[SCOUT].fuel_max
			-destroyer_pd*kProperty[DESTROYER].fuel_max
			-carrier_pd*kProperty[CARRIER].fuel_max;
		int metal=GetState(our_base_index)->metal;
		if(fuel >= 100 && metal >= kProperty[SCOUT].cost)
		{
			Produce(SCOUT);
			scout_pd++;
		}
	}

	//补充fighter到9
	if(fighter_total < 9)
	{
		int fuel=GetState(our_base_index)->fuel
			-cargo_pd*150
			-submarine_pd*kProperty[SUBMARINE].fuel_max
			-fighter_pd*kProperty[FIGHTER].fuel_max
			-scout_pd*kProperty[SCOUT].fuel_max
			-destroyer_pd*kProperty[DESTROYER].fuel_max
			-carrier_pd*kProperty[CARRIER].fuel_max;
		int metal=GetState(our_base_index)->metal;
		int fuel_temp=fuel,metal_temp=metal;
		for(int i=0;i< 9 - fighter_total; i++)
		{
			if(fuel_temp >= 100 && metal >= kProperty[FIGHTER].cost)
			{
				Produce(FIGHTER);
				fuel_temp-=kProperty[FIGHTER].fuel_max;metal_temp-=kProperty[FIGHTER].cost;
				fighter_pd++;
			}
		}
	}
	//资源充足时优先造fighter，其次是destroyer。
	//若没有submarine造一个submarine，资源充足时造另一个。

//造carrier
	if(carrier_total < 3)
	{
		int fuel=GetState(our_base_index)->fuel
			-cargo_pd*150
			-submarine_pd*kProperty[SUBMARINE].fuel_max
			-fighter_pd*kProperty[FIGHTER].fuel_max
			-scout_pd*kProperty[SCOUT].fuel_max
			-destroyer_pd*kProperty[DESTROYER].fuel_max
			-carrier_pd*kProperty[CARRIER].fuel_max;
		int metal=GetState(our_base_index)->metal;
		int fuel_temp=fuel,metal_temp=metal;
		for(int i=0;i<8-destroyer_total;i++)
		{
			if( fuel_temp > 100 && metal_temp >= kProperty[DESTROYER].cost)
			{
				Produce(DESTROYER);
				fuel_temp-=kProperty[DESTROYER].fuel_max;metal_temp-=kProperty[DESTROYER].cost;
				destroyer_pd++;
			}
		}
		for(int i=0;i<1-carrier_total;i++)
		{
			if( fuel_temp > 150 && metal_temp >= kProperty[CARRIER].cost)
			{
				Produce(CARRIER);
				fuel_temp-=kProperty[CARRIER].fuel_max;metal_temp-=kProperty[CARRIER].cost;
				carrier_pd++;
			}
		}
		for(int i=0;i<8-destroyer_total;i++)
		{
			if( fuel_temp > 100 && metal_temp >= kProperty[DESTROYER].cost)
			{
				Produce(DESTROYER);
				fuel_temp-=kProperty[DESTROYER].fuel_max;metal_temp-=kProperty[DESTROYER].cost;
				destroyer_pd++;
			}
		}
		for(int i=0;i<2-carrier_total;i++)
		{
			if( fuel_temp > 150 && metal_temp >= kProperty[CARRIER].cost)
			{
				Produce(CARRIER);
				fuel_temp-=kProperty[CARRIER].fuel_max;metal_temp-=kProperty[CARRIER].cost;
				carrier_pd++;
			}
		}
		for(int i=0;i<8-destroyer_total;i++)
		{
			if( fuel_temp > 100 && metal_temp >= kProperty[DESTROYER].cost)
			{
				Produce(DESTROYER);
				fuel_temp-=kProperty[DESTROYER].fuel_max;metal_temp-=kProperty[DESTROYER].cost;
				destroyer_pd++;
			}
		}
		for(int i=0;i<3-carrier_total;i++)
		{
			if( fuel_temp > 150 && metal_temp >= kProperty[CARRIER].cost)
			{
				Produce(CARRIER);
				fuel_temp-=kProperty[CARRIER].fuel_max;metal_temp-=kProperty[CARRIER].cost;
				carrier_pd++;
			}
		}
		for(int i=0;i<8-destroyer_total;i++)
		{
			if( fuel_temp > 100 && metal_temp >= kProperty[DESTROYER].cost)
			{
				Produce(DESTROYER);
				fuel_temp-=kProperty[DESTROYER].fuel_max;metal_temp-=kProperty[DESTROYER].cost;
				destroyer_pd++;
			}
		}
	}
	else
	{
		int fuel=GetState(our_base_index)->fuel
			-cargo_pd*150
			-submarine_pd*kProperty[SUBMARINE].fuel_max
			-fighter_pd*kProperty[FIGHTER].fuel_max
			-scout_pd*kProperty[SCOUT].fuel_max
			-destroyer_pd*kProperty[DESTROYER].fuel_max
			-carrier_pd*kProperty[CARRIER].fuel_max;
		int metal=GetState(our_base_index)->metal;
		int fuel_temp=fuel,metal_temp=metal;
		if(submarine_total == 0 && fuel > 100 && metal > kProperty[SUBMARINE].cost)
		{
			Produce(SUBMARINE);
			fuel_temp-=kProperty[SUBMARINE].fuel_max;metal_temp-=kProperty[SUBMARINE].cost;
		}

		if(cargo_total <= 4 && fuel_temp >100 && metal_temp >= kProperty[CARGO].cost)
		{
			Produce(CARGO);
			fuel_temp-=kProperty[CARGO].fuel_max;metal_temp-=kProperty[CARGO].cost;
		}

		if(fuel_temp >100 && metal_temp >= kProperty[DESTROYER].cost)
		{
			Produce(DESTROYER);
			fuel_temp-=kProperty[DESTROYER].fuel_max;metal_temp-=kProperty[DESTROYER].cost;
		}

		if(cargo_total == 4 && fuel_temp >100 && metal_temp >= kProperty[CARGO].cost)
		{
			Produce(CARGO);
			fuel_temp-=kProperty[CARGO].fuel_max;metal_temp-=kProperty[CARGO].cost;
		}
		
		if(fighter_total <= 4 && fuel_temp >100 && metal_temp >= kProperty[FIGHTER].cost)
		{
			Produce(FIGHTER);
			fuel_temp-=kProperty[FIGHTER].fuel_max;metal_temp-=kProperty[FIGHTER].cost;
		}

		if(cargo_total == 5 && fuel_temp >100 && metal_temp >= kProperty[CARGO].cost)
		{
			Produce(CARGO);
			fuel_temp-=kProperty[CARGO].fuel_max;metal_temp-=kProperty[CARGO].cost;
		}

		if(submarine_total <2 && fuel_temp > 100 && metal_temp >= kProperty[SUBMARINE].cost)
		{
			Produce(SUBMARINE);
			fuel_temp-=kProperty[SUBMARINE].fuel_max;metal_temp-=kProperty[SUBMARINE].cost;
		}
	}
}

//scout位置
void scout_movement()
{
	//获取我方fighter信息
	int index = 0;
	Position Dest, my = GetState(our_base_index)->pos, emy = GetState(enemy_base_index)->pos;
	Dest.x = my.x + (emy.x - my.x >= 8 ? 3 : (emy.x - my.x <= -8 ? -3 : 0));
	Dest.y = my.y + (emy.y - my.y >= 8 ? 3 : (emy.x - my.x <= -8 ? -3 : 0));
	Dest.z = 2;
	for(int i=0;i<our_num;i++)//存活的fighter
	{
		if(our_element[i].type == SCOUT)
		{
			index = our_element[i].index;
			ChangeDest(index, Dest);
			break;
		}
	}
}

//fighter行为函数，以fighter的燃料，弹药和血量判断行为
void fighter_movement()
{
	//获取我方fighter信息
	int fighter_total=0;
	vector<int>Fighter_Indexes;
	for(int i=0;i<our_num;i++)//存活的fighter
	{
		if(our_element[i].type == FIGHTER)
		{
			fighter_total+=1;
			Fighter_Indexes.push_back(our_element[i].index);
			int flag = 1;
			for (unsigned int j = 0; j < Defend_Fighter.size(); j++)//确认是否是防守者
			{
				if (Defend_Fighter[j] == our_element[i].index)
				{
					flag = 0;
					break;
				}
			}
			if (flag)
				Defend_Fighter.push_back(our_element[i].index);
		}
	}
	//防守者确认
	for (unsigned int j = 0; j < Defend_Fighter.size(); j++)//确认防守者是否存活
	{
		if (!if_alive(Defend_Fighter[j]))
		{
			remove_v(Defend_Fighter,Defend_Fighter[j]);
		}
	}

	if (GetState(our_base_index)->health >= 400 && fighter_total > 5)
		Defend_Fighter.resize(fighter_total * 2 / 3);

	//防守飞机默认位置，当有后续函数有新安排时此处不起作用
	Position Dest, my = GetState(our_base_index)->pos, emy = GetState(enemy_base_index)->pos;
	Dest.x = my.x + (emy.x - my.x > 0 ? 4 : -4);
	Dest.y = my.y + (emy.y - my.y > 0 ? 4 : -4);
	Dest.z = 2;

	//FIGHTER行为函数
	for(int i=0;i<fighter_total;i++)
	{
		//后撤并请求维修
		if( GetState(Fighter_Indexes[i])->health <= 25)
		{
			if(distance(Fighter_Indexes[i],enemy_base_index) <= kProperty[FIGHTER].fire_ranges[2])
				AttackUnit(Fighter_Indexes[i],enemy_base_index);
			ChangeBase(Fighter_Indexes[i],our_base_index);
			need_help he={FIGHTER,Fighter_Indexes[i],help_fix};
			Need_HELP.push_back(he);
			help_num+=1;
		}
		//后撤并请求补给
		else if( (GetState(Fighter_Indexes[i])->ammo <= 10) 
			|| (GetState(Fighter_Indexes[i])->fuel <= distance(Fighter_Indexes[i],our_base_index)+10)
			|| (GetState(Fighter_Indexes[i])->fuel <= 20))//没弹药或者燃料不足
		{
			if(distance(Fighter_Indexes[i],enemy_base_index) <= kProperty[FIGHTER].fire_ranges[2])
				AttackUnit(Fighter_Indexes[i],enemy_base_index);
			ChangeBase(Fighter_Indexes[i],our_base_index);
			need_help he={FIGHTER,Fighter_Indexes[i],help_supply};
			Need_HELP.push_back(he);
			help_num+=1;
		}
		else//全部加入攻击序列
		{
			int flag = 1;
			int index=Fighter_Indexes[i];
			for (unsigned int j = 0; j < Defend_Fighter.size(); j++)//确认是否是防守者
			{
				if (Defend_Fighter[j] == index)
				{
					flag = 0;
					break;
				}
			}
			if (flag)
			{
				Attack_Indexes.push_back(index);
				attack_num+=1;
			}
			else
			{
				if( (GetState(Fighter_Indexes[i])->ammo <= 9) )
				{
					ChangeBase(Fighter_Indexes[i],our_base_index);
					need_help he={FIGHTER,Fighter_Indexes[i],help_supply};
					Need_HELP.push_back(he);
					help_num+=1;
				}
				else
				{
					Defend_Index.push_back(index);
					ChangeDest(index, Dest);
				}
			}
		}
	}
	Fighter_Indexes.clear();
}

//destroyer逻辑
void destroyer_movement()
{
	//获取我方destroyer信息
	int destroyer_total=0;
	vector<int>Destroyer_Indexes;
	for(int i=0;i<our_num;i++)//存活的destroyer
	{
		if(our_element[i].type == DESTROYER)
		{
			destroyer_total+=1;
			Destroyer_Indexes.push_back(our_element[i].index);
		}
	}
	//DESTROYER行为函数
	for(int i=0;i<destroyer_total;i++)
	{
		//后撤并请求补给
		if( (GetState(Destroyer_Indexes[i])->ammo <= 0) 
			|| (GetState(Destroyer_Indexes[i])->fuel <= 20))//没弹药或者燃料不足
		{
			if(distance(Destroyer_Indexes[i],enemy_base_index) <= kProperty[DESTROYER].fire_ranges[1])
				AttackUnit(Destroyer_Indexes[i],enemy_base_index);
			ChangeBase(Destroyer_Indexes[i],our_base_index);
			if (distance(Destroyer_Indexes[i],our_base_index) > 1)
			{
				NeedSupply(Destroyer_Indexes[i]);
			}
			else
			{
				need_help he={DESTROYER,Destroyer_Indexes[i],help_supply};
				Need_HELP.push_back(he);
				help_num+=1;
			}
		}
		//后撤并请求维修
		else if( GetState(Destroyer_Indexes[i])->health <= 25)
		{
			if(distance(Destroyer_Indexes[i],enemy_base_index) <= kProperty[DESTROYER].fire_ranges[1])
				AttackUnit(Destroyer_Indexes[i],enemy_base_index);
			ChangeBase(Destroyer_Indexes[i],our_base_index);
			need_help he={DESTROYER,Destroyer_Indexes[i],help_fix};
			Need_HELP.push_back(he);
			help_num+=1;
		}
		else//加入攻击序列
		{
			int index=Destroyer_Indexes[i];
			Attack_Indexes.push_back(index);
			attack_num+=1;
		}
	}
	Destroyer_Indexes.clear(); 
}

//carrier逻辑
void carrier_movement()
{
	//获取我方carrier信息
	int carrier_total=0;
	vector<int>Carrier_Indexes;
	for(int i=0;i<our_num;i++)//存活的carrier
	{
		if(our_element[i].type == CARRIER)
		{
			carrier_total+=1;
			Carrier_Indexes.push_back(our_element[i].index);
		}
	}
	//carrier行为函数
	for(int i=0;i<carrier_total;i++)
	{
		//后撤并请求补给
		if( (GetState(Carrier_Indexes[i])->ammo <= 0) 
			|| (GetState(Carrier_Indexes[i])->fuel <= 20))//没弹药或者燃料不足
		{
			if(distance(Carrier_Indexes[i],enemy_base_index) <= kProperty[CARRIER].fire_ranges[1])
				AttackUnit(Carrier_Indexes[i],enemy_base_index);
			ChangeBase(Carrier_Indexes[i],our_base_index);
			if (distance(Carrier_Indexes[i],our_base_index) > 1)
			{
				NeedSupply(Carrier_Indexes[i]);
			}
			else
			{
				need_help he={CARRIER,Carrier_Indexes[i],help_supply};
				Need_HELP.push_back(he);
				help_num+=1;
			}
		}
		//后撤并请求维修
		else if( GetState(Carrier_Indexes[i])->health <= 25)
		{
			if(distance(Carrier_Indexes[i],enemy_base_index) <= kProperty[CARRIER].fire_ranges[1])
				AttackUnit(Carrier_Indexes[i],enemy_base_index);
			ChangeBase(Carrier_Indexes[i],our_base_index);
			need_help he={CARRIER,Carrier_Indexes[i],help_fix};
			Need_HELP.push_back(he);
			help_num+=1;
		}
		else//全部加入攻击序列
		{
			int index=Carrier_Indexes[i];
			Attack_Indexes.push_back(index);
			attack_num+=1;
		}
	}
	Carrier_Indexes.clear();
}

//submarine逻辑
void submarine_movement()
{
	//获取我方submarine信息
	int submarine_total=0;
	vector<int>Submarine_Indexes;
	for(int i=0;i<our_num;i++)//存活的submarine
	{
		if(our_element[i].type == SUBMARINE)
		{
			submarine_total+=1;
			Submarine_Indexes.push_back(our_element[i].index);
		}
	}
	//submarine行为函数
	for(int i=0;i<submarine_total;i++)
	{
		//后撤并请求补给
		if( (GetState(Submarine_Indexes[i])->ammo <= 0) 
			|| (GetState(Submarine_Indexes[i])->fuel <= 20))//没弹药或者燃料不足
		{
			//if(distance(Submarine_Indexes[i],enemy_base_index) <= kProperty[SUBMARINE].fire_ranges[1])
				//AttackUnit(Submarine_Indexes[i],enemy_base_index);
			ChangeBase(Submarine_Indexes[i],our_base_index);
			if (distance(Submarine_Indexes[i],our_base_index) > 1)
			{
				NeedSupply(Submarine_Indexes[i]);
			}
			else
			{
				need_help he={SUBMARINE,Submarine_Indexes[i],help_supply};
				Need_HELP.push_back(he);
				help_num+=1;
			}
		}
		//后撤并请求维修
		else if( GetState(Submarine_Indexes[i])->health <= 25)
		{
			//if(distance(Submarine_Indexes[i],enemy_base_index) <= kProperty[SUBMARINE].fire_ranges[1])
				//AttackUnit(Submarine_Indexes[i],enemy_base_index);
			ChangeBase(Submarine_Indexes[i],our_base_index);
			need_help he={SUBMARINE,Submarine_Indexes[i],help_fix};
			Need_HELP.push_back(he);
			help_num+=1;
		}
		else//加入攻击序列
		{
			int index=Submarine_Indexes[i];
			Attack_Indexes.push_back(index);
			attack_num+=1;
		}
	}
	Submarine_Indexes.clear(); 
}

//据点防御函数,并不能保证持续攻击同一个目标。并没有写补给接口
void fort_defend()
{
	vector<int>our_fort;//我方据点
	int our_fort_num=0;//我方据点数量
	for(int i=0;i<INFO->element_num;i++)
	{
		if(INFO->elements[i]->type == FORT && (INFO->elements[i]->team == INFO->team_num) )
		{
			our_fort.push_back(INFO->elements[i]->index);
			our_fort_num+=1;
		}
	}
	int dis;
	for(int i=0;i<our_fort_num;i++)
		for(int j=0;j<enemy_num;j++)
			if( (dis = distance(our_fort[i],enemy_element[j].index)) <= kProperty[FORT].fire_ranges[GetFireRange(enemy_element[j].type)])
			{
				AttackUnit(our_fort[i],enemy_element[j].index);
				if (enemy_element[j].health != GetState(enemy_element[j].index)->health)
				{
					enemy_element[j].health -= hurt_by_distance(FORT, enemy_element[j].type, dis, 0);
				}
				else
					enemy_element[j].health -= hurt_by_distance(FORT, enemy_element[j].type, dis, 1);
			}
	our_fort.clear();
}

void fort_supply()
{
	vector<int>our_fort;//我方据点
	int our_fort_num=0;//我方据点数量
	for(int i=0;i<INFO->element_num;i++)
	{
		if(INFO->elements[i]->type == FORT && (INFO->elements[i]->team == INFO->team_num) )
		{
			our_fort.push_back(INFO->elements[i]->index);
			our_fort_num+=1;
		}
	}
	for(int i=0;i<our_fort_num;i++)
		for(int j=0;j<our_num;j++)
			if( ((our_element[j].type != FIGHTER && our_element[j].type != SCOUT && distance(our_fort[i],our_element[j].index) <= 1)
				|| distance(our_fort[i],our_element[j].index) == 0)
				&& our_element[j].fuel <= 15)
				Supply(our_fort[i], our_element[j].index, 50, 0, 0);

	our_fort.clear();
}

//基地防御函数,并不能保证持续攻击同一个目标。偷懒四个for循环没有优化。
void base_defend()
{
	enemy_count = 0;//移到攻击函数中计算
	int dis;
	for(int i=0;i<enemy_num;i++)
	{
		if( ((enemy_element[i].type == FIGHTER) 
			|| (enemy_element[i].type == SCOUT))
			&& distance(our_base_index,enemy_element[i].index) <= 25)
		{
			base_ask_for_help=true;
			base_help_index=enemy_element[i].index;
		}
		if( ((enemy_element[i].type == FIGHTER) 
			|| (enemy_element[i].type == SCOUT))
			&& (dis = distance(our_base_index,enemy_element[i].index)) <= (int)kProperty[BASE].fire_ranges[2])
		{
			if(BaseC.attack == false)
			{
				AttackUnit(our_base_index,enemy_element[i].index);
				BaseC.attack=true;
				BaseC.attack_index=enemy_element[i].index;
			}
			base_ask_for_help=true;
			base_help_index=enemy_element[i].index;
			break;
		}
	}

	for(int i=0;i<enemy_num;i++)
	{
		if(BaseC.attack == true)
			break;
		if(enemy_element[i].type == SUBMARINE //找家附近的敌方潜艇
			&& distance(enemy_element[i].index,our_base_index) <= (int)kProperty[BASE].fire_ranges[0])
		{
			AttackUnit(our_base_index,enemy_element[i].index);
			find_submarine=true;
			submarine_target_index=enemy_element[i].index;
			BaseC.attack=true;
			BaseC.attack_index=enemy_element[i].index;
			break;
		}
	} 
	
	for(int i=0;i<enemy_num;i++)
	{
		if( BaseC.attack == true && GetState(BaseC.attack_index)->type != SUBMARINE)
			break;
		if( ((enemy_element[i].type == DESTROYER) 
			|| (enemy_element[i].type == CARRIER))
			&& distance(our_base_index,enemy_element[i].index) <= 20)
		{
			base_ask_for_help=true;
			base_help_index=enemy_element[i].index;
		}
		if( ((enemy_element[i].type == DESTROYER) 
			|| (enemy_element[i].type == CARRIER))
			&& distance(our_base_index,enemy_element[i].index) <= (int)kProperty[BASE].fire_ranges[1])
		{
			AttackUnit(our_base_index,enemy_element[i].index);
			BaseC.attack=true;
			BaseC.attack_index=enemy_element[i].index;
			base_ask_for_help=true;
			base_help_index=enemy_element[i].index;
			break;
		}
	}
}

//基地维修函数，没有优化同级维修序列，补给和维修互斥所以默认都在里面了
void base_fix()
{
	//检索请求维修的元素，按优先级维修（fighter>cargo），同时置维修指令
	vector<need_help>need_fix;
	int fix_num=0;
	vector<int>fix_fighter;
	int fighter=0;
	vector<int>fix_destroyer;
	int destroyer=0;
	vector<int>fix_cargo;
	int cargo=0;
	vector<int>fix_carrier;
	int carrier=0;
	vector<int>fix_submarine;
	int submarine=0;
	vector<int>fix_scout;
	int scout=0;
	for(int i=0;i<help_num;i++)
	{
		//if(Need_HELP[i].help_command == help_fix)
		//{
			fix_num+=1;
			need_fix.push_back(Need_HELP[i]);
		//}
	}
	
	for(int i=0;i<fix_num;i++)
	{
		if(need_fix[i].type == FIGHTER || need_fix[i].type == SCOUT)//飞机距离0才可维修
		{
			if( distance(need_fix[i].index,our_base_index) != 0 )
				continue;
			else
			{
				if(need_fix[i].type == FIGHTER)
				{
					fix_fighter.push_back(need_fix[i].index);
					fighter+=1;
				}
				else
				{
					fix_scout.push_back(need_fix[i].index);
					scout+=1;
				}
			}
		}
		else
		{
			if( distance(need_fix[i].index,our_base_index) > 1 )
				continue;
			else
			{
				if(need_fix[i].type == CARGO)
				{
					fix_cargo.push_back(need_fix[i].index);
					cargo+=1;
				}
				else if(need_fix[i].type == SUBMARINE)
				{
					fix_submarine.push_back(need_fix[i].index);
					submarine+=1;
				}
				else if(need_fix[i].type == DESTROYER)
				{
					fix_destroyer.push_back(need_fix[i].index);
					destroyer+=1;
				}
				else if(need_fix[i].type == CARRIER)
				{
					fix_carrier.push_back(need_fix[i].index);
					carrier+=1;
				}
			}
		}
	}

	//这里是同级优化
	resort(fighter,fix_fighter);
	resort(destroyer,fix_destroyer);
	resort(cargo,fix_cargo);
	resort(carrier,fix_carrier);
	resort(submarine,fix_submarine);
	resort(scout,fix_scout);

	if(fighter!=0)
	{
		Fix(our_base_index,fix_fighter[0]);
		BaseC.fix=true;
		BaseC.fix_index=fix_fighter[0];
	}
	else
	{
		if(carrier!=0) 
		{
			Fix(our_base_index,fix_carrier[0]);
			BaseC.fix=true;
			BaseC.fix_index=fix_carrier[0];
		}
		else
		{
			if(cargo!=0)
			{
				Fix(our_base_index,fix_cargo[0]);
				BaseC.fix=true;
				BaseC.fix_index=fix_cargo[0];
			}
			else
			{
				if(submarine!=0)
				{
					Fix(our_base_index,fix_submarine[0]);
					BaseC.fix=true;
					BaseC.fix_index=fix_submarine[0];
				}
				else
				{
					if(destroyer!=0)  
					{
						Fix(our_base_index,fix_destroyer[0]);
						BaseC.fix=true;
						BaseC.fix_index=fix_destroyer[0];
					}
					else
					{
						if(scout!=0)
						{
							Fix(our_base_index,fix_scout[0]);
							BaseC.fix=true;
							BaseC.fix_index=fix_scout[0];
						}
					}
				}
			}
		}
	}
	need_fix.clear();
	fix_fighter.clear();
	fix_destroyer.clear();
	fix_cargo.clear();
	fix_carrier.clear();
	fix_submarine.clear();
	fix_scout.clear();
}

//维修的同级优化
void resort(int &type,vector<int>&fix_type)
{
	for(int i=0;i<type;i++)
	{
		for(int j=i+1;j<type;j++)
		{
			if( GetState(fix_type[i])->fuel <= GetState(fix_type[j])->fuel )
			{
				int swap=fix_type[i];
				fix_type[i]=fix_type[j];
				fix_type[j]=swap;
			}
		}
	}
}

//补给
void NeedSupply(int index)
{
	int flag = 1;
	for (unsigned int j = 0; j < HelpList.size(); j++)//确认是否已经在队列中
	{
		if (HelpList[j].index == index)//存活
		{
			flag = 0;
			break;
		}
	}
	if (flag)
		HelpList.push_back(*new Help(index));
}

//cargo逻辑，并不给任何战斗序列补给，没有考虑阻挡情况，没有写cargo维修
vector<int> cargo_movement(vector<int>Cargo_last,int last_num)
{
	const int pre=3;
	const int fuel_warning=300,metal_warning=100;
	int cargo_producelist=0,flag=0;
	vector<int>Cargo_fully_free;
	int cargo_free=0;
	vector<int>Cargo_busy;
	int cargo_busy=0;
	vector<int>All_Cargo;
	int cargo_total=0;
	vector<int>cargo_metal_free;
	int metal_free=0;
	vector<int>cargo_oil_free;
	int oil_free=0;


	for(int i=0;i<our_num;i++)
	{ 
		if(our_element[i].type == CARGO)
		{
			int flag = 0;
			int index=our_element[i].index;
			if(our_element[i].fuel < 5)
			{
				flag = 1;
				our_element[i].destination = our_element[i].pos;
				for (unsigned int j = 0; j < HelpList.size(); j++)//确认是否已经在队列中
				{
					if (HelpList[j].index == our_element[i].index)//存在
					{
						flag = 0;
						break;
					}
				}
			}
			else
			{
				cargo_total+=1;
				All_Cargo.push_back(index);
			}
			//基地给运输舰补给
			if (distance(our_base_index, index) <= 1 && (our_element[i].ammo < 120 || our_element[i].fuel < 50))
			{
				if (our_element[i].fuel >= 50)
					Supply(our_base_index, index, 0, kProperty[CARGO].ammo_max, 0);
				else
				{
					Supply(our_base_index, index, 50, kProperty[CARGO].ammo_max, 0);
					flag = 0;
				}
			}
			if (flag)
			{
				HelpList.push_back(*new Help(our_element[i].index));
			}
		}
	}

	vector<int>All_Cargo_temp = All_Cargo;
	int cargo_total_temp = cargo_total;

	int needHelp = 0;//需要另外准备运输船去补给（已经去了的除外）
	for (unsigned int i = 0; i < HelpList.size(); i++)//确认已经被派去补给的船是否存活
	{
		if (if_alive(HelpList[i].index)
			&& (GetState(HelpList[i].index)->fuel < 30 || GetState(HelpList[i].index)->ammo <= 0))//被帮助者存活
		{
			if (HelpList[i].helper && if_alive(HelpList[i].helper))//帮助者存活
			{
				if(distance(HelpList[i].helper,HelpList[i].index) >1)
				{
					if (GetState(HelpList[i].index)->fuel >0)
						ChangeDest(HelpList[i].helper,GetState(HelpList[i].index)->destination);
					else
						ChangeDest(HelpList[i].helper,GetState(HelpList[i].index)->pos);
					cargo_total--;
					remove_v(All_Cargo, HelpList[i].helper);
				}
				else
				{
					int fuel = GetState(HelpList[i].helper)->fuel - 100;
					Supply(HelpList[i].helper, HelpList[i].index,
						fuel>0 ? fuel : 0, GetState(HelpList[i].helper)->ammo, 0);
					HelpList.erase(remove(HelpList.begin(),HelpList.end(),HelpList[i]),HelpList.end());
				}
			}
			else//帮助者死亡或未分配
			{
				HelpList[i].helper = 0;
				needHelp++;
			}
		}
		else//被帮助者死亡
			HelpList.erase(remove(HelpList.begin(),HelpList.end(),HelpList[i]),HelpList.end());
	}

	for(int i=0;i<cargo_total;i++)
	{ 
		int index=All_Cargo[i];
		for(int j=0;j<cargo_temp_num;j++)
		{
			if(index == Cargo_Temp[j])
			{
				flag=1;
				break;
			}
		}
		flag=0;
		//cargo补给函数
		if(needHelp && GetState(index)->fuel >= kProperty[CARGO].fuel_max-50 && (GetState(our_base_index)->fuel >= fuel_warning))
		{
			needHelp--;
			for (unsigned int i = 0; i < HelpList.size(); i++)//确认去补给的船是否存活
			{
				if (!HelpList[i].helper)
				{
					HelpList[i].helper = index;
					if(distance(index,HelpList[i].index) >1)
						if (GetState(HelpList[i].index)->fuel >0)
							ChangeDest(index,GetState(HelpList[i].index)->destination);
						else
							ChangeDest(index,GetState(HelpList[i].index)->pos);
					else
					{
						int fuel = GetState(HelpList[i].helper)->fuel - 100;
						Supply(HelpList[i].helper, HelpList[i].index,
							fuel>0 ? fuel : 0, GetState(HelpList[i].helper)->ammo, 0);
						HelpList.erase(remove(HelpList.begin(),HelpList.end(),HelpList[i]),HelpList.end());
					}
					break;
				}
			}
		}

		//cargo维修函数
		else if(0)
		{}

		else
		{	
			if(GetState(index)->metal >= kProperty[CARGO].metal_max && GetState(index)->fuel <150)
			{
				cargo_oil_free.push_back(index);	
				oil_free+=1;	
			}
			if(GetState(index)->fuel >= kProperty[CARGO].fuel_max-150 && GetState(index)->metal == 0)
			{	
				cargo_metal_free.push_back(index);
				metal_free+=1;
			}

			if( (GetState(index)->metal >= kProperty[CARGO].metal_max) && (GetState(our_base_index)->metal < kProperty[BASE].metal_max) )//都满了,busy	
			{	
				cargo_busy+=1;	
				Cargo_busy.push_back(index);
			}		
			else if( (GetState(index)->fuel >= kProperty[CARGO].fuel_max-150) && (GetState(our_base_index)->fuel < kProperty[BASE].fuel_max) )	
			{	
				cargo_busy+=1;		
				Cargo_busy.push_back(index);	
			}
			else	
			{
				cargo_free+=1;	
				Cargo_fully_free.push_back(index);
			}
		}
	}
	
	if(cargo_busy>0)//让满载运输舰先回去
	{
		for(int i=0;i<cargo_busy;i++)
		{
			int index=Cargo_busy[i],flag1=0;
			if((GetState(our_base_index)->fuel >= fuel_warning) && (GetState(our_base_index)->metal >= metal_warning))
			{
				for(int j=0;j<oil_free;j++)	
				{
					if(index == cargo_oil_free[j])
					{	
						flag1=1;	
						break;
					}
				}
				for(int j=0;j<metal_free;j++)	
				{	
					if(index == cargo_metal_free[j])		
					{
						flag1=1;	
						break;	
					}		
				}
				if(flag1==1)	
				{	
					flag1=0;	
					continue;
				}
			}
			int fuel;
			if( (fuel=GetState(index)->fuel-80) < 0)
				fuel=0;
			if(distance(index,our_base_index) >1)
			{
				ChangeBase(index,our_base_index);
				//ChangeDest(index,GetState(our_base_index)->pos);
			}
			else
			{
				Supply(index,our_base_index,fuel,GetState(index)->ammo,GetState(index)->metal);
				Cargo_fully_free.push_back(index);
				cargo_free+=1;
			}
			if(GetState(index)->fuel < 100 && GetState(index)->metal < 10 )
			{
				Cargo_fully_free.push_back(index);
				cargo_free+=1;
			}
		}
	}

	if( (GetState(our_base_index)->fuel < fuel_warning) || (GetState(our_base_index)->metal < metal_warning) )//首先考虑基地燃料金属不足
	{
		if(GetState(our_base_index)->fuel < fuel_warning)//燃料严重不足，全员抢燃料。
		{
			for(int i=0;i<cargo_free;i++)
			{
				int nearest_oilfield=a_near_b_near_c(Cargo_fully_free[i],OILFIELD,our_base_index);
				Target_Indexes.push_back(nearest_oilfield);
				target_num+=1;
				ChangeDest(Cargo_fully_free[i],GetState(nearest_oilfield)->pos);
			}
			for(int i=0;i<oil_free;i++)
			{
				int nearest_oilfield=a_near_b_near_c(cargo_oil_free[i],OILFIELD,our_base_index);
				Target_Indexes.push_back(nearest_oilfield);
				target_num+=1;
				ChangeDest(cargo_oil_free[i],GetState(nearest_oilfield)->pos);
			}
		}
		else if(GetState(our_base_index)->metal < metal_warning)//燃料够但是金属严重不足，全员抢金属。
		{
			for(int i=0;i<cargo_free;i++)
			{
				int nearest_mine=a_near_b_near_c(Cargo_fully_free[i],MINE,our_base_index);
				Target_Indexes.push_back(nearest_mine);
				target_num+=1;
				ChangeDest(Cargo_fully_free[i],GetState(nearest_mine)->pos);
			}
			for(int i=0;i<metal_free;i++)
			{
				int nearest_mine=a_near_b_near_c(cargo_metal_free[i],MINE,our_base_index);
				Target_Indexes.push_back(nearest_mine);
				target_num+=1;
				ChangeDest(cargo_metal_free[i],GetState(nearest_mine)->pos);
			}
		}
	}
	else//基地资源充足，2抢金属，剩下的抢燃料
	{
		if(cargo_free <= 2)
		{
			for(int i=0;i<cargo_free;i++)
			{
				int nearest_mine=a_near_b_near_c(Cargo_fully_free[i],MINE,our_base_index);
				Target_Indexes.push_back(nearest_mine);
				target_num+=1;
				ChangeDest(Cargo_fully_free[i],GetState(nearest_mine)->pos);
			}
		}
		else
		{
			for(int i=0;i<2;i++)
			{
				int nearest_mine=a_near_b_near_c(Cargo_fully_free[i],MINE,our_base_index);
				Target_Indexes.push_back(nearest_mine);
				target_num+=1;
				ChangeDest(Cargo_fully_free[i],GetState(nearest_mine)->pos);
			}
			for(int i=2;i<cargo_free-2;i++)
			{
				int nearest_oilfield=a_near_b_near_c(Cargo_fully_free[i],OILFIELD,our_base_index);
				Target_Indexes.push_back(nearest_oilfield);
				target_num+=1;
				ChangeDest(Cargo_fully_free[i],GetState(nearest_oilfield)->pos);
			}
		}
		for(int i=0;i<oil_free;i++)
		{
			int nearest_oilfield=a_near_b_near_c(cargo_oil_free[i],OILFIELD,our_base_index);
			Target_Indexes.push_back(nearest_oilfield);
			target_num+=1;
			ChangeDest(cargo_oil_free[i],GetState(nearest_oilfield)->pos);
		}
		for(int i=0;i<metal_free;i++)
		{
			int nearest_mine=a_near_b_near_c(cargo_metal_free[i],MINE,our_base_index);
			Target_Indexes.push_back(nearest_mine);
			target_num+=1;
			ChangeDest(cargo_metal_free[i],GetState(nearest_mine)->pos);
		}
	}
	Cargo_fully_free.clear();
	Cargo_busy.clear();
	cargo_metal_free.clear();
	cargo_oil_free.clear();
	cargo_temp_num=cargo_total_temp;
	All_Cargo.clear();
	return All_Cargo_temp;
}

//获取最近的元素位置,若nearindex=-1，则找不到这样的元素
int GetNear(Position pos, ElementType type,int team)//team=1 ours;team=0 enemy's;team=2 empty;team=3 all
{ 
	int i,near_index = -1,min_distance = 1000;
	for(i=0;i<INFO->element_num;i++)
	{
		const State *target = INFO->elements[i];
		if(target->type == type &&  distance_cal(target->pos, pos)<min_distance)
		{
			if(type == OILFIELD) 
			{
				near_index = target->index;
				min_distance = distance_cal(target->pos, pos);
			}
			else if(type == MINE)
			{
				near_index = target->index;
				min_distance = distance_cal(target->pos, pos);
			}
			else if(type != OILFIELD && type!=MINE)
			{
				if(team==2)//empty
				{
					if(target->team == 2)
					{
						near_index = target->index;
						min_distance = distance_cal(target->pos,pos);
					}
				}
				else if(team==1)//ours
				{
					if(target->team == INFO->team_num)
					{
						near_index = target->index;
						min_distance = distance_cal(target->pos,pos);
					}
				}
				else if(team==0)//enemy
				{
					if( (target->team != INFO->team_num) && (target->team !=2) )
					{
						near_index = target->index;
						min_distance = distance_cal(target->pos,pos);
					}
				}
				else//all
				{
					near_index = target->index;
					min_distance = distance_cal(target->pos, pos);
				}
			}
			else ;
		}
	}
	return near_index;
}

//寻找a经过b到c的最短路径，返回满足此条件的b(cargo专用，有target防重)。若index为-1则找不到这样的目标。
int a_near_b_near_c(int operand_index,ElementType eletype,int destination_index)
{
	int near_index=-1,dis=10000,dis_temple,index_temple=-1;
	int index_a=operand_index,index_c=destination_index;
	int index_b;
	int flag=0;
	for(int i=0;i<INFO->element_num;i++)//初始化
	{
		if(INFO->elements[i]->type == eletype)
		{
			for (unsigned int j = 0; j < SourceList.size(); j++)
			{
				if (SourceList[j].index == INFO->elements[i]->index)
				{
					if (SourceList[j].source > 0)
						index_temple = INFO -> elements[i] -> index;
					break;
				}
			}
			if (index_temple!=-1)
				break;
		}
	}

	for(int i=0;i<INFO->element_num;i++)
	{
		if(INFO->elements[i]->type == eletype)
		{
			index_b=INFO->elements[i]->index;
			if(distance(index_b,enemy_base_index) <= 10)//离对方远一点
				flag=1;
			for (unsigned int j = 0; j < SourceList.size(); j++)
			{
				if (SourceList[j].index == index_b)
				{
					if (SourceList[j].source <= 0)
						flag = 1;
					break;
				}
			}
			for(int j=0;j<target_num;j++)
			{
				if(index_b == Target_Indexes[j])
				{
					if(distance(index_a,index_b)+distance(index_b,index_c) > dis - 20)
						flag=1;
					break;
				}
			}
			if(flag==1)
			{
				flag=0;
				continue;
			}
			else
			{
				if( (dis_temple=distance(index_a,index_b)+distance(index_b,index_c)) < dis )
				{
					dis=dis_temple;
					near_index=index_b;
				}
			}
		}
	}

	if(near_index == -1)
		near_index=index_temple;
	if(index_temple == -1)
		near_index=our_base_index;
	return near_index;
}

//两点间距离计算函数
int distance_cal(Position pos1, Position pos2)
{
	return abs(pos1.x - pos2.x) + abs(pos1.y - pos2.y);
}

//两点间距离按规则返回最小值函数，基地与基地计算效率低
int distance(int index1,int index2)
{
	//遍历基地的3x3坐标，找到距离最小值
	int dis = 100000, dbt;
	Position center1, center2;
	if( (GetState(index1) -> type == BASE) && (GetState(index2) -> type != BASE) )//如果1是基地，2不是
	{
		center1 = GetState(index1) -> pos;
		for(int i = center1.x - 1 ; i <= center1.x + 1 ; i++)
			for(int j = center1.y - 1 ; j <= center1.y + 1 ; j++)
			{
				Position point = { i, j, 0 };
				if( (dbt = distance_cal(point, GetState(index2) -> pos)) < dis)
					dis = dbt;
			}
	}

	else if( (GetState(index1) -> type != BASE) && (GetState(index2) -> type == BASE) )//如果1不是基地，2是
	{
		center2 = GetState(index2) -> pos;
		for(int i = center2.x - 1 ; i <= center2.x + 1 ; i++)
			for(int j = center2.y - 1 ; j <= center2.y + 2 ; j++)
			{
				Position point = { i, j, 0 };
				if( (dbt = distance_cal(point, GetState(index1) -> pos)) < dis)
					dis = dbt;
			}
	}

	else if( (GetState(index1) -> type == BASE) && (GetState(index2) -> type == BASE) )//如果两个都是基地
	{
		center1 = GetState(index1) -> pos;
		center2 = GetState(index2) -> pos;
		for(int i = center1.x - 1 ; i <= center1.x + 1 ; i++)
			for(int j = center1.y - 1 ; j <= center1.y + 1 ; j++)
				for(int m = center2.x - 1 ; m <= center2.x + 1 ; m++)
					for(int n = center2.y - 1 ; n <= center2.y + 2 ; n++)
					{
						Position point1 = { i, j, 0 }, point2 = { m, n, 0 };
						if( (dbt = distance_cal(point1, point2)) < dis)
							dis = dbt;
					}
	}

	else if( (GetState(index1) -> type != BASE) && (GetState(index2) -> type != BASE) )//如果两个都不是基地
	{
		dis = distance_cal(GetState(index1) -> pos, GetState(index2) -> pos);
	}
	return dis;
}

//判断某个元素是否存活
int if_alive(int operand)
{
	for(int i=0;i<INFO->element_num;i++)
		if(INFO->elements[i]->index == operand)
			return 1;
	return 0;
}

int GetFireRange(ElementType type)
{
	if(type == SUBMARINE)
		return 0;
	else if(type == CARGO || type == CARRIER || type == DESTROYER)
		return 1;
	else if(type == BASE || type == FORT || type == OILFIELD || type == MINE)
		return 1;
	else if(type == SCOUT || type == FIGHTER)
		return 2;
	else
		return -1;
}

//根据Info()初始化当时的敌人
void enemy_init()
{
	//敌方单位，不包括矿场和油田和无主据点和基地
	enemy_num =0;
	for(int i=0;i<Info()->element_num;i++)
		if(Info()->elements[i]->team != Info()->team_num  
			&& Info()->elements[i]->type != MINE 
			&& Info()->elements[i]->type != OILFIELD 
			&& Info()->elements[i]->type != BASE
			&& Info()->elements[i]->team != 2)
		{
			enemy_num += 1;
			Enemy_Indexes.push_back(i);
		}	
	enemy_element = new State[enemy_num];//delete []enemy_element;
	for(int i=0; i<enemy_num; i++)
	{
		enemy_element[i] = *(INFO->elements[Enemy_Indexes[i]]);
	}
	Enemy_Indexes.clear();
}

//根据Info()初始化我方状况
void our_init()
{
	base_ask_for_help = false;
	find_submarine = false;
	BaseC.attack = false;BaseC.attack_index=-1;BaseC.fix=false;BaseC.fix_index=-1;BaseC.supply=false;BaseC.supply_index=-1;
	help_num = 0;
	target_num = 0;
	pos_time = 0;
	attack_num = 0;
	dis_go_num = 0;
	//我方单位，不包括据点和基地
	our_num = 0;
	for(int i=0;i<Info()->element_num;i++)
	{
		if( (Info()->elements[i]->team == Info()->team_num) 
			&& (Info()->elements[i]->type != FORT)
			&& (Info()->elements[i]->type != BASE) )
		{
			our_num+=1;
			Our_Indexes.push_back(i);
		}
		if( INFO->elements[i]->type == MINE || INFO->elements[i]->type == OILFIELD)
		{
			if (INFO->elements[i]->visible == true)
			{
				for (unsigned int j = 0; j < SourceList.size(); j++)
				{
					if (SourceList[j].index == INFO->elements[i]->index)
					{
						SourceList[j].source = (INFO->elements[i]->type == MINE) ? INFO->elements[i]->metal : INFO->elements[i]->fuel;
						break;
					}
				}
			}
		}
	}
	our_element = new State[our_num];//delete []our_element;
	for(int i=0;i<our_num;i++)
	{
		our_element[i]=*(INFO->elements[Our_Indexes[i]]);
	}
	Our_Indexes.clear();
	/*
	//遍历攻击序列，将已经死亡的元素踢出
	for(int i=0;i<attack_num;i++)
		if(GetState(Attack_Indexes[i])==NULL)
		{
			remove_v(Attack_Indexes,Attack_Indexes[i]);
			attack_num-=1;
		}
	*/
}

void ChangeBase(int oprend,int base)
{
	Position base_pos=GetState(base)->pos;
	switch(pos_time%9)
	{
	case 0:ChangeDest(oprend,base_pos);pos_time+=1;break;
	case 1:base_pos.x+=1;ChangeDest(oprend,base_pos);pos_time+=1;break;
	case 2:base_pos.x-=1;ChangeDest(oprend,base_pos);pos_time+=1;break;
	case 3:base_pos.y+=1;ChangeDest(oprend,base_pos);pos_time+=1;break;
	case 4:base_pos.y+=1;base_pos.x+=1;ChangeDest(oprend,base_pos);pos_time+=1;break;
	case 5:base_pos.y+=1;base_pos.x-=1;ChangeDest(oprend,base_pos);pos_time+=1;break;
	case 6:base_pos.y-=1;ChangeDest(oprend,base_pos);pos_time+=1;break;
	case 7:base_pos.y-=1;base_pos.x+=1;ChangeDest(oprend,base_pos);pos_time+=1;break;
	case 8:base_pos.y-=1;base_pos.x-=1;ChangeDest(oprend,base_pos);pos_time+=1;break;
	default:break;
	}
}

void ChangeDefend(int oprend,int target)
{
	Position base_pos, my = GetState(our_base_index)->pos, emy = GetState(target)->pos;
	base_pos.x = my.x + ((emy.x - my.x >= 15 || emy.x - my.x <= -15) ? (emy.x - my.x) / 5 : (emy.x - my.x >3 ? 1 : (emy.x - my.x < 3 ? -1 : 0)));
	base_pos.y = my.y + ((emy.y - my.y >= 15 || emy.y - my.y <= -15) ? (emy.y - my.y) / 5 : (emy.y - my.y >3 ? 1 : (emy.x - my.x < 3 ? -1 : 0)));
	base_pos.z = (my.z + emy.z)/2;
	switch(pos_time%9)
	{
	case 0:ChangeDest(oprend,base_pos);pos_time+=1;break;
	case 1:base_pos.x+=1;ChangeDest(oprend,base_pos);pos_time+=1;break;
	case 2:base_pos.x-=1;ChangeDest(oprend,base_pos);pos_time+=1;break;
	case 3:base_pos.y+=1;ChangeDest(oprend,base_pos);pos_time+=1;break;
	case 4:base_pos.y+=1;base_pos.x+=1;ChangeDest(oprend,base_pos);pos_time+=1;break;
	case 5:base_pos.y+=1;base_pos.x-=1;ChangeDest(oprend,base_pos);pos_time+=1;break;
	case 6:base_pos.y-=1;ChangeDest(oprend,base_pos);pos_time+=1;break;
	case 7:base_pos.y-=1;base_pos.x+=1;ChangeDest(oprend,base_pos);pos_time+=1;break;
	case 8:base_pos.y-=1;base_pos.x-=1;ChangeDest(oprend,base_pos);pos_time+=1;break;
	default:break;
	}
}
void distance_to_go(int target,int distance,MapType type)
{
	Position pos = GetState(target) ->pos;
	Position pos_array = pos;
	for(int x = -distance; x <= distance; x++)
	{
		int y = distance - abs(x);
		int mapx = pos.x + x;
		int mapy = pos.y + y;
		if(mapx < 0)
			mapx = 0;
		if(mapx > INFO->x_max - 1)
			mapx = INFO->x_max - 1;
		if(mapy < 0)
			mapy = 0;
		if(mapy > INFO->y_max - 1)
			mapy = INFO->y_max - 1;
		if(Map(mapx,mapy) == type)
		{
			pos_array.x = mapx;
			pos_array.y = mapy;
			Distance_Go.push_back(pos_array);
			dis_go_num+=1;
		}
		if(Map(mapx,mapy) == type)
		{
			pos_array.x = mapx;
			pos_array.y = mapy;
			Distance_Go.push_back(pos_array);
			dis_go_num+=1;
		}
	}
}

int hurt_by_distance( ElementType typeA , ElementType typeB , int distance, int def)//def:是否计算防御
{
	int hurt = 1000;
	int modifiedAttack[2] = { 0, 0 }, fire_range = 0, attack[2] = { 0, 0 }, defence[2] = { 0, 0 } , damage[2] = { 0, 0 };
	fire_range = kProperty[typeA].fire_ranges[kProperty[typeB].level];
	attack[FIRE] = kProperty[typeA].attacks[FIRE];
	attack[TORPEDO] = kProperty[typeA].attacks[TORPEDO];
	defence[FIRE] = kProperty[typeB].defences[FIRE];
	defence[TORPEDO] = kProperty[typeB].defences[TORPEDO];
	
	if( typeA != CARRIER ) 
		modifiedAttack[FIRE] = int((1 - float(distance - fire_range / 2) / (fire_range + 1)) * attack[FIRE]);
	else
		modifiedAttack[FIRE] = attack[FIRE];
	if( typeA != CARRIER )
		modifiedAttack[TORPEDO] = int((1 - float(distance - fire_range / 2) / (fire_range + 1)) * attack[TORPEDO]);
	else
		modifiedAttack[TORPEDO] = attack[TORPEDO];
	if( defence[FIRE] == -1 || (damage[FIRE] = modifiedAttack[FIRE] - /*def **/ defence[FIRE]) < 0 )
		damage[FIRE] = 0;
	if( defence[TORPEDO] == -1 || (damage[TORPEDO] = modifiedAttack[TORPEDO] - /*def **/ defence[TORPEDO]) < 0 )		
		damage[TORPEDO] = 0;
	hurt = damage[FIRE] + damage[TORPEDO];
	return hurt;
}
