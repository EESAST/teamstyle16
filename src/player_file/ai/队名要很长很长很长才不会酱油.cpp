#include "basic.h"
#include<cmath>
#include<iostream>
#include<fstream>
#include<cstdlib>
// Remove this line if you konw C++, and don't want a dirty namespace
using namespace teamstyle16;
using std::ofstream;
using std::ios;
using std::endl;


int number[11],product[11],start,mround=-1;
int base_a,base_b;
int amap[100][100][3],bmap[100][100][3];
int product_oil,product_metal;
int oilfield[20],mine[20],fort[20],oilfiled_num=0,mine_num=0,fort_num;
bool oilflag[20],mineflag[20];
int oiltake[20],minetake[20],oiltake1[20],minetake1[20];
int take[100][100][3];
int cargo_num,fighter_num,carrier_num,submarine_num;
int my_cargo[100],my_fighter[100],my_carrier[100],my_submarine[10],task[10000],tas[10000],cnt=0;
int  catch_metal=0;
int xx,yy;
bool game_flag=false,Fort_attack=false,Fort_back=false;
int now_fort=-1,then_fort,now_mine;
int f_attack[10],now_supply=-1,fighter_base=-1,base_attacked=-1;
int hurt_take[10000],hurt_flag[10000];
int disbase;
int fd=0,qk=0;
bool atb=false;



const GameInfo *INFO=Info();
const char * GetTeamName()
{
    return "THU";  // Name of your team
}

void insert(int ind)
{ for (int i=0;i<cnt;++i)
  if (tas[i]==ind) return;
  tas[cnt++]=ind;
};
int cod(int ind)
{ for (int i=0;i<cnt;++i)
  if (tas[i]==ind) return i;
};

int max(int a,int b)
{if (a>b) return a;else return b;};
int min(int a,int b)
{if (a<b) return a;else return b;};

int distance(Position pos1, Position pos2)
{
	return abs(pos1.x - pos2.x) + abs(pos1.y - pos2.y);
}
int distance_base(Position a,int ba)
{ int answer=10000;
  Position temp;
  for (int i=-1;i<2;++i)
	  for (int j=-1;j<2;++j)
	  {temp=GetState(ba)->pos;
       temp.x+=i;temp.y+=j;
	   answer=min(answer,distance(a,temp));
      };
  return answer;
};

int hurt(int gong,int shou)
{ int type1= GetState(gong)->type,type2=GetState(shou)->type;
  float dis;
  
  if (((type1!=BASE)&&(type2!=BASE))||((type1==BASE)&&(type2==BASE))) dis=distance(GetState(gong)->pos,GetState(shou)->pos);
  else if (type1==BASE) dis=distance_base(GetState(shou)->pos,gong);
  else if (type2==BASE) dis=distance_base(GetState(gong)->pos,shou);
  float fire=kProperty[type1].fire_ranges[kProperty[type2].level];
  if (dis>fire) return -1;
  float ans=0;
  if ((kProperty[type1].attacks[0]>0)&&(kProperty[type2].defences[0]!=-1))
	  ans+=float(kProperty[type1].attacks[0])*(1-(dis-fire/2)/(fire+1))-float(kProperty[type2].defences[0]);
  if ((kProperty[type1].attacks[1]>0)&&(kProperty[type2].defences[1]!=-1))
	  ans+=float(kProperty[type1].attacks[1])*(1-(dis-fire/2)/(fire+1))-float(kProperty[type2].defences[1]);
  return int(ans);
};   

void new_produce(ElementType type)
{ Produce(type);
  product_metal+=kProperty[type].cost;
  product_oil+=kProperty[type].fuel_max;
  if (type==CARGO) product_oil-=150;
  ++number[type];
 
}


void init()
{ for (int i=0;i<11;++i) number[i]=0;
  for (int i=0;i<11;++i) product[i]=0;
  for (int i=0;i<100;++i)
	  for (int j=0;j<100;++j)
		  for (int k=0;k<3;++k)
		  {amap[i][j][k]=-1;bmap[i][j][k]=-1;};
  for (int i=0;i<10000;++i) hurt_take[i]=0;
  for (int i=0;i<INFO->element_num;i++)
  {if (INFO->elements[i]->team==INFO->team_num)
     amap[INFO->elements[i]->pos.x][INFO->elements[i]->pos.y][INFO->elements[i]->pos.z]=INFO->elements[i]->type;
     else bmap[INFO->elements[i]->pos.x][INFO->elements[i]->pos.y][INFO->elements[i]->pos.z]=INFO->elements[i]->type;
  
  if (INFO->elements[i]->team==INFO->team_num)
  {++number[INFO->elements[i]->type];};

  if (INFO->elements[i]->team!=INFO->team_num)
	  if ((INFO->elements[i]->type==FIGHTER)||(INFO->elements[i]->type==SCOUT))
		  if (distance_base(INFO->elements[i]->pos,base_a)<=8)
			  fighter_base=mround;

  insert(INFO->elements[i]->index);
  };
  
  product_metal=0;product_oil=0;
  for(int i=0;i<INFO->production_num;++i)
	{   ++number[INFO->production_list[i].unit_type];
        if (INFO->production_list[i].round_left==0)
        ++product[INFO->production_list[i].unit_type];
		product_oil+=kProperty[INFO->production_list[i].unit_type].fuel_max;
		if (INFO->production_list[i].unit_type==CARGO) product_oil-=150;
    }
  
  for (int i=0;i<oilfiled_num;++i)
  { if ((GetState(oilfield[i])->visible==true)&&(GetState(oilfield[i])->fuel<distance_base(GetState(oilfield[i])->pos,base_a)))
    oilflag[i]=false;
    oiltake[i]=0;
  };
  for (int i=0;i<mine_num;++i)
  { if ((GetState(mine[i])->visible==true)&&(GetState(mine[i])->metal==0))
    mineflag[i]=false;
    minetake[i]=0;
  }
	  
  
 }

void first()
{
	for (int i=0;i<20;++i)
    {oilflag[i]=true;mineflag[i]=true;oiltake[i]=0;minetake[i]=0;oiltake1[i]=0;minetake1[i]=0;};
    
	xx=INFO->x_max;yy=INFO->y_max;
	oilfiled_num=0;mine_num=0;fort_num=0;
  for (int i=0;i<INFO->element_num;i++)
  {   int j=INFO->elements[i]->index; 
	  if (INFO->elements[i]->type==BASE)
	  {if(INFO->elements[i]->team==INFO->team_num)
	  base_a=j;else base_b=j;};
      if (INFO->elements[i]->type==OILFIELD)
	   oilfield[oilfiled_num++]=j;
	  if (INFO->elements[i]->type==MINE)
	   mine[mine_num++]=j;
	  if (INFO->elements[i]->type==FORT)
		  fort[fort_num++]=j;
    };
  disbase=distance(GetState(base_a)->pos,GetState(base_b)->pos);
   if (disbase<=110) {for (int i=0;i<8;i++) new_produce(CARGO);}
	
    for (int i=0;i<100;++i)
	for (int j=0;j<100;++j)
    for (int k=0;k<3;++k)
		take[i][j][k]=0;
   for (int i=0;i<10;++i) f_attack[i]=-1;	
   for (int i=0;i<10000;++i) hurt_flag[i]=-1;

   for (int i=0;i<oilfiled_num;++i)
   {if (Map(GetState(oilfield[i])->pos.x+1,GetState(oilfield[i])->pos.y)!=LAND) ++oiltake1[i];
   if (Map(GetState(oilfield[i])->pos.x-1,GetState(oilfield[i])->pos.y)!=LAND) ++oiltake1[i];
   if (Map(GetState(oilfield[i])->pos.x,GetState(oilfield[i])->pos.y+1)!=LAND) ++oiltake1[i];
   if (Map(GetState(oilfield[i])->pos.x,GetState(oilfield[i])->pos.y-1)!=LAND) ++oiltake1[i];
   };
    for (int i=0;i<mine_num;++i)
   {if (Map(GetState(mine[i])->pos.x+1,GetState(mine[i])->pos.y)!=LAND) ++minetake1[i];
	if (Map(GetState(mine[i])->pos.x-1,GetState(mine[i])->pos.y)!=LAND) ++minetake1[i];
	if (Map(GetState(mine[i])->pos.x,GetState(mine[i])->pos.y+1)!=LAND) ++minetake1[i];
	if (Map(GetState(mine[i])->pos.x,GetState(mine[i])->pos.y-1)!=LAND) ++minetake1[i];
	};

}

