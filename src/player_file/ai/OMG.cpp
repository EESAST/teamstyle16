#include <cstdlib>
#include "basic.h"
#include <stdio.h>
#include<math.h>


using namespace teamstyle16;

const char * GetTeamName()
{
    return "OMG";  
}


const GameInfo *INFO = Info();
int distance(Position pos1,Position pos2);
void intialize();
void baseproduce();
void cargoact();
void movedestroyer(int i);
void movefighter(int i);
void movescout(int i);
void movesub(int i);
void attackfort();
void generalattack();
void ele_supply(int i);

int prod=0;
int fort=-1;
Position nearest_mine;
Position nearest_fuel;
Position nearest_fort;
Position center;
int fort_des;
int mybase;
int enermybase;
int des_num;
int sco_num;
int car_num;
static int cargostate[40]={0};  //CARGOSTATE有三种状态：0在基地待命，1在油田，2在矿山, 3在补给 , 4维修


void AIMain()
{
	TryUpdate();
	intialize();
	baseproduce();
	cargoact();
	generalattack();
	return;
}

int if_occupied(Position posi)
{
	int i;

	for(i=0;i<INFO->element_num;i++)
		if(INFO->elements[i]->pos.x==posi.x  &&  INFO->elements[i]->pos.y==posi.y && INFO->elements[i]->pos.z==posi.z)
			return 0;

	return 1;
}

Position findOcean(Position pos)
{
	Position sea;

	for(int i=-1; i<=1; i++)
		for(int j=-1;j<=1;j++)
		{
			if(i==0&&j==0)continue;
			sea.x = pos.x +i;
			sea.y = pos.y+j;
			if(Map(pos.x,pos.y)==OCEAN)
				return sea;
		}

	return pos;
}

Position findbaseOcean(Position pos)
{
	Position sea;
	sea.z=1;

	for(int i=-2; i<=2; i++)
		for(int j=-2;j<=2;j++)
		{
			if(abs(i)==2&&abs(j)==2)continue;
			sea.x = pos.x +i;
			sea.y = pos.y+j;
			if(Map(sea.x,sea.y)==OCEAN&&if_occupied(sea)==1)
				return sea;
		}

	return pos;
}

void nearest()
{
	int i;
	int dis;
	int fuel=0,mine=0;

	nearest_mine.x=0;
	nearest_mine.y=0;
	nearest_mine.z=0;
	dis=10000;
	
	for(i=0;i<INFO->element_num;i++)
		if(INFO->elements[i]->type==MINE
			&& INFO->elements[i]->metal>2&& distance(INFO->elements[mybase]->pos , INFO->elements[i]->pos)<dis)
		{
			nearest_mine=INFO->elements[i]->pos;
			dis=distance(INFO->elements[mybase]->pos , INFO->elements[i]->pos);
		}

	nearest_fuel.x=0;
	nearest_fuel.y=0;
	nearest_fuel.z=0;
	dis=10000;

	for(i=0;i<INFO->element_num;i++)
		if(INFO->elements[i]->type==OILFIELD 
			&& INFO->elements[i]->fuel>0  && distance(nearest_mine , INFO->elements[i]->pos)<dis)
		{
			nearest_fuel=INFO->elements[i]->pos;
			dis=distance(nearest_mine , nearest_fuel);
		}

	nearest_fort.x=0;
	nearest_fort.y=0;
	nearest_fort.z=0;
	dis=1000;
	fort=-1;

	for(i=0;i<INFO->element_num;i++)
		if(INFO->elements[i]->type==FORT && INFO->elements[i]->team!=INFO->team_num
			&& distance(INFO->elements[mybase]->pos , INFO->elements[i]->pos)<dis)
		{
			nearest_fort=INFO->elements[i]->pos;
			fort=i;
			dis=distance(INFO->elements[mybase]->pos , INFO->elements[i]->pos);
		}

	return;
}

