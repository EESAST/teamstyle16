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
void DefineUnit();

const GameInfo *INFO = Info();
int enemy_num;
State * enemy_element;
int (*command)[2];  //本回合发出的指令，一个效果指令，一个移动指令
double parameter[4];

int BaseMineCargo = -2, BaseFuelCargo = -2;  //三个补给基地的     初始化为-2，-1表示在生产
int base_defender[3] = {-2, -2, -2};      //留守基地的驱逐舰，初始化为 -2，-1表示在生产

vector<int>Enemy_Indexes;

int DISTANCE_NEAR_BASE = 20;
int BEGINGAME = 0;
Position BaseDefendPos;

void AIMain()
{  
	//TryUpdate();
	//回合开始时的初始化
	init();

	DefineUnit();
	for(int i=0;i<INFO->element_num;i++)
		if(GetState(INFO->elements[i])->team == INFO->team_num)
		{
			const State *Element = GetState(INFO->elements[i]);
			Supply_Repair(i); //该补给或维修就去补给，维修

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
	Update();
	delete []enemy_element;
}

//初始化定义各种量的函数
void init()
{
	if(BEGINGAME == 0)
	{
		parameter[0] = 0.3; //血量
		parameter[1] = 0.3; //金属
		parameter[2] = 0.3; //燃料
		parameter[3] = 0.4; //弹药
	
		const State *Base = GetState(INFO->elements[GetBase(INFO->team_num)]);
		if(Map(Base->pos.x-3, Base->pos.y-1) == OCEAN)
		{
			BaseDefendPos.x = Base->pos.x-3;
			BaseDefendPos.y = Base->pos.y-1;
		}	
		else if(Map(Base->pos.x-3, Base->pos.y+1) == OCEAN)
		{
			BaseDefendPos.x = Base->pos.x-3;
			BaseDefendPos.y = Base->pos.y+1;
		}	
		else if(Map(Base->pos.x+5, Base->pos.y-1) == OCEAN)
		{
			BaseDefendPos.x = Base->pos.x+5;
			BaseDefendPos.y = Base->pos.y-1;
		}	
		else
		{
			BaseDefendPos.x = Base->pos.x+5;
			BaseDefendPos.y = Base->pos.y+1;
		}	
		BaseDefendPos.z = 1;

		BEGINGAME = 1;
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
		return;
	}
	else if((Element->ammo <  parameter[1]*kElementInfos[Element->type].ammo_max && Element->ammo != -1) ||   //金属不足
			Element->fuel < parameter[2]*kElementInfos[Element->type].fuel_max
			&& Element->type != CARGO)          //燃料不足
	{
		//回基地
		int base_index = GetBase(INFO->team_num);
		if(DisToBase(Element->pos, GetState(INFO->elements[base_index])->pos) < DISTANCE_NEAR_BASE)
			if(if_command(i, RETURNBASE))
			{
				ChangeDest(INFO->elements[i], GetState(INFO->elements[base_index])->pos);
				command[i][1] = RETURNBASE;	
				return;
			}

		int cargo_index = GetNear(Element->pos, CARGO);
		if(cargo_index >= 0  && cargo_index != BaseMineCargo && cargo_index != BaseFuelCargo)
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
				|| element->type == SUBMARINE
				|| target == CARGO)
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
	if(!if_command(index, SUPPLYUNIT))
		return;
	for(int j=0;j<INFO->element_num;j++)
	{
		target = GetState(INFO->elements[j]);
		if(target->type != CARGO &&
			distance(element->pos,GetState(INFO->elements[j])->pos) <= 1 
			&& target->team == INFO->team_num)
		{
			if(target->type == BASE)
			{
				if((target->fuel + 0.5*element->fuel < kElementInfos[BASE].fuel_max
					&& element->fuel >= 0.4*kElementInfos[element->type].fuel_max)
				|| (target->metal + 0.7*element->metal < kElementInfos[BASE].metal_max
					&& element->metal >= 0.4*kElementInfos[element->type].metal_max))
				{
					//Supply(INFO->elements[index], target->index, 0, element->fuel, element->fuel);
					command[index][0] = SUPPLYBASE;
					Supply(INFO->elements[index], target->index, (int)(0.7*element->fuel), 0, element->metal);
				}

				if(element->index == BaseMineCargo)     //金属不足
				{	
					ChangeDest(element->index,GetState(INFO->elements[GetNear(element->pos,MINE)])->pos);
					command[index][1] = RETURNBASE;
					return;
				}

				if(element->index == BaseFuelCargo)
				{
					ChangeDest(element->index,GetState(INFO->elements[GetNear(element->pos,MINE)])->pos);
					command[index][1] = RETURNBASE;
					return;
				}
				
				int MinFuel = 1000, UnitIndex = -1;
				for(int i=0; i<INFO->element_num; i++)
				{
					const State *unit = GetState(INFO->elements[i]);
					if(unit->type > OILFIELD && unit->team == INFO->team_num
						&& unit->fuel < MinFuel)
					{
						MinFuel = unit->fuel;
						UnitIndex = i;
					}
				}
				if(UnitIndex != -1)
				{
					ChangeDest(element->index, GetState(INFO->elements[UnitIndex])->pos);
					command[index][1] = RETURNBASE;
				}
			}
				
		
			else if(target->type > OILFIELD)
            {
				if((target->fuel + 0.3*element->fuel < kElementInfos[target->type].fuel_max
					&& element->fuel >= 0.4*kElementInfos[element->type].fuel_max)
				|| (target->ammo + 0.4*element->ammo < kElementInfos[BASE].ammo_max
					&& element->ammo >= 0.4*kElementInfos[element->type].ammo_max))
				{
					command[index][0] = SUPPLYUNIT;
					Supply(INFO->elements[index], INFO->elements[j], kElementInfos[target->type].fuel_max - target->fuel,
							kElementInfos[target->type].ammo_max - target->ammo, 0);
					return;
				}
            }
			//else if(target->type == FORT)command[index][0] = SUPPLYFORT;
			else continue;
		}
	}	
}

//对于程序最后仍然没有命令的运输船，去资源点收集，或者回基地补给，或者呆在前线
void MoveCargo(int i)        
{
	const State *Element = GetState(INFO->elements[i]);
	const State *base = GetState(INFO->elements[GetBase(INFO->team_num)]);
	if(if_command(i, CARGOSUPPLY))
	{
		if(Element->fuel < 0.5 * kElementInfos[CARGO].fuel_max)          //燃料不足
		{   
			ChangeDest(Element->index, GetState(INFO->elements[GetNear(Element->pos,OILFIELD)])->pos);
			command[i][1] = CARGOSUPPLY;
			return;
		}
		else if(Element->metal < 0.5 * kElementInfos[CARGO].metal_max)     //金属不足
		{	
			ChangeDest(Element->index,GetState(INFO->elements[GetNear(Element->pos,MINE)])->pos);
			command[i][1] = CARGOSUPPLY;
			return;
		}
		else;
	}

	if(if_command(i, RETURNBASE) &&
		(Element->index == BaseMineCargo || Element->index == BaseFuelCargo)
		&& ((base->metal + Element->metal < kElementInfos[BASE].metal_max
				&& Element->metal > 0.4*kElementInfos[Element->type].metal_max)
			|| (base->fuel + (int)(0.7*Element->fuel) < kElementInfos[BASE].fuel_max
				&& Element->fuel >0.6*kElementInfos[Element->type].fuel_max)))         //是补给基地的船
	{	
		ChangeDest(Element->index, base->pos);
		command[i][1] = RETURNBASE;
		return;
	}

	if(if_command(i, FORWARD))                   //是补给前线的船
	{
        int Distance = 1000;
        Position target;
        for(int j=0;j<INFO->element_num;j++)
            if(distance(Element->pos,GetState(INFO->elements[j])->pos) < Distance 
                && GetState(INFO->elements[j])->type != CARGO
                && GetState(INFO->elements[j])->team == INFO->team_num)
            {
                Distance =  distance(Element->pos,GetState(INFO->elements[j])->pos);
                target = GetState(INFO->elements[j])->pos;
            }
        if(Distance != 1000)
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
	const State	*Element = GetState(INFO->elements[index]);
	int health = 1000,enemy_index = -1;
	if(!if_command(index,ATTACKBASE))
		return;
	for(int i=0;i<enemy_num;i++)
	{
		State *enemy = &enemy_element[i];
		if(enemy->type == BASE && enemy->team == 1-INFO->team_num
			&& DisToBase(Element->pos, enemy->pos) <= kElementInfos[Element->type].fire_ranges[SURFACE]
			&& if_command(index, ATTACKBASE, BASE))
		{												//敌方基地
			AttackUnit(Element->index,enemy->index);
			command[index][0] = ATTACKBASE;
			if(if_command(index, FORWARD))
			{
				ChangeDest(Element->index, enemy->pos);
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
					ChangeDest(Element->index, enemy->pos);
					command[index][1] = FORWARD;
				}
				return;
			}
			if(health > enemy->health 
				&& (enemy_index == -1 
					|| (enemy_index >=0
					&& if_command(index, ATTACKUNIT, ElementType(enemy_element[enemy_index].type)))))   
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
		if(if_command(index,FORWARD)
			&& !if_in(INFO->elements[index], base_defender, 3))
		{
			//射程远于敌方
			if(kElementInfos[Element->type].fire_ranges[kElementInfos[enemy_element[enemy_index].type].level]
				> kElementInfos[enemy_element[enemy_index].type].fire_ranges[kElementInfos[Element->type].level])
				ChangeDest(Element->index, minus(Element->pos,enemy_element[enemy_index].pos,
					kElementInfos[Element->type].fire_ranges[kElementInfos[enemy_element[enemy_index].type].level]));
			//射程近于敌方
			else 
				ChangeDest(Element->index, enemy_element[enemy_index].pos);
			command[index][1] = FORWARD;
		}
	}
}

//基地的维修，生产
void BaseAct(int index)
{
	const State *target;

	//FIX
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
		}
	}

	//SUPPLY
	for(int i=0;i<INFO->element_num;i++)
	{
		target = GetState(INFO->elements[i]);
		if(DisToBase(target->pos, GetState(INFO->elements[index])->pos)<=1 
			&& target->index != INFO->elements[index]
			&& target->type != CARGO
			&& target->team == INFO->team_num)
		{
			if(target->fuel < parameter[2]*kElementInfos[target->type].fuel_max
					|| target->ammo < parameter[3]*kElementInfos[target->type].ammo_max)
			{
				if(if_command(index, SUPPLYUNIT))
				{	
					Supply(INFO->elements[index], INFO->elements[i], kElementInfos[target->type].fuel_max-target->fuel, 
                                                        kElementInfos[target->type].ammo_max-target->ammo, 0);
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
	if(!if_command(index, PRODUCE))return;

    if(BaseMineCargo == -2 || !if_alive(BaseMineCargo))
    {
        Produce(CARGO);
		command[index][0] = PRODUCE;
		BaseMineCargo = -1;
		return;
    }
    if(BaseFuelCargo == -2 || !if_alive(BaseFuelCargo))
    {
        Produce(CARGO);
		command[index][0] = PRODUCE;
		BaseFuelCargo = -1;
		return;
    }
    for(int i=0; i<3; i++)
        if(base_defender[i] == -2 || !if_alive(base_defender[i]))
        {
            Produce(DESTROYER);
            command[index][0] = PRODUCE;
			base_defender[i] = -1;
            return;
        }
    
	if(INFO->round%4 == 1)
	{
		Produce(FIGHTER);
		command[index][0] = PRODUCE;
		return;
	}
	else if(INFO->round%4 == 0)
	{
		Produce(DESTROYER);
		command[index][0] = PRODUCE;
		return;
	}
	else if(INFO->round%4 == 2)
	{
		Produce(CARGO);
		command[index][0] = PRODUCE;
		return;
	}
	else
	{
		Produce(SUBMARINE);
		command[index][0] = PRODUCE;
		return;
	}
}

void DefineUnit() //flag = 0,1,2对应BaseMineCargo, BaseFuelCargo, base_defender
 //i对应base_defender的标号
{
    const State *Element, *base; 
    base = GetState(INFO->elements[GetBase(INFO->team_num)]);

	for(int i=0; i<3; i++)
		if(base_defender[i] == -2 || !if_alive(base_defender[i]))
		{
			int Distance = 1000, index = -1;
			for(int j=0; j<INFO->element_num; j++)
			{
				Element = GetState(INFO->elements[j]);
				if(Element->team == INFO->team_num && Distance > distance(Element->pos, base->pos) 
					&& Element->type == DESTROYER && !if_in(Element->index, base_defender, 3))
				{
					Distance = distance(Element->pos, base->pos);
					index = INFO->elements[j];
				}
			 }
			if(Distance != 1000)
				base_defender[i] = index;
		}

	if(BaseMineCargo == -2 || !if_alive(BaseMineCargo))
	{
		int Distance = 1000, index = -1;
		for(int j=0; j<INFO->element_num; j++)
		{
			Element = GetState(INFO->elements[j]);
			if(Element->team == INFO->team_num && Distance > distance(Element->pos, base->pos) && Element->type == CARGO)
			{
				Distance = distance(Element->pos, base->pos);
				index = INFO->elements[j];
			}
		}
		if(Distance != 1000)
			BaseMineCargo = index;
	}
	if(BaseFuelCargo == -2 || !if_alive(BaseFuelCargo))
	{
		int Distance = 1000, index = -1;
		for(int j=0; j<INFO->element_num; j++)
		{
			Element = GetState(INFO->elements[j]);
			if(Element->team == INFO->team_num && Distance > distance(Element->pos, base->pos) && Element->type == CARGO)
			{
				Distance = distance(Element->pos, base->pos);
				index = INFO->elements[j];
			}
			}
		if(Distance != 1000)
			BaseFuelCargo = index;
	}
}
////寻找最近的敌军前进，如果是运输船就打
void Forward(int index)
{
	const State *Element = GetState(INFO->elements[index]);
	int Distance=1000; 
	Position target;
	if(!if_command(index, FORWARD))return;
	if(Element->type == BASE || Element->type == FORT 
        || Element->type == CARGO
		|| if_in(Element->index, base_defender, 3))
		return;
	else if(if_in(INFO->elements[index], base_defender, 3)
		&& (Element->pos.x != BaseDefendPos.x && Element->pos.y != BaseDefendPos.y))
	{
		ChangeDest(Element->index, BaseDefendPos);
		command[index][1] = FORWARD;
		return;
	}

	else
	{
		for(int i=0;i<enemy_num;i++)
			if(distance(Element->pos, enemy_element[i].pos) < Distance)
			{
				Distance = distance(Element->pos,enemy_element[i].pos);
				target = enemy_element[i].pos;
			}
	}
	if(Distance != 1000)
	{
		ChangeDest(Element->index, target);
		command[index][1] = FORWARD;	
	}
}