void add_produce()
{  while ((number[CARGO]<4)&&(product_oil<=GetState(base_a)->fuel-150)
   &&(product_metal<=GetState(base_a)->metal-16)) new_produce(CARGO);
   while ((number[FIGHTER]<8)&&(product_oil<=GetState(base_a)->fuel-100)
   &&(product_metal<=GetState(base_a)->metal-14)) new_produce(FIGHTER);
   while ((number[SUBMARINE]<2)&&(product_oil<=GetState(base_a)->fuel-120)
   &&(product_metal<=GetState(base_a)->metal-7)) new_produce(SUBMARINE); 
	while ((number[SUBMARINE]<4)&&(product_oil<=GetState(base_a)->fuel-250)
   &&(product_metal<=GetState(base_a)->metal-23)) new_produce(SUBMARINE);
	while ((number[CARRIER]<6)&&(product_oil<=GetState(base_a)->fuel-350)
		&&(product_metal<=GetState(base_a)->metal-46))new_produce(CARRIER);
};

void add_produce1()
{  if (fd<2) {++fd;return;};
   if (fd==2) {++fd; for (int i=0;i<5;i++)new_produce(FIGHTER);return;};
   while ((number[CARGO]<8)&&(product_oil<=GetState(base_a)->fuel-150)
   &&(product_metal<=GetState(base_a)->metal-16)) new_produce(CARGO);
   while ((number[CARGO]>=8)&&(product_oil<=GetState(base_a)->fuel-100)
   &&(product_metal<=GetState(base_a)->metal-14)) new_produce(FIGHTER);
  
};

void add_produce2()
{  while ((number[CARGO]<8)&&(product_oil<=GetState(base_a)->fuel-150)
   &&(product_metal<=GetState(base_a)->metal-16)) new_produce(CARGO);
while ((number[CARGO]>=8)&&(number[SUBMARINE]<14)&&(product_oil<=GetState(base_a)->fuel-120)
	   &&(product_metal<=GetState(base_a)->metal-7)) new_produce(SUBMARINE);
   while ((number[CARGO]>=8)&&(number[CARRIER]<6)&&(product_oil<=GetState(base_a)->fuel-200)
	   &&(product_metal<=GetState(base_a)->metal-30)) new_produce(CARRIER);
  
};

void add_produce3()
{  while ((number[CARGO]<8)&&(product_oil<=GetState(base_a)->fuel-150)
   &&(product_metal<=GetState(base_a)->metal-16)) new_produce(CARGO);
   while ((number[CARGO]>=8)&&(number[CARRIER]<13)&&(product_oil<=GetState(base_a)->fuel-200)
	   &&(product_metal<=GetState(base_a)->metal-30)) new_produce(CARRIER);
 };

void make_code()
{ cargo_num=-1;fighter_num=-1;carrier_num=-1,submarine_num=-1;
  for (int i=0;i<INFO->element_num;++i)
  { if (INFO->elements[i]->team!=INFO->team_num) continue;
    switch (INFO->elements[i]->type)
      {case CARGO:{my_cargo[++cargo_num]=i;break;};
       case FIGHTER:{my_fighter[++fighter_num]=i;break;};
	   case CARRIER:{my_carrier[++carrier_num]=i;break;};
	   case SUBMARINE:{my_submarine[++submarine_num]=i;break;};
      };
  };
 
};

int Find_oil(int code)
{ int min_dis=10000,answer=-1;
  for (int i=0;i<oilfiled_num;++i)
  {if (oilflag[i]==false) continue;
   if (oiltake[i]==oiltake1[i]) continue;
   if (min_dis>distance(INFO->elements[code]->pos,GetState(oilfield[i])->pos))
   {min_dis=distance(INFO->elements[code]->pos,GetState(oilfield[i])->pos);
    answer=i;
   };
  };
  return answer;
};
int Find_mine(int code)
{ int min_dis=10000,dis,answer=-1;
  for (int i=0;i<mine_num;++i)
  {if (mineflag[i]==false) continue;
   if (minetake[i]==minetake1[i]) continue;
   dis=distance(INFO->elements[code]->pos,GetState(mine[i])->pos)+distance_base(GetState(mine[i])->pos,base_a);
   if (min_dis>dis)
   {min_dis=dis;
    answer=i;
   };
  };
  now_mine=answer;
  return answer;
};

void Move_cargo_base(int code)
{ if (distance_base(INFO->elements[code]->pos,base_a)==1)
  {ChangeDest(INFO->elements[code]->index,INFO->elements[code]->pos);return;};
  int dis=10000;Position ans;
  for (int i=0;i<xx;++i)
  for (int j=0;j<yy;++j)
  {if (Map(i,j)==LAND) continue;
   Position temp;
   temp.x=i;temp.y=j;temp.z=1;
   if ((amap[i][j][1]!=-1)||(bmap[i][j][1]!=-1)) continue;
   if (take[i][j][1]==mround)  continue;
   if (distance_base(temp,base_a)>1) continue;
   if (distance(INFO->elements[code]->pos,temp)>=dis) continue;
   dis=distance(INFO->elements[code]->pos,temp);
   ans=temp;
  };
  if (dis<10000) {ChangeDest(INFO->elements[code]->index,ans);
  if (distance(INFO->elements[code]->pos,ans)<=11)take[ans.x][ans.y][1]=mround;}
  else ChangeDest(INFO->elements[code]->index,GetState(base_a)->pos);
};

void Move_cargo(int code,int d,int n)
{ 
  if (d==-1) return;
  int des;
  if (n==0) des=oilfield[d]; else des=mine[d];
  if (distance(INFO->elements[code]->pos,GetState(des)->pos)==1)
  {if (n==0) ++oiltake[d];else ++minetake[d];return;};
  if ((distance(INFO->elements[code]->pos,GetState(des)->pos)+5>INFO->elements[code]->fuel)&&
	  (distance_base(INFO->elements[code]->pos,base_a)>1)) return;
  int temp_cost;
  if ((n==0)&&(distance_base(INFO->elements[code]->pos,base_a)==1)) temp_cost=distance(INFO->elements[code]->pos,GetState(des)->pos)+20;
  else temp_cost=0;
  
  if (n==0) Supply(INFO->elements[code]->index,base_a,max(INFO->elements[code]->fuel-temp_cost,0),0,INFO->elements[code]->metal);
  else  Supply(INFO->elements[code]->index,base_a,max(INFO->elements[code]->fuel-temp_cost*2,0),0,INFO->elements[code]->metal);
    
  if (temp_cost>=INFO->elements[code]->fuel)
  {if (now_supply==mround) return;
   Supply(base_a,INFO->elements[code]->index,temp_cost-INFO->elements[code]->fuel,120,0);
   now_supply=mround;
  };
  for (int i=0;i<xx;++i)
  for (int j=0;j<yy;++j)
  {Position temp;
   temp.x=i;temp.y=j;temp.z=1;
   if (Map(i,j)==LAND) continue;
   if (distance(temp,GetState(des)->pos)>1) continue;
   if ((amap[i][j][1]!=-1)||(bmap[i][j][1]!=-1)) continue;
   if (take[i][j][1]==mround)  continue;

   ChangeDest(INFO->elements[code]->index,temp);
   if (distance(INFO->elements[code]->pos,temp)<=11)
	   {take[i][j][1]=mround;
        if (n==0) ++oiltake[d];
		else ++minetake[d];
       };
   return;
  };

  if (n==0) {++oiltake[d];;Move_cargo(code,Find_oil(code),n);}
  else {++minetake[d];Move_cargo(code,Find_mine(code),n);};
};

void Get_out(int code)
{ if (distance_base(INFO->elements[code]->pos,base_a)>1) return;
  int min_dis=10000;
  Position ans;
  for (int i=0;i<xx;++i)
    for (int j=0;j<yy;++j)
	{   Position temp;
        temp.x=i;temp.y=j;temp.z=1;
		if (Map(i,j)==LAND) continue;
        if (amap[i][j][1]!=-1) continue;
		if (bmap[i][j][1]!=-1) continue;
		if (take[i][j][1]==mround) continue;
		if (distance_base(temp,base_a)<=1)continue;
		if (distance_base(temp,base_b)>distance_base(INFO->elements[code]->pos,base_b)) continue;
		int dis=distance(INFO->elements[code]->pos,temp);
		if (min_dis>dis)
		{min_dis=dis;ans=temp;};
  };
  ChangeDest(INFO->elements[code]->index,ans);
  take[ans.x][ans.y][ans.z]=mround;
};

void Get_out2(int code)
{ if (distance_base(INFO->elements[code]->pos,base_a)>1) return;
  int min_dis=10000;
  Position ans;
  for (int i=0;i<xx;++i)
    for (int j=0;j<yy;++j)
	{   Position temp;
        temp.x=i;temp.y=j;temp.z=0;
		if (Map(i,j)==LAND) continue;
        if (amap[i][j][0]!=-1) continue;
		if (bmap[i][j][0]!=-1) continue;
		if (take[i][j][0]==mround) continue;
		if (distance_base(temp,base_a)<=1)continue;
		if (distance_base(temp,base_b)>distance_base(INFO->elements[code]->pos,base_b)) continue;
		int dis=distance(INFO->elements[code]->pos,temp);
		if (min_dis>dis)
		{min_dis=dis;ans=temp;};
  };
  ChangeDest(INFO->elements[code]->index,ans);
  take[ans.x][ans.y][ans.z]=mround;
};

