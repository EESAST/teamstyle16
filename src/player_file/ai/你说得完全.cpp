#include <cstdlib>
#include "basic.h"
#include<iostream>
#include<cmath>
#include<vector>
// Remove this line if you konw C++, and don't want a dirty namespace
using namespace teamstyle16;
using std::vector;
using std::cout;
using std::endl;

int myfortnum = 0;
int enemyfortnum = 0;
int con = 3;
int deltamyfortnum = 0;

struct fighter{
	Position pos;
	int index = 0;
}fighter[20];

struct cargo{
	Position pos;
	int index;
}cargo[2];

struct mine{
	Position pos;
}mine[4];

struct Fort{
	int index;
	Position pos;
}myfort[30], enemyfort[30];

const char * GetTeamName()
{
	return "lzhbrian";  // Name of your team
}

//宣告函数
int distance(Position, Position);
int GetBase(int);
void BaseAct();
void FortAct();
int DisToBase();
int max();
int min();
int if_alive();
int GetCargo();
int GetFighter();
int GetFort();
void fortbubble_sort();
void cargo_metal();
void cargo_fuel();
void BaseSupply_cargo();
void cargo_fix();
void producecargo();
void make_team(ElementType type, int num, int team_num);
void SupllyAllFighterNearBase();
void FighterAttack(int fighterindex);
void AttackFort(int fighterindex, int fortindex);



/*******************小能手的代码**********************/

//全局变量



//采集金属
void cargo_metal()
{
	int a = GetBase(Info()->team_num);
	for (int i = 0; i<Info()->element_num; i++)
	{
		if (Info()->elements[i]->type == CARGO && Info()->elements[i]->team == Info()->team_num)
		{
			if (Info()->elements[i]->metal <= 10 && Info()->elements[a]->metal <= 130)/**/
			{
				int dis = 200;
				int dis_index = 0;
				for (int j = 0; j<Info()->element_num; j++)
				{
					if (Info()->elements[j]->type == MINE && Info()->elements[j]->metal >= 50)/**/
					{
						//shortest path
						if (distance(Info()->elements[a]->pos, Info()->elements[j]->pos)<dis)
						{
							dis = distance(Info()->elements[a]->pos, Info()->elements[j]->pos);
							dis_index = j;
						}
					}
				}
				ChangeDest(Info()->elements[i]->index, Info()->elements[dis_index]->pos);
			}

			else if (Info()->elements[i]->metal>10 && Info()->elements[a]->metal <= 130)
			{
				ChangeDest(Info()->elements[i]->index, Info()->elements[a]->pos);
				Supply(Info()->elements[i]->index, Info()->elements[a]->index, 0, 0, 49);/**/
			}

			else if (Info()->elements[a]->metal>130 && Info()->elements[a]->fuel<900)
			{
				if (Info()->elements[i]->fuel <= 100)/**/
				{
					int dis = 300;
					int dis_index = 0;
					for (int k = 0; k<Info()->element_num; k++)
					{
						if (Info()->elements[k]->type == OILFIELD && Info()->elements[k]->fuel >= 100)/**/
						{
							//shortest path

							if (distance(Info()->elements[a]->pos, Info()->elements[k]->pos)<dis)
							{
								dis = distance(Info()->elements[a]->pos, Info()->elements[k]->pos);
								dis_index = k;
							}
						}
					}
					ChangeDest(Info()->elements[i]->index, Info()->elements[dis_index]->pos);
				}

				else
				{
					ChangeDest(Info()->elements[i]->index, Info()->elements[a]->pos);
					if (Info()->elements[i]->fuel>200 && Info()->elements[i]->fuel <= 300)
					{
						Supply(Info()->elements[i]->index, Info()->elements[a]->index, 100, 0, 0);	/**/
					}
					else if (Info()->elements[i]->fuel <= 200 && Info()->elements[i]->fuel>150)
					{
						Supply(Info()->elements[i]->index, Info()->elements[a]->index, 60, 0, 0);	/**/
					}
					else if (Info()->elements[i]->fuel <= 150 && Info()->elements[i]->fuel>100)
					{
						Supply(Info()->elements[i]->index, Info()->elements[a]->index, 20, 0, 0);	/**/
					}
				}
			}
		}
	}
}

