#include "basic.h"
#include <math.h>
#include <cstdlib>

using namespace teamstyle16;
void laytodefend(ElementType);
void cargofunction();
void fufillrepairall();
void myfortattack();
Position defenceposition();
int Distance(Position, Position);
int numofthistype(ElementType);
int closeofthistype(bool, ElementType);
int closeenemy(int);
int closeenemy(Position);
int enemybase();
int mybase();
int attackobject();
void attackaction(ElementType, ElementType);
int cargo_num = 0;
int fighter_num = 0;
int destoryer_num = 0;
int submarine_num = 0;
bool signal_1 = false;
bool signal_2 = false;
bool signal_3 = false;
State *minetofort[50];
int minetofortmax = 0;
Position basetarget;
Position mine_site;
Position EBase;

const char * GetTeamName()
{
    return "WANTTOWIN";  // Name of your team
}

void AIMain()
{
	cargo_num = numofthistype(CARGO);
	destoryer_num = numofthistype(DESTROYER);
	fighter_num = numofthistype(FIGHTER);
	submarine_num = numofthistype(SUBMARINE);
	if (cargo_num < 3 || (Info()->round > 30 && cargo_num < 5 && Info()->round %4 == 0))
	{
		Produce(CARGO);
	}
	if (cargo_num <= 3 && GetState(mybase())->fuel < 50)
	{
		Produce(CARGO);
	}
	if (destoryer_num < 5 || (Info()->round > 30 && cargo_num < 5 && Info()->round %4 == 0))
	{
		if (GetState(mybase())->fuel > 100)
			Produce(DESTROYER);
	}
	if (Info()->round == 4 || Info()->round == 6 || (fighter_num < 5 && Info()->round > 10 && Info()->round % 3==0))
	{
		if (GetState(mybase())->fuel > 100)
			Produce(FIGHTER);
	}
	if (submarine_num < 2)
	{
		if (GetState(mybase())->fuel > 100)
			Produce(SUBMARINE);
	}
	if (Info()->round > 15 && GetState(mybase())->fuel > 650)
	{
		int signal_6 = 0;
		for (int i = 0; i < Info()->production_num; ++i)
		{
			if (Info()->production_list[i].unit_type == DESTROYER)
			{
				++signal_6;
			}
		}
		if (Info()->production_num <= 2)
		{
		if (signal_6 <= 1 && destoryer_num < 6)
		{
			Produce(DESTROYER);
			Produce(DESTROYER);
		}
		else
		{
			Produce(FIGHTER);
			Produce(FIGHTER);
		}
		}
	}
	bool signal_4 = false;
    if (Distance(GetState(closeofthistype(false, FIGHTER))->pos, GetState(mybase())->pos) < 14)
	{
		attackaction(SUBMARINE, FIGHTER);
		signal_4 = true;
	}
	if (Distance(GetState(closeofthistype(false, CARRIER))->pos, GetState(mybase())->pos) < 14)
	{
		attackaction(SUBMARINE, CARRIER);
		signal_4 = true;
	}
	if (Distance(GetState(closeofthistype(false, DESTROYER))->pos, GetState(mybase())->pos) < 14)
	{
		attackaction(SUBMARINE, DESTROYER);
		signal_4 = true;
	}
	if (Distance(GetState(closeofthistype(false, SUBMARINE))->pos, GetState(mybase())->pos) < 14)
	{
		attackaction(SUBMARINE, SUBMARINE);
		signal_4 = true;
	}
	if (Distance(GetState(closeofthistype(false, CARGO))->pos, GetState(mybase())->pos) < 14)
	{
		attackaction(SUBMARINE, CARGO);
		signal_4 = true;
	}
	bool signal_5 = false;
	if (Distance(GetState(closeofthistype(false,CARRIER))->pos, GetState(mybase())->pos) < 14)
	{
		attackaction(FIGHTER, CARRIER);
		signal_5 = true;
	}
	if (Distance(GetState(closeofthistype(false,DESTROYER))->pos, GetState(mybase())->pos) < 14)
	{
		attackaction(FIGHTER, DESTROYER);
		signal_5 = true;
	}
	if (Distance(GetState(closeofthistype(false, SUBMARINE))->pos, GetState(mybase())->pos) < 14)
	{
		attackaction(FIGHTER, SUBMARINE);
		signal_5 = true;
	}
	if (Distance(GetState(closeofthistype(false,SCOUT))->pos, GetState(mybase())->pos) < 14)
	{
		attackaction(FIGHTER, SCOUT);
		signal_5 = true;
	}
	if (Distance(GetState(closeofthistype(false, CARGO))->pos, GetState(mybase())->pos) < 14)
	{
		attackaction(FIGHTER, CARGO);
		signal_5 = true;
	}
	if (Distance(GetState(closeofthistype(false,FIGHTER))->pos, GetState(mybase())->pos) < 14)
	{
		attackaction(FIGHTER, FIGHTER);
		signal_5 = true;
	}
	if (signal_4 == false)
	{
		laytodefend(SUBMARINE);
	}
	if (signal_5 == false)
	{
		laytodefend(FIGHTER);
	}
	//基地攻击
	AttackUnit(mybase(), closeenemy(mybase()));
	//驱逐舰出动
	attackaction(DESTROYER, FORT);
	if (closeenemy(mybase()) == enemybase() && closeofthistype(false, FORT) == enemybase())
	{
		attackaction(FIGHTER, BASE);
	}
	if (closeofthistype(false, FORT) == enemybase())
	{
		if (fighter_num < 3)
			laytodefend(FIGHTER);
		else
		{
			for (int i = 0; i < Info()->element_num; ++i)
			{
				if (Info()->elements[i]->team == Info()->team_num && (Info()->elements[i]->type == DESTROYER || (closeenemy(mybase()) == enemybase() && Info()->elements[i]->type == FIGHTER)))
				{
					ChangeDest(Info()->elements[i]->index, GetState(attackobject())->pos);
					AttackUnit(Info()->elements[i]->index, attackobject());
				}
			}
		}
	}
	//已占领据点攻击
	myfortattack();
	//修理
	fufillrepairall();
	//运输舰出动
	cargofunction();
    // Your codes here
	for (int i = 0; i < Info()->element_num; ++i)
	{
		if (Info()->elements[i]->team == Info()->team_num && Info()->elements[i]->type == CARGO && Info()->elements[i]->fuel > 220 
			&& Distance(Info()->elements[i]->pos, GetState(mybase())->pos) <= 3)
		{
			ChangeDest(Info()->elements[i]->index, GetState(mybase())->pos);
			Supply(Info()->elements[i]->index, mybase(), Info()->elements[i]->fuel-50, 0, 50);
		}
	}
}