void Get_out1(int code)
{ if (distance_base(INFO->elements[code]->pos,base_a)>0)
  {ChangeDest(INFO->elements[code]->index,INFO->elements[code]->pos);return;};
  int min_dis=10000;
  Position ans;
  for (int i=0;i<xx;++i)
    for (int j=0;j<yy;++j)
	{   Position temp;
        temp.x=i;temp.y=j;temp.z=2;
        if (amap[i][j][2]!=-1) continue;
		if (bmap[i][j][2]!=-1) continue;
		if (take[i][j][2]==mround) continue;
		if (distance_base(temp,base_a)==0)continue;
		if (distance_base(temp,base_b)>distance_base(INFO->elements[code]->pos,base_b)) continue;
		int dis=distance(INFO->elements[code]->pos,temp);
		if (min_dis>dis)
		{min_dis=dis;ans=temp;};
  };
  Position temp=INFO->elements[code]->destination;
  take[temp.x][temp.y][temp.z]=mround-1;
  ChangeDest(INFO->elements[code]->index,ans);
  take[ans.x][ans.y][ans.z]=mround;
  
};

void control_cargo()
{  for (int i=0;i<=cargo_num;++i)
   { int j=my_cargo[i];
	 if (distance_base(INFO->elements[j]->pos,base_a)==1)
	 {if (GetState(base_a)->fuel<1000) task[cod(INFO->elements[j]->index)]=1;
	  else task[cod(INFO->elements[j]->index)]=3;
	 };
	 
	 if (task[cod(INFO->elements[j]->index)]==3)
	 { if (GetState(base_a)->fuel<1000) task[cod(INFO->elements[j]->index)]=1;
	 else if (GetState(base_a)->metal<=200) task[cod(INFO->elements[j]->index)]=1;};
	 
	 if (distance_base(INFO->elements[j]->pos,base_a)>1)
	 {if ((INFO->elements[j]->fuel>=250)&&(task[cod(INFO->elements[j]->index)]==1)) 
	    if (GetState(base_a)->metal<=200) task[cod(INFO->elements[j]->index)]=2;
		else task[cod(INFO->elements[j]->index)]=0;
	 if ((INFO->elements[j]->metal>0)&&(task[cod(INFO->elements[j]->index)]==2))
	 task[cod(INFO->elements[j]->index)]=0;
	 };
   };
  
  for (int i=0;i<=cargo_num;++i)
  { int j=my_cargo[i],in=INFO->elements[j]->index;
    if (task[cod(in)]==0) Move_cargo_base(j);
	else if (task[cod(in)]==1) Move_cargo(j,Find_oil(j),0);
	else if (task[cod(in)]==2) Move_cargo(j,Find_mine(j),1);
	else {Get_out(j);Supply(INFO->elements[j]->index,base_a,max(INFO->elements[j]->fuel-200,0),0,INFO->elements[j]->metal);};
  };
}

void CARRIER_sequence()
{ for (int i=0;i<=carrier_num;++i) 
if (INFO->elements[my_carrier[i]]->fuel==200)
  Get_out(my_carrier[i]);
};

void SUBMARINE_sequence()
{ for (int i=0;i<=submarine_num;++i) 
  if (INFO->elements[my_submarine[i]]->fuel==120)
	Get_out2(my_submarine[i]);
};


void FIGHTER_sequence()
{ if (fighter_num==16) return;
  int cnt=0;
  for (int i=0;i<=fighter_num;++i)
	  if (distance_base(INFO->elements[my_fighter[i]]->destination,base_a)==0) ++cnt;
  for (int i=0;i<INFO->element_num;++i)
  {if (INFO->elements[i]->team==INFO->team_num) continue;
   if (INFO->elements[i]->pos.z!=2) continue;
   if (distance_base(INFO->elements[i]->pos,base_a)==0)++cnt;};
  cnt=product[FIGHTER]-(9-cnt);
  if (cnt==0)
  { for (int i=0;i<=fighter_num;++i)
    if (distance_base(INFO->elements[my_fighter[i]]->destination,base_a)==0)
	{for (int bx=-1;bx<=1;++bx)
	 for (int by=-1;by<=1;++by)
	 {Position temp=GetState(base_a)->pos;
      temp.x+=bx;temp.y+=by;temp.z=2;
	  if (amap[temp.x][temp.y][temp.z]!=-1) continue;
	  if (bmap[temp.x][temp.y][temp.z]!=-1) continue;
	  if (take[temp.x][temp.y][temp.z]==mround) continue;
	  if (distance_base(temp,base_b)<(distance_base(INFO->elements[my_fighter[i]]->destination,base_b)))
		  ChangeDest(INFO->elements[my_fighter[i]]->index,temp);
     };
	};
	return;
  };

  for (int i=0;i<=fighter_num;++i)
  {if (cnt<=0)return;
   if (distance_base(INFO->elements[my_fighter[i]]->destination,base_a)==0)
   {Get_out1(my_fighter[i]);--cnt;};
  };
};

void Move_fighter_back(int code)
{  Position temp;
   if (distance_base(GetState(code)->pos,base_a)==0) 
   {ChangeDest(code,GetState(code)->pos);return;};
   for (int i=0;i<xx;++i)
	   for (int j=0;j<yy;++j)
	   { temp.x=i;temp.y=j;temp.z=2;
         if (distance_base(temp,base_a)>0) continue;
		 if (amap[i][j][2]!=-1) continue;
		 if (bmap[i][j][2]!=-1) continue;
		 if (take[i][j][2]==mround) continue;
		 ChangeDest(code,temp);
		 take[i][j][2]=mround;
		 return;
        };
   ChangeDest(code,GetState(base_a)->pos);
};

void Move_back_submarine(int code)
{ if (distance_base(GetState(code)->pos,base_a)==1) return;
  int min_dis=10000,dis;
  bool find=false;
  Position ans;
	for (int i=0;i<xx;++i)
    for (int j=0;j<yy;++j)
	{ if (Map(i,j)==LAND) continue;
      Position temp;temp.x=i;temp.y=j;temp.z=0;
      if (distance_base(temp,base_a)>1) continue;
      if (amap[i][j][0]!=-1) continue;
	  if (bmap[i][j][0]!=-1) continue;
	  if (take[i][j][0]==mround) continue;
	  dis=distance(GetState(code)->pos,temp);
	  if (dis<min_dis) {min_dis=dis;ans=temp;find=true;};
     };
	if (find==true)
	{ChangeDest(code,ans);take[ans.x][ans.y][ans.z]=mround;return;};
	ChangeDest(code,GetState(base_a)->pos);
};

void Move_carrier_back(int code)
{Position temp;
   if (distance_base(GetState(code)->pos,base_a)==1) return;
   for (int i=0;i<xx;++i)
	   for (int j=0;j<yy;++j)
	   { temp.x=i;temp.y=j;temp.z=1;
         if (distance_base(temp,base_a)>1) continue;
		 if (amap[i][j][1]!=-1) continue;
		 if (bmap[i][j][1]!=-1) continue;
		 if (take[i][j][1]==mround) continue;
		 ChangeDest(code,temp);
		 take[i][j][1]=mround;
		 return;
        };
}