//cargo补cargo
void cargo_cargo()
{
	int a = GetBase(Info()->team_num);
	for (int i = 0; i<Info()->element_num; i++)
	{
		if (Info()->elements[i]->type == CARGO 
			&& Info()->elements[i]->team == Info()->team_num 
			&& Info()->elements[i]->metal == 0)
		{
			if (Info()->elements[i]->fuel >= 150)
			{
				for (int j = 0; j<Info()->element_num; j++)
				{
					if (Info()->elements[j]->type == CARGO 
						&& Info()->elements[j]->team == Info()->team_num 
						&& Info()->elements[i]->fuel == 0)
					{
						ChangeDest(Info()->elements[i]->index, Info()->elements[j]->pos);
						Supply(Info()->elements[a]->index, Info()->elements[i]->index, 60, 0, 0);
					}
				}
			}
		}
	}
}


//采集油
void cargo_fuel()
{
	int a = GetBase(Info()->team_num);
	for (int i = 0; i<Info()->element_num; i++)
	{
		if (Info()->elements[i]->type == CARGO && Info()->elements[i]->team == Info()->team_num && Info()->elements[i]->metal == 0)
		{
			if (Info()->elements[i]->fuel <= 100)/**/
			{
				int dis = 200;
				int dis_index = 0;
				for (int k = 0; k<Info()->element_num; k++)
				{
					if (Info()->elements[k]->type == OILFIELD && Info()->elements[k]->fuel >= 100)/**/
					{
						//shortest path

						if (distance(Info()->elements[a]->pos, Info()->elements[k]->pos)<dis)
						{
							dis = distance(Info()->elements[a]->pos, Info()->elements[k]->pos);
							dis_index = k;
						}
					}
				}
				ChangeDest(Info()->elements[i]->index, Info()->elements[dis_index]->pos);
			}

			else if (Info()->elements[i]->fuel>100 && Info()->elements[a]->fuel <= 1000)
			{
				ChangeDest(Info()->elements[i]->index, Info()->elements[a]->pos);
				if (Info()->elements[i]->fuel>200 && Info()->elements[i]->fuel <= 300)
				{
					Supply(Info()->elements[i]->index, Info()->elements[a]->index, 100, 0, 0);	/**/
				}
				else if (Info()->elements[i]->fuel <= 200 && Info()->elements[i]->fuel>150)
				{
					Supply(Info()->elements[i]->index, Info()->elements[a]->index, 80, 0, 0);	/**/
				}
				else if (Info()->elements[i]->fuel <= 150 && Info()->elements[i]->fuel>100)
				{
					Supply(Info()->elements[i]->index, Info()->elements[a]->index, 20, 0, 0);	/**/
				}
			}
		}
	}
}

//基地补油
void BaseSupply_cargo()
{
	int a = GetBase(Info()->team_num);
	for (int i = 0; i<Info()->element_num; i++)
	{
		if (Info()->elements[i]->type == CARGO && Info()->elements[i]->team == Info()->team_num && Info()->elements[i]->fuel <= 80) /**/
		{
			if (Info()->elements[a]->fuel>100 && Info()->elements[a]->fuel<300)
			{
				Supply(Info()->elements[a]->index, Info()->elements[i]->index, 80, 0, 0);/**/
			}
			if (Info()->elements[a]->fuel >= 300 && Info()->elements[a]->fuel<600)
			{
				Supply(Info()->elements[a]->index, Info()->elements[i]->index, 100, 0, 0);/**/
			}
			if (Info()->elements[a]->fuel >= 600 && Info()->elements[a]->fuel <= 1000)
			{
				Supply(Info()->elements[a]->index, Info()->elements[i]->index, 130, 0, 0);/**/
			}
		}
	}
}





//cargo维修
void cargo_fix()
{
	int a = GetBase(Info()->team_num);
	for (int i = 0; i<Info()->element_num; i++)
	{
		if (GetState(Info()->elements[i]->index)->type == CARGO && Info()->elements[i]->team == Info()->team_num && Info()->elements[i]->health <= 45)/**/
		{
			ChangeDest(Info()->elements[i]->index, Info()->elements[a]->pos);
			Fix(Info()->elements[a]->index, Info()->elements[i]->index);
		}
	}
}