int Distance(Position a, Position b)
{
	return abs(a.x-b.x) + abs(a.y-b.y);
}

int numofthistype(ElementType type_)
{
	int num_ = 0;
	for (int i = 0; i < Info()->element_num; ++i)
	{
		if (Info()->elements[i]->team == Info()->team_num && Info()->elements[i]->type == type_)
		{
			++num_;
		}
	}
	for (int i = 0; i < Info()->production_num; ++i)
	{
		if (Info()->production_list[i].unit_type == type_)
			++num_;
	}
	return num_;
}

int closeofthistype(bool isme, ElementType type_)
{
	bool sig = false;
	Position tem;
	int temind = -1;
	if (isme == false)
	{
	for (int i = 0; i < Info()->element_num; ++i)
	{
		if (Info()->elements[i]->type == type_ && Info()->elements[i]->team == NO_TEAM)
		{
			if (sig == false)
			{
				if ((Info()->elements[i]->type != MINE && Info()->elements[i]->type != OILFIELD) || (Info()->elements[i]->type == MINE && Info()->elements[i]->metal > 0) || (Info()->elements[i]->type == OILFIELD && Info()->elements[i]->fuel >0))
				{sig = true;
				tem = Info()->elements[i]->pos;
				temind = Info()->elements[i]->index;
				}
			}
			else
			{
				if (Distance(Info()->elements[i]->pos, GetState(mybase())->pos) < Distance(tem, GetState(mybase())->pos))
				{
				if ((Info()->elements[i]->type != MINE && Info()->elements[i]->type != OILFIELD) ||((Info()->elements[i]->type == MINE && Info()->elements[i]->metal > 0) || (Info()->elements[i]->type == OILFIELD && Info()->elements[i]->fuel >0)))
				{
					tem = Info()->elements[i]->pos;
					temind = Info()->elements[i]->index;
				}
				}
			}
		}
	}
	if (sig == false)
	{
		for (int i = 0; i < Info()->element_num; ++i)
		{
			if (Info()->elements[i]->type == type_ && Info()->elements[i]->team == !Info()->team_num)
			{
			if (sig == false)
			{
				sig = true;
				tem = Info()->elements[i]->pos;
				temind = Info()->elements[i]->index;
			}
			else
			{
				if (Distance(Info()->elements[i]->pos, GetState(mybase())->pos) < Distance(tem, GetState(mybase())->pos))
				{
					tem = Info()->elements[i]->pos;
					temind = Info()->elements[i]->index;
				}
			}
			}
		}
	}
	}
	else
	{
    for (int i = 0; i < Info()->element_num; ++i)
	{
		if (Info()->elements[i]->type == type_ && Info()->elements[i]->team == Info()->team_num)
		{
			if (sig == false)
			{
				sig = true;
				tem = Info()->elements[i]->pos;
				temind = Info()->elements[i]->index;
			}
			else
			{
				if (Distance(Info()->elements[i]->pos, GetState(mybase())->pos) < Distance(tem, GetState(mybase())->pos))
				{
					tem = Info()->elements[i]->pos;
					temind = Info()->elements[i]->index;
				}
			}
		}
	}
	}
	if (temind == -1)
	{
		return enemybase();
	}
	else 
		return temind;
}