void all_supply()
{   
	if (now_supply==mround)return;
	bool supply_flag=false;
    int min_health=100,min_fuel=300,min_ammo=300,code=-1;
	for (int i=0;i<=cargo_num;++i)
	{ if (distance_base(INFO->elements[my_cargo[i]]->pos,base_a)>1) continue;
	  if (INFO->elements[my_cargo[i]]->health>30) continue;
	  if (INFO->elements[my_cargo[i]]->health<min_health)
	  {min_health=INFO->elements[my_cargo[i]]->health;
	  code=i;supply_flag=true;};
	};
	if (supply_flag)
	{Fix(base_a,INFO->elements[my_cargo[code]]->index);return;};

	for (int i=0;i<=fighter_num;++i)
	{ if (distance_base(INFO->elements[my_fighter[i]]->pos,base_a)>0) continue;
	  if (INFO->elements[my_fighter[i]]->fuel>30) continue;
	  if (INFO->elements[my_fighter[i]]->fuel<min_fuel)
	  {min_fuel=INFO->elements[my_fighter[i]]->fuel;
	  code=i;supply_flag=true;};
	};
	if (supply_flag)
	{Supply(base_a,INFO->elements[my_fighter[code]]->index,100,21,0);return;};

	for (int i=0;i<=fighter_num;++i)
	{ if (distance_base(INFO->elements[my_fighter[i]]->pos,base_a)>0)continue;
	  if (INFO->elements[my_fighter[i]]->ammo>9) continue;
	  if (INFO->elements[my_fighter[i]]->ammo<min_ammo)
	  {min_ammo=INFO->elements[my_fighter[i]]->ammo;
	   code=i;supply_flag=true;};
	};
	if (supply_flag)
	{Supply(base_a,INFO->elements[my_fighter[code]]->index,100,21,0);return;};	    

	for (int i=0;i<=submarine_num;++i)
	{ if (distance_base(INFO->elements[my_submarine[i]]->pos,base_a)>1)continue;
	  if (INFO->elements[my_submarine[i]]->fuel>30) continue;
	  if (INFO->elements[my_submarine[i]]->fuel<min_fuel)
	  {min_fuel=INFO->elements[my_submarine[i]]->fuel;
	   code=i;supply_flag=true;};
	};
	if(supply_flag)
	{Supply(base_a,INFO->elements[my_submarine[code]]->index,120,20,0);
	if (GetState(base_a)->fuel>=120) Get_out2(my_submarine[code]);return;};
	
	for (int i=0;i<=submarine_num;++i)
	{ if (distance_base(INFO->elements[my_submarine[i]]->pos,base_a)>1)continue;
	  if (INFO->elements[my_submarine[i]]->ammo>8) continue;
	  if (INFO->elements[my_submarine[i]]->ammo<min_ammo)
	  {min_ammo=INFO->elements[my_submarine[i]]->ammo;
	   code=i;supply_flag=true;};
	};
	if(supply_flag)
	{Supply(base_a,INFO->elements[my_submarine[code]]->index,120,20,0);
	 if (GetState(base_a)->fuel>=120) Get_out2(my_submarine[code]);return;};

	for (int i=0;i<=carrier_num;++i)
	{ if (distance_base(INFO->elements[my_carrier[i]]->pos,base_a)>1) continue;
	  if (INFO->elements[my_carrier[i]]->health>30) continue;
	  if (INFO->elements[my_carrier[i]]->health<min_health)
	  {min_health=INFO->elements[my_carrier[i]]->health;
	  code=i;supply_flag=true;};
	};
	if (supply_flag)
	{Fix(base_a,INFO->elements[my_carrier[code]]->index);return;};

	for (int i=0;i<=carrier_num;++i)
	{ if (distance_base(INFO->elements[my_carrier[i]]->pos,base_a)>1)continue;
	  if (INFO->elements[my_carrier[i]]->fuel>30) continue;
	  if (INFO->elements[my_carrier[i]]->fuel<min_fuel)
	  {min_fuel=INFO->elements[my_carrier[i]]->fuel;
	   code=i;supply_flag=true;};
	};
	if(supply_flag)
	{Supply(base_a,INFO->elements[my_carrier[code]]->index,200,70,0);
	 if (GetState(base_a)->fuel>=200) Get_out(my_carrier[code]);return;};
	
	for (int i=0;i<=carrier_num;++i)
	{ if (distance_base(INFO->elements[my_carrier[i]]->pos,base_a)>1)continue;
	  if (INFO->elements[my_carrier[i]]->ammo>8) continue;
	  if (INFO->elements[my_carrier[i]]->ammo<min_ammo)
	  {min_ammo=INFO->elements[my_carrier[i]]->ammo;
	   code=i;supply_flag=true;};
	};
	if(supply_flag)
    {Supply(base_a,INFO->elements[my_carrier[code]]->index,200,70,0);
	 if (GetState(base_a)->fuel>=200) Get_out(my_carrier[code]);return;};

	base_attacked=mround;

	for (int i=0;i<=fighter_num;++i)
	{ if (distance_base(INFO->elements[my_fighter[i]]->pos,base_a)>0)continue;
	  if (INFO->elements[my_fighter[i]]->ammo==21) continue;
	  if (INFO->elements[my_fighter[i]]->ammo<min_ammo)
	  {min_ammo=INFO->elements[my_fighter[i]]->ammo;
	   code=i;supply_flag=true;};
	};
	if (supply_flag)
	{Supply(base_a,INFO->elements[my_fighter[code]]->index,100,21,0);return;};	    

	for (int i=0;i<=submarine_num;++i)
	{ if (distance_base(INFO->elements[my_submarine[i]]->pos,base_a)>1)continue;
	  if (INFO->elements[my_submarine[i]]->ammo==20) continue;
	  if (INFO->elements[my_submarine[i]]->ammo<min_ammo)
	  {min_ammo=INFO->elements[my_submarine[i]]->ammo;
	   code=i;supply_flag=true;};
	};
	if(supply_flag)
	{Supply(base_a,INFO->elements[my_submarine[code]]->index,120,20,0);
	 if (GetState(base_a)->fuel>=120) Get_out2(my_submarine[code]);return;};
    
	for (int i=0;i<=fighter_num;++i)
	{ if (distance_base(INFO->elements[my_fighter[i]]->pos,base_a)>0) continue;
	  if (INFO->elements[my_fighter[i]]->fuel>90) continue;
	  if (INFO->elements[my_fighter[i]]->fuel<min_fuel)
	  {min_fuel=INFO->elements[my_fighter[i]]->fuel;
	  code=i;supply_flag=true;};
	};
	if (supply_flag)
	{Supply(base_a,INFO->elements[my_fighter[code]]->index,100,21,0);return;};

	for (int i=0;i<=submarine_num;++i)
	{ if (distance_base(INFO->elements[my_submarine[i]]->pos,base_a)>1)continue;
	  if (INFO->elements[my_submarine[i]]->fuel>100) continue;
	  if (INFO->elements[my_submarine[i]]->fuel<min_fuel)
	  {min_fuel=INFO->elements[my_submarine[i]]->fuel;
	   code=i;supply_flag=true;};
	};
	if(supply_flag)
	{Supply(base_a,INFO->elements[my_submarine[code]]->index,120,20,0);
	 if (GetState(base_a)->fuel>=120) Get_out2(my_submarine[code]);return;};
	
	for (int i=0;i<=fighter_num;++i)
	{ if (distance_base(INFO->elements[my_fighter[i]]->pos,base_a)>0) continue;
	  if (INFO->elements[my_fighter[i]]->health==50) continue;
	  if (INFO->elements[my_fighter[i]]->health<min_health)
	  {min_health=INFO->elements[my_fighter[i]]->health;
	  code=i;supply_flag=true;};
	};
	if (supply_flag)
	{Fix(base_a,INFO->elements[my_fighter[code]]->index);return;};

	for (int i=0;i<=cargo_num;++i)
	{ if (distance_base(INFO->elements[my_cargo[i]]->pos,base_a)>1) continue;
	  if (INFO->elements[my_cargo[i]]->health==80) continue;
	  if (INFO->elements[my_cargo[i]]->health<min_health)
	  {min_health=INFO->elements[my_cargo[i]]->health;
	  code=i;supply_flag=true;};
	};
	if (supply_flag)
	{Fix(base_a,INFO->elements[my_cargo[code]]->index);return;};
	
	for (int i=0;i<=fighter_num;++i)
	{ if (distance_base(INFO->elements[my_fighter[i]]->pos,base_a)>0) continue;
	  if (INFO->elements[my_fighter[i]]->fuel==100) continue;
	  if (INFO->elements[my_fighter[i]]->fuel<min_fuel)
	  {min_fuel=INFO->elements[my_fighter[i]]->fuel;
	  code=i;supply_flag=true;};
	};
	if (supply_flag)
	{Supply(base_a,INFO->elements[my_fighter[code]]->index,100,21,0);return;};

	for (int i=0;i<=submarine_num;++i)
		if (INFO->elements[my_submarine[i]]->fuel<120)
		{Supply(base_a,INFO->elements[my_submarine[i]]->index,120,20,0);
	     if (GetState(base_a)->fuel>=120) Get_out2(my_submarine[i]);return;};

	for (int i=0;i<=carrier_num;++i)
		if (INFO->elements[my_carrier[i]]->fuel<200)
		{Supply(base_a,INFO->elements[my_fighter[code]]->index,200,70,0);
	     if (GetState(base_a)->fuel>=120) Get_out(my_carrier[i]);return;};
  	
}