void intialize()
{
	int i;
	INFO=Info();
	des_num=0;sco_num=0;
	center.x=INFO->x_max/2;
	center.y=INFO->y_max/2;
	mybase=-1;enermybase=-1;
	car_num=0;

	for(i=0;i<INFO->element_num;i++)
		if(INFO->elements[i]->type==BASE && (INFO->elements[i]->team)==INFO->team_num )
			mybase=i;
		else if(INFO->elements[i]->type==BASE && (INFO->elements[i]->team)!=INFO->team_num )
			enermybase=i;
		else if(mybase!=-1 && enermybase!=-1)break;

	for(i=INFO->element_num-1;i>=0;i=i-1)
	{
		if ((INFO->elements[i]->type==DESTROYER ||INFO->elements[i]->type==CARRIER)&& INFO->elements[i]->team==INFO->team_num )
			if(INFO->elements[i]->fuel<kProperty[INFO->elements[i]->type].fuel_max*0.3 ||
				INFO->elements[i]->ammo<kProperty[INFO->elements[i]->type].ammo_max*0.3)
				ele_supply(i);
			else
				movedestroyer(i);

		else if(INFO->elements[i]->type==FIGHTER && (INFO->elements[i]->team)==INFO->team_num )
					if(INFO->elements[i]->fuel<kProperty[INFO->elements[i]->type].fuel_max*0.3 ||
						INFO->elements[i]->ammo<kProperty[INFO->elements[i]->type].ammo_max*0.3)
						ele_supply(i);
					else;

		else  if(INFO->elements[i]->type==SUBMARINE && (INFO->elements[i]->team)==INFO->team_num )
					if(INFO->elements[i]->fuel<kProperty[INFO->elements[i]->type].fuel_max*0.3 ||
						INFO->elements[i]->ammo<kProperty[INFO->elements[i]->type].ammo_max*0.3)
						ele_supply(i);
					else
			movesub(i);

		else if(INFO->elements[i]->type==CARGO && (INFO->elements[i]->team)==INFO->team_num )
		{
			car_num++;
					if(INFO->elements[i]->fuel<kProperty[INFO->elements[i]->type].fuel_max*0.3 ||
						INFO->elements[i]->ammo<kProperty[INFO->elements[i]->type].ammo_max*0.3)
						ele_supply(i);
					else;
		}
		else;
	}


	nearest();
	
	return;
}

int distance(Position pos1,Position pos2)
{
	return abs(pos1.x-pos2.x)+abs(pos1.y-pos2.y);
}

void baseproduce()
{
		switch(prod%5)
		{
		case 3:
			if(INFO->elements[mybase]->fuel<80)return;
			if(car_num<=4)Produce(CARGO);
			else Produce(FIGHTER);
			prod++;
			break;
		case 1:
			if(INFO->elements[mybase]->fuel<80)return;
			Produce(CARRIER);
			prod++;
			break;
		case 2:
			if(INFO->elements[mybase]->fuel<80)return;
			Produce(FIGHTER);
			prod++;
			break;
		case 4:
			if(INFO->elements[mybase]->fuel<80)return;
			if(prod%2==0)Produce(SUBMARINE);
			else Produce(FIGHTER);
			prod++;
			break;
		case 0:  
			if(INFO->elements[mybase]->fuel<80)return;
			Produce(DESTROYER);
			prod++;
			break;
		default: break;
		}
		
	if(INFO->elements[mybase]->fuel<80)return;

	if(prod<=1)
		Produce(CARGO);
	else if(prod<=2)
	{
		Produce(CARGO);
		if(INFO->elements[mybase]->fuel<80)return;
		Produce(SUBMARINE);
		if(INFO->elements[mybase]->fuel<80)return;
		Produce(SCOUT);
	}
	else 	if(prod<5)
	{
		if(INFO->elements[mybase]->fuel<80)return;
		Produce(DESTROYER);
	}
	return ;
}

Position wposition()
{
	Position wpos;
	wpos.z=1;
	wpos.x=(int)((INFO->elements[mybase]->pos.x*3+center.x)/4);
	wpos.y=(int)((INFO->elements[mybase]->pos.y*3+center.y)/4);
	
	return wpos;
}

Position aposition(Position pos1,Position pos2,int fire_range) //pos2为ene的位置
{
	Position target,pos3;
	pos3=INFO->elements[mybase]->pos;

	target.x = pos3.x > pos2.x ? 
		pos3.x-(distance(pos3,pos2)-fire_range)/2-1 :
		pos3.x +(distance(pos3,pos2)-fire_range)/2+1;
	target.y = pos3.y > pos2.y ? 
		pos3.y-(distance(pos3,pos2)-fire_range)/2-1:
		pos3.y +((distance(pos3,pos2)-fire_range))/2+1;
	target.z = pos3.z;

	Position sea;
	for(int i=-2; i<=2; i++)
		for(int j=-2;j<=2;j++)
	{
		sea.x = target.x +i;
		sea.y = target.y+j;
		if(Map(target.x,target.y)==OCEAN && distance(sea,pos2)<fire_range && if_occupied(sea))
			return sea;
	}

	return target;	


}

