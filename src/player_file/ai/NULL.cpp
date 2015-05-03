#include <cstdlib>
#include "basic.h"
using namespace teamstyle16;


const char * GetTeamName()
{
    return "很文静的男子";  // Name of your team
}

//declaration
    void gather();//gather()
	int  GetIndex(ElementType,int);//GetIndex(ELEMENT,team)
	int  distance(Position,Position);
	int  distance(int,int);
	void prepare();
	
	bool ifalive(int);
	bool fight_condition(int );
	bool pos_avali(Position,int);
    int  abs(int );
	int  max(int,int);
	int  min(int,int);

	void Get_FORT_index();
	void Get_MINE_index();
	void Get_OILFIELD_index();
	void Get_fundnum();          //get the const_variables
	void Get_nearcorner();       //the corner nearest to mybase
    void sort(int *,int);        //sort
	void enemy1_find();
	void enemy2_find();
	void pos_used_initial();

	void FORT_attack(); 
	void BASE_Attack();
	
	void make_team(ElementType,int,int);//to make a team（type，size of team,team_num）
	void team_Attack(int,int,int);//（team_num，size of team,the index of which you want to attack）
	void team_Changdest(int,int,Position);//（team_num，size of team,postion you want to move to）
	void changedest(int,Position);
	void FORT_control();
	enum base_state{null=0,attack_st,fix_st,supply_st};//current state of mybase

/*
 team[1] 
 team[2] 
 team[3] 
 team[4] */
int MyTeam=-1;
int wait_num[30]={0};
int team[30][10]={0};
int OILFIELD_index[20]={0},MINE_index[20]={0},FORT_index[21]={0},FORT_enemy[21]={0};//the num of FORT should less than 10,the num of MINE AND OILFIELD should less than 20(not together)
bool teamed[10000]={0};
int My_BASE_index=0,Enemy_BASE_index=0,enemy_index1=0,enemy_index2=0;
int MINE_now=0,OILFIELD_now=0,FORT_now=1;//WARNING:FORT_now is beginning from 1
bool pos_used[80][80][3]={0};//map should smaller than 80*80
/*int round_su;//????
base_state base_state_cur;*/
Position near_BASE={0,0,0};//the nearest corner