//submarine到达敌方MINE
void sub_move()
{
	int b = GetBase(1 - Info()->team_num);
	for (int i = 0; i<Info()->element_num; i++)
	{
		if (Info()->elements[i]->type == SUBMARINE && Info()->elements[i]->team == Info()->team_num)
		{
			for (int j = 0; j<Info()->element_num; j++)
			{
				if ((Info()->elements[j]->type == MINE)
					&& distance(Info()->elements[j]->pos, Info()->elements[b]->pos)<10
					&& distance(Info()->elements[i]->pos, Info()->elements[j]->pos)>3)   /**/
				{
					ChangeDest(Info()->elements[i]->index, Info()->elements[j]->pos);
				}
			}
		}
	}
}

//submarine攻击敌方cargo
void sub_attack_cargo()
{
	for (int i = 0; i<Info()->element_num; i++)
	{
		if (Info()->elements[i]->type == SUBMARINE && Info()->elements[i]->team == Info()->team_num)
		{
			for (int j = 0; j<Info()->element_num; j++)
			{
				if (Info()->elements[j]->type == CARGO && Info()->elements[j]->team == (1 - Info()->team_num))
				{
					AttackUnit(Info()->elements[i]->index, Info()->elements[j]->index);
				}
			}
		}
	}
}

//基地补submarine弹药和油并维修
void Base_sub()
{
	int a = GetBase(Info()->team_num);
	for (int i = 0; i<Info()->element_num; i++)
	{
		if (Info()->elements[i]->type == SUBMARINE && Info()->elements[i]->team == Info()->team_num)
		{
			if (Info()->elements[i]->ammo<2 || Info()->elements[i]->fuel<60 || Info()->elements[i]->health<15) /**/
			{
				ChangeDest(Info()->elements[i]->index, Info()->elements[a]->pos);
				Supply(Info()->elements[a]->index, Info()->elements[i]->index, 60, 20, 0);
				Fix(Info()->elements[a]->index, Info()->elements[i]->index);
			}
		}
	}
}

//submarine总命令
void sub_all()
{
	sub_move();
	sub_attack_cargo();
	Base_sub();
}

//cargo生存判断
void cargo_metal_alive()
{
	int count1 = 0;
	for (int i = 0; i<Info()->element_num; i++)
	{
		if (Info()->elements[i]->type == CARGO 
			&& Info()->elements[i]->team == Info()->team_num 
			&& Info()->elements[i]->metal>0)
		{
			count1++;
		}
	}
	if (count1<2)
	{
		Produce(CARGO);
		cargo_metal();
	}
}

void cargo_fuel_alive()
{
	int count2 = 0;
	for (int i = 0; i<Info()->element_num; i++)
	{
		if (Info()->elements[i]->type == CARGO 
			&& Info()->elements[i]->team == Info()->team_num 
			&& Info()->elements[i]->metal == 0)
		{
			count2++;
		}
	}
	if (count2<1)
	{
		Produce(CARGO);
		cargo_fuel();
	}
}




/******************************************************/




//两点之间的距离
int distance(Position pos1, Position pos2){
	return abs(pos1.x - pos2.x) + abs(pos1.y - pos2.y);
}

//获得己方或对方基地的编号
int GetBase(int team){
	int i;
	for (i = 0; i<Info()->element_num; i++)
		if (Info()->elements[i]->type == BASE && Info()->elements[i]->team == team)
			return i;
	return -1;
}

//基地攻击附近血量最低的目标
void BaseAct(){
	int mybase = GetBase(Info()->team_num);
	int enemybase = GetBase(1 - Info()->team_num);
	Position enemypos = Info()->elements[enemybase]->pos;
	Position mypos = Info()->elements[mybase]->pos;

	int i, minhealth = 301, eindex = 0;
	for (i = 0; i < Info()->element_num; i++){
		if (distance(mypos, Info()->elements[i]->pos) <= 8
			&& Info()->elements[i]->team != Info()->team_num
			&& (Info()->elements[i]->type == DESTROYER
			|| Info()->elements[i]->type == CARRIER
			|| Info()->elements[i]->type == CARGO
			|| Info()->elements[i]->type == FORT)
			&& Info()->elements[i]->health < minhealth
			){
			minhealth = Info()->elements[i]->health;
			eindex = Info()->elements[i]->index;
			AttackUnit(Info()->elements[mybase]->index, eindex);
		}
	}
	for (i = 0; i < Info()->element_num; i++){
		if (distance(mypos, Info()->elements[i]->pos) <= 6
			&& Info()->elements[i]->team != Info()->team_num
			&& (Info()->elements[i]->type == FIGHTER
			|| Info()->elements[i]->type == SCOUT)
			&& Info()->elements[i]->health < minhealth
			){
			minhealth = Info()->elements[i]->health;
			eindex = Info()->elements[i]->index;
			AttackUnit(Info()->elements[mybase]->index, eindex);
		}
		for (int j = 0; j < Info()->element_num; j++){
			if (Info()->elements[j]->type == BASE
				&& distance(Info()->elements[i]->pos, Info()->elements[enemybase]->pos) <= 8){
				AttackUnit(Info()->elements[i]->index, Info()->elements[enemybase]->index);
				break;
			}
		}
	}
}