int if_attack(int ene)
{
	int i;
	if(distance(INFO->elements[ene]->pos,INFO->elements[mybase]->pos)<25)return 1;
	if(INFO->elements[ene]->type==FIGHTER|| INFO->elements[ene]->type==SCOUT)
		for(i=0;i<INFO->element_num;i++)
			if(INFO->elements[i]->team==INFO->team_num 
				&& distance(INFO->elements[i]->pos,INFO->elements[ene]->pos)<=kProperty[INFO->elements[ene]->type].fire_ranges[AIR])
				return 1;

	if(INFO->elements[ene]->type==CARGO|| INFO->elements[ene]->type==DESTROYER|| INFO->elements[ene]->type==CARRIER)
		for(i=0;i<INFO->element_num;i++)
			if(INFO->elements[i]->team==INFO->team_num 
				&& distance(INFO->elements[i]->pos,INFO->elements[ene]->pos)<=kProperty[INFO->elements[ene]->type].fire_ranges[SURFACE])
				return 1;

	if(INFO->elements[ene]->type==SUBMARINE)
		for(i=0;i<INFO->element_num;i++)
			if(INFO->elements[i]->team==INFO->team_num 
				&& distance(INFO->elements[i]->pos,INFO->elements[ene]->pos)<=kProperty[INFO->elements[ene]->type].fire_ranges[UNDERWATER])
				return 1;

	return 0;
}

Level elelevel(int i)
{
	if(INFO->elements[i]->type==SCOUT || INFO->elements[i]->type==FIGHTER)
		return AIR;
	if(INFO->elements[i]->type==CARRIER || INFO->elements[i]->type==CARGO || INFO->elements[i]->type==DESTROYER)
		return SURFACE;
	if(INFO->elements[i]->type==SUBMARINE)
		return UNDERWATER;

	return SURFACE;
}

int defend2()
{
	int i,j;

	int dis=1000;
	int ene=enermybase;

	for(i=INFO->element_num-1;i>=0;i--)
		if(INFO->elements[i]->team==1-INFO->team_num)
			if( INFO->elements[i]->visible==true&&distance(INFO->elements[i]->pos,INFO->elements[mybase]->pos)<center.x&&
				distance(INFO->elements[i]->pos,INFO->elements[mybase]->pos)<dis && if_attack(i)==1)
			{
				dis=distance(INFO->elements[i]->pos,INFO->elements[mybase]->pos);
				ene=i;
			}
			else;
		else; 

		if(ene==enermybase) 
			return 0;

		for(i=0;i<INFO->element_num;i++)
			if(INFO->elements[i]->team==INFO->team_num&& INFO->elements[i]->type!=CARGO)
			{
				int t=0;
				if(INFO->elements[i]->fuel<kProperty[INFO->elements[i]->type].fuel_max*0.3 
					&& INFO->elements[i]->type!= BASE && INFO->elements[i]->type!= FORT 
					&& distance(INFO->elements[i]->pos,INFO->elements[mybase]->pos)<=3)
				{
					ele_supply(i);
					t=1;
				}
				else if((INFO->elements[i]->fuel<kProperty[INFO->elements[i]->type].fuel_max*0.3 
					&& INFO->elements[i]->type!= BASE && INFO->elements[i]->type!= FORT 
					&& distance(INFO->elements[i]->pos,INFO->elements[mybase]->pos)>3))
				{
					t=1;
					ChangeDest(INFO->elements[i]->index,findbaseOcean(INFO->elements[mybase]->pos));
				}
				else
				{
					for(j=0;j<INFO->element_num;j++)
						if(INFO->elements[j]->team==1-INFO->team_num 
							&& distance(INFO->elements[i]->pos,INFO->elements[j]->pos)<=
							kProperty[INFO->elements[i]->type].fire_ranges[elelevel(j)])
						{
							AttackUnit(INFO->elements[i]->index,INFO->elements[j]->index);
							t=1;
							break;
						}

					if(t==1)continue;
					else if(INFO->elements[i]->type!=BASE && INFO->elements[i]->type!=FORT && INFO->elements[i]->type!=FIGHTER)
					{
							ChangeDest(INFO->elements[i]->index,
								aposition(INFO->elements[mybase]->pos,INFO->elements[ene]->pos,
								kProperty[INFO->elements[i]->type].fire_ranges[elelevel(INFO->elements[ene]->type)]));
					}
				}
			}
			return 1;
}

void movecargo()
{
	for(int i=0;i<INFO->element_num;i++)
		if(INFO->elements[i]->team==INFO->team_num && INFO->elements[i]->type==CARGO)
		{
			ChangeDest(INFO->elements[i]->index,center);
			return;
		}

	return;
}