int mybase()
{
	for (int i = 0; i < Info()->element_num; ++i)
	{
		if (Info()->elements[i]->type == BASE && Info()->elements[i]->team == Info()->team_num)
			return Info()->elements[i]->index;
	}
}

int enemybase()
{
	for (int i = 0; i < Info()->element_num; ++i)
	{
		if (Info()->elements[i]->type == BASE && Info()->elements[i]->team == !Info()->team_num)
			return Info()->elements[i]->index;
	}
}

int closeenemy(int index_)
{
	bool sig = false;
	Position tem;
	int temind = -1;
	for (int i = 0; i < Info()->element_num; ++i)
	{
		if (Info()->elements[i]->team == !Info()->team_num &&(((GetState(index_)->type == BASE)
			&& (Info()->elements[i]->type != SUBMARINE)) || (GetState(index_)->type != BASE)))
		{
			if (sig == false)
			{
				sig = true;
				tem = Info()->elements[i]->pos;
				temind = Info()->elements[i]->index;
			}
			else
			{
				if (Distance(Info()->elements[i]->pos, GetState(index_)->pos) < Distance(tem, GetState(index_)->pos))
				{
					tem = Info()->elements[i]->pos;
					temind = Info()->elements[i]->index;
				}
			}
		}
	}
	return temind;
}

void attackaction(ElementType my_, ElementType enemy_)
{
	int attactindex_ = -1;
	for (int i = 0; i < Info()->element_num; ++i)
	{
		if (Info()->elements[i]->type == my_ && Info()->elements[i]->team == Info()->team_num)
		{
			ChangeDest(Info()->elements[i]->index, GetState(closeofthistype(false, enemy_))->pos);
			AttackUnit(Info()->elements[i]->index, closeofthistype(false, enemy_));
			if (closeenemy(closeofthistype(false, enemy_)) != -1)
			{
			if (attactindex_ == -1 && Distance(GetState(closeenemy(closeofthistype(false, enemy_)))->pos, GetState(closeofthistype(false, enemy_))->pos) <= 8)
			{
				attactindex_ = closeenemy(closeofthistype(false, enemy_));
				ChangeDest(Info()->elements[i]->index, GetState(attactindex_)->pos);
				AttackUnit(Info()->elements[i]->index, attactindex_);
			}
			else if (attactindex_ != -1)
			{
				ChangeDest(Info()->elements[i]->index, GetState(attactindex_)->pos);
				AttackUnit(Info()->elements[i]->index, attactindex_);
			}
			}
		}
	}
}