void AIMain()
{
	if(Info()->round==1)  
		prepare();

	

	if(Info()->round>=2)
	{
		    pos_used_initial();
		    if(Info()->round>=4)
				make_team(CARGO,4,1);

			    gather();
			
			enemy1_find();

			make_team(FIGHTER,5,2);
			if(enemy_index1!=0)
			{
				team_Attack(2,5,enemy_index1);
			    AttackUnit(My_BASE_index,enemy_index1);
			}
			//make_team(DESTROYER,5,3);
			//team_Attack(3,5,Enemy_BASE_index);
			if(Info()->round>=8)
				FORT_control();
			if(Info()->round>=17)
			{
				make_team(CARRIER,4,5);
				/*if(enemy_index1!=0)
					team_Attack(5,4,enemy_index1);
				else 
					team_Changdest(5,4,near_BASE);*/
				team_Attack(5,4,Enemy_BASE_index);
			}
			if(Info()->round>=12)
			{
				make_team(SUBMARINE,4,4);
				if(enemy_index1==0)return;
				if(kProperty[GetState(enemy_index1)->type].level!=AIR)
				team_Attack(4,4,enemy_index1);
				else
					{
						enemy2_find();
						if(enemy_index2!=0)
							team_Attack(4,4,enemy_index2);
						else
							team_Changdest(4,4,near_BASE);
					}
			}
			//BASE_Attack();
			//Update();
	}
	
	
	//base couldn't attack when supplying or doing other things (except produce)
	/*if(Info()->round-round_su>=2)
		base_state_cur=null;
	if(base_state_cur==null)*/
	
}
void gather()
{//to be wrote:CARGO should move when attacked **************
	for(int i=0;i<4;i++)
		if(team[1][i]==0)continue;
		else
		if(ifalive(team[1][i])==1)
		if(!fight_condition(team[1][i]))
			 if(distance(team[1][i],My_BASE_index)>=1)
	     	 changedest(team[1][i],GetState(My_BASE_index)->pos);
		  else
			 {
				 Fix(My_BASE_index,team[1][i]);
				 /*base_state_cur=fix_st;
				 round_su=Info()->round;*/
		     }
			 if(OILFIELD_index[0]==0)return;
			 if(MINE_index[0]==0)return;
	
     if(GetState(My_BASE_index)->metal<4*kProperty[BASE].metal_max/5&&!(MINE_now!=0&&MINE_index[MINE_now]==0))//metal in base not enough
	 {
		 if(GetState(MINE_index[MINE_now])->visible==1&&GetState(MINE_index[MINE_now])->metal==0)  //metal in this MINE is 0
	     MINE_now++;
		 for(int i=2;i<4;i++)
		 {
	      if(team[1][i]==0)continue;
	      if(GetState(team[1][i])->metal>3*kProperty[CARGO].metal_max/5)
	      {
	     	 changedest(team[1][i],GetState(My_BASE_index)->pos);
	    	 Supply(team[1][i],My_BASE_index,0,0,kProperty[CARGO].metal_max);
	    	 if(GetState(team[1][i])->fuel<kProperty[CARGO].fuel_max/3)
	    	 Supply(My_BASE_index,team[1][i],kProperty[CARGO].fuel_max/2-GetState(team[1][i])->fuel,0,0);
	      }
		  else
		  {
			  changedest(team[1][i],GetState(MINE_index[MINE_now])->pos);
	      }
		 }
	 }
	else                             //CARGO stay in the nearest corner when my base has enough metal
		 for(int i=2;i<4;i++)
			 changedest(team[1][i],near_BASE);
    if(GetState(My_BASE_index)->fuel<4*kProperty[BASE].fuel_max/5&&!(OILFIELD_now!=0&&OILFIELD_index[OILFIELD_now]==0))//fuel in base not enough
	   {
		   if(GetState(OILFIELD_index[OILFIELD_now])->visible==1&&GetState(OILFIELD_index[OILFIELD_now])->fuel==0)//fuel in this OILFIELD is 0
	       OILFIELD_now++;
		   for(int i=0;i<2;i++)
		 {
	      if(team[1][i]==0)continue;
	      if(GetState(team[1][i])->fuel>2*kProperty[CARGO].fuel_max/5)
	      {
	     	 changedest(team[1][i],GetState(My_BASE_index)->pos);
	    	 Supply(team[1][i],My_BASE_index,GetState(team[1][i])->fuel-2*kProperty[CARGO].fuel_max/5,0,0);
	      }
		  else
		  {
			  changedest(team[1][i],GetState(OILFIELD_index[OILFIELD_now])->pos);
	      }
		 }
	}
	 else                             //CARGO stay in the nearest corner when my base has enough fuel
		 for(int i=0;i<2;i++)
			 if(team[1][i]==0)continue;
			 else
			 changedest(team[1][i],near_BASE);
}
int  GetIndex(ElementType type,int team)//search for index  mark teamed[index]=1 when find
{
	for(int i=0;i<Info()->element_num;i++)
		if(Info()->elements[i]->type==type&&Info()->elements[i]->team==team&&teamed[Info()->elements[i]->index]==0)
		{
			teamed[Info()->elements[i]->index]=1;
		    return Info()->elements[i]->index;
		}
	return -1;
}
int  distance(Position pos1,Position pos2)
{
	return abs(pos1.x-pos2.x)+abs(pos1.y-pos2.y);
}
int  distance(int index1,int index2)
{
	return abs(GetState(index1)->pos.x-GetState(index2)->pos.x)+abs(GetState(index1)->pos.y-GetState(index2)->pos.y);
}
int  abs(int x)
{
	return x>0 ? x:-x;
}
bool ifalive(int index)
{
	for(int i=0;i<Info()->element_num;i++)
		if(Info()->elements[i]->index==index)
			return 1;
	return 0;
}
void make_team(ElementType type,int num,int team_num)
{
  	int lack=0;//lack

 for(int i=0;i<num;i++)
	 if(team[team_num][i]==0)
	 {
	     lack++;
	 }
	 else if(ifalive(team[team_num][i])==0)//when dead,mark teamed[index]=0;
	 {
		 lack++;
		 teamed[team[team_num][i]]=0; 
		 team[team_num][i]=0;         //delete from the team
	 }


    int wait_num_cur=wait_num[team_num];

	for(int i=0;i<lack-wait_num_cur;i++)//produce
 {
	 Produce(type);
	 wait_num[team_num]++;
 }

 for(int i=0;i<Info()->element_num;i++)//search for CARGO not teamed[]
 {
	 if(Info()->elements[i]->type==type&&Info()->elements[i]->team==MyTeam&&teamed[Info()->elements[i]->index]==0&&wait_num[team_num]>0)
	 {
		 wait_num[team_num]--;
		 for(int k=0;k<num;k++)//when find,get it in my team
			 if(team[team_num][k]==0)
			 {
				 team[team_num][k]=Info()->elements[i]->index;
			     teamed[team[team_num][k]]=1;
			     break;
			 }
	 }
 }


}
void team_Attack(int team_num,int num,int goal_index)
{
	if(goal_index==0)return;
  for(int i=0;i<num;i++)
  {
	  if(team[team_num][i]==0)continue;
	  if(ifalive(team[team_num][i])==1)
	  if(!fight_condition(team[team_num][i]))
		  if(distance(team[team_num][i],My_BASE_index)>=3)
	     	 changedest(team[team_num][i],GetState(My_BASE_index)->pos);
		  else
			  if(GetState(team[team_num][i])->health!=kProperty[GetState(team[team_num][i])->type].health_max)
			   {
				 Fix(My_BASE_index,team[team_num][i]);
				/* base_state_cur=fix_st;
				 round_su=Info()->round;*/
		       }
			  else
               {
				   Supply(My_BASE_index,team[team_num][i],kProperty[GetState(team[team_num][i])->type].fuel_max,kProperty[GetState(team[team_num][i])->type].health_max,0);
			/*	 base_state_cur=fix_st;
				 round_su=Info()->round;*/
		       }
	  else
	  {if(distance(team[team_num][i],goal_index)>=3)changedest(team[team_num][i],GetState(goal_index)->pos);
	   AttackUnit(team[team_num][i],goal_index);}
  }
}
void FORT_control()
{
	make_team(FIGHTER,4,3);

	if(FORT_index[0]==0)return;

	int flag=0;
	if(FORT_index[FORT_now]!=0)
	if(GetState(FORT_index[FORT_now])->team==MyTeam)
	{
		flag=-1;
		for(int i=0;i<10;i++)
			if(FORT_index[i]==0)break;
			else
			if(GetState(FORT_index[i])->team!=MyTeam)
			{
				FORT_now=i;
				flag=1;
				break;
			}
	}
	if(flag==-1)
	{
		team_Changdest(3,4,GetState(My_BASE_index)->pos);
		return;
	}
	team_Attack(3,4,FORT_index[FORT_now]);
}
void team_Changdest(int team_num,int num,Position pos)
{
	for(int i=0;i<num;i++)
  {
	  if(team[team_num][i]==0)continue;
	  if(ifalive(team[team_num][i])==1)
	  if(!fight_condition(team[team_num][i]))
		  if(distance(GetState(team[team_num][i])->pos,GetState(My_BASE_index)->pos)>=3)
	     	 changedest(team[team_num][i],GetState(My_BASE_index)->pos);
		  else
			  if(GetState(team[team_num][i])->health!=kProperty[GetState(team[team_num][i])->type].health_max)
			   {
				 Fix(My_BASE_index,team[team_num][i]);
				/* base_state_cur=fix_st;
				 round_su=Info()->round;*/
		       }
			  else
               {
				   Supply(My_BASE_index,team[team_num][i],kProperty[GetState(team[team_num][i])->type].fuel_max,kProperty[GetState(team[team_num][i])->type].health_max,0);
			/*	 base_state_cur=fix_st;
				 round_su=Info()->round;*/
		       }
	  else
	  changedest(team[team_num][i],pos);
  }
}
void changedest(int index,Position pos)
{
	if(index==0)return;
	int z;
	int type_note=0;            //to mark whether this is a water vehicle(couldn't move to land)
	z=kProperty[GetState(index)->type].level;
	if(z!=2)
		type_note=1;

	pos_used[GetState(index)->destination.x][GetState(index)->destination.y][z]=0;

	Position pos1=pos;
	if(pos_avali(pos1,z)==1&&!(Map(pos1.x,pos1.y)==LAND&&type_note==1))
	{ChangeDest(index,pos1);pos_used[pos1.x][pos1.y][z]=1;return;}

		pos1.x=pos.x;
		pos1.y=pos.y+1;
		pos1.z=pos.z;

    if(pos_avali(pos1,z)==1&&!(Map(pos1.x,pos1.y)==LAND&&type_note==1))
	{ChangeDest(index,pos1);pos_used[pos1.x][pos1.y][z]=1;return;}

	    pos1.x=pos.x-1;
		pos1.y=pos.y;
		pos1.z=pos.z;

    if(pos_avali(pos1,z)==1&&!(Map(pos1.x,pos1.y)==LAND&&type_note==1))
	{ChangeDest(index,pos1);pos_used[pos1.x][pos1.y][z]=1;return;}

	    pos1.x=pos.x;
		pos1.y=pos.y-1;
		pos1.z=pos.z;

    if(pos_avali(pos1,z)==1&&!(Map(pos1.x,pos1.y)==LAND&&type_note==1))
	{ChangeDest(index,pos1);pos_used[pos1.x][pos1.y][z]=1;return;}

	    pos1.x=pos.x+1;
		pos1.y=pos.y;
		pos1.z=pos.z;

    if(pos_avali(pos1,z)==1&&!(Map(pos1.x,pos1.y)==LAND&&type_note==1))
	{ChangeDest(index,pos1);pos_used[pos1.x][pos1.y][z]=1;return;}

	    pos1.x=pos.x-1;
		pos1.y=pos.y+1;
		pos1.z=pos.z;

    if(pos_avali(pos1,z)==1&&!(Map(pos1.x,pos1.y)==LAND&&type_note==1))
	{ChangeDest(index,pos1);pos_used[pos1.x][pos1.y][z]=1;return;}

	    pos1.x=pos.x-1;
		pos1.y=pos.y-1;
		pos1.z=pos.z;

    if(pos_avali(pos1,z)==1&&!(Map(pos1.x,pos1.y)==LAND&&type_note==1))
	{ChangeDest(index,pos1);pos_used[pos1.x][pos1.y][z]=1;return;}

	    pos1.x=pos.x+1;
		pos1.y=pos.y-1;
		pos1.z=pos.z;

    if(pos_avali(pos1,z)==1&&!(Map(pos1.x,pos1.y)==LAND&&type_note==1))
	{ChangeDest(index,pos1);pos_used[pos1.x][pos1.y][z]=1;return;}

	    pos1.x=pos.x+1;
		pos1.y=pos.y+1;
		pos1.z=pos.z;

    if(pos_avali(pos1,z)==1&&!(Map(pos1.x,pos1.y)==LAND&&type_note==1))
	{ChangeDest(index,pos1);pos_used[pos1.x][pos1.y][z]=1;return;}
	
		pos1.x=pos.x+1;
		pos1.y=pos.y+2;
		pos1.z=pos.z;

    if(pos_avali(pos1,z)==1&&!(Map(pos1.x,pos1.y)==LAND&&type_note==1))
	{ChangeDest(index,pos1);pos_used[pos1.x][pos1.y][z]=1;return;}
	    pos1.x=pos.x;
	 	pos1.y=pos.y+2;
		pos1.z=pos.z;

    if(pos_avali(pos1,z)==1&&!(Map(pos1.x,pos1.y)==LAND&&type_note==1))
	{ChangeDest(index,pos1);pos_used[pos1.x][pos1.y][z]=1;return;}
	    pos1.x=pos.x-1;
		pos1.y=pos.y+2;
		pos1.z=pos.z;

    if(pos_avali(pos1,z)==1&&!(Map(pos1.x,pos1.y)==LAND&&type_note==1))
	{ChangeDest(index,pos1);pos_used[pos1.x][pos1.y][z]=1;return;}
	    pos1.x=pos.x-2;
		pos1.y=pos.y+1;
		pos1.z=pos.z;

    if(pos_avali(pos1,z)==1&&!(Map(pos1.x,pos1.y)==LAND&&type_note==1))
	{ChangeDest(index,pos1);pos_used[pos1.x][pos1.y][z]=1;return;}
	    pos1.x=pos.x-2;
		pos1.y=pos.y;
		pos1.z=pos.z;

    if(pos_avali(pos1,z)==1&&!(Map(pos1.x,pos1.y)==LAND&&type_note==1))
	{ChangeDest(index,pos1);pos_used[pos1.x][pos1.y][z]=1;return;}
	    pos1.x=pos.x-2;
		pos1.y=pos.y+1;
		pos1.z=pos.z;

    if(pos_avali(pos1,z)==1&&!(Map(pos1.x,pos1.y)==LAND&&type_note==1))
	{ChangeDest(index,pos1);pos_used[pos1.x][pos1.y][z]=1;return;}
	    pos1.x=pos.x-1;
		pos1.y=pos.y-2;
		pos1.z=pos.z;

    if(pos_avali(pos1,z)==1&&!(Map(pos1.x,pos1.y)==LAND&&type_note==1))
	{ChangeDest(index,pos1);pos_used[pos1.x][pos1.y][z]=1;return;}
	    pos1.x=pos.x;
		pos1.y=pos.y-2;
		pos1.z=pos.z;

    if(pos_avali(pos1,z)==1&&!(Map(pos1.x,pos1.y)==LAND&&type_note==1))
	{ChangeDest(index,pos1);pos_used[pos1.x][pos1.y][z]=1;return;}
	    pos1.x=pos.x+1;
		pos1.y=pos.y-2;
		pos1.z=pos.z;

    if(pos_avali(pos1,z)==1&&!(Map(pos1.x,pos1.y)==LAND&&type_note==1))
	{ChangeDest(index,pos1);pos_used[pos1.x][pos1.y][z]=1;return;}
	    pos1.x=pos.x+2;
		pos1.y=pos.y-1;
		pos1.z=pos.z;

    if(pos_avali(pos1,z)==1&&!(Map(pos1.x,pos1.y)==LAND&&type_note==1))
	{ChangeDest(index,pos1);pos_used[pos1.x][pos1.y][z]=1;return;}
	    pos1.x=pos.x+2;
		pos1.y=pos.y;
		pos1.z=pos.z;

    if(pos_avali(pos1,z)==1&&!(Map(pos1.x,pos1.y)==LAND&&type_note==1))
	{ChangeDest(index,pos1);pos_used[pos1.x][pos1.y][z]=1;return;}
	    pos1.x=pos.x+2;
		pos1.y=pos.y+1;
		pos1.z=pos.z;

    if(pos_avali(pos1,z)==1&&!(Map(pos1.x,pos1.y)==LAND&&type_note==1))
	{ChangeDest(index,pos1);pos_used[pos1.x][pos1.y][z]=1;return;}

	ChangeDest(index,pos);return;
}
bool fight_condition(int index)//3/10health  3/10fuel 1 ammo_once
{
	if(GetState(index)->type!=CARGO)
	     return (GetState(index)->health>=3*kProperty[GetState(index)->type].health_max/10)&&(GetState(index)->fuel>=3*kProperty[GetState(index)->type].fuel_max/10)&&(GetState(index)->ammo>=kProperty[GetState(index)->type].ammo_once);
    else
	     return (GetState(index)->health>=3*kProperty[GetState(index)->type].health_max/10)&&(GetState(index)->fuel>=2*kProperty[GetState(index)->type].fuel_max/10);
}
bool pos_avali(Position pos,int z)
{
	if(pos_used[pos.x][pos.y][z]==1)return 0;
	for(int i=0;i<Info()->element_num;i++)
		if(Info()->elements[i]->pos.x==pos.x&&Info()->elements[i]->pos.y==pos.y&&Info()->elements[i]->pos.z==z)
				return 0;
	return 1;
}
void Get_FORT_index()
{
	int k=0;
	for(int i=0;i<Info()->element_num;i++)
		if(Info()->elements[i]->type==FORT)
		{
			FORT_index[k]=Info()->elements[i]->index;
			k++;
		}
}
void Get_MINE_index()
{
	int k=0;
	for(int i=0;i<Info()->element_num;i++)
		if(Info()->elements[i]->type==MINE)
		{
			MINE_index[k]=Info()->elements[i]->index;
			k++;
		}
}
void Get_OILFIELD_index()
{
	int k=0;
	for(int i=0;i<Info()->element_num;i++)
		if(Info()->elements[i]->type==OILFIELD)
		{
			OILFIELD_index[k]=Info()->elements[i]->index;
			k++;
		}
}
void Get_fundnum()
{			   
			                Get_FORT_index();                        
							Get_MINE_index();                       
	                        Get_OILFIELD_index();                    
	                        MyTeam=Info()->team_num;                 
	                        My_BASE_index=GetIndex(BASE,MyTeam);     
	                        Enemy_BASE_index=GetIndex(BASE,1-MyTeam);
							Get_nearcorner();                        
							sort(OILFIELD_index,20);
							sort(MINE_index,20);
							sort(FORT_index,10);
						//	for(int i=19;i>=0;i--)
						//		FORT_index[i]=FORT_index[i+1];
}
void Get_nearcorner()
{
	Position left_up={0,0,0},right_up={Info()->x_max,0,1},left_down={0,Info()->y_max,1},right_down={Info()->x_max,Info()->y_max,1};
	if(distance(left_up,GetState(My_BASE_index)->pos)>distance(right_up,GetState(My_BASE_index)->pos))
		near_BASE=right_up;
	if(distance(near_BASE,GetState(My_BASE_index)->pos)>distance(left_down,GetState(My_BASE_index)->pos))
		near_BASE=left_down;
	if(distance(near_BASE,GetState(My_BASE_index)->pos)>distance(right_down,GetState(My_BASE_index)->pos))
		near_BASE=right_down;
}
void enemy1_find()
{
	int flag1=0,flag2=0,i; //flag1 is to mark whether find a satisfied enemy from elements,flag2 is to mark whether the enemy(last round) is dead or visible
			if(enemy_index1==0)
			for(i=0,flag2=1;i<Info()->element_num;i++)
				if(Info()->elements[i]->team==1-MyTeam&&distance(Info()->elements[i]->index,My_BASE_index)<12)
				{enemy_index1=Info()->elements[i]->index;flag1=1;}
				else;
			else if(ifalive(enemy_index1)==0)
			for(i=0,flag2=1;i<Info()->element_num;i++)
				if(Info()->elements[i]->team==1-MyTeam&&distance(Info()->elements[i]->index,My_BASE_index)<12)
				{enemy_index1=Info()->elements[i]->index;flag1=1;}
				else;
			else if(GetState(enemy_index1)->visible==0)
			for(i=0,flag2=1;i<Info()->element_num;i++)
				if(Info()->elements[i]->team==1-MyTeam&&distance(Info()->elements[i]->index,My_BASE_index)<12)
				{enemy_index1=Info()->elements[i]->index;flag1=1;}

			if(flag1==0&&flag2==1)enemy_index1=0;
}
void enemy2_find()
{
	int flag1=0,flag2=0,i; //flag1 is to mark whether find a satisfied enemy from elements,flag2 is to mark whether the enemy(last round) is dead or visible
			if(enemy_index2==0)
			for(i=0,flag2=1;i<Info()->element_num;i++)
				if(Info()->elements[i]->team==1-MyTeam&&distance(Info()->elements[i]->index,My_BASE_index)<12)
				{enemy_index2=Info()->elements[i]->index;flag1=1;}
				else;
			else if(ifalive(enemy_index2)==0)
			for(i=0,flag2=1;i<Info()->element_num;i++)
				if(Info()->elements[i]->team==1-MyTeam&&distance(Info()->elements[i]->index,My_BASE_index)<12)
				{enemy_index2=Info()->elements[i]->index;flag1=1;}
				else;
			else if(GetState(enemy_index2)->visible==0)
			for(i=0,flag2=1;i<Info()->element_num;i++)
				if(Info()->elements[i]->team==1-MyTeam&&distance(Info()->elements[i]->index,My_BASE_index)<12)
				{enemy_index2=Info()->elements[i]->index;flag1=1;}

			if(flag1==0&&flag2==1)enemy_index2=0;
}
void BASE_Attack()
{
	for(int i=0;i<Info()->element_num;i++)
		if(Info()->elements[i]->team==1-MyTeam)
			AttackUnit(My_BASE_index,Info()->elements[i]->index);
}
void sort(int *index,int num)//
{
	int type_num=0;
	for(int i=0;i<num;i++)//sort according to the distance between my base and enemybase 
		if(index[i]==0)break;
		else if(distance(index[i],My_BASE_index)<distance(index[i],Enemy_BASE_index))continue;
		else
			for(int j=i+1;j<num;j++)
				if(index[j]==0)break;
	            else if(distance(index[j],My_BASE_index)<distance(index[j],Enemy_BASE_index))
				{
					int temp=index[i];
					index[i]=index[j];
					index[j]=temp;
				}
   for(int i=0;i<num;i++)
	   if(distance(index[i+1],My_BASE_index)>distance(index[i+1],Enemy_BASE_index))break;
	   else
		   for(int j=i+1;j<num;j++)
			   if(distance(index[j],My_BASE_index)>distance(index[j],Enemy_BASE_index))break;
			   else 
				   if(distance(index[j],My_BASE_index)<distance(index[i],My_BASE_index))
				   {
					   int temp=index[i];
					   index[i]=index[j];
					   index[j]=temp;
				   }
   for(int i=0;i<num;i++)
	   if(index[i+1]==0)break;
	   else if(distance(index[i+1],My_BASE_index)<distance(index[i+1],Enemy_BASE_index))continue;
	   else
		   for(int j=i+1;j<num;j++)
			   if(index[j]==0)break;
			   else if(distance(index[j],Enemy_BASE_index)>distance(index[i],Enemy_BASE_index))
				   {
					   int temp=index[i];
					   index[i]=index[j];
					   index[j]=temp;
				   }
}
void prepare()
{
	    Get_fundnum();
		Produce(CARGO);
		Produce(CARGO);
		Produce(FIGHTER);
		Produce(FIGHTER);
		Produce(FIGHTER);
		Produce(FIGHTER);
		Produce(FIGHTER);
		wait_num[2]=5;
		wait_num[1]=2;
}
int  max(int a,int b)
{
	return a>b?a:b;
}
int  min(int a,int b)
{
	return a>b?b:a;
}
void pos_used_initial()
{
	for(int i=0;i<80;i++)
		for(int j=0;j<80;j++)
			for(int k=0;k<3;k++)
				pos_used[i][j][k]=0;
}
void FORT_attack()
{
	for(int i=0;i<21;i++)
	{
		if(FORT_index[i]==0)continue;
		if(GetState(FORT_index[i])->team==MyTeam)
		{
			int flag1=0,flag2=0,j; //flag1 is to mark whether find a satisfied enemy from elements,flag2 is to mark whether the enemy(last round) is dead or visible
			if(FORT_enemy[i]==0)
			for(j=0,flag2=1;j<Info()->element_num;j++)
				if(Info()->elements[j]->team==1-MyTeam&&distance(Info()->elements[j]->index,My_BASE_index)<12)
				{enemy_index1=Info()->elements[j]->index;flag1=1;}
				else;
			else if(ifalive(enemy_index1)==0)
			for(j=0,flag2=1;j<Info()->element_num;j++)
				if(Info()->elements[j]->team==1-MyTeam&&distance(Info()->elements[j]->index,My_BASE_index)<12)
				{enemy_index1=Info()->elements[j]->index;flag1=1;}
				else;
			else if(GetState(enemy_index1)->visible==0)
			for(j=0,flag2=1;j<Info()->element_num;j++)
				if(Info()->elements[j]->team==1-MyTeam&&distance(Info()->elements[j]->index,My_BASE_index)<12)
				{enemy_index1=Info()->elements[j]->index;flag1=1;}

			if(flag1==0&&flag2==1)enemy_index1=0;
		}
	}
}