void Defence_fighter()
{ int temp;
  for (int i=0;i<=fighter_num;++i)
  { if (distance_base(INFO->elements[my_fighter[i]]->pos,base_a)==0) task[cod(INFO->elements[my_fighter[i]]->index)]=0;
    if (task[cod(INFO->elements[my_fighter[i]]->index)]==0) 
	{ if (INFO->elements[my_fighter[i]]->ammo==0){Move_fighter_back(INFO->elements[my_fighter[i]]->index);continue;};
	  
	  int figheter_max=0,fighter_attack=-1;bool fighter_get=false;
	  for (int j=0;j<INFO->element_num;++j)
	  { if (INFO->elements[j]->team==INFO->team_num)continue;
	    if (INFO->elements[j]->type==SUBMARINE) continue;
		if (hurt_flag[j]==mround) continue;
	    temp=hurt(INFO->elements[my_fighter[i]]->index,INFO->elements[j]->index);
	    if (temp>figheter_max) {figheter_max=temp;fighter_attack=j;};
	   };
	  if (fighter_attack!=-1) 
	  {AttackPos(INFO->elements[my_fighter[i]]->index,GetState(INFO->elements[fighter_attack]->index)->pos);f_attack[i]=mround;
	  if (INFO->elements[fighter_attack]->visible==true)
		  if (hurt_take[fighter_attack]-5>=INFO->elements[fighter_attack]->health) hurt_flag[fighter_attack]=mround;
	  hurt_take[fighter_attack]+=figheter_max;
	   if (INFO->elements[my_fighter[i]]->ammo<=3){Move_fighter_back(INFO->elements[my_fighter[i]]->index);continue;}; }
	  else 
	    { bool fighter_flag=false;
	      for (int j=0;j<INFO->element_num;++j)
		  {if (INFO->elements[j]->team==INFO->team_num) continue;
		  if (INFO->elements[j]->type==OILFIELD)continue;
		  if (INFO->elements[j]->type==MINE) continue;
		  if (INFO->elements[j]->type==FORT) continue;
		  if (INFO->elements[j]->type==SUBMARINE)continue;
		  if (hurt_flag[j]==mround) continue;
		  if (distance_base(INFO->elements[j]->pos,base_a)<=8)
		  {Position temp=INFO->elements[j]->pos;temp.z=2;
		   ChangeDest(INFO->elements[my_fighter[i]]->index,temp);
		   AttackPos(INFO->elements[my_fighter[i]]->index,GetState(INFO->elements[j]->index)->pos);
		   f_attack[i]=mround;
		   fighter_flag=true;break;
		  };
		  };
		  if (fighter_flag==false)
		    { for (int j=0;j<INFO->element_num;++j)
	          { if (INFO->elements[j]->team==INFO->team_num)continue;
		        if (hurt_flag[j]==mround) continue;
				if (qk==1) continue;
	            temp=hurt(INFO->elements[my_fighter[i]]->index,INFO->elements[j]->index);
	            if (temp>figheter_max) {figheter_max=temp;fighter_attack=j;};
	          };
	          if (fighter_attack!=-1) 
			  {AttackPos(INFO->elements[my_fighter[i]]->index,GetState(INFO->elements[fighter_attack]->index)->pos);f_attack[i]=mround;
			   if (INFO->elements[fighter_attack]->visible==true)
		         if (hurt_take[fighter_attack]-5>=INFO->elements[fighter_attack]->health) hurt_flag[fighter_attack]=mround;
	           hurt_take[fighter_attack]+=figheter_max;
			   if (INFO->elements[my_fighter[i]]->ammo<=3){Move_fighter_back(INFO->elements[my_fighter[i]]->index);continue;};}
			   else 
	           {  for (int j=0;j<INFO->element_num;++j)
		         {if (INFO->elements[j]->team==INFO->team_num) continue;
		          if (INFO->elements[j]->type==OILFIELD)continue;
		          if (INFO->elements[j]->type==MINE) continue;
		          if (INFO->elements[j]->type==FORT) continue;
				  if (hurt_flag[j]==mround) continue;
				  if (qk==1) continue;
		          if (distance_base(INFO->elements[j]->pos,base_a)<=8)
		          {Position temp=INFO->elements[j]->pos;temp.z=2;
		           ChangeDest(INFO->elements[my_fighter[i]]->index,temp);
				   AttackPos(INFO->elements[my_fighter[i]]->index,GetState(INFO->elements[j]->index)->pos);
		           f_attack[i]=mround;
		           fighter_flag=true;break;
		           };
		          };
		         if (fighter_flag==false)
				 {
					 for (int j=0;j<INFO->element_num;++j)
	                { if (INFO->elements[j]->team==INFO->team_num)continue;
	                  if (INFO->elements[j]->type==SUBMARINE) continue;
		              temp=hurt(INFO->elements[my_fighter[i]]->index,INFO->elements[j]->index);
	                 if (temp>figheter_max) {figheter_max=temp;fighter_attack=j;};
	                };
	                if (fighter_attack!=-1) 
					{AttackPos(INFO->elements[my_fighter[i]]->index,GetState(INFO->elements[fighter_attack]->index)->pos);f_attack[i]=mround;
	                if (INFO->elements[my_fighter[i]]->ammo<=3){Move_fighter_back(INFO->elements[my_fighter[i]]->index);continue;}; }
	                else 
	                { for (int j=0;j<INFO->element_num;++j)
		             {if (INFO->elements[j]->team==INFO->team_num) continue;
		              if (INFO->elements[j]->type==OILFIELD)continue;
		              if (INFO->elements[j]->type==MINE) continue;
		              if (INFO->elements[j]->type==FORT) continue;
		              if (INFO->elements[j]->type==SUBMARINE)continue;
		              if (distance_base(INFO->elements[j]->pos,base_a)<=8)
		              {Position temp=INFO->elements[j]->pos;temp.z=2;
		              ChangeDest(INFO->elements[my_fighter[i]]->index,temp);
					  AttackPos(INFO->elements[my_fighter[i]]->index,GetState(INFO->elements[j]->index)->pos);
		              f_attack[i]=mround;
		              fighter_flag=true;break;
		             };
		            };
		            if (fighter_flag==false)
		            { for (int j=0;j<INFO->element_num;++j)
	                  { if (INFO->elements[j]->team==INFO->team_num)continue;
					    if (qk==1)continue;
	                    temp=hurt(INFO->elements[my_fighter[i]]->index,INFO->elements[j]->index);
	                    if (temp>figheter_max) {figheter_max=temp;fighter_attack=INFO->elements[j]->index;};
	                };
	                if (fighter_attack!=-1) 
					{AttackPos(INFO->elements[my_fighter[i]]->index,GetState(fighter_attack)->pos);f_attack[i]=mround;
			        if (INFO->elements[my_fighter[i]]->ammo<=3){Move_fighter_back(INFO->elements[my_fighter[i]]->index);continue;};}
			        else
	                {for (int j=0;j<INFO->element_num;++j)
		            {if (INFO->elements[j]->team==INFO->team_num) continue;
		             if (INFO->elements[j]->type==OILFIELD)continue;
		             if (INFO->elements[j]->type==MINE) continue;
		             if (INFO->elements[j]->type==FORT) continue;
					 if (qk==1)continue;
		             if (distance_base(INFO->elements[j]->pos,base_a)<=8)
		             {Position temp=INFO->elements[j]->pos;temp.z=2;
		              ChangeDest(INFO->elements[my_fighter[i]]->index,temp);
					  AttackPos(INFO->elements[my_fighter[i]]->index,GetState(INFO->elements[j]->index)->pos);
		              f_attack[i]=mround;
		              fighter_flag=true;break;
		             };
		             };
		             if (fighter_flag==false)
			         Move_fighter_back(INFO->elements[my_fighter[i]]->index);
			        };
				    };
			       };
		          };
	             };
		  };
		  };
	  if (fighter_base==mround) Move_fighter_back(INFO->elements[my_fighter[i]]->index);
	  };
    };

};