//据点攻击附近目标
void FortAct(){
	int mybase = GetBase(Info()->team_num);
	int enemybase = GetBase(1 - Info()->team_num);
	Position enemypos = Info()->elements[enemybase]->pos;
	Position mypos = Info()->elements[mybase]->pos;


	for (int i = 0; i < Info()->element_num; i++){
		if (Info()->elements[i]->type == FORT && Info()->elements[i]->team == Info()->team_num){
			int minhealth = 301, eindex = 0;

			for (int j = 0; j < Info()->element_num; j++){
				if (distance(Info()->elements[i]->pos, Info()->elements[j]->pos) <= 8
					&& Info()->elements[j]->team != Info()->team_num
					&& (Info()->elements[j]->type == DESTROYER
					|| Info()->elements[j]->type == CARRIER
					|| Info()->elements[j]->type == CARGO
					|| Info()->elements[j]->type == FORT)
					){
					eindex = Info()->elements[j]->index;
					AttackUnit(Info()->elements[i]->index, eindex);
				}
			}
			for (int j = 0; j < Info()->element_num; j++){
				if (distance(Info()->elements[i]->pos, Info()->elements[j]->pos) <= 6
					&& Info()->elements[j]->team != Info()->team_num
					&& (Info()->elements[j]->type == FIGHTER
					|| Info()->elements[j]->type == SCOUT)
					){
					eindex = Info()->elements[j]->index;
					AttackUnit(Info()->elements[i]->index, eindex);
				}
			}
			for (int j = 0; j < Info()->element_num; j++){
				if (Info()->elements[j]->type == BASE
					&& distance(Info()->elements[i]->pos, Info()->elements[enemybase]->pos) <= 8){
					AttackUnit(Info()->elements[i]->index, Info()->elements[enemybase]->index);
					break;
				}
			}
		}
	}

}

//返回到基地的距离
int DisToBase(Position pos1, Position pos2)  //pos2表示基地左上角
{
	int min = 1000;
	if (pos1.x < pos2.x)//分为九块
	{
		if (pos1.y > pos2.y)
			return abs(pos1.x - pos2.x) + abs(pos1.y - pos2.y);
		else if (pos1.y < pos2.y - 2)
			return abs(pos1.x - pos2.x) + abs(pos1.y - pos2.y + 2);
		else
			return abs(pos1.x - pos2.x);
	}
	else if (pos1.x > pos2.x + 2)
	{
		if (pos1.y > pos2.y)
			return abs(pos1.x - pos2.x - 2) + abs(pos1.y - pos2.y);
		else if (pos1.y < pos2.y - 2)
			return abs(pos1.x - pos2.x - 2) + abs(pos1.y - pos2.y + 2);
		else
			return abs(pos1.x - pos2.x - 2);
	}
	else
	{
		if (pos1.y > pos2.y)
			return abs(pos1.y - pos2.y);
		else if (pos1.y < pos2.y - 2)
			return abs(pos1.y - pos2.y + 2);
		else
			return 0;
	}
}

//最大值
int max(int x, int y)
{
	return x>y ? x : y;
}

//最小值
int min(int x, int y)
{
	return x<y ? x : y;
}

//判断某单位是否仍存活
int if_alive(int operand)
{
	for (int i = 0; i<Info()->element_num; i++)
		if (Info()->elements[i]->index == operand)
			return 1;
	return 0;
}