void laytodefend(ElementType type_)
{
	int attackindex_ = -1;
	for (int i = 0; i < Info()->element_num; ++i)
	{
		if (Info()->elements[i]->team == Info()->team_num && Info()->elements[i]->type == type_)
		{
			ChangeDest(Info()->elements[i]->index, defenceposition());
			if (closeenemy(defenceposition()) != -1)
			{
				if (attackindex_ == -1 && Distance(GetState(closeenemy(Info()->elements[i]->index))->pos, defenceposition()) <= 10)
			{
				attackindex_ = closeenemy(Info()->elements[i]->index);
				ChangeDest(Info()->elements[i]->index, GetState(attackindex_)->pos);
				AttackUnit(Info()->elements[i]->index, attackindex_);
			}
			else if (attackindex_ != -1)
			{
				ChangeDest(Info()->elements[i]->index, GetState(attackindex_)->pos);
				AttackUnit(Info()->elements[i]->index, attackindex_);
			}
			}
		}
	}
}

Position defenceposition()
{
	Position pos_;
	pos_.x = int(GetState(mybase())->pos.x * 0.75 + GetState(enemybase())->pos.x * 0.25);
	pos_.y = int(GetState(mybase())->pos.y * 0.75 + GetState(enemybase())->pos.y * 0.25);
	return pos_;
}

void cargofunction()
{
	bool sig_;
	Position pos_[7];
	int k = 0;
	for (int i = 0; i < Info()->element_num; ++i)
	{
		if (Info()->elements[i]->team == Info()->team_num && Info()->elements[i]->type == CARGO)
		{
			if (Info()->elements[i]->metal < 50)
			{
				ChangeDest(Info()->elements[i]->index, GetState(closeofthistype(false, MINE))->pos);
			}
			if (Info()->elements[i]->fuel < 250)
			{
				ChangeDest(Info()->elements[i]->index, GetState(closeofthistype(false, OILFIELD))->pos);
			}
			if (Info()->elements[i]->fuel > 220 && Info()->elements[i]->metal > 10)
			{
				ChangeDest(Info()->elements[i]->index, GetState(mybase())->pos);
				Supply(Info()->elements[i]->index, mybase(), Info()->elements[i]->fuel-50, 0, 50);
				if (Info()->elements[i]->ammo < 70)
					Supply(mybase(), Info()->elements[i]->index, 0, 120, 0);
				if (Info()->elements[i]->health < 65)
				{
					Fix(mybase(), Info()->elements[i]->index);
				}
			}
		/*	if (Info()->elements[i]->fuel > 180)
			{
				for (int j = 0; j < Info()->element_num; ++j)
				{
					if (Info()->elements[j]->team == Info()->team_num)
					{
						sig_ = false;
						for (int t = 0; t < k; ++t)
						{
							if (Info()->elements[j]->pos.x == pos_[t].x && Info()->elements[j]->pos.y == pos_[t].y
								&& Info()->elements[j]->pos.z == pos_[t].z)
							{
								sig_ = true;
								break;
							}
						}
						if (sig_ == false)
						{
							if (Info()->elements[j]->type == FIGHTER && (Info()->elements[j]->fuel < 20 || Info()->elements[j]->ammo <= 9))
							{
								pos_[k] = Info()->elements[j]->pos;
								++k;
								ChangeDest(Info()->elements[i]->index, Info()->elements[j]->pos);
								Supply(Info()->elements[i]->index, Info()->elements[j]->index, 100, 21, 0);
								break;
							}
							if (Info()->elements[j]->type == DESTROYER &&(Info()->elements[j]->fuel < 20 || Info()->elements[j]->ammo <= 12))
							{
								pos_[k] = Info()->elements[j]->pos;
								++k;
								ChangeDest(Info()->elements[i]->index, Info()->elements[j]->pos);
								Supply(Info()->elements[i]->index, Info()->elements[j]->index, 150, 40, 0);
								break;
							}
						}
					}
				}
			}*/
		}
	}
}