void Defence_submarine()
{int temp;
 for (int i=0;i<=submarine_num;++i)
	{ if (INFO->elements[my_submarine[i]]->ammo==0){Move_back_submarine(INFO->elements[my_submarine[i]]->index);continue;};
	  int submarine_max=0,submarine_attack=-1;
	  int min_dis=10000,sub_ans;bool find_submarine=false;
	  for (int j=0;j<INFO->element_num;++j)
	  { if (INFO->elements[j]->team==INFO->team_num)continue;
	    if (INFO->elements[j]->type!=SUBMARINE) continue;
		if (hurt_flag[j]==mround) continue;
		if (distance_base(INFO->elements[j]->pos,base_a)<=8)
		{ Position temp1=INFO->elements[j]->pos;temp1.z=0;
		  int dis=distance(temp1,INFO->elements[my_submarine[i]]->pos);
		  if (min_dis>dis) { min_dis=dis;sub_ans=j;find_submarine=true;}
		};
	  };
	  if (find_submarine)
	  {ChangeDest(INFO->elements[my_submarine[i]]->index,INFO->elements[sub_ans]->pos);
	  if (min_dis<=5) 
	  {AttackPos(INFO->elements[my_submarine[i]]->index,GetState(INFO->elements[sub_ans]->index)->pos);
	   if (INFO->elements[sub_ans]->visible==true)
		   if (hurt_take[sub_ans]>=INFO->elements[sub_ans]->health) hurt_flag[sub_ans]=mround;
	   hurt_take[sub_ans]+=hurt(INFO->elements[my_submarine[i]]->index,INFO->elements[sub_ans]->index);
	   if (INFO->elements[my_submarine[i]]->ammo<=2) Move_back_submarine(INFO->elements[my_submarine[i]]->index);}
	  else {for (int j=0;j<INFO->element_num;++j)
	        { if (INFO->elements[j]->team==INFO->team_num)continue;
	          if (hurt_flag[j]==mround) continue;
	          temp=hurt(INFO->elements[my_submarine[i]]->index,INFO->elements[j]->index);
	          if (temp>submarine_max) {submarine_max=temp;submarine_attack=j;};
	         };
	         if (submarine_attack!=-1) 
			 {AttackPos(INFO->elements[my_submarine[i]]->index,GetState(INFO->elements[submarine_attack]->index)->pos);
			  if (INFO->elements[submarine_attack]->visible==true)
				  if (hurt_take[submarine_attack]>=INFO->elements[submarine_attack]->health) hurt_flag[submarine_attack]=mround;
			  hurt_take[submarine_attack]+=submarine_max;
			  if (INFO->elements[my_submarine[i]]->ammo<=2) Move_back_submarine(INFO->elements[my_submarine[i]]->index);};
	        };
	   continue;
	  };	   
	  for (int j=0;j<INFO->element_num;++j)
	  { if (INFO->elements[j]->team==INFO->team_num)continue;
	    if (hurt_flag[j]==mround)continue;
	    temp=hurt(INFO->elements[my_submarine[i]]->index,INFO->elements[j]->index);
	    if (temp>submarine_max) {submarine_max=temp;submarine_attack=j;};
	   };
	  if (submarine_attack!=-1) 
	  {AttackPos(INFO->elements[my_submarine[i]]->index,GetState(INFO->elements[submarine_attack]->index)->pos);
	       if (INFO->elements[submarine_attack]->visible==true)
				  if (hurt_take[submarine_attack]>=INFO->elements[submarine_attack]->health) hurt_flag[submarine_attack]=mround;
			  hurt_take[submarine_attack]+=submarine_max;
	       if (INFO->elements[my_submarine[i]]->ammo<=2) Move_back_submarine(INFO->elements[my_submarine[i]]->index);
		   continue;}
	  else 
	  { bool submarine_flag=false;
	      for (int j=0;j<INFO->element_num;++j)
		  {if (INFO->elements[j]->team==INFO->team_num) continue;
		  if (INFO->elements[j]->type==OILFIELD)continue;
		  if (INFO->elements[j]->type==MINE) continue;
		  if (INFO->elements[j]->type==FORT) continue;
		  if (INFO->elements[j]->pos.z==2)continue;
		  if (hurt_flag[j]==mround)continue;
		  if (distance_base(INFO->elements[j]->pos,base_a)<=8)
		  {Position temp=INFO->elements[j]->pos;temp.z=0;
		   ChangeDest(INFO->elements[my_submarine[i]]->index,temp);
		   AttackPos(INFO->elements[my_submarine[i]]->index,GetState(INFO->elements[j]->index)->pos);
		   submarine_flag=true;break;
		  };
	      };
		  if (submarine_flag==true) continue;
		  for (int j=0;j<INFO->element_num;++j)
	     { if (INFO->elements[j]->team==INFO->team_num)continue;
	       if (INFO->elements[j]->type!=SUBMARINE) continue;
		   if (hurt_flag[j]==mround) continue;
		   if (distance_base(INFO->elements[j]->pos,base_a)<=8)
		   { Position temp1=INFO->elements[j]->pos;temp1.z=0;
		     int dis=distance(temp1,INFO->elements[my_submarine[i]]->pos);
		     if (min_dis>dis) { min_dis=dis;sub_ans=j;find_submarine=true;}
		 };
	     };
	     if (find_submarine)
	     {ChangeDest(INFO->elements[my_submarine[i]]->index,INFO->elements[sub_ans]->pos);
	     if (min_dis<=5) 
		 {AttackPos(INFO->elements[my_submarine[i]]->index,GetState(INFO->elements[sub_ans]->index)->pos);
	      if (INFO->elements[my_submarine[i]]->ammo<=2) Move_back_submarine(INFO->elements[my_submarine[i]]->index);}
	     else {for (int j=0;j<INFO->element_num;++j)
	        { if (INFO->elements[j]->team==INFO->team_num)continue;
	          temp=hurt(INFO->elements[my_submarine[i]]->index,INFO->elements[j]->index);
	          if (temp>submarine_max) {submarine_max=temp;submarine_attack=INFO->elements[j]->index;};
	         };
	         if (submarine_attack!=-1) 
			 {AttackPos(INFO->elements[my_submarine[i]]->index,GetState(submarine_attack)->pos);
			      if (INFO->elements[my_submarine[i]]->ammo<=2) Move_back_submarine(INFO->elements[my_submarine[i]]->index);};
	        };
	       continue;
	      };	   
	     for (int j=0;j<INFO->element_num;++j)
	    { if (INFO->elements[j]->team==INFO->team_num)continue;
	      temp=hurt(INFO->elements[my_submarine[i]]->index,INFO->elements[j]->index);
	      if (temp>submarine_max) {submarine_max=temp;submarine_attack=INFO->elements[j]->index;};
	     };
	     if (submarine_attack!=-1) 
		  {AttackPos(INFO->elements[my_submarine[i]]->index,GetState(submarine_attack)->pos);
	       if (INFO->elements[my_submarine[i]]->ammo<=2) Move_back_submarine(INFO->elements[my_submarine[i]]->index);}
	     else 
	     {  for (int j=0;j<INFO->element_num;++j)
		    {if (INFO->elements[j]->team==INFO->team_num) continue;
		     if (INFO->elements[j]->type==OILFIELD)continue;
		     if (INFO->elements[j]->type==MINE) continue;
		     if (INFO->elements[j]->type==FORT) continue;
		     if (INFO->elements[j]->pos.z==2)continue;
		     if (distance_base(INFO->elements[j]->pos,base_a)<=8)
		     {Position temp=INFO->elements[j]->pos;temp.z=0;
		      ChangeDest(INFO->elements[my_submarine[i]]->index,temp);
			  AttackPos(INFO->elements[my_submarine[i]]->index,GetState(INFO->elements[j]->index)->pos);
		      submarine_flag=true;break;
		     };
		    };
		   //if (submarine_flag==false) Move_back_submarine(INFO->elements[my_submarine[i]]->index);
		 };
	  };
	};  
};

void Defence_carrier()
{   int temp;
	for (int i=0;i<=carrier_num;++i)
	{ int carrier_max=0,carrier_attack=-1;
	  for (int j=0;j<INFO->element_num;++j)
	          { if (INFO->elements[j]->team==INFO->team_num)continue;
	            if (hurt_flag[j]==mround) continue;
	            temp=hurt(INFO->elements[my_carrier[i]]->index,INFO->elements[j]->index);
	            if (temp>carrier_max) {carrier_max=temp;carrier_attack=j;};
	          };
	          if (carrier_attack!=-1) 
			  {AttackPos(INFO->elements[my_carrier[i]]->index,GetState(INFO->elements[carrier_attack]->index)->pos);
			   if (INFO->elements[carrier_attack]->visible==true)
				   if (hurt_take[carrier_attack]>=INFO->elements[carrier_attack]->health) hurt_flag[carrier_attack]=mround;
			   hurt_take[carrier_attack]+=carrier_max;
			  }
			   else 
	           { bool carrier_flag=false;
	             for (int j=0;j<INFO->element_num;++j)
		         {if (INFO->elements[j]->team==INFO->team_num) continue;
		          if (INFO->elements[j]->type==OILFIELD)continue;
		          if (INFO->elements[j]->type==MINE) continue;
		          if (INFO->elements[j]->type==FORT) continue;
				  if (hurt_flag[j]==mround) continue; 
		          if (distance_base(INFO->elements[j]->pos,base_a)<=8)
		          {Position temp=INFO->elements[j]->pos;temp.z=1;
		           ChangeDest(INFO->elements[my_carrier[i]]->index,temp);
				   AttackPos(INFO->elements[my_carrier[i]]->index,GetState(INFO->elements[j]->index)->pos);
		           carrier_flag=true;break;
		           };
		          };
				 if (carrier_flag==false)
				 { for (int j=0;j<INFO->element_num;++j)
	              { if (INFO->elements[j]->team==INFO->team_num)continue;
	                temp=hurt(INFO->elements[my_carrier[i]]->index,INFO->elements[j]->index);
	                if (temp>carrier_max) {carrier_max=temp;carrier_attack=INFO->elements[j]->index;};
	               };
	               if (carrier_attack!=-1) 
				   {AttackPos(INFO->elements[my_carrier[i]]->index,GetState(carrier_attack)->pos);}
			      else 
	              { for (int j=0;j<INFO->element_num;++j)
		           {if (INFO->elements[j]->team==INFO->team_num) continue;
		            if (INFO->elements[j]->type==OILFIELD)continue;
		            if (INFO->elements[j]->type==MINE) continue;
		            if (INFO->elements[j]->type==FORT) continue;
		            if (distance_base(INFO->elements[j]->pos,base_a)<=8)
		            {Position temp=INFO->elements[j]->pos;temp.z=1;
		             ChangeDest(INFO->elements[my_carrier[i]]->index,temp);
					 AttackPos(INFO->elements[my_carrier[i]]->index,GetState(INFO->elements[j]->index)->pos);
		             carrier_flag=true;break;
		             };
		            };
			       };
			     };
			    };
			  if (INFO->elements[my_carrier[i]]->health<=30) Move_carrier_back(INFO->elements[my_carrier[i]]->index);
	 };

};