//据点距离基地距离冒泡排序
void fortbubble_sort(struct Fort *a, int n)
{
	int mybase = GetBase(Info()->team_num);
	int enemybase = GetBase(1 - Info()->team_num);
	Position enemypos = Info()->elements[enemybase]->pos;
	Position mypos = Info()->elements[mybase]->pos;
	int i, j;
	struct Fort temp;
	for (j = 0; j < n - 1; j++){
		for (i = 0; i < n - 1 - j; i++){
			if (distance(a[i].pos, mypos) > distance(a[i + 1].pos, mypos)){//数组元素大小按升序排列由小到大
				temp = a[i];
				a[i] = a[i + 1];
				a[i + 1] = temp;
			}
		}
	}
}

void producecargo(){
	int count = 0;
	for (int j = 0; j < Info()->element_num; j++){
		if (Info()->elements[j]->type == CARGO && Info()->elements[j]->team == Info()->team_num){
			count++;
		}
	}
	if (Info()->round == 1)
	{
		Produce(CARGO);
	}

	if (Info()->round == 8)/***/
	{
		Produce(CARGO);
	}

}


/*****************逗比鹿的代码****************/

int MyTeam = -1;
int wait_num[30] = { 0 };
int team[30][10] = { 0 };
bool teamed[10000] = { 0 };

void make_team(ElementType type, int num, int team_num)
{
	int lack = 0;//lack

	for (int i = 0; i < num; i++)
		if (team[team_num][i] == 0)
		{
		lack++;
		}
		else if (if_alive(team[team_num][i]) == 0)//when dead,mark teamed[index]=0;
		{
			lack++;
			teamed[team[team_num][i]] = 0;
			team[team_num][i] = 0;         //delete from the team
		}


	int wait_num_cur = wait_num[team_num];

	for (int i = 0; i < lack - wait_num_cur; i++)//produce
	{
		Produce(type);
		wait_num[team_num]++;
	}

	for (int i = 0; i < Info()->element_num; i++)//search for CARGO not teamed[]
	{
		if (Info()->elements[i]->type == type&&Info()->elements[i]->team == MyTeam&&teamed[Info()->elements[i]->index] == 0 && wait_num[team_num] > 0)
		{
			wait_num[team_num]--;
			for (int k = 0; k < num; k++)//when find,get it in my team
				if (team[team_num][k] == 0)
				{
				team[team_num][k] = Info()->elements[i]->index;
				teamed[team[team_num][k]] = 1;
				break;
				}
		}
	}


}

/***************************************/




//FIGHTER攻击FORT的函数
void AttackFort(int fighterindex, int fortindex){
	for (int i = 0; i < Info()->element_num; i++){
		if (Info()->elements[i]->index == fighterindex){

			for (int j = 0; j < Info()->element_num; j++){
				if (Info()->elements[j]->index == fortindex){
					ChangeDest(fighterindex, Info()->elements[j]->pos);
					if (distance(Info()->elements[i]->pos, Info()->elements[j]->pos) <= 1){
						AttackUnit(fighterindex, fortindex);
					}
				}
			}

		}
	}
}

//补充所有基地附近的FIGHTER
void SupplyAllFighterNearBase(){
	int mybase = GetBase(Info()->team_num);
	int enemybase = GetBase(1 - Info()->team_num);
	Position enemypos = Info()->elements[enemybase]->pos;
	Position mypos = Info()->elements[mybase]->pos;

	for (int i = 0; i < Info()->element_num; i++){
		if (Info()->elements[i]->type == FIGHTER
			&& Info()->elements[i]->team == Info()->team_num
			&& (Info()->elements[i]->fuel <= 90 || Info()->elements[i]->ammo<20)){
			Supply(Info()->elements[mybase]->index, Info()->elements[i]->index, 100, 21, 0);
		}
	}

}

//FIGHTER自卫函数
void FighterAttack(int fighterindex){

	int eindex = 0;
	for (int j = 0; j < Info()->element_num; j++){
		for (int i = 0; i < Info()->element_num; i++){
			if (Info()->elements[i]->index == fighterindex){
				if (distance(Info()->elements[i]->pos, Info()->elements[j]->pos) <= 3
					&& Info()->elements[j]->team != Info()->team_num
					&& (Info()->elements[j]->type == DESTROYER
					|| Info()->elements[j]->type == CARRIER
					|| Info()->elements[j]->type == CARGO)
					){
					eindex = Info()->elements[j]->index;
					AttackUnit(Info()->elements[i]->index, eindex);
				}
			}
		}
	}
	for (int j = 0; j < Info()->element_num; j++){
		for (int i = 0; i < Info()->element_num; i++){
			if (Info()->elements[i]->index == fighterindex){
				if (distance(Info()->elements[i]->pos, Info()->elements[j]->pos) <= 4
					&& Info()->elements[j]->team != Info()->team_num
					&& (Info()->elements[j]->type == FIGHTER
					|| Info()->elements[j]->type == SCOUT)
					){
					eindex = Info()->elements[j]->index;
					AttackUnit(Info()->elements[i]->index, eindex);
				}
			}
		}
	}
}