int closeenemy(Position pos_)
{
	int index_ = -1;
	bool sig_ = false;
	for (int i = 0; i < Info()->element_num; ++i)
	{
		if (Info()->elements[i]->team == !Info()->team_num)
		{
			if (sig_ == false)
			{
				index_ = Info()->elements[i]->index;
				sig_ = true;
			}
			else if (sig_ == true)
			{
				if (Distance(pos_, Info()->elements[i]->pos) < Distance(pos_, GetState(index_)->pos))
				{
					index_ = Info()->elements[i]->index;
				}
			}
		}
	}
	return index_;
}

void fufillrepairall()
{
	for (int i = 0; i < Info()->element_num; ++i)
	{
		if (Info()->elements[i]->team == Info()->team_num && (Info()->elements[i]->type == FIGHTER || Info()->elements[i]->type == DESTROYER || Info()->elements[i]->type == SUBMARINE))
		{
			if (Info()->elements[i]->health < 29)
			{
				if (Distance(Info()->elements[i]->pos, GetState(mybase())->pos) <= 1)
				{
					Fix(mybase(), Info()->elements[i]->index);
				}
				else
				{
					ChangeDest(Info()->elements[i]->index, GetState(mybase())->pos);
					Fix(mybase(), Info()->elements[i]->index);
				}
			}
			else if ((Info()->elements[i]->fuel < Distance(Info()->elements[i]->pos, GetState(mybase())->pos)+10 || Info()->elements[i]->ammo < 4) && Info()->elements[i]->type != CARGO)
			{
				if (GetState(mybase())->fuel > 100)
				{
				if (Distance(Info()->elements[i]->pos, GetState(mybase())->pos) <= 1)
				{
					Supply(mybase(), Info()->elements[i]->index, GetState(mybase())->fuel-49, 50, 0);
				}
				else
				{
					ChangeDest(Info()->elements[i]->index, GetState(mybase())->pos);
					Supply(mybase(), Info()->elements[i]->index, GetState(mybase())->fuel-49, 50, 0);
				}
				}
				else if (GetState(mybase())->fuel == 0 && Info()->elements[i]->type == DESTROYER)
				{
					ChangeDest(Info()->elements[i]->index, GetState(enemybase())->pos);
					AttackUnit(Info()->elements[i]->index, enemybase());
				}
			}
			else if (Distance(Info()->elements[i]->pos, GetState(mybase())->pos) < 5 && Info()->elements[i]->fuel < 35)
			{
				ChangeDest(Info()->elements[i]->index, GetState(mybase())->pos);
				Supply(mybase(), Info()->elements[i]->index, GetState(mybase())->fuel-49, 50, 0);
			}
		}
	}
}

void myfortattack()
{
	for (int i = 0; i < Info()->element_num; ++i)
	{
		if (Info()->elements[i]->type == FORT && Info()->elements[i]->team == Info()->team_num)
		{
			AttackUnit(Info()->elements[i]->index, closeenemy(Info()->elements[i]->index));
		}
	}
}

int attackobject()
{
	for (int j = 0; j < 10; ++j)
	{
	for (int i = 0; i < Info()->element_num; ++i)
	{
		if (Distance(Info()->elements[i]->pos, GetState(enemybase())->pos) == 10-j && Info()->elements[i]->team == !(Info()->team_num))
		{
			return (Info()->elements[i]->index);
		}
	}
	}
	return enemybase();
}