void Defence()
{ int temp;
  if (base_attacked==mround)
  {int base_max=0,base_attack=-1;
  for (int i=0;i<INFO->element_num;++i)
  { if (INFO->elements[i]->team==INFO->team_num) continue;
     temp=hurt(base_a,INFO->elements[i]->index);
	 if (temp>base_max) {base_max=temp;base_attack=i;};
  };
  if (base_attack!=-1) {AttackPos(base_a,GetState(INFO->elements[base_attack]->index)->pos);hurt_take[base_attack]+=base_max;};
};

  for (int i=0;i<fort_num;++i)
  { if (GetState(fort[i])->team!=INFO->team_num)continue;
    int fort_max=0,fort_attack=-1;
    for (int j=0;j<INFO->element_num;++j)
	{ if (INFO->elements[j]->team==INFO->team_num) continue;
	  temp=hurt(fort[i],INFO->elements[j]->index);
	  if (temp>fort_max){fort_max=temp;fort_attack=j;};
	};
	if (fort_attack!=-1) {AttackPos(fort[i],INFO->elements[fort_attack]->pos);hurt_take[fort_attack]+=fort_max;};
  };

  
  Defence_submarine(); 
  Defence_fighter();
  Defence_carrier();  

	

}

int find_fort()
{ int min_dis=10000,ans=-1,dis;
  for (int i=0;i<fort_num;++i)
  { if (GetState(fort[i])->team==INFO->team_num) continue;
     dis=distance_base(GetState(fort[i])->pos,base_a);
	 if (min_dis>dis) {min_dis=dis;ans=i;};
   };
  return ans;
};

void Move_fighter(int gong,int shou)
{ Position pshou=GetState(shou)->pos;
  int min_dis1=distance(GetState(gong)->pos,pshou),min_dis2=0,dis1,dis2;
  Position ans;
  bool find=false;
  for (int i=0;i<xx;++i)
	  for (int j=0;j<yy;++j)
	  { if (amap[i][j][2]!=-1)continue;
        if (bmap[i][j][2]!=-1)continue;
		if (take[i][j][2]==mround) continue;
		Position temp;temp.x=i;temp.y=j;temp.z=2;
		dis1=distance(temp,pshou);dis2=distance(temp,GetState(gong)->pos);
		if ((min_dis1>dis1)||((min_dis1==dis1)&&(min_dis2>dis2)))
		{min_dis1=dis1;min_dis2=dis2;ans=temp;find=true;};
      };
  if (find==true)
  {ChangeDest(gong,ans);take[ans.x][ans.y][2]=mround;return;};
  pshou.z=2;
  ChangeDest(gong,pshou);
};
      
void Attack_Fort()
{ for (int i=0;i<=fighter_num;++i)
  if (task[cod(INFO->elements[my_fighter[i]]->index)]==1)
  { int figheter_max=0,fighter_attack=-1;
	  for (int j=0;j<INFO->element_num;++j)
	  { if (INFO->elements[j]->team==INFO->team_num)continue;
	    int temp=hurt(INFO->elements[my_fighter[i]]->index,INFO->elements[j]->index);
	    if (temp>figheter_max) {figheter_max=temp;fighter_attack=INFO->elements[j]->index;};
	   };
	  if (fighter_attack!=-1) 
	  AttackUnit(INFO->elements[my_fighter[i]]->index,fighter_attack);
	  Move_fighter_back(INFO->elements[my_fighter[i]]->index);
   };
   
  if (Fort_back==true) 
  { for (int i=0;i<fighter_num;i++)
    if (task[cod(INFO->elements[my_fighter[i]]->index)]==1) return;
    Fort_back=false;
  };
  
  if (now_fort==-1)now_fort=find_fort();
  if (now_fort==-1){Fort_attack=false;return;};

  int cnt=0;
  for (int i=0;i<=fighter_num;++i)
  if (task[cod(INFO->elements[my_fighter[i]]->index)]==2) ++cnt;
  if (cnt==0) Fort_attack=false;
  
  if (Fort_attack==false)
  {if (fighter_num<7) return;
   now_fort=find_fort();
   if (now_fort==-1) return;
   if (fighter_base==mround) return;
   for (int i=0;i<4;++i) 
	   if ((INFO->elements[my_fighter[i]]->ammo<21)||(f_attack[i]==mround)||
		   (INFO->elements[my_fighter[i]]->fuel-10<distance(INFO->elements[my_fighter[i]]->pos,GetState(fort[now_fort])->pos))) return;
   for (int i=0;i<4;++i) task[cod(INFO->elements[my_fighter[i]]->index)]=2;
   Fort_attack=true;
  };
  
  if (GetState(fort[now_fort])->team==INFO->team_num)
  { Fort_back=true;Fort_attack=false;
    for (int i=0;i<=fighter_num;++i)
		if (task[cod(INFO->elements[my_fighter[i]]->index)]==2)
		{task[cod(INFO->elements[my_fighter[i]]->index)]=1;Move_fighter_back(INFO->elements[my_fighter[i]]->index);};
  };

  for (int i=0;i<=fighter_num;++i)
	  if (task[cod(INFO->elements[my_fighter[i]]->index)]==2)
	  {Move_fighter(INFO->elements[my_fighter[i]]->index,fort[now_fort]);
       if (distance(INFO->elements[my_fighter[i]]->pos,GetState(fort[now_fort])->pos)<=3)
		   {AttackUnit(INFO->elements[my_fighter[i]]->index,fort[now_fort]);
	        if (INFO->elements[my_fighter[i]]->fuel<=3)
			{task[cod(INFO->elements[my_fighter[i]]->index)]=1;Move_fighter_back(INFO->elements[my_fighter[i]]->index);};
	       }
	   else
	   {int figheter_max=0,fighter_attack=-1;
	    for (int j=0;j<INFO->element_num;++j)
	    { if (INFO->elements[j]->team==INFO->team_num)continue;
	      int temp=hurt(INFO->elements[my_fighter[i]]->index,INFO->elements[j]->index);
	      if (temp>figheter_max) {figheter_max=temp;fighter_attack=INFO->elements[j]->index;};
	    };
	    if (fighter_attack!=-1) 
			{AttackUnit(INFO->elements[my_fighter[i]]->index,fighter_attack);
		     if (INFO->elements[my_fighter[i]]->fuel<=3)
			 {task[cod(INFO->elements[my_fighter[i]]->index)]=1;Move_fighter_back(INFO->elements[my_fighter[i]]->index);};
	       };
	   };
      };
};

void cargo_supply()
{ int max_fuel=0,ans=-1;
  for (int i=0;i<=cargo_num;++i)
  {if (distance_base(INFO->elements[my_cargo[i]]->pos,base_a)==1) continue;
   int max_fuel=0,ans=-1;
   for (int j=0;j<INFO->element_num;++j)
   {if (INFO->elements[j]->team!=INFO->team_num)continue;
    if (INFO->elements[j]->pos.z==2) continue;
	if (distance(INFO->elements[j]->pos,INFO->elements[my_cargo[i]]->pos)>1)continue;
	int temp=kProperty[INFO->elements[j]->type].ammo_max-INFO->elements[j]->ammo;
	if (temp>max_fuel) {max_fuel=temp;ans=j;};
   };
   if (ans==-1)continue;
   Supply(INFO->elements[my_cargo[i]]->index,INFO->elements[ans]->index,0,max_fuel,0);
  };
};