void AIMain()
{


	int mybase = GetBase(Info()->team_num);
	int enemybase = GetBase(1 - Info()->team_num);
	Position enemypos = Info()->elements[enemybase]->pos;
	Position mypos = Info()->elements[mybase]->pos;



	Position pos[9];
	pos[0].x = mypos.x + 1; pos[0].y = mypos.y + 1;
	pos[1].x = mypos.x + 1; pos[1].y = mypos.y;
	pos[2].x = mypos.x + 1; pos[2].y = mypos.y - 1;

	pos[3].x = mypos.x; pos[3].y = mypos.y + 1;
	pos[4].x = mypos.x; pos[4].y = mypos.y;
	pos[5].x = mypos.x; pos[5].y = mypos.y - 1;

	pos[6].x = mypos.x - 1; pos[6].y = mypos.y + 1;
	pos[7].x = mypos.x - 1; pos[7].y = mypos.y;
	pos[8].x = mypos.x - 1; pos[8].y = mypos.y - 1;



	Position pos1[9];
	pos1[0].x = enemypos.x + 1; pos1[0].y = enemypos.y + 1;
	pos1[1].x = enemypos.x + 1; pos1[1].y = enemypos.y;
	pos1[2].x = enemypos.x + 1; pos1[2].y = enemypos.y - 1;

	pos1[3].x = enemypos.x; pos1[3].y = enemypos.y + 1;
	pos1[4].x = enemypos.x; pos1[4].y = enemypos.y;
	pos1[5].x = enemypos.x; pos1[5].y = enemypos.y - 1;

	pos1[6].x = enemypos.x - 1; pos1[6].y = enemypos.y + 1;
	pos1[7].x = enemypos.x - 1; pos1[7].y = enemypos.y;
	pos1[8].x = enemypos.x - 1; pos1[8].y = enemypos.y - 1;


	for (int j = 0; j < Info()->element_num; j++){
		if (Info()->elements[j]->type == FIGHTER){
			FighterAttack(Info()->elements[j]->index);//Fighter自卫
		}
	}


	/***********************第一波函数体开始************************/

	if ((Info()->round <= 30 && deltamyfortnum < 5)
		|| (Info()->round >= 30 && Info()->round <= 40 && deltamyfortnum < 4)
		|| (Info()->round <= 41 && !(Info()->round <= 30 && deltamyfortnum < 5) && !(Info()->round >= 30 && Info()->round <= 40 && deltamyfortnum < 4))){

		//fort数量，编号等
		if (Info()->round == 1){
			int count = 1;
			int a = 0, b = 0;
			for (int j = 0; j < Info()->element_num; j++){
				if (Info()->elements[j]->type == FORT){
					if (Info()->elements[j]->team == Info()->team_num){
						myfort[a].index = Info()->elements[j]->index;
						myfort[a].pos = Info()->elements[j]->pos;
						a++;
					}
					if (Info()->elements[j]->team != Info()->team_num){
						enemyfort[b].index = Info()->elements[j]->index;
						enemyfort[b].pos = Info()->elements[j]->pos;
						b++;
					}
				}
				myfortnum = a;
				enemyfortnum = b;

			}

			//将据点按照距离基地的距离从小到大排序
			fortbubble_sort(myfort, a);
			fortbubble_sort(enemyfort, b);

		}

		if (Info()->round == 1){
			Produce(FIGHTER);
			Produce(FIGHTER);
			Produce(FIGHTER);
			Produce(FIGHTER);
			Produce(FIGHTER);
			Produce(FIGHTER);
			//Supply(Info()->elements[mybase]->index, Info()->elements[GetCargo()]->index, 100, 0, 0);
		}

		//给fighter编号

		if (Info()->round == 4){
			int a = 0;
			int c[6] = { 0 };
			for (int j = 0; j < Info()->element_num; j++){
				if (Info()->elements[j]->type == FIGHTER && Info()->elements[j]->team == Info()->team_num){
					fighter[a].index = Info()->elements[j]->index;
					fighter[a].pos = Info()->elements[j]->pos;
					c[a] = j;
					a++;
					if (a == 6)break;
				}
			}
		}



		int a = 0;
		int b = 0;
		while (a < 3){

			//对不是己方的据点攻击
			int c = 0;
			for (int i = 0; i < Info()->element_num; i++){
				if (Info()->elements[i]->index == enemyfort[b].index){
					if (Info()->elements[i]->team != Info()->team_num){
						c = b;
						AttackFort(fighter[a].index, enemyfort[c].index);
						AttackFort(fighter[5 - a].index, enemyfort[c].index);
					}
					else{
						//打完第一波以后用打完的据点补充
						Supply(enemyfort[b].index, fighter[a].index, 50, 21, 0);
						Supply(enemyfort[b].index, fighter[5 - a].index, 50, 21, 0);

						//前往下一个目的地
						for (int j = 0; j < Info()->element_num; j++){
							if (Info()->elements[j]->index == fighter[a].index){
								if (Info()->elements[j]->fuel >= 95){
									ChangeDest(fighter[a].index, enemyfort[b + con].pos);
								}
								if (distance(Info()->elements[j]->pos, enemyfort[b + con].pos) <= 1){
									AttackUnit(fighter[a].index, enemyfort[b + con].index);
								}
							}
						}
						for (int j = 0; j < Info()->element_num; j++){
							if (Info()->elements[j]->index == fighter[5 - a].index){
								if (Info()->elements[j]->fuel >= 95){
									ChangeDest(fighter[5 - a].index, enemyfort[b + con].pos);
								}
								if (distance(Info()->elements[j]->pos, enemyfort[b + con].pos) <= 1){
									AttackUnit(fighter[5 - a].index, enemyfort[b + con].index);
								}
							}
						}

						//打完第二波以后用打完的据点补充
						for (int k = 0; k < Info()->element_num; k++){
							if (Info()->elements[k]->index == enemyfort[b + con].index){
								if (Info()->elements[k]->team == Info()->team_num){

									for (int j = 0; j < Info()->element_num; j++){
										if (Info()->elements[j]->index == fighter[a].index){
											if (Info()->elements[j]->fuel < 90){
												Supply(enemyfort[b + con].index, fighter[a].index, 50, 21, 0);

											}
											if (Info()->elements[j]->fuel >= 95){
												ChangeDest(fighter[a].index, pos[a]);
											}
										}
									}
									for (int j = 0; j < Info()->element_num; j++){
										if (Info()->elements[j]->index == fighter[5 - a].index){
											if (Info()->elements[j]->fuel < 90){
												Supply(enemyfort[b + con].index, fighter[5 - a].index, 50, 21, 0);

											}
											if (Info()->elements[j]->fuel >= 95){
												ChangeDest(fighter[5 - a].index, pos[5 - a]);
											}
										}
									}

								}
							}
						}

					}
				}
			}

			a++;
			b++;
		}

		int myfortnumprime = 0;
		for (int j = 0; j < Info()->element_num; j++){
			if (Info()->elements[j]->type == FORT){
				if (Info()->elements[j]->team == Info()->team_num){
					myfort[a].index = Info()->elements[j]->index;
					myfort[a].pos = Info()->elements[j]->pos;
					myfortnumprime++;
				}
			}
		}
		deltamyfortnum = myfortnumprime - myfortnum;//计算增加的fort数量







		SupplyAllFighterNearBase();

	}
	/***********************第一波函数体结束************************/
	

	int dd1 = distance(myfort[0].pos, mypos);
	int dd2 = distance(myfort[1].pos, mypos);
	int dd3 = distance(myfort[2].pos, mypos);
	int dd4 = distance(myfort[3].pos, mypos);

	if (dd2>10){

		if (Info()->round >= 10 && Info()->elements[mybase]->metal >= 100 && Info()->elements[mybase]->fuel >= 700){
			SupplyAllFighterNearBase();
			Produce(FIGHTER);
			Produce(FIGHTER);
			Produce(FIGHTER);
			Produce(FIGHTER);
			Produce(FIGHTER);
			Produce(FIGHTER);
		}
		int a = 19;
		if (Info()->round >= 30){
			SupplyAllFighterNearBase();
			for (int j = 0; j < Info()->element_num; j++){
				if (Info()->elements[j]->type == FIGHTER && Info()->elements[j]->team == Info()->team_num){
					if (Info()->elements[j]->fuel >= 80 && Info()->elements[j]->ammo >= 20){

						fighter[a].index = Info()->elements[j]->index;
						fighter[a].pos = Info()->elements[j]->pos;

						a--;

						if (a == 10)break;

						/*
						ChangeDest(Info()->elements[j]->index, enemypos);

						AttackUnit(Info()->elements[j]->index, Info()->elements[enemybase]->index);



						for (int k = 0; k < Info()->element_num; k++){
						if (Info()->elements[k]->type == FORT
						&&Info()->elements[k]->team != Info()->team_num
						&&distance(Info()->elements[k]->pos, enemypos) <= 8){

						ChangeDest(Info()->elements[j]->index, Info()->elements[k]->pos);

						AttackUnit(Info()->elements[j]->index, Info()->elements[k]->index);
						}
						}
						*/

						FighterAttack(Info()->elements[j]->index);//Fighter自卫

					}
				}
			}

			for (int i = 19; i > 10; i--){
				ChangeDest(fighter[i].index, pos1[19 - i]);
				AttackUnit(Info()->elements[i]->index, Info()->elements[enemybase]->index);
			}

		}
	}
	
	else if(Info()->round>=20){
		//全部飞机在基地守家
		int countcount = 0;
		for (int i = 0; i < Info()->element_num; i++){
			if ((Info()->elements[i]->pos.x == pos[0].x &&Info()->elements[i]->pos.y == pos[0].y)
				|| (Info()->elements[i]->pos.x == pos[1].x &&Info()->elements[i]->pos.y == pos[1].y)
				|| (Info()->elements[i]->pos.x == pos[2].x &&Info()->elements[i]->pos.y == pos[2].y)
				|| (Info()->elements[i]->pos.x == pos[3].x &&Info()->elements[i]->pos.y == pos[3].y)
				|| (Info()->elements[i]->pos.x == pos[4].x &&Info()->elements[i]->pos.y == pos[4].y)
				|| (Info()->elements[i]->pos.x == pos[5].x &&Info()->elements[i]->pos.y == pos[5].y)
				|| (Info()->elements[i]->pos.x == pos[6].x &&Info()->elements[i]->pos.y == pos[6].y)
				|| (Info()->elements[i]->pos.x == pos[7].x &&Info()->elements[i]->pos.y == pos[7].y)
				|| (Info()->elements[i]->pos.x == pos[8].x &&Info()->elements[i]->pos.y == pos[8].y)
				){
				countcount++;
			}
		}
		if (countcount < 9){
			for (int h = 0; h < 9 - countcount; h++){
				Produce(FIGHTER);
			}
		}

	}
	


	if (Info()->round == 8){
		
		Produce(FIGHTER);

	}
	int cou = 0;
	for (int j = 0; j < Info()->element_num; j++){
		if (Info()->elements[j]->type == FIGHTER
			&& distance(Info()->elements[j]->pos, mypos) <= 2
			&& cou <= 2){
			cou++;
			FighterAttack(Info()->elements[j]->index);//Fighter自卫
		}
	}


	if (Info()->round >= 3)
	{
		cargo_metal();
		BaseSupply_cargo();
		//sub_all();
	}

	if (Info()->round == 1)
	{
		Produce(CARGO);
		Produce(CARGO);
		Produce(CARGO);
	}

	if (Info()->round >= 10)
	{
		cargo_fuel();
	}

	if (Info()->round == 10000)/***/
	{
		Produce(CARGO);
		//Produce(CARGO);
	}
	/*
	if (Info()->round >= 10)
	{
	//随机判断	(bug)
	cargo_fuel_alive();
	cargo_metal_alive();
	}
	*/
	
	if (Info()->round >= 35){
		for (int i = 0; i < Info()->element_num; i++){
			if (Info()->elements[i]->type == FIGHTER && Info()->elements[i]->ammo == 0){
				ChangeDest(Info()->elements[i]->index, mypos);
			}
		}
	}

	//优先
	cargo_fix();

	//基地自卫
	BaseAct();

	//据点自卫
	FortAct();

	// Your codes here
}

