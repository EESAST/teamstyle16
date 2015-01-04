#include "basic.h"
#include<iostream>
#include<cmath>
#include<vector>


// Remove this line if you konw C++, and don't want a dirty namespace
using namespace teamstyle16;
using std::vector;
using std::cout;
using std::endl;

#define INF 1000
const char * GetTeamName()
{
    return "teamstyle16";  // Name of your team
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
int if_command(int i,CommandType type,ElementType target = kElementTypes);//判断对该成员的该命令是否可以下达（如果已经下达更高级的指令则不可）
void Cargo_Supply(int index);//对于回合开始时相距为1的单位进行补给
void MoveCargo(int i);//对于程序最后仍然没有命令的运输船，去资源点收集，或者回基地补给，或者呆在前线
void Attack(int index);//寻找射程内的基地或据点，或选择血量最少的单位攻击,攻击后运动到距敌人恰好射程的地方
void BaseAct(int index);//基地的维修，生产
void BaseProduce(int index);///基地的生产
void Forward(int index);////寻找最近的敌军前进，如果是运输船
void Difference(int index);//判断刚被生产出来的时候单位的从属


const GameInfo *INFO = Info();
int enemy_num,DISTANCE;
State * enemy_element;
int (*command)[2];  //本回合发出的指令，一个效果指令，一个移动指令
double parameter[4];
int FormerElement[100];  //上回合本方单位的index数组
int cargo_index[2];  //两个补给基地的     初始化为-2，-1表示在生产
int resource_defender[2];  //守在资源点潜艇，初始化为-2，-1表示在生产
int base_defender[2];      //留守基地的潜艇，初始化为 -2，-1表示在生产
Position origin_target[5];  //中三路驱逐舰+潜艇+飞机*2+侦察机,两侧飞机*2+侦察机  的目的地
int produce_state[2];   //第一个表示路，中路，中上，中下，上，下分别为0-4
						//第二个表示生产情况，生产顺序为潜艇，驱逐舰，飞机，飞机，侦察机
						//初始化为-1，0
int unit_union[4];   //即将生产出来的潜艇，驱逐舰，飞机，侦察机从属的集团，初始化为0 ；对于飞机，初始化为1，变为-1，变为2,变为-2等
vector<int>Enemy_Indexes;


void AIMain()
{  
	TryUpdate();
	//回合开始时的初始化
	init(); 
	for(int i=0;i<INFO->element_num;i++)
		if(GetState(INFO->elements[i])->team == INFO->team_num)
		{
			const State *Element = GetState(INFO->elements[i]);
			Supply_Repair(i); //该补给或维修就去补给，维修
			Difference(i);    //判断刚出现的单位的从属

			//TryUpdate();
			//enemy_init();
			Attack(i);        //攻击

			if(Element->type == BASE)
				BaseAct(i);     //基地维修，补给，生产
			else if(Element->type == CARGO)
			{
				Cargo_Supply(i);   //运输舰补给
				MoveCargo(i);      //运输舰运动
			}	
			else ;

			Forward(i);       //前进
		}

	for(int i=0;i<Info()->element_num;i++)   //记录一回合结束时的单位，方便下一回合确定新生产出来的单位
		if(GetState(Info()->elements[i])->team == Info()->team_num)
			FormerElement[i] = Info()->elements[i];
	delete []enemy_element;	
}

//初始化定义各种量的函数
void init()
{
	if(INFO->round == 1)
	{
		parameter[0] = 0.3; //血量
		parameter[1] = 0.3; //金属
		parameter[2] = 0.3; //燃料
		parameter[3] = 0.4; //弹药

		cargo_index[0] = -2;  //补给基地的运输舰
		cargo_index[1] = -2;

		resource_defender[0] = -2;   //护卫资源的飞机
		resource_defender[1] = -2;

		base_defender[0] = -2;  //防守基地的潜艇
		base_defender[1] = -2;

		produce_state[0] = -1;   //生产情况
		produce_state[1] = 0;

		unit_union[0] = 0;   //生产出来的战斗单位从属的集团
		unit_union[1] = 0;
		unit_union[2] = 0;
		unit_union[3] = 0;

		//初始目的地
		origin_target[0].x = INFO->x_max/2;   //中路目的地
		origin_target[0].y = INFO->y_max/2;
		origin_target[0].z = 1;

		for(int i = 0; i <= 1; i++)        //中上，中下路目的地
		{
			origin_target[1 + i].x = INFO->x_max/2 + (2 * i - 1)*INFO->x_max/4; 
			origin_target[1 + i].y = INFO->y_max/2 - (2 * i - 1)*INFO->y_max/4; 
			origin_target[1 + i].z = 1;
		}

		for(int i = 0; i <= 1; i++)  //上下路目的地
		{
			origin_target[3 + i].x = min(INFO->x_max/2 + (2 * i - 1)*INFO->x_max/2, INFO->x_max - 1); 
			origin_target[3 + i].y = min(INFO->y_max/2 - (2 * i - 1)*INFO->y_max/2, INFO->y_max - 1); 
			origin_target[3 + i].z = 1;
		}
	}
	command = new int[INFO->element_num][2];    //命令列表初始化
	for(int i = 0; i<INFO->element_num; i++)
		for(int j=0;j<2;j++)
			command[i][j] = -1; 
	enemy_init();
}

//根据Info()初始化当时的敌人
void enemy_init()
{
	//敌方单位构建包括敌方单位和无主据点，不包括矿场和油田
	enemy_num =0;
	for(int i=0;i<Info()->element_num;i++)
		if(GetState(Info()->elements[i])->team != Info()->team_num  
			&& GetState(Info()->elements[i])->type != MINE && GetState(Info()->elements[i])->type != OILFIELD)
		{
			enemy_num += 1;
			Enemy_Indexes.push_back(i);
		}	
	enemy_element = new State[enemy_num];
	for(int i=0; i<enemy_num; i++)
		{
			enemy_element[i] = *GetState(Info()->elements[Enemy_Indexes[i]]);
		}
	Enemy_Indexes.clear();
}

/////该补充燃料或者弹药的时候，或者血量太少需要回基地维修
void Supply_Repair(int i)
{
	const State *Element = GetState(INFO->elements[i]);
	if(Element->health < parameter[0] * kElementInfos[Element->type].health_max 
		&& Element->health != -1
		&& if_command(i, RETURNBASE))
	{
		ChangeDest(Element->index, GetState(INFO->elements[GetBase(INFO->team_num)])->pos);
		command[i][1] = RETURNBASE;
	}
	else if((Element->ammo <  parameter[1]*kElementInfos[Element->type].ammo_max && Element->ammo != -1) ||   //金属不足
			Element->fuel < parameter[2]*kElementInfos[Element->type].fuel_max)          //燃料不足
	{
		int cargo_index = GetNear(Element->pos, CARGO);
		if(cargo_index >= 0)
		{
			if(if_command(cargo_index, CARGOSUPPLY))
			{
				ChangeDest(INFO->elements[cargo_index], Element->pos);
				command[cargo_index][1] = CARGOSUPPLY;	
			}
			else
			{
				ChangeDest(Element->index, GetState(INFO->elements[cargo_index])->pos);
				command[i][1] = CARGOSUPPLY;
			}
		}
		else
		{
			ChangeDest(Element->index, GetState(INFO->elements[GetBase(INFO->team_num)])->pos);
			command[i][1] = RETURNBASE;
		}
		
	}
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
int if_in(int i,int *a,int len)
{
	for(int j=0;j<len;j++)
		if(i==a[j])
			return 1;
	return 0;
}

//获得己方或对方基地的索引
int GetBase(int team)
{
	int i;
	for(i=0; i<INFO->element_num; i++)
		if(GetState(INFO->elements[i])->type == BASE && GetState(INFO->elements[i])->team == team)
			return i; 
	return -1;
}

//两点之间的距离
int distance(Position pos1, Position pos2)
{
	return abs(pos1.x - pos2.x) + abs(pos1.y - pos2.y);
}

//返回到基地的距离
int DisToBase(Position pos1, Position pos2)  //pos2表示基地左上角
{
	int min = 1000;
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
	}
}

//判断某单位是否仍存活
int if_alive(int operand)
{
	for(int i=0;i<INFO->element_num;i++)
		if(INFO->elements[i] == operand)
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
	int i,near_index = -1,min_distance = 10000;
	for(i=0;i<INFO->element_num;i++)
	{
		const State *target = GetState(INFO->elements[i]);
		if(target->type == type &&  distance(target->pos, pos)<min_distance)
		{
			if( type == OILFIELD && target->fuel >= kElementInfos[CARGO].fuel_max) //油田还有石油
			{
				near_index = i;
				min_distance = distance(target->pos, pos);
			}
			else if(type == MINE  && target->metal >= kElementInfos[CARGO].metal_max)//保证矿场还有金属
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
int if_command(int i,CommandType type,ElementType target)
{
	const State *element = GetState(INFO->elements[i]);
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
				|| element->type == SUBMARINE)
				return 0;
			return 1;
		}
		else if(target == kElementTypes)
			return 1;
		else if((kElementInfos[element->type].attacks[0] <= kElementInfos[target].defences[0] 
					|| kElementInfos[target].defences[0] == -1)
			&& (kElementInfos[element->type].attacks[1] <= kElementInfos[target].defences[1]
					|| kElementInfos[target].defences[1] == -1))
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
	element = GetState(INFO->elements[index]);
	for(int j=0;j<INFO->element_num;j++)
	{
		target = GetState(INFO->elements[j]);
		if(distance(element->pos,GetState(INFO->elements[j])->pos) <= 1 
			&& index!=j 
			&& target->team == INFO->team_num
			&& target->type != CARGO)
		{
			if(target->type > OILFIELD)command[index][0] = SUPPLYUNIT;
			else if(target->type == FORT)command[index][0] = SUPPLYFORT;
			else if(target->type == BASE)command[index][0] = SUPPLYBASE;
			else continue;
			Supply(INFO->elements[index], INFO->elements[j],INF,INF,INF);
			return;
		}
	}	
}

//对于程序最后仍然没有命令的运输船，去资源点收集，或者回基地补给，或者呆在前线
void MoveCargo(int i)        
{
	const State *Element = GetState(INFO->elements[i]);
	if(command[i][1] == -1 )
	{
		if(Element->fuel < 0.5 * kElementInfos[CARGO].fuel_max)          //燃料不足
		{   
			ChangeDest(Element->index,GetState(INFO->elements[GetNear(Element->pos,OILFIELD)])->pos);
			command[i][1] = CARGOSUPPLY;
		}
		else if(Element->metal < 0.5 * kElementInfos[CARGO].metal_max)     //金属不足
		{	
			ChangeDest(Element->index,GetState(INFO->elements[GetNear(Element->pos,MINE)])->pos);
			command[i][1] = CARGOSUPPLY;
		}
		else if(if_in(Element->index, cargo_index, 2))         //是补给基地的船
		{	
			ChangeDest(Element->index,GetState(INFO->elements[GetBase(INFO->team_num)])->pos);   
			command[i][1] = RETURNBASE;
		}	
		else                   //是补给前线的船
			Forward(i);
	}
}

//寻找射程内的基地或据点，或选择血量最少的单位攻击,攻击后运动到距敌人恰好射程的地方
void Attack(int index)
{
	const State	*Element = GetState(INFO->elements[index]);
	int health = 1000,enemy_index = -1;
	if(!if_command(index,ATTACKBASE))
		return;
	for(int i=0;i<enemy_num;i++)
	{
		State *enemy = &enemy_element[i];
		cout<<Element->type<<" "<<enemy->type<<" "<<kElementInfos[enemy->type].level<<endl;
		cout<<distance(Element->pos, enemy->pos)<<" "<<kElementInfos[Element->type].fire_ranges[kElementInfos[enemy->type].level]<<endl;
		cout<<endl;
		if(enemy->type == BASE && enemy->team == 1-INFO->team_num
			&& DisToBase(Element->pos, enemy->pos) <= kElementInfos[Element->type].fire_ranges[SURFACE]
			&& if_command(index, ATTACKBASE, BASE))
		{												//敌方基地
			AttackUnit(Element->index,enemy->index);
			command[index][0] = ATTACKBASE;
			if(if_command(index, FORWARD))
			{
				ChangeDest(Element->index,
					minus(Element->pos,enemy->pos,kElementInfos[Element->type].fire_ranges[SURFACE]));
				command[index][1] = FORWARD;
			}
			return;
		}
		if(distance(Element->pos, enemy->pos) <= kElementInfos[Element->type].fire_ranges[kElementInfos[enemy->type].level])
		{	
			if(enemy->type == FORT
				&& if_command(index,ATTACKFORT,FORT))                                     //据点
			{
				AttackUnit(Element->index,enemy->index);
				command[index][0] = ATTACKFORT;
				if(if_command(index,FORWARD))
				{
					ChangeDest(Element->index,
						minus(Element->pos,enemy->pos,kElementInfos[Element->type].fire_ranges[SURFACE]));
					command[index][1] = FORWARD;
				}
				return;
			}
			if(health > enemy->health 
				&& (enemy_index == -1 
					|| (enemy_index >=0 && 
					if_command(index, ATTACKUNIT, ElementType(enemy_element[enemy_index].type)))))   
			{
				health=enemy->health;					//寻找血量最少的敌方单位
				enemy_index=i;
			}
		}
	}
	if(enemy_index != -1)
	{
		AttackUnit(Element->index, enemy_element[enemy_index].index);
		command[index][0] = ATTACKUNIT;
		if(if_command(index,FORWARD))
		{
			ChangeDest(Element->index, minus(Element->pos,enemy_element[enemy_index].pos,
				kElementInfos[Element->type].fire_ranges[kElementInfos[enemy_element[enemy_index].type].level]));
			command[index][1] = FORWARD;
		}
	}
}

//基地的维修，生产
void BaseAct(int index)
{
	const State *target;
	for(int i=0;i<INFO->element_num;i++)
	{
		target = GetState(INFO->elements[i]);
		if(DisToBase(target->pos, GetState(INFO->elements[index])->pos)<=1 
			&& target->index != INFO->elements[index]
			&& target->team == INFO->team_num)
		{
			if(target->health < parameter[0]*kElementInfos[target->type].health_max
				&& target->health != -1)
			{
				if(if_command(index, FIX))
				{	
					Fix(INFO->elements[index], INFO->elements[i]);
					command[index][0] = FIX;
					return;
				}
			}
			else if(target->fuel < parameter[2]*kElementInfos[target->type].fuel_max
					|| target->ammo < parameter[3]*kElementInfos[target->type].ammo_max)
			{
				if(if_command(index, SUPPLYUNIT))
				{	
					Supply(INFO->elements[index], INFO->elements[i], INF, INF, INF);
					command[index][0] = SUPPLYUNIT;
					return;
				}
			}
		}
	}
	BaseProduce(index);   //生产命令
}

///基地的生产
void BaseProduce(int index)
{
	State Element = *GetState(INFO->elements[index]);
	if(!if_command(index, PRODUCE))return;
	for(int i=0; i<2;i++)        //判断补给基地的两条船是否存活，不存活则直接生产运输舰
	{
		if(cargo_index[i] == -2 || (cargo_index[i]>=0 && !if_alive(cargo_index[i])))
		{
			Produce(CARGO);
			command[index][0] = PRODUCE;
			cargo_index[i] = -1;
			return;
		}
	} 

	//中间三路的生产
	if(produce_state[0]<3 && produce_state[1]<= 4)         
		switch(produce_state[1])
		{
			case 0: Produce(SUBMARINE);
					command[index][0] = PRODUCE;
					produce_state[1] += 1;
					return;
			case 1: Produce(DESTROYER);
					command[index][0] = PRODUCE;
					produce_state[1] += 1;
					return;
			case 2:
			case 3: Produce(FIGHTER);
					command[index][0] = PRODUCE;
					produce_state[1] += 1;
					return;
			case 4: Produce(SCOUT);
					command[index][0] = PRODUCE;
					produce_state[0] += 1; 
					return;
			default:;
		}

	 //上下路的生产
	if(produce_state[0]>= 3 && produce_state[0]<5)        
		switch(produce_state[1])
		{
			case 0: 
			case 1:
			case 2:
			case 3: Produce(FIGHTER);
					command[index][0] = PRODUCE;
					produce_state[1] += 1;
					return;
			case 4: Produce(SCOUT);
					command[index][0] = PRODUCE;
					produce_state[0] = produce_state[0] == 4 ? 0 :4;
					produce_state[1] = 0;
					return;
			default:;
		}

	 //在中路和中下路生产完后各生产一条运输船
	if((produce_state[0] == 1 || produce_state[0] == 3 )&& produce_state[1] == 5)    
	{
		Produce(CARGO);
		command[index][0] = PRODUCE;
		produce_state[0] += 1;
		produce_state[1] = 0;
		return;
	}

	//在中上路，和上路生产完后，生产战斗机去防守资源
	if((produce_state[0] == 2 || produce_state[0] == 4)&& produce_state[1] == 5 
		&& (resource_defender[1] == -2     //防守资源的单位没有或者已经死了
			|| (resource_defender[1] >= 0
			&& ! if_alive(resource_defender[1]))))
	{															
		int i = resource_defender[0] == -2 ? 0 : 1;
		Produce(FIGHTER);
		command[index][0] = PRODUCE;
		resource_defender[i] = -1;
		produce_state[0] += 1;
		if(produce_state[0] == 5)produce_state[0] = 0;
		produce_state[1] = 0;
		return;
	}

	//生产潜艇防守基地
	if(INFO->round > 5 && produce_state[0] == 0 && produce_state[1] == 5
		&& (base_defender[1] == -2     //防守资源的单位没有或者已经死了
			|| (base_defender[1] >= 0
			&& ! if_alive(base_defender[1]))))
	{
		int i = base_defender[0] == -2 ? 0 : 1;
		Produce(SUBMARINE);
		command[index][0] = PRODUCE;
		base_defender[i] = -1;
		produce_state[0] += 1;
		produce_state[1] = 0;
	}

}

////寻找最近的敌军前进，如果是运输船就打
void Forward(int index)
{
	State Element = *GetState(INFO->elements[index]);
	int Distance=1000; 
	Position target;
	if(!if_command(index, FORWARD))return;
	if(Element.type == BASE || Element.type == FORT)
		return;
	else if(Element.type == CARGO && ! if_in(Element.index, cargo_index, 2))
	{
		for(int i=0;i<INFO->element_num;i++)
			if(distance(Element.pos,GetState(INFO->elements[i])->pos) < Distance 
				&& index!=i 
				&& GetState(INFO->elements[i])->team == INFO->team_num)
			{
				Distance =  distance(Element.pos,GetState(INFO->elements[i])->pos);
				target = GetState(INFO->elements[i])->pos;
			}
	}
	else
	{
		for(int i=0;i<enemy_num;i++)
			if(distance(Element.pos, enemy_element[i].pos) < Distance)
			{
				Distance = distance(Element.pos,enemy_element[i].pos);
				target = enemy_element[i].pos;
			}
	}
	if(Distance != 1000)
	{
		ChangeDest(Element.index, target);
		command[index][1] = FORWARD;	
	}
}

//判断刚被生产出来的时候单位的从属
void Difference(int index)
{
	State Element = *GetState(INFO->elements[index]);
	for(int i=0;i<INFO->element_num - enemy_num;i++)  //判断它是否是原来就存在的
		if(index == FormerElement[i])
			return ;
	switch(Element.type)
	{
	case BASE:break;
	case FORT:break;
	case CARGO:
		for(int i=0;i<=1;i++)     //判断是否为刚生产出来的补给基地的运输船
			if(cargo_index[i] == -1 && DisToBase(Element.pos,GetState(INFO->elements[GetBase(INFO->team_num)])->pos) <= 1)
			{
				cargo_index[i] = Element.index;
				return;
			}
		return;
	case SUBMARINE:
		for(int i=0;i<2;i++)
			if(base_defender[i] == -1 && DisToBase(Element.pos,GetState(INFO->elements[GetBase(INFO->team_num)])->pos) <= 1)
			{
				base_defender[i] = Element.index;
				return;
			}
		ChangeDest(Element.index,origin_target[unit_union[0]]);
		if(unit_union[0] == 4)
			unit_union[0] = 0;
		else unit_union[0] += 1;
		return;
	case DESTROYER:
		ChangeDest(Element.index,origin_target[unit_union[1]]);
		if(unit_union[1] == 4)
			unit_union[1] = 0;
		else unit_union[1] += 1;
		return;
	case FIGHTER:
		ChangeDest(Element.index,origin_target[abs(unit_union[1])-1]);
		if(unit_union[2] > 0)
			unit_union[2] = -unit_union[2];
		else if(unit_union[2] == -5) 
			unit_union[2] = 1;
		else 
			unit_union[2] = -unit_union[2] + 1;
		return;
	case SCOUT:
		ChangeDest(Element.index,origin_target[abs(unit_union[1])-1]);
		if(unit_union[3] == 4)
			unit_union[3] = 0;
		else unit_union[3] += 1;
		return;
	}
}