void cargoact()
{
	int i;
	int cargonum=0;
	if(nearest_fuel.x==0&&nearest_fuel.y==0)
	{
		movecargo();
	}

	for(i=0;i<INFO->element_num;i++)
		if(INFO->elements[i]->type==CARGO && INFO->elements[i]->team==INFO->team_num &&INFO->elements[i]->health>0)
		{
			switch(cargostate[cargonum])
			{
			case 0: 
				ChangeDest(INFO->elements[i]->index,nearest_mine);
				if(distance(INFO->elements[i]->pos,nearest_mine)<=1)
					cargostate[cargonum]=1;
				break;
			case 1:
				ChangeDest(INFO->elements[i]->index,nearest_fuel);
				if(distance(INFO->elements[i]->pos,nearest_fuel)<=1)
					cargostate[cargonum]=2;
				break;
			case 2: 
				ChangeDest(INFO->elements[i]->index,findbaseOcean(INFO->elements[mybase]->pos));
				if(distance(INFO->elements[i]->pos,INFO->elements[mybase]->pos)<=3)
				cargostate[cargonum]=3;
				break;
			case 3:
				if(INFO->elements[i]->fuel>0.3*kProperty[CARGO].fuel_max)
					Supply(INFO->elements[i]->index,INFO->elements[mybase]->index, 
					INFO->elements[i]->fuel-0.3*kProperty[CARGO].fuel_max,0, INFO->elements[i]->metal);
				else
					Supply(INFO->elements[i]->index,INFO->elements[mybase]->index, 
					0,0, INFO->elements[i]->metal);
				if( INFO->elements[i]->health<0.7*kProperty[CARGO].health_max )
					cargostate[cargonum]=4;
				else cargostate[cargonum]=0;
				break;
			case 4:
				if( INFO->elements[i]->health<0.7*kProperty[CARGO].health_max )
					Fix(INFO->elements[mybase]->index,INFO->elements[i]->index);
				cargostate[cargonum]=0;
				break;
			default: break;
			}
			cargonum++;
		}

	return ;
}

void movedestroyer(int des)
{
	int de=des;
	if(distance(INFO->elements[de]->pos,INFO->elements[mybase]->pos)<=3 )
		ChangeDest(INFO->elements[de]->index,findOcean(INFO->elements[de]->pos));
	return ;
}

void movesub(int des)
{
	int de=des;
	if(distance(INFO->elements[de]->pos,INFO->elements[mybase]->pos)<=3 )
		ChangeDest(INFO->elements[de]->index,findOcean(INFO->elements[de]->pos));
	return ;
}

void attackfort()
{
	int i;
	int j=0;

	if(fort==-1)return;

	for(i=0;i<INFO->element_num;i++)
		if(INFO->elements[i]->team==INFO->team_num &&( INFO->elements[i]->type==DESTROYER ||  INFO->elements[i]->type==CARRIER )
			&&  INFO->elements[i]->ammo>kProperty[DESTROYER].ammo_max*0.3 && j<4)
		{
			j++;
			if(distance(INFO->elements[i]->pos,nearest_fort)>kProperty[DESTROYER].fire_ranges[SURFACE])
				ChangeDest(INFO->elements[i]->index,
				aposition(INFO->elements[i]->pos,INFO->elements[fort]->pos,kProperty[INFO->elements[j]->type].fire_ranges[SURFACE]));
			else 
				AttackUnit(INFO->elements[i]->index,INFO->elements[fort]->index);
		}
		else if(INFO->elements[i]->team==INFO->team_num
			&&INFO->elements[i]->type!=BASE && INFO->elements[i]->type!=FORT
			&& INFO->elements[i]->type!=CARGO&& INFO->elements[i]->type!=FIGHTER)
			ChangeDest(INFO->elements[i]->index,wposition());

	return ;
}

void generalattack()
{
	int j=0;
	Position pos;

	pos.x=INFO->x_max-INFO->elements[mybase]->pos.x;
	pos.y=INFO->elements[mybase]->pos.y;

	int p=defend2();
	if(p==0)attackfort();

	return ;
}

void ele_supply(int i)
{
	if(INFO->elements[mybase]->fuel<80 )return;
		if(distance(INFO->elements[mybase]->pos,INFO->elements[i]->pos)>3)
			ChangeDest(INFO->elements[i]->index,INFO->elements[mybase]->pos);
		else 
			if(INFO->elements[i]->type==CARGO);
			else
			Supply(INFO->elements[mybase]->index,INFO->elements[i]->index,
			0.8*kProperty[INFO->elements[i]->type].fuel_max-INFO->elements[i]->fuel,kProperty[DESTROYER].ammo_max,0);
	return;
}