void attack_Fighter(int gong)
{ int fighter_max=0,t,a;
  Position ans;
  for (int i=0;i<INFO->element_num;++i)
 {if (INFO->elements[i]->team==INFO->team_num) continue;
  if (INFO->elements[i]->type==FORT) continue;
  if (INFO->elements[i]->type==CARGO) continue;
  if (INFO->elements[i]->type==SUBMARINE) continue;
  if (hurt_flag[i]==mround) continue;
  t=hurt(INFO->elements[my_fighter[gong]]->index,INFO->elements[i]->index);
  if (t>fighter_max){fighter_max=t;a=i;};
  };
  if (fighter_max>0)
  { AttackPos(INFO->elements[my_fighter[gong]]->index,INFO->elements[a]->pos);
     if (INFO->elements[a]->visible==true)
		  if (hurt_take[a]-5>=INFO->elements[a]->health) hurt_flag[a]=mround;
	  hurt_take[a]+=fighter_max;
	  return;
  };


  int min_dis=10000,dis;Position temp,temp1=GetState(base_b)->pos;
  for (int i=-1;i<=1;++i)
  for (int j=-1;j<=1;++j)
  {temp.x=temp1.x+i;
   temp.y=temp1.y+j;
   temp.z=temp1.z;
   dis=distance(INFO->elements[my_fighter[gong]]->pos,temp);
   if (min_dis>dis)
   {min_dis=dis;ans=temp;};
  };
  AttackPos(INFO->elements[my_fighter[gong]]->index,ans);
};

void attack_carrier(int gong)
{ int carrier_max=0,t,a;
  Position ans;
  for (int i=0;i<INFO->element_num;++i)
 {if (INFO->elements[i]->team==INFO->team_num) continue;
  if (INFO->elements[i]->type==FORT) continue;
  if (INFO->elements[i]->type==CARGO) continue;
  if (hurt_flag[i]==mround) continue;
  t=hurt(INFO->elements[my_carrier[gong]]->index,INFO->elements[i]->index);
  if (t>carrier_max){carrier_max=t;a=i;};
  };
  if (carrier_max>0)
  { AttackPos(INFO->elements[my_carrier[gong]]->index,INFO->elements[a]->pos);
     if (INFO->elements[a]->visible==true)
		  if (hurt_take[a]-5>=INFO->elements[a]->health) hurt_flag[a]=mround;
	  hurt_take[a]+=carrier_max;
	  return;
  };


  int min_dis=10000,dis;Position temp,temp1=GetState(base_b)->pos;
  for (int i=-1;i<=1;++i)
  for (int j=-1;j<=1;++j)
  {temp.x=temp1.x+i;
   temp.y=temp1.y+j;
   temp.z=temp1.z;
   dis=distance(INFO->elements[my_carrier[gong]]->pos,temp);
   if (min_dis>dis)
   {min_dis=dis;ans=temp;};
  };
  AttackPos(INFO->elements[my_carrier[gong]]->index,ans);
};

void attack_submarine(int gong)
{ int submarine_max=0,t,a;
  for (int i=0;i<INFO->element_num;++i)
 {if (INFO->elements[i]->team==INFO->team_num) continue;
  if (INFO->elements[i]->type==FORT) continue;
  if (hurt_flag[i]==mround) continue;
  t=hurt(INFO->elements[my_submarine[gong]]->index,INFO->elements[i]->index);
  if (t>submarine_max){submarine_max=t;a=i;};
  };
  if (submarine_max>0)
  { AttackPos(INFO->elements[my_submarine[gong]]->index,INFO->elements[a]->pos);
     if (INFO->elements[a]->visible==true)
		  if (hurt_take[a]-5>=INFO->elements[a]->health) hurt_flag[a]=mround;
	  hurt_take[a]+=submarine_max;
	  return;
  };
};

void go_carrrier(int code)
{int min_dis1=distance_base(INFO->elements[my_carrier[code]]->pos,base_b),min_dis2=0,dis1,dis2;
  Position ans;
  bool find=false;
  for (int i=0;i<xx;++i)
	  for (int j=0;j<yy;++j)
	  { if (Map(i,j)==LAND) continue;
		if (amap[i][j][1]!=-1)continue;
        if (bmap[i][j][1]!=-1)continue;
		if (take[i][j][1]==mround) continue;
		if ((i-GetState(base_a)->pos.x)*(i-GetState(base_b)->pos.x)>0) continue;
		if ((j-GetState(base_a)->pos.y)*(j-GetState(base_b)->pos.y)>0) continue;
		Position temp;temp.x=i;temp.y=j;temp.z=1;
		dis1=distance_base(temp,base_b);dis2=distance(temp,INFO->elements[my_carrier[code]]->pos);
		if ((min_dis1>dis1)||((min_dis1==dis1)&&(min_dis2>dis2)))
		{min_dis1=dis1;min_dis2=dis2;ans=temp;find=true;};
      };
  if (find==true)
  {ChangeDest(INFO->elements[my_carrier[code]]->index,ans);take[ans.x][ans.y][1]=mround;return;};
}


void go_submarine(int code)
{int min_dis1=distance_base(INFO->elements[my_submarine[code]]->pos,base_b),min_dis2=0,dis1,dis2;
  Position ans;
  bool find=false;
  for (int i=0;i<xx;++i)
	  for (int j=0;j<yy;++j)
	  { if (Map(i,j)==LAND) continue;
		if (amap[i][j][0]!=-1)continue;
        if (bmap[i][j][0]!=-1)continue;
		if (take[i][j][0]==mround) continue;
		if ((i-GetState(base_a)->pos.x)*(i-GetState(base_b)->pos.x)>0) continue;
		if ((j-GetState(base_a)->pos.y)*(j-GetState(base_b)->pos.y)>0) continue;
		Position temp;temp.x=i;temp.y=j;temp.z=0;
		dis1=distance_base(temp,base_b);dis2=distance(temp,INFO->elements[my_submarine[code]]->pos);
		if ((min_dis1>dis1)||((min_dis1==dis1)&&(min_dis2>dis2)))
		{min_dis1=dis1;min_dis2=dis2;ans=temp;find=true;};
      };
  if (find==true)
  {ChangeDest(INFO->elements[my_submarine[code]]->index,ans);take[ans.x][ans.y][0]=mround;return;};
}

void attackbase1()
{  if (atb==false)
   {if (fighter_num<16) return;
    for (int i=0;i<=fighter_num;++i)
		if (f_attack[i]==mround) return;
	atb=true;
   };
   for (int i=0;i<=fighter_num;++i) 
	   {Move_fighter(INFO->elements[my_fighter[i]]->index,base_b);
        attack_Fighter(i);
       };
};

void attackbase2()
{   
	if (atb==false)
	{if (submarine_num<13) return;
	 if (carrier_num<5) return;
	 for (int i=0;i<INFO->element_num;++i)
	 {if (INFO->elements[i]->type==FORT) continue;
	  if (INFO->elements[i]->type==OILFIELD) continue;
	  if (INFO->elements[i]->type==MINE) continue;
	  if (INFO->elements[i]->type==CARGO) continue;
	  if (INFO->elements[i]->team==INFO->team_num) continue;
	  if (distance_base(INFO->elements[i]->pos,base_a)>8) continue;
	  return;
	 };
	 atb=true;
	};
	for (int i=0;i<=carrier_num;++i) {go_carrrier(i);attack_carrier(i);};
	for (int i=0;i<=submarine_num;++i) {go_submarine(i);attack_submarine(i);};

}

void attackbase3()
{  	if (atb==false)
	{if (carrier_num<12) return;
	 for (int i=0;i<INFO->element_num;++i)
	 {if (INFO->elements[i]->type==FORT) continue;
	  if (INFO->elements[i]->type==OILFIELD) continue;
	  if (INFO->elements[i]->type==MINE) continue;
	  if (INFO->elements[i]->type==CARGO) continue;
	  if (INFO->elements[i]->team==INFO->team_num) continue;
	  if (distance_base(INFO->elements[i]->pos,base_a)>8) continue;
	  return;
	 };
	 atb=true;
	};
	for (int i=0;i<=carrier_num;++i) {go_carrrier(i);attack_carrier(i);};
}

void AIMain()
{   
	TryUpdate();
	if (INFO->round==mround) return;
	mround=INFO->round;
	if (game_flag==false) {first();game_flag=true;return;};
	if  (disbase<=80)
	{qk=1;
	init();
	add_produce1();
    make_code();
	control_cargo();
	all_supply();
	Defence();
	cargo_supply();
	FIGHTER_sequence();
	attackbase1();
	}
	else if (disbase<=110)
	{init(); 
	add_produce2();
    make_code();
	control_cargo();
	CARRIER_sequence();
	SUBMARINE_sequence();
	all_supply();
	Defence();
	cargo_supply();
	attackbase2();
	}
	else
	{init(); 
	add_produce3();
    make_code();
	control_cargo();
	CARRIER_sequence();
	all_supply();
	Defence();
	cargo_supply();	
	attackbase3();
	};
	
	// Your codes here
}