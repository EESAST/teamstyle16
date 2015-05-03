#include "basic.h"
#include<iostream>
#include<fstream>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cmath>
#include<map>
#include<vector>
#include<algorithm>

int MyTactics=0;//{开启第几套AI战术： 0=空军防御+进攻流  1=空军压制流  2=航母潜艇压制流  3=航母潜艇一波流  4=纯航母一波流  5=纯航母压制流}
int AI_Begin=false;

// Remove this line if you konw C++, and don't want a dirty namespace
using namespace teamstyle16;
using std::min;
using std::max;
using std::vector;
using std::string;
using std::ofstream;
using std::map;
using std::swap;
using std::cout;
using std::endl;
using std::sort;

//ofstream output("DEBUG.txt");
const int INF=10000; //INF为10000
const char * GetTeamName()//返回常变量字符指针的队名函数
{
    return "ShangShi_44_Kill";  // Name of your team
}

int f(int a){return a<0?INF:a;}//计算参数的真实值(-1→INF)
struct coor
{
	coor(int a=0,int b=0,int c=0):x(a),y(b),z(c){}
	coor(const Position &a):x(a.x),y(a.y),z(a.z){}
	int x,y,z;
};
bool operator<(const coor &a,const coor &b)
{
	if(a.x!=b.x)return a.x<b.x;
	if(a.y!=b.y)return a.y<b.y;
	if(a.z!=b.z)return a.z<b.z;
	return false;
}

const GameInfo *INFO = Info();//这个函数干啥的？

//常量
const int map_size_max = 110;//地图大小
const int Move_Offset[4][2]={1,0,0,1,-1,0,0,-1};//四连通偏移量
const int Priority_Order[10]={7,4,8,9,5,6,0,1,2,3};//单位动作优先级
int Element_Max[10]={1,0,0,0,14,0,6,8,17,1};//单位上限设置
int LowerFuel[10]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};//单位从基地出发最少携带的燃料量
const int Min_Weather=-4;//迷雾小于此时强制保留一个侦察机

//常用变量
int MyTeam,EnemyTeam;//敌我队伍编号
const State *MyBase,*EnemyBase;
coor MyBase_Coor,EnemyBase_Coor;//敌我双方基地坐标
int MyBase_ID;//我方基地在INFO->elements中的编号
int MyBase_Index,EnemyBase_Index;//敌我双方基地结构体
int DistToEnemyPort,DistToEnemyFireRange;//我方基地至敌方基地/航母火力范围最远最短路
int My_Element_Now[10];//当前地图上与队列中的部队数
vector<coor>MyPort,MyAirport,EnemyPort,EnemyAirport;//港口，空港坐标

//常用函数
bool check(const coor &a){return a.x>=0&&a.y>=0&&a.x<INFO->x_max&&a.y<INFO->y_max;}//检查坐标a是否在地图内
int distance(const State *p,const State *q)//返回两个单位之间的距离
{
	if(p->type == BASE || q->type == BASE)
	{
		return 
			((p->pos.x <  q->pos.x)?q->pos.x-p->pos.x-1:((p->pos.x >  q->pos.x)?p->pos.x-q->pos.x-1:0)) +
			((p->pos.y <  q->pos.y)?q->pos.y-p->pos.y-1:((p->pos.y >  q->pos.y)?p->pos.y-q->pos.y-1:0));
	}
	else return abs(p->pos.x - q->pos.x) + abs(p->pos.y - q->pos.y);
}
int distance(coor pos1, coor pos2)//两点之间的距离
{
	return abs(pos1.x - pos2.x) + abs(pos1.y - pos2.y);
}
int DisToBase(coor pos1, coor pos2)//返回到基地的距离
{
	return 
		((pos1.x <  pos2.x)?pos2.x-pos1.x-1:((pos1.x >  pos2.x)?pos1.x-pos2.x-1:0)) +
		((pos1.y <  pos2.y)?pos2.y-pos1.y-1:((pos1.y >  pos2.y)?pos1.y-pos2.y-1:0));
}
int fire(int attack,int distance,int fire_range,bool isCarrier=false)//火力计算公式
{
	if(distance>fire_range)return 0;
	if(isCarrier)return f(attack);
	else return int((1 - float(distance - fire_range / 2) / (fire_range + 1)) * attack);
}
int fire(const State *p,const State *q)//计算p攻击q的真实伤害(考虑是否在射程内)
{
	const Property *P=&kProperty[p->type],*Q=&kProperty[q->type];
	int dist=distance(p,q);
	if(dist > P->fire_ranges[Q->level])return 0;
	int once =
		max(0,fire(P->attacks[0],dist,P->fire_ranges[Q->level],p->type==CARRIER) - f(Q->defences[0])) +
		max(0,fire(P->attacks[1],dist,P->fire_ranges[Q->level],p->type==CARRIER) - f(Q->defences[1]));
	return once;
}

struct Shortest_Path_Struct
{
	int x,y,z;//记录当前a,b是以那个点为起点的最短路
	int b[map_size_max][map_size_max];//最短路数据
	vector<coor>a;//距离内点集
	int operator[](const coor &a){return b[a.x][a.y];}
	Shortest_Path_Struct(int t=0x3f3f3f3f)
	{
		x=y=z=-1;
		for(int i=0;i<map_size_max;i++)
			for(int j=0;j<map_size_max;j++)
			b[i][j]=t;
	}
	void clear(int t=0x3f3f3f3f)
	{
		for(int i=0;i<map_size_max;i++)
			for(int j=0;j<map_size_max;j++)
			b[i][j]=t;
		while(a.size())a.pop_back();
	}
	void BFS(coor S,int Dist)//计算S点距离范围Dist内最短路存至b,并将坐标集合存至a
	{
		x=S.x;y=S.y;z=S.z;
		coor T;
		int tot=0,k;
		while(k=a.size())
		{
			b[a[k-1].x][a[k-1].y]=0x3f3f3f3f;
			a.pop_back();
		}
		b[S.x][S.y]=0;
		a.push_back(S);
		while(tot<(int)a.size())
		{
			S=a[tot++];
			if(b[S.x][S.y]<Dist)
			{
				for(k=0;k<4;k++)
				{
					T.x=S.x+Move_Offset[k][0];
					T.y=S.y+Move_Offset[k][1];
					T.z=S.z;
					if(check(T)&&(T.z==2||Map(T.x,T.y)==OCEAN)&&b[T.x][T.y]>INF)
					{
						a.push_back(T);
						b[T.x][T.y]=b[S.x][S.y]+1;
					}
				}
			}
		}
	}
	void Shortest_Path(coor S)
	{
		BFS(S,INF);
	}
	void Dist_To_Base(coor S,int D)//计算至基地周边<=D距离任意一点最短路
	{
		coor T;
		int tot=0,k;
		while(k=a.size())
		{
			b[a[k-1].x][a[k-1].y]=0x3f3f3f3f;
			a.pop_back();
		}
		for(int i=0;i<INFO->x_max;i++)
			for(int j=0;j<INFO->y_max;j++)
			{
				T.x=i;
				T.y=j;
				T.z=S.z;
				if(DisToBase(S,T)<=D)
				{
					if(S.z==2 || Map(T.x,T.y) == OCEAN)
					{
						b[T.x][T.y]=0;
						a.push_back(T);
					}
				}
			}
		while(tot<(int)a.size())
		{
			S=a[tot++];
			for(k=0;k<4;k++)
			{
				T.x=S.x+Move_Offset[k][0];
				T.y=S.y+Move_Offset[k][1];
				T.z=S.z;
				if(check(T)&&(T.z==2||Map(T.x,T.y)==OCEAN)&&b[T.x][T.y]>INF)
				{
					a.push_back(T);
					b[T.x][T.y]=b[S.x][S.y]+1;
				}
			}
		}
	}
}
EnemyPortDist/*距敌军港口最短路*/,
MyPortDist/*距我军港口最短路*/,
Base_Attack_Range/*距敌军基地8距离最短路*/,
MyAirportDist,/*距我军空港最短路*/
EnemyAirportDist,/*距敌军空港最短路*/
StagingDist/*海军集结地最短路*/,
SPS,MOVE_SPS/*临时*/,
Cargo_Appointment;/*每个位置有几个运输船预约*/;
struct Map_Struct
{
	int a[3][map_size_max][map_size_max];
	void clear(){memset(a,0,sizeof(a));}
	void Make_Power(int Team,AttackType T,const Property &D)//制作队伍Team火力类型T对兵种D的火力图存于a
	{
		clear();
		const State *p;
		const Property *P;
		coor S;
		int t,l,i;
		for(t=0;t<INFO->element_num;t++)
		{
			p=INFO->elements[t];
			P=&kProperty[p->type];
			if(p->type!=OILFIELD && p->type!=MINE)
			if(p->team == Team || (Team==EnemyTeam && p->team==2))
			{
				S=p->pos;
				S.z=2;
				for(l=0;l<3;l++)
				{
					SPS.BFS(S,P->fire_ranges[l] + 2*(int)(p->type==0)-1);
					for(i=0;i<(int)SPS.a.size();i++)
					{
						int dist= (p->type==0)?DisToBase(SPS.a[i],p->pos):distance(p->pos,SPS.a[i]);
						if(dist <= P->fire_ranges[l])
							a[l][SPS.a[i].x][SPS.a[i].y]+=max(0,fire(P->attacks[T],dist,P->fire_ranges[l])-f(D.defences[T]));
					}
				}
			}
		}
	}
}EnemyFire[10]/*每个兵种的火力图*/,EnemyTorpedo[10]/*每个兵种的鱼雷图*/,Hinder,Hinder_Now;
struct Move_Manager
{
	
	void Move_Safety(const State *p,const Shortest_Path_Struct &B,bool Hinder_ifConsider = true)//以B为最短路基础沿安全路径移动p（是否考虑队友卡位）
	{
		const Property *P=&kProperty[p->type];
		int i,j,k;
		Position Destination;
		MOVE_SPS.BFS(p->pos,P->speed);
		for(i=0,j=k=INF;i<(int)MOVE_SPS.a.size();i++)
		{
			coor T=MOVE_SPS.a[i];
			if(Hinder_ifConsider==false||!Hinder.a[T.z][T.x][T.y])
			{
				int dist = B.b[T.x][T.y]*2 + Hinder_Now.a[T.z][T.x][T.y] - (int)(p->pos.x==T.x&&p->pos.y==T.y);
				int hurt = EnemyFire[p->type].a[T.z][T.x][T.y] + EnemyTorpedo[p->type].a[T.z][T.x][T.y];
				if(hurt < j || (hurt == j && dist < k))
				{
					j=hurt;
					k=dist;
					Destination.x=T.x;
					Destination.y=T.y;
					Destination.z=T.z;
				}
			}
		}
		if(k<INF)
		{
			Hinder.a[Destination.z][Destination.x][Destination.y]++;
			ChangeDest(p->index,Destination);
		}
	}
	void Move_Straight(const State *p,const Shortest_Path_Struct &B,bool Hinder_ifConsider = true)//以B为最短路基础直线移动p（是否考虑队友卡位）
	{
		int i,k;
		const Property *P=&kProperty[p->type];
		Position Destination;
		MOVE_SPS.BFS(p->pos,P->speed);
		for(i=0,k=INF;i<(int)MOVE_SPS.a.size();i++)
		{
			coor T=MOVE_SPS.a[i];
			if(Hinder_ifConsider==false||!Hinder.a[T.z][T.x][T.y])
			{
				int dist = B.b[T.x][T.y]*2 + Hinder_Now.a[T.z][T.x][T.y] - (int)(p->pos.x==T.x&&p->pos.y==T.y);//距离加权估价 实际距离*2+当前存在单位*1-原地停留
				if(dist < k)
				{
					k=dist;
					Destination.x=T.x;
					Destination.y=T.y;
					Destination.z=T.z;
				}
			}
		}
		if(k<INF)
		{
			Hinder.a[Destination.z][Destination.x][Destination.y]++;
			ChangeDest(p->index,Destination);
		}
	}
}Move;
struct Resource_Manager//负责记录维护地图上资源点的信息
{
	map<int,int>a/*每个index的剩余资源量*/;
	map<coor,int>id/*每个坐标的的分配ID*/;
	vector<Shortest_Path_Struct>SPS;//SPS[i]记录到坐标点ID=i的最短路图
	Shortest_Path_Struct* operator[](const coor &a){return &SPS[id[a]];}//返回坐标所在最短路图的指针
	void init()//游戏开始时初始化地图
	{
		const State *p;
		//初始化资源点
		for(int i=0;i<INFO->element_num;i++)
		{
			const State *p=INFO->elements[i];
			if(p->type == 2)a[p->index] = 500;
			if(p->type == 3)a[p->index] = 1000;
		}
		//初始化到矿点周边的最短路
		int tot=0;
		coor S;
		for(int i=0;i<INFO->element_num;i++)
		{
			p=INFO->elements[i];
			if(p->type == 2 || p->type == 3)
			{
				for(int k=0;k<4;k++)
				{
					S.x=p->pos.x + Move_Offset[k][0];
					S.y=p->pos.y + Move_Offset[k][1];
					S.z=1;
					if(id.find(S)==id.end())
					{
						SPS.push_back(Shortest_Path_Struct());
						SPS[tot].BFS(S,INF);
						id[S]=tot++;
					}
				}
			}
		}
		//初始化到港口的最短路
		for(int i=0;i<(int)MyPort.size();i++)
		{
			S=MyPort[i];
			if(id.find(S)==id.end())
			{
				SPS.push_back(Shortest_Path_Struct());
				SPS[tot].BFS(S,INF);
				id[S]=tot++;
			}
		}
	}
	void update()//更新视野内资源点的信息
	{
		for(int i=0;i<INFO->element_num;i++)
		{
			const State *p=INFO->elements[i];
			if(p->visible == true)
			{
				if(p->type == 2)a[p->index] = p->metal;
				if(p->type == 3)a[p->index] = p->fuel;
			}
		}
	}
}Resource;//地图资源管理器
struct Supply_Manager
{
	struct Demand{const State *p;int F,A,M;};
	vector<Demand>a,save;
	void clear(){while(a.size())a.pop_back();}
	void del(int t)//删除a中编号t的元素
	{
		if(t<0||t>=(int)a.size())return;
		for(;t<(int)a.size()-1;t++)swap(a[t],a[t+1]);
		a.pop_back();
	}
	void add(const State *p,int F,int A=0,int M=0)//添加一条需求
	{
		Demand r;
		r.p=p;r.F=F;r.A=A;r.M=M;
		a.push_back(r);
	}
	Demand Supply_Dying(const State *p)//满足p周围最严重单位的补给需求,返回是否具体方案(无方案是返回{-1,-1,-1})
	{
		Demand *t;
		int i,j,k=-1,Order;
		int Fuel_upper_bound=f(p->fuel);//能接受的对外燃料输出上界
		int Ammo_upper_bound=f(p->ammo);//能接受的对外弹药输出上界
		for(i=0;i<(int)a.size();i++)
		{
			t=&a[i];
			int dist=(p->type==0)?DisToBase(t->p->pos,p->pos):distance(t->p->pos,p->pos);
			if(dist<=1)
			{
				if(Ammo_upper_bound>=t->A && Fuel_upper_bound>=t->F)//若自己有能力实施补给则补给
				{
					j=5*t->A+t->F;//优先级
					if(j>k)
					{
						k=j;
						Order=i;
					}
				}
			}
		}
		if(k>=0)
		{
			t=&a[Order];
			del(Order);
			return *t;
		}
		Demand re;re.F=re.A=re.M=-1;
		return re;
	}
}Demand;//补给/维修需求列表
struct Collect_Manager//采集AI，负责统筹矿车的移动路径，通过solve()成员函数调用
{
	map<int,coor>Move_Target;
	map<int,int>S_index,SF,SM,f_r;//index向S_index补给的fuel和metal量,若燃料不足则请求补给，补给量为f_r

	//矿车类；F,A,M为期望携带燃料，弹药，金属数量；SF,SM为向S_index补给的量，target为当前决策的目的地,f_r为补给需求量
	struct cargo{const State *p;int F,A,M,S_index,SF,SM,f_r;coor target;}Base/*基地*/;
	struct resource{const State *p;int rest;vector<coor>a;};//资源点类 rest为预计剩余资源 a为该资源点临海坐标
	vector<cargo>a;//矿车
	vector<resource>b;//资源点
	void clear()
	{
		while(a.size())a.pop_back();
		while(b.size())b.pop_back();
		Move_Target.clear();
		SF.clear();
		SM.clear();
		S_index.clear();
		f_r.clear();
	}
	void init()//构造a和b
	{
		clear();
		coor T;T.z=1;
		for(int i=0;i<INFO->element_num;i++)
		{
			const State *p=INFO->elements[i];
			if((p->type==MINE || p->type==OILFIELD) && Resource.a[p->index]>0)//未枯竭的矿场或油田
			{
				int k=b.size();
				b.push_back(resource());
				b[k].p=p;
				b[k].rest=Resource.a[p->index];
				for(int j=0;j<4;j++)
				{
					T.x=p->pos.x+Move_Offset[j][0];
					T.y=p->pos.y+Move_Offset[j][1];
					if(check(T) && Map(T.x,T.y)==OCEAN)
						b[k].a.push_back(T);
				}
			}
			else if(p->type==CARGO && p->team == MyTeam)
			{
				int k=a.size();
				a.push_back(cargo());
				a[k].p=p;
				a[k].F=p->fuel;
				a[k].A=p->ammo;
				a[k].M=p->metal;
			}
		}
	}
	void update()//依据INFO更新a,b数组的初值,并还原Cargo_Appointment
	{
		Base.F=MyBase->fuel;
		Base.A=INF;
		Base.M=MyBase->metal;
		for(int i=0;i<(int)a.size();i++)
		{
			a[i].F=a[i].p->fuel;
			a[i].A=a[i].p->ammo;
			a[i].M=a[i].p->metal;
			a[i].SF=a[i].SM=0;
			a[i].S_index=-1;
			a[i].f_r=0;
		}
		for(int i=0;i<(int)b.size();i++)
		{
			b[i].rest=Resource.a[b[i].p->index];
			for(int j=0;j<(int)b[i].a.size();j++)
			Cargo_Appointment.b[b[i].a[j].x][b[i].a[j].y]=0;
		}
		for(int i=0;i<(int)MyPort.size();i++)
			Cargo_Appointment.b[MyPort[i].x][MyPort[i].y]=0;
	}
	int calc(int Pt1)//根据a的顺序贪心寻找最优解(此时a中的单位全部需要采集) Pt1为人工矿点个数
	{
		Demand.save=Demand.a;
		//缺什么去哪种矿区,两个都缺的话枚举二元组
		const int Speed = kProperty[CARGO].speed;
		cargo *A;
		int i,k,t;
		int n=a.size();
		int m=b.size();
		int re=0;//每有一个矿车正在基地排队扣分10000，加所有矿车的累计运输的回合数
		for(t=0;t<n;t++)
		{
			A=&a[t];
			//作为人工矿点
			if(t<Pt1)
			{
				bool PF=false,PM=false;//周围存在的矿点有哪些
				for(i=0;i<(int)b.size();i++)
					if(distance(A->p,b[i].p)<=1)
					{
						if(b[i].p->type==OILFIELD && b[i].rest>30)PF=true;
						else if(b[i].rest>0)PM=true;
					}
				int MAX=-1,SF=0,SM=0,order,id;//对周围资源最稀缺的矿车尽力补给
				for(i=0;i<n;i++)
					if(distance(a[i].p->pos,A->p->pos)==1)
					{
						k=(kProperty[CARGO].fuel_max - a[i].F)*PF + (kProperty[CARGO].metal_max - a[i].M)*PM*5;
						if(k>MAX)
						{
							MAX=k;
							SF=(kProperty[CARGO].fuel_max - a[i].F)*PF;
							SM=(kProperty[CARGO].metal_max - a[i].M)*PM;
							order=a[i].p->index;
							id=i;
						}
					}
				if(MAX>0)
				{
					A->target=A->p->pos;
					A->S_index=order;
					A->SM=SM;
					A->SF=SF;
					a[id].F+=A->SF;
					a[id].M+=A->SM;
					continue;
				}
			}

			//排队向基地补给

			//优先进行补给
			/*Supply_Manager::Demand t=Demand.Supply_Dying(A->p);
			if(t.F!=-1)
			{
				//优先进行补给
				re+=INF;
				A->target=A->p->pos;
				A->f_r=0;
				A->S_index=t.p->index;
				A->SF=t.F;
				A->SM=t.M;
				continue;
			}*/

			//移动采集AI
			bool B1=false;//B1为true时是采集，为false时是回基地
			int MIN=0x7fffffff;
			coor Target,Target2=coor(-1,-1,-1);
			resource *T1=NULL,*T2=NULL;
			int fuel_r=0;//燃料需求
			bool free_fuel=false;//必须采集燃料
			if(distance(A->p,MyBase)==1)//必须采集燃料
				free_fuel=true;
			if((A->F<=LowerFuel[CARGO]*2 || free_fuel) && A->M==0)//既需要采燃料又需要采矿
			{
				for(int o1=0;o1<m;o1++)
					if(b[o1].p->type==OILFIELD && b[o1].rest>=50)
						for(int o2=0;o2<(int)b[o1].a.size();o2++)
						{
							coor FC=b[o1].a[o2];//采油目的地
							for(int m1=0;m1<m;m1++)
								if(b[m1].p->type==MINE && b[m1].rest>0)
									for(int m2=0;m2<(int)b[m1].a.size();m2++)
									{
										coor MC=b[m1].a[m2];
										{
											bool type=0;//为0是先油田后矿场，为1相反
											
											int dist1= 2*(((*Resource[FC])[A->p->pos] + Speed - 1)/Speed + ( (*Resource[FC])[MC] + Speed - 1)/Speed + (MyPortDist[MC] + Speed - 1)/Speed) + Cargo_Appointment.b[FC.x][FC.y] + Cargo_Appointment.b[MC.x][MC.y]/2; //先油田
											int dist2= 2*(((*Resource[MC])[A->p->pos] + Speed - 1)/Speed + ( (*Resource[FC])[MC] + Speed - 1)/Speed + (MyPortDist[FC] + Speed - 1)/Speed) + Cargo_Appointment.b[FC.x][FC.y]/2 + Cargo_Appointment.b[MC.x][MC.y]; //先矿场
											if((*Resource[FC])[A->p->pos] > A->p->fuel && free_fuel==false)dist1=0x7fffffff;
											if((*Resource[MC])[A->p->pos] + (*Resource[FC])[MC] > A->p->fuel && free_fuel==false)dist2=0x7fffffff;
											int dist;
											if(dist1<=dist2)dist=dist1,type=0;
											else dist=dist2,type=1;
											if(dist<MIN)
											{
												MIN=dist;
												if(type==0)Target=FC,Target2=MC,T1=&b[o1],T2=&b[m1],fuel_r=(*Resource[FC])[A->p->pos];
												else Target=MC,Target2=FC,T1=&b[m1],T2=&b[o1],fuel_r=(*Resource[MC])[A->p->pos] + (*Resource[FC])[MC];
												B1=true;
											}
										}
									}
						}
				if(MIN==0x7fffffff)re-=INF;//估价失败
			}
			if((A->F<=LowerFuel[CARGO]*2 || free_fuel ) && MIN==0x7fffffff)//只需采油
			{
				for(int o1=0;o1<m;o1++)
					if(b[o1].p->type==OILFIELD && b[o1].rest>=50)
						for(int o2=0;o2<(int)b[o1].a.size();o2++)
						{
							coor FC=b[o1].a[o2];//采油目的地
							int dist= 2*(((*Resource[FC])[A->p->pos] + Speed - 1)/Speed + (MyPortDist[FC] + Speed - 1)/Speed) + Cargo_Appointment.b[FC.x][FC.y];
							if(((*Resource[FC])[A->p->pos]<=A->F || free_fuel) &&dist<MIN)
							{
								MIN=dist;
								Target=FC;
								T1=&b[o1];
								fuel_r=(*Resource[FC])[A->p->pos];
								B1=true;
							}
						}
				if(MIN==0x7fffffff)re-=INF;//估价失败
			}
			if(A->M==0 && MIN==0x7fffffff)//只需采矿
			{
				for(int o1=0;o1<m;o1++)
					if(b[o1].p->type==MINE && b[o1].rest>0)
						for(int o2=0;o2<(int)b[o1].a.size();o2++)
						{
							coor MC=b[o1].a[o2];//采矿目的地
							int dist= 2*(((*Resource[MC])[A->p->pos] + Speed - 1)/Speed + (MyPortDist[MC] + Speed - 1)/Speed) + Cargo_Appointment.b[MC.x][MC.y]; 
							if(dist<MIN)
							{
								MIN=dist;
								Target=MC;
								T1=&b[o1];
								B1=true;
							}
						}
				if(MIN==0x7fffffff)re-=INF;//估价失败
			}
			if(MIN==0x7fffffff)//资源充足，返回基地补给
			{
				for(i=0;i<(int)MyPort.size();i++)
				{
					int dist= (((*Resource[MyPort[i]])[A->p->pos] + Speed - 1)/Speed)*4 + Cargo_Appointment.b[MyPort[i].x][MyPort[i].y] - INF*(distance(A->p,MyBase)==1);
					if(dist<MIN)
					{
						MIN=dist;
						Target=MyPort[i];
					}
				}
				MIN=0;
			}

			//修改状态
			if(free_fuel)//停留在基地附近考虑供给问题
			{
				//尽力补给金属
				A->S_index=MyBase_Index;
				A->SM=min(200-Base.M,A->M);
				A->SF=max(0,A->F - fuel_r - 5);
			}
			if(free_fuel)//燃料未向基地补给完成(由于基地容量上限未补给量过多)，强制保持不动
			{
				if(A->SF + Base.F>1000)
				{
					A->SF=1000 - Base.F;
					Target=A->p->pos;
					Target2=coor(-1,-1,-1);
					B1=false;
					re-=100;//被容量限制的加成
				}
				else if(A->F<fuel_r+5)
				{
					A->f_r=fuel_r+5 - A->F;
					Target=A->p->pos;
					B1=false;
					re-=1000;
				}
				Base.F+=A->SF;
				Base.M+=A->SM;
			}
			re+=MIN;
			A->M-=A->SM;
			A->F-=A->SF;
			A->target=Target;
			Cargo_Appointment.b[Target.x][Target.y]+=2;//直接目的地加两个站位
			if(Target2.z>=0)Cargo_Appointment.b[Target2.x][Target2.y]++;//间接目的地加一个站位
			if(B1)//前往采集的状态
			{
				if(T1->p->type==MINE)T1->rest -= min(T1->rest,50 - A->M);//矿点资源扣除
				if(T1->p->type==OILFIELD)T1->rest -= min(T1->rest,200 - (A->F - fuel_r));//油田资源扣除
				if(T2!=NULL)
				{
					if(T2->p->type==MINE)T2->rest -= min(T2->rest,50 - A->M);//矿点资源扣除
					if(T2->p->type==OILFIELD)T2->rest -= min(T2->rest,200 - (A->F - fuel_r));//油田资源扣除
				}
			}
			if(A->SF==0 && A->SM==0)A->S_index=-1;
		}
		Demand.a=Demand.save;
		return re;
	}
	void solve()//解决我方所有运输船的操作
	{
		//计算矿车的整体最优决策
		//库存较多时在基地外围排队等候补给基地
		//停留在资源点周边的矿车充当人工资源点
		//库存不足是依照随机贪心法寻找最优路径(金属，燃料，金属+燃料)

		init();
		//随机贪心法
		int i,j,k=0,n,m,cnt,MAX=-999999999;
		n=a.size();m=b.size();
		if(n*m==0)return;//没有可行方案
		cnt=100000/(n*m*m+n*n+10)+1;
		//cnt=1;//////DEBUG用
		while(cnt--)
		{
			//随机爬山法
			for(i=1;i<n;i++)swap(a[rand()%(i+1)],a[i]);
			//将停留在资源周边当做人工矿点优先处理
			for(i=k=0;i<n;i++)
				for(j=0;j<m;j++)
					if(distance(a[i].p,b[j].p)<=1)
					{
						swap(a[i],a[k++]);
						break;
					}

			update();//还原信息
			if((j=calc(k))>MAX)
			{
				MAX=j;
				S_index.clear();
				SF.clear();
				SM.clear();
				f_r.clear();
				for(i=0;i<n;i++)
				{
					Move_Target[a[i].p->index]=a[i].target;
					if(a[i].S_index>=0)
					{
						S_index[a[i].p->index]=a[i].S_index;
						SF[a[i].p->index]=a[i].SF;
						SM[a[i].p->index]=a[i].SM;
					}
					if(a[i].f_r>0)f_r[a[i].p->index]=a[i].f_r;
				}
			}
		}
		Base.F=MyBase->fuel;
		Base.M=MyBase->metal;
		for(i=0;i<n;i++)//执行决策
		{
			//将单位a[i].p->index 移动到 Move_Target[a[i].p->index]
			int Index=a[i].p->index;
			Move.Move_Straight(a[i].p,*Resource[Move_Target[a[i].p->index]],false);
			if(S_index.find(Index)!=S_index.end())
			{
				//补给
				Supply(Index,S_index[Index],SF[Index],0,SM[Index]);
				if(S_index[Index]==MyBase->index)
				{
					Base.F+=SF[Index];
					Base.M+=SM[Index];
				}
			}
			else Demand.Supply_Dying(a[i].p);
			if(f_r[Index]>0)//发送补给请求
			{
				Demand.add(a[i].p,f_r[Index]);
			}
		}
	}
}Collect;
struct Attack_Manager//火力AI，负责决策最优攻击目标(将决定攻击的单位push进来，然后由solve()统筹决策)
{
	struct status{const State *p;int HP,target;};//单位类，HP为预测剩余血量，target为攻击目标的index
	vector<status>a,b;//a我方待攻击单位，b敌方单位
	map<int,int>Attack_Target;//我军Index的最终攻击目标Index
	void init()
	{
		clear();
		//将所有敌军信息统计至b
		for(int i=0;i<INFO->element_num;i++)
			if(INFO->elements[i]->team!=MyTeam && INFO->elements[i]->type!=OILFIELD && INFO->elements[i]->type!=MINE)
				for(int j=0;j<INFO->element_num;j++)
					if(INFO->elements[j]->team==MyTeam && fire(INFO->elements[j],INFO->elements[i])>0)
					{
						push(INFO->elements[i],true);
						break;
					}
	}
	void clear()
	{
		while(a.size())a.pop_back();
		while(b.size())b.pop_back();
		Attack_Target.clear();
	}
	void push(const State *p,bool T=false)//将单位加入候选列表中(默认为a,T=true)
	{
		if(!T)
		{
			int k=a.size();
			a.push_back(status());
			a[k].p=p;
			a[k].HP=p->health;
			a[k].target=-1;
		}
		else
		{
			int k=b.size();
			b.push_back(status());
			b[k].p=p;
			b[k].HP=p->health;
			b[k].target=-1;
		}
	}
	bool check(const State *p)//决策p是否需要进行攻击,若需要push进待攻击列表
	{
		const Property *P=&kProperty[p->type];
		if(f(p->ammo) < P->ammo_once)return false;
		//若射程内有敌军则须攻击
		for(int i=0;i<(int)b.size();i++)
		{
			if(fire(p,b[i].p)>0)
			{
				push(p);
				return true;
			}
		}
		return false;
	}
	bool Attack_Dying(const State *p)//寻找射程内最濒死的单位并向logic发送攻击指令,返回是否成功
	{
		const State *q,*re=NULL;
		const Property *P=&kProperty[p->type],*Q;
		if(f(p->ammo) < P->ammo_once)return false;
		int MIN=INF;
		for(int t=0;t<INFO->element_num;t++)
		{
			q=INFO->elements[t];
			Q=&kProperty[q->type];
			int dist= (p->type==0 || q->type==0)?DisToBase(p->pos,q->pos):distance(p->pos,q->pos);
			if(q->team == 1 - p->team && dist<=P->fire_ranges[Q->level])
			{
				int once =
					max(0,fire(P->attacks[0],dist,P->fire_ranges[Q->level],p->type==CARRIER) - f(Q->defences[0])) +
					max(0,fire(P->attacks[1],dist,P->fire_ranges[Q->level],p->type==CARRIER) - f(Q->defences[1]));
				if(once > 0)
				{
					int remain = (q->health + once -1) / once;
					if(remain < MIN)
					{
						MIN=remain;
						re=q;
					}
				}
			}
		}
		if(re!=NULL)
		{
			AttackUnit(p->index,re->index);
			return true;
		}
		return false;
	}
	int calc()//依据a数组现在的顺序贪心决策攻击目标，并返回攻击方案的评估值
	{
		//优先选择攻击范围内最濒死的单位攻击，若攻击范围内所有单位都将死亡，优先补刀最不死的一个
		int i,j,n,m;
		int re=0;//估价
		n=a.size();m=b.size();
		for(i=0;i<m;i++)b[i].HP=b[i].p->health;

		for(i=0;i<n;i++)
		{
			int MIN=2147483647,order=-1,MIN2;
			for(j=0;j<m;j++)
			{
				int once=fire(a[i].p,b[j].p);
				if(once>0)
				{
					int rest=(b[j].HP+once-1)/once;
					if(rest<=0)rest+=INF;//该单位将会死亡时优先攻击最不死的一个
					if(a[i].p->type==FIGHTER&&(b[j].p->type==FIGHTER||b[j].p->type==BASE))rest-=100;//绝对优先攻击飞机和基地(空军一波流加成)
					if(rest<MIN||(rest==MIN&&once>MIN2))
					{
						MIN=rest;
						MIN2=once;
						order=j;
					}
				}
			}
			if(order>=0)
			{
				if(b[order].HP<=0)re++;//若对方已死，节约了弹药估价+1
				b[order].HP-=fire(a[i].p,b[order].p);
				a[i].target=b[order].p->index;
			}else a[i].target=-1;
		}

		//评估值：杀死一个单位奖励20分，存活一个单位扣除血量数值的分
		for(i=0;i<m;i++)
			if(b[i].HP<=0)re+=20;
			else re-=b[i].HP;
		return re;
	}
	void solve()//对列表中的单位执行决策并向logic发送动作命令
	{
		//采用随机贪心法，对数组a进行若干次随机排布，计算贪心解取最优
		int i,n,m,cnt,MAX=-999999999;
		n=a.size();m=b.size();
		if(n*m==0)return;//没有可行方案
		cnt=100000/(n*m*2)+1;
		while(cnt--)
		{
			//随机爬山法
			for(i=1;i<n;i++)swap(a[rand()%(i+1)],a[i]);
			if(calc()>MAX)
				for(i=0;i<n;i++)Attack_Target[a[i].p->index]=a[i].target;
		}
		for(i=0;i<n;i++)
		{
			AttackUnit(a[i].p->index,Attack_Target[a[i].p->index]);
		}
	}
}Attack;
struct AI
{
	coor Staging;//部队攻击前集结地
	map<int,int>Tactics;//各单位当前战术
	/*战术：
		战斗机:{0//防御基地，1/进攻}
	*/
	void init()
	{
		//更新视野内资源点数据
		Resource.update();
		//清空
		Hinder.clear();
		Hinder_Now.clear();
		Cargo_Appointment.clear(0);
		Demand.clear();
		Attack.init();
		Collect.init();
		//预处理全图的火力值
		for(int i=0;i<10;i++)
		{		
			EnemyFire[i].Make_Power(EnemyTeam,FIRE,kProperty[i]);
			EnemyTorpedo[i].Make_Power(EnemyTeam,TORPEDO,kProperty[i]);
		}
		//计算我方部队数量 和 MyBase_ID MyBase EnemyBase
		memset(My_Element_Now,0,sizeof(My_Element_Now));
		for(int i=0;i<INFO->element_num;i++)
			if(INFO->elements[i]->team==MyTeam)
			{
				if(INFO->elements[i]->type==0)MyBase_ID=i,MyBase=INFO->elements[i];
				My_Element_Now[INFO->elements[i]->type]++;
			}
			else if(INFO->elements[i]->team==EnemyTeam)
			{
				if(INFO->elements[i]->type==0)EnemyBase=INFO->elements[i];
			}
		for(int i=0;i<INFO->production_num;i++)
			My_Element_Now[INFO->production_list[i].unit_type]++;
		//计算现在有单位的格子存于Hinder_Now
		for(int i=0;i<INFO->element_num;i++)
		{
			const State *p=INFO->elements[i];
			Hinder_Now.a[p->pos.z][p->pos.x][p->pos.y]=1;
		}

		//对EnemyAirportDist进行加权计算
		EnemyAirportDist.Dist_To_Base(coor(EnemyBase_Coor.x,EnemyBase_Coor.y,2),0);
		for(int i=0;i<INFO->x_max;i++)
			for(int j=0;j<INFO->y_max;j++)
				EnemyAirportDist.b[i][j]*=2;
		for(int i=0;i<INFO->element_num;i++)
			if(INFO->elements[i]->type==FIGHTER)
			{
				if(distance(INFO->elements[i],EnemyBase)==0)
					EnemyAirportDist.b[INFO->elements[i]->pos.x][INFO->elements[i]->pos.y]+=INF;
			}

		//剔除Tactics
		for(map<int,int>::iterator iter=Tactics.begin();iter!=Tactics.end();)
		{
			int i=0;
			for(i=0;i<INFO->element_num;i++)
				if(INFO->elements[i]->index == iter->first)
					break;
			if(i<INFO->element_num)iter++;
			else Tactics.erase(iter++);
		}
	}
	void DefenceStaging()//计算新的防御集结地更新Staging和StagingDist
	{
		const State *p;
		coor order;
		int MIN=0x7fffffff,i,j;
		for(i=0;i<INFO->element_num;i++)
		{
			p=INFO->elements[i];
			if(p->team==EnemyTeam && (j=distance(p,MyBase))<=8)
			{
				if(j<MIN)
				{
					MIN=j;
					order=p->pos;
				}
			}
		}
		if(MIN<0x7fffffff)
		{
			Staging=order;
			StagingDist.BFS(Staging,INF);
		}
	}
	void AI_Run()
	{
		//初始化
		init();
		
		//采集决策
		Collect.solve();
		
		//建造决策
		Produce_AI();
		
		//重置防御集结点
		DefenceStaging();

		//防御集结地
		
		//针对每个单位做决策
		const State *p;
		const Property *P;
		for(int PO=0;PO<10;PO++)
		{
			int Type=Priority_Order[PO];
			for(int t=0;t<INFO->element_num;t++)
			{
				p=INFO->elements[t];
				P=&kProperty[p->type];
				if(p->type==Type&&p->team==INFO->team_num)
				{
					if(Type==BASE)Base_AI(p);
					if(Type==SUBMARINE)Submarine_AI(p);
					if(Type==CARRIER)Carrier_AI(p);
					//if(Type==CARGO)Cargo_AI(p);
					if(Type==FIGHTER)Fighter_AI(p);
					if(Type==FORT)Fort_AI(p);
					if(Type==SCOUT)Scout_AI(p);
				}
			}
		}
		Attack.solve();//发送攻击指令
	}
	void AI_Start()//对战前的准备工作(计算一些变量/不变结构体的值)
	{
		const State *p;
		//计算敌我队伍编号，敌我基地坐标
		MyTeam=INFO->team_num;
		EnemyTeam=1-MyTeam;
		for(int i=0;i<INFO->element_num;i++)
		{
			p=INFO->elements[i];
			if(p->type == 0)
			{
				if(p->team == INFO->team_num)MyBase_Coor=p->pos,MyBase_Index=p->index,MyBase=p;
				else EnemyBase_Coor=p->pos,EnemyBase_Index=p->index,EnemyBase=p;
			}
		}
		//计算EnemyPortDist,Base_Attack_Range
		EnemyPortDist.Dist_To_Base(EnemyBase_Coor,1);
		MyPortDist.Dist_To_Base(MyBase_Coor,1);
		Base_Attack_Range.Dist_To_Base(EnemyBase_Coor,8);
		EnemyAirportDist.Dist_To_Base(coor(EnemyBase_Coor.x,EnemyBase_Coor.y,2),0);
		MyAirportDist.Dist_To_Base(coor(MyBase_Coor.x,MyBase_Coor.y,2),0);
		//计算DistToEnemyPort,DistToEnemyFireRange
		for(int i=0;i<INFO->x_max;i++)
			for(int j=0;j<INFO->y_max;j++)
				if(DisToBase(coor(i,j),MyBase_Coor)<=1&&Map(i,j)==OCEAN)
				{
					DistToEnemyPort=max(DistToEnemyPort,EnemyPortDist.b[i][j]);
					DistToEnemyFireRange=max(DistToEnemyFireRange,Base_Attack_Range.b[i][j]);
				}

		//计算港口/空港坐标
		coor S;
		for(S.x=0;S.x<INFO->x_max;S.x++)
			for(S.y=0;S.y<INFO->y_max;S.y++)
			{
				if(DisToBase(S,MyBase->pos)==1 && Map(S.x,S.y)==OCEAN)S.z=1,MyPort.push_back(S);
				if(DisToBase(S,EnemyBase->pos)==1 && Map(S.x,S.y)==OCEAN)S.z=1,EnemyPort.push_back(S);
				if(DisToBase(S,MyBase->pos)==0)S.z=2,MyAirport.push_back(S);
				if(DisToBase(S,EnemyBase->pos)==0)S.z=2,EnemyAirport.push_back(S);
			}

		//还原资源点数据
		Resource.init();

		//计算单位出发最低燃料储量
		LowerFuel[4]=min(120,DistToEnemyPort+5);
		LowerFuel[6]=min(200,DistToEnemyFireRange+20);
		LowerFuel[7]=(INFO->x_max+INFO->y_max)*5/12;
		LowerFuel[SCOUT]=kProperty[SCOUT].fuel_max-1;

		//防御集结地初始化
		{
			int j=0x7fffffff,k;
			for(int i=0;i<(int)MyPort.size();i++)
			{
				if(EnemyPortDist.b[MyPort[i].x][MyPort[i].y]<j)
				{
					j=EnemyPortDist.b[MyPort[i].x][MyPort[i].y];
					k=i;
				}
			}
			Staging=MyPort[k];
			StagingDist.BFS(Staging,INF);
		}
	}
	void Produce_AI()
	{
		int i;
		int Metal_Rest=INFO->elements[MyBase_ID]->metal;//当前基地剩余金属
		int Fuel_Rest=Collect.Base.F;//当前基地预计剩余金属
		//封锁海港防止意外驶入
		for(i=0;i<(int)MyPort.size();i++)
		{
			coor S=MyPort[i];
			Hinder.a[S.z][S.x][S.y]++;
			S.z=0;
			Hinder.a[S.z][S.x][S.y]++;
		}

		//至少保留1个侦察机
		if(INFO->weather<Min_Weather && My_Element_Now[SCOUT]<Element_Max[SCOUT] && Metal_Rest>=kProperty[SCOUT].cost)
		{
			Produce(SCOUT);
			My_Element_Now[SCOUT]++;
			Metal_Rest-=kProperty[SCOUT].cost;
		}
		
		if(MyTactics==4 || MyTactics==5 || (DistToEnemyPort>120 && distance(MyBase,EnemyBase)>80))
		{
			//纯航母一波流
			Element_Max[CARGO]=8-(INFO->weather<Min_Weather);
			
			//优先将运输船数量造满
			for(i=0;My_Element_Now[CARGO]<Element_Max[CARGO] && Metal_Rest>=kProperty[CARGO].cost;i++)
			{
				Produce(CARGO);
				My_Element_Now[CARGO]++;
				Metal_Rest-=kProperty[CARGO].cost;
			}

			//策略转换，防御→进攻
			{
				int cnt=0;
				for(int i=0;i<INFO->element_num;i++)
					if(INFO->elements[i]->team==MyTeam)
					{
						if(INFO->elements[i]->type==SUBMARINE && Tactics[INFO->elements[i]->index]==0)cnt+=kProperty[SUBMARINE].population;
						else if(INFO->elements[i]->type==CARRIER && Tactics[INFO->elements[i]->index]==0)cnt+=kProperty[CARRIER].population;
					}
				if(cnt>=60-Element_Max[CARGO]-3)//构成50人口进攻阵型
				for(int i=0;i<INFO->element_num;i++)
					if(INFO->elements[i]->team==MyTeam && (INFO->elements[i]->type==SUBMARINE || INFO->elements[i]->type==CARRIER))
						Tactics[INFO->elements[i]->index]=1;
			}

			ElementType Type;
			if(INFO->round>2)
			while(INFO->population>55 || My_Element_Now[CARRIER]*4+My_Element_Now[CARGO]<=57)
			{
				Type=CARRIER;
				if(Metal_Rest>=kProperty[Type].cost)
				{
					Produce(Type);
					Metal_Rest-=kProperty[Type].cost;
					My_Element_Now[Type]++;
				}
				else break;
			}
			//在航母生产出来时封锁港口
			{
				int k=0;
				for(int i=0;i<INFO->element_num;i++)
					if(INFO->elements[i]->team==MyTeam && INFO->elements[i]->type==CARRIER)k++;
				int cnt=0;
				for(int i=0;i<INFO->production_num;i++)
				if(INFO->production_list[i].unit_type==CARRIER)
					if(INFO->production_list[i].round_left<2)cnt++;
				cnt=min(cnt,Fuel_Rest/kProperty[CARRIER].fuel_max);
				cnt=min(cnt,Element_Max[CARRIER]-k);
				for(int i=0;cnt>0&&i<(int)MyPort.size();i++)
				{
					for(k=0;k<INFO->element_num;k++)
						if(INFO->elements[k]->team == MyTeam && distance(INFO->elements[k]->pos,MyPort[i])==0)
							break;
					if(k<INFO->element_num || !Hinder_Now.a[MyPort[i].z][MyPort[i].x][MyPort[i].y])
					{
						Hinder.a[MyPort[i].z][MyPort[i].x][MyPort[i].y]++;
						cnt--;
					}
				}
			}
		}
		else
		//1航母:3潜艇 比例战斗群建造 人口满时在建造列表里堆航母
		if(MyTactics==2 || MyTactics==3 || distance(MyBase,EnemyBase)>80)
		{
			Element_Max[CARGO]=9;
			
			//优先将运输船数量造满
			for(i=0;My_Element_Now[CARGO]<Element_Max[CARGO] && Metal_Rest>=kProperty[CARGO].cost;i++)
			{
				Produce(CARGO);
				My_Element_Now[CARGO]++;
				Metal_Rest-=kProperty[CARGO].cost;
			}

			//策略转换，防御→进攻
			{
				int cnt=0;
				for(int i=0;i<INFO->element_num;i++)
					if(INFO->elements[i]->team==MyTeam)
					{
						if(INFO->elements[i]->type==SUBMARINE && Tactics[INFO->elements[i]->index]==0)cnt+=kProperty[SUBMARINE].population;
						else if(INFO->elements[i]->type==CARRIER && Tactics[INFO->elements[i]->index]==0)cnt+=kProperty[CARRIER].population;
					}
				if(cnt>=60-Element_Max[CARGO]-2)//构成50人口进攻阵型
				for(int i=0;i<INFO->element_num;i++)
					if(INFO->elements[i]->team==MyTeam && (INFO->elements[i]->type==SUBMARINE || INFO->elements[i]->type==CARRIER))
						Tactics[INFO->elements[i]->index]=1;
			}

			int K=2;
			ElementType Type;
			if(INFO->round>2)
			while(INFO->population>55 || My_Element_Now[CARRIER]*4+My_Element_Now[SUBMARINE]*2+My_Element_Now[CARGO]<=57)
			{
				if(My_Element_Now[CARRIER]*K<=My_Element_Now[SUBMARINE])Type=CARRIER;
				else Type=SUBMARINE;
				if(Metal_Rest>=kProperty[Type].cost)
				{
					Produce(Type);
					Metal_Rest-=kProperty[Type].cost;
					My_Element_Now[Type]++;
				}
				else break;
			}
			//在航母生产出来时封锁港口
			{
				int k=0;
				for(int i=0;i<INFO->element_num;i++)
					if(INFO->elements[i]->team==MyTeam && INFO->elements[i]->type==CARRIER)k++;
				int cnt=0;
				for(int i=0;i<INFO->production_num;i++)
				if(INFO->production_list[i].unit_type==CARRIER)
					if(INFO->production_list[i].round_left<2)cnt++;
				cnt=min(cnt,Fuel_Rest/kProperty[CARRIER].fuel_max);
				cnt=min(cnt,Element_Max[CARRIER]-k);
				for(int i=0;cnt>0&&i<(int)MyPort.size();i++)
				{
					for(k=0;k<INFO->element_num;k++)
						if(INFO->elements[k]->team == MyTeam && distance(INFO->elements[k]->pos,MyPort[i])==0)
							break;
					if(k<INFO->element_num || !Hinder_Now.a[MyPort[i].z][MyPort[i].x][MyPort[i].y])
					{
						Hinder.a[MyPort[i].z][MyPort[i].x][MyPort[i].y]++;
						cnt--;
					}
				}
			}
		}
		else
		{
			//优先将运输船数量造满
			for(i=0;My_Element_Now[CARGO]<Element_Max[CARGO] && Metal_Rest>=kProperty[CARGO].cost;i++)
			{
				Produce(CARGO);
				My_Element_Now[CARGO]++;
				Metal_Rest-=kProperty[CARGO].cost;
			}
			//策略转换，防御→进攻
			{
				int cnt=0;
				for(int i=0;i<INFO->element_num;i++)
					if(INFO->elements[i]->team==MyTeam && INFO->elements[i]->type==FIGHTER && Tactics[INFO->elements[i]->index]==0)cnt++;
				if(cnt>=Element_Max[FIGHTER])//17飞机构成进攻阵型
				for(int i=0;i<INFO->element_num;i++)
					if(INFO->elements[i]->team==MyTeam && INFO->elements[i]->type==FIGHTER)
						Tactics[INFO->elements[i]->index]=1;
			}

			//疯狂爆飞机
			if(INFO->round>1)
			while(My_Element_Now[FIGHTER]<Element_Max[FIGHTER]*2  && Metal_Rest>=kProperty[FIGHTER].cost)
			{
				Produce(FIGHTER);
				My_Element_Now[FIGHTER]++;
				Metal_Rest-=kProperty[FIGHTER].cost;
			}
			//在飞机生产出来时或存在飞机补给请求时封锁空港
			{
				int k=0;
				for(int i=0;i<INFO->element_num;i++)
					if(INFO->elements[i]->team==MyTeam && INFO->elements[i]->type==FIGHTER)k++;
				int cnt=0;
				for(int i=0;i<INFO->production_num;i++)
				if(INFO->production_list[i].unit_type==FIGHTER)
					if(INFO->production_list[i].round_left<2)cnt++;
				cnt=min(cnt,Fuel_Rest/kProperty[FIGHTER].fuel_max);
				cnt=min(cnt,Element_Max[FIGHTER]-k);
				for(int i=0;i<INFO->element_num;i++)
					if(INFO->elements[i]->team==MyTeam && INFO->elements[i]->type==FIGHTER && INFO->elements[i]->ammo<kProperty[FIGHTER].ammo_max && distance(INFO->elements[i],MyBase)<=kProperty[FIGHTER].speed)
						cnt++;
				for(int i=0;cnt&&i<(int)MyAirport.size();i++)
				{
					for(k=0;k<INFO->element_num;k++)
						if(INFO->elements[k]->team == MyTeam && distance(INFO->elements[k]->pos,MyAirport[i])==0)
							break;
					if(k<INFO->element_num || !Hinder_Now.a[MyAirport[i].z][MyAirport[i].x][MyAirport[i].y])
					{
						Hinder.a[MyAirport[i].z][MyAirport[i].x][MyAirport[i].y]++;
						cnt--;
					}
				}
			}
		}
	}
	void Base_AI(const State *p)//基地AI
	{
		//优先维修需求单位
		//优先攻击敌方濒死单位
		//优先补给需求单位

		//攻击
		if(!Attack.check(p))
		{
			Supply_Manager::Demand t=Demand.Supply_Dying(p);
			if(t.F!=-1)Supply(p->index,t.p->index,t.F,t.A,t.M);
			else
			{
				return;
			}
		}
	}
	void Fort_AI(const State *p)//据点AI
	{
		//优先攻击敌方濒死单位
		//优先补给需求单位

		//攻击
		if(!Attack.check(p))
		{
			Supply_Manager::Demand t=Demand.Supply_Dying(p);
			if(t.F!=-1)Supply(p->index,t.p->index,t.F,t.A,t.M);
			else
			{
				return;
			}
		}
	}
	bool Cargo_Collect(const State *p,int Type)//前往回合最近点采集Type种资源，返回是否成功
	{
		const Property *P=&kProperty[p->type];
		const State *q;
		int k,t;
		int MinRest=INF,MinDis=INF;
		coor Order;
		//采集
		for(t=0;t<INFO->element_num;t++)
		{
			q=INFO->elements[t];
			if(q->type == Type && Resource.a[q->index]>0)
			{
				for(k=0;k<4;k++)
				{
					coor S;
					S.x = q->pos.x + Move_Offset[k][0];
					S.y = q->pos.y + Move_Offset[k][1];
					S.z = 1;
					if(check(S)&&Map(S.x,S.y)==OCEAN)
					{
						int dist=MyPortDist.b[S.x][S.y];
						int rest=(distance(p->pos,S) + P->speed - 1)/(P->speed) + (MyPortDist.b[S.x][S.y] + P->speed - 1)/(P->speed) + Cargo_Appointment.b[S.x][S.y]*3/2;
						if(rest<MinRest || (rest==MinRest && dist<MinDis))
						{
							MinRest=rest;
							MinDis=dist;
							Order=S;
						}
					}
				}
			}
		}
		if(MinRest<INF)
		{
			Cargo_Appointment.b[Order.x][Order.y]++;
			Move.Move_Straight(p,Resource.SPS[Resource.id[Order]],false);
			return true;
		}
		return false;
	}
	void Cargo_AI(const State *p)
	{
		//当地图没有资源时自杀
		//优先满足周围单位的补给需求
		//以最高效率采集(计算有余量矿点的最快采集时间)
		//装载饱和时返回基地
		//在矿点周围的矿车优先补给邻近的待采矿矿车
		//在基地周围的矿车若基地负载已满则等待直到资源成功运输
		
		//发送补给需求
		if(p->fuel==0 || (p->fuel<LowerFuel[p->type]&&DisToBase(p->pos,MyBase_Coor)<=1))
		{
			int require=(DisToBase(p->pos,MyBase_Coor)<=1)?LowerFuel[p->type]-p->fuel:Base_Attack_Range.b[p->pos.x][p->pos.y]+10;
			Demand.add(p,require);
		}

		//将资源送至基地
		Supply_Manager::Demand t=Demand.Supply_Dying(p);
		if(t.F!=-1)Supply(p->index,t.p->index,t.F,t.A,t.M);
		else if(DisToBase(p->pos,MyBase_Coor)<=1)
		{
			//补给基地
			Supply(p->index,MyBase_Index,max(0,p->fuel - LowerFuel[p->type]),0,p->metal);
		}
		else
		{
			int i;
			for(i=0;i<INFO->element_num;i++)
			{
				const State *q=INFO->elements[i];
				if(q->type==2||q->type==3)
					if(distance(p,q)<=1)break;
			}
			//处于矿点周围且有矿车随便选择空余量最多的矿车补给
			if(i<INFO->element_num)
			{
				for(i=0;i<INFO->element_num;i++)
				{
					const State *q=INFO->elements[i];
					if(q->team==MyTeam&&q->type==7&&p->index!=q->index&&distance(p,q)<=1)
					{
						Supply(p->index,q->index,max(0,p->fuel - LowerFuel[p->type]),0,p->metal);
						break;
					}
				}
			}
		}

		//采集
			//油量不足时采集石油
			//没有金属时优先采集金属
			//饱和时返回基地
			bool Move_Success=false;//优先的操作是否成功
			if(Move_Success==false && p->fuel<=LowerFuel[p->type])Move_Success|=Cargo_Collect(p,3);//采油
			if(Move_Success==false && p->metal==0)Move_Success|=Cargo_Collect(p,2);//采矿
			//返回基地
			if(Move_Success==false && (p->metal>0 || p->fuel>LowerFuel[p->type]))
			{
				Move.Move_Straight(p,MyPortDist);
				Move_Success=true;
			}
			if(Move_Success==false)
			{
				Move.Move_Straight(p,EnemyPortDist);
				Move_Success=true;
			}
		//采集END

	}
	void Carrier_AI(const State *p)//航母AI
	{
		//生产出后根据自身燃料选择是否在基地周围停留
		//在基地周围燃料不足时或在海外弹尽粮绝时发送补给请求
		//优先满足周围单位的补给需求
		//向包围敌方基地的方向移动,移动过程中选取火力稀疏点
		//弹尽粮绝时返回己方基地
		//攻击范围内濒死单位
		const Property *P=&kProperty[p->type];


		//发送补给请求
		if(p->fuel==0 || (p->fuel<LowerFuel[p->type]&&DisToBase(p->pos,MyBase_Coor)<=1))
		{
			int require=(DisToBase(p->pos,MyBase_Coor)<=1)?LowerFuel[p->type]-p->fuel:Base_Attack_Range.b[p->pos.x][p->pos.y]+10;
			Demand.add(p,require);
		}

		if(DisToBase(p->pos,MyBase_Coor)>1 || p->fuel>=LowerFuel[p->type])
		{
			if(MyTactics == 2 || MyTactics == 5  || Tactics[p->index]==1)Move.Move_Safety(p,Base_Attack_Range);//进攻
			else if(Tactics[p->index]==0)Move.Move_Safety(p,StagingDist);//防御
		}
		
		//退还资源
		if(p->fuel>LowerFuel[p->type]&&DisToBase(p->pos,MyBase_Coor)<=1)
		{
			Supply(p->index,MyBase_Index,p->fuel-LowerFuel[p->type],0,0);
		}
		//攻击
		else
		Attack.check(p);
	}
	void Submarine_AI(const State *p)//潜艇AI
	{
		//在基地周围燃料不足时或在海外弹尽粮绝时发送补给请求
		//向包围敌方港口的方向移动
		//攻击范围内濒死单位
		const Property *P=&kProperty[p->type];

		//发送补给请求
		if(p->fuel==0 || (p->fuel<LowerFuel[p->type]&&DisToBase(p->pos,MyBase_Coor)<=1))
		{
			int require=(DisToBase(p->pos,MyBase_Coor)<=1)?LowerFuel[p->type]-p->fuel:Base_Attack_Range.b[p->pos.x][p->pos.y];
			Demand.add(p,require);
		}


		if(DisToBase(p->pos,MyBase_Coor)>1 || p->fuel>=LowerFuel[p->type])
		{
			if(MyTactics == 2 || Tactics[p->index]==1)Move.Move_Straight(p,EnemyPortDist);//进攻
			else if(Tactics[p->index]==0)Move.Move_Straight(p,StagingDist);//防御
		}

		//攻击
		Attack.check(p);
	}
	void Fighter_AI(const State *p)//攒齐一波直推基地
	{
		const Property *P=&kProperty[p->type];
		//if(DisToBase(p->pos,EnemyBase)<=P->fire_ranges[LAND])AttackUnit(p->index,EnemyBase_Index);

		Attack.check(p);
		if(p->ammo<kProperty[p->type].ammo_once)//弹药不够时在敌军基地周边乱动直到燃料耗尽而坠毁
		{
			if(distance(p->pos,EnemyBase_Coor)<P->speed-3)
			{
				for(int i=0;i<INFO->x_max;i++)
					for(int j=0;j<INFO->y_max;j++)
						MyAirportDist.b[i][j]*=-1;
				Move.Move_Straight(p,MyAirportDist);
				for(int i=0;i<INFO->x_max;i++)
					for(int j=0;j<INFO->y_max;j++)
						MyAirportDist.b[i][j]*=-1;
			}
			else
			Move.Move_Straight(p,MyAirportDist);//弹药不足返回基地
		}
		else
		if(MyTactics==0 && Tactics[p->index]==0)//防御基地
		{
			Move.Move_Straight(p,MyAirportDist);
		}
		else 
		if(MyTactics==1 || Tactics[p->index]==1)//进攻
		{
			if(distance(p,EnemyBase)>1)Move.Move_Straight(p,EnemyAirportDist);
			else
			{
				//尽量抢占基地上空的空位
				if(distance(p,EnemyBase)==0)Hinder.a[p->pos.z][p->pos.x][p->pos.y]++;
				else
				{
					int i,j,k=0x7fffffff;
					coor order;
					for(i=0;i<(int)EnemyAirport.size();i++)
					{
						coor T=EnemyAirport[i];
						j=Hinder.a[T.z][T.x][T.y]*1000-distance(p->pos,T);
						if(j<k)
						{
							order=T;
							k=j;
						}
					}
					Position Target;
					Target.x=order.x;
					Target.y=order.y;
					Target.z=order.z;
					ChangeDest(p->index,Target);
				}
			}
		}
	}
	void Scout_AI(const State *p)
	{
		const Property *P=&kProperty[p->type];

		//发送补给请求
		if(p->fuel==0 || (p->fuel<LowerFuel[p->type]&&DisToBase(p->pos,MyBase_Coor)<=1))
		{
			int require=(DisToBase(p->pos,MyBase_Coor)<=1)?LowerFuel[p->type]-p->fuel:Base_Attack_Range.b[p->pos.x][p->pos.y]+10;
			Demand.add(p,require);
		}

		//攻击
		Attack.check(p);

		//向所有非运输船/基地/据点 的中位数坐标处移动
		vector<int>x_,y_;
		for(int i=0;i<INFO->element_num;i++)
			if(INFO->elements[i]->type>3 && INFO->elements[i]->type!=CARGO && INFO->elements[i]->type!=SCOUT)
			{
				const State *q=INFO->elements[i];
				x_.push_back(q->pos.x);
				y_.push_back(q->pos.y);
			}
		coor target=p->pos;
		if(x_.size())
		{
			sort(x_.begin(),x_.end());
			sort(y_.begin(),y_.end());
			int x=x_[x_.size()/2],y=y_[y_.size()/2];
			
			if(distance(coor(x,y),p->pos)>3)target=coor(x,y,2);
		}
		Shortest_Path_Struct SPC;
		SPC.Shortest_Path(target);
		Move.Move_Safety(p,SPC);
	}
}AI;
const int DEBUG=true;
const char Element_Name[10][10]={"基地0","据点1","矿场2","油田3","潜艇4","驱逐舰5","航母6","运输舰7","战斗机8","侦察机9"};
/*struct DEBUGER_
{
	void draw(char *s){if(!DEBUG)return;string S(s);output<<S;}
	void draw(coor a){if(!DEBUG)return;output<<'('<<a.x<<','<<a.y<<','<<a.z<<')';}
	void draw(Size a){if(!DEBUG)return;output<<'('<<a.x_length<<','<<a.y_length<<')';}
	void draw(State a)
	{
		if(!DEBUG)return;
		char order[500];
		sprintf_s(order,"Index=%d %s:(%d,%d,%d) Team=%d Visible=%d Health=%d Fuel=%d Ammo=%d Metal=%d Destination:(%d,%d,%d)\n",a.index,Element_Name[a.type],a.pos.x,a.pos.y,a.pos.z,a.team,a.visible,a.health,a.fuel,a.ammo,a.metal,a.destination.x,a.destination.y,a.destination.z);
		draw(order);
	};
	void draw(ProductionEntry a)
	{
		if(!DEBUG)return;
		char order[30];
		sprintf_s(order,"(%s: %d rd)",Element_Name[a.unit_type],a.round_left);
		draw(order);
}
	void draw(const GameInfo &a)
	{
		if(!DEBUG)return;
		char order[500];
		if(a.round==0)
		{
			sprintf_s(order,"MyTeam=%d MapSize(%d,%d)  Population_Limit=%d  Round_Limit=%d Weather=%d Time_Per_Round=%.2f\n",a.team_num,a.x_max,a.y_max,a.population_limit,a.round_limit,a.weather,a.time_per_round);
			draw(order);
			for(int i=0;i<INFO->x_max;i++,output<<endl)
				for(int j=0;j<INFO->y_max;j++)output<<Map(i,j)<<' ';
			output<<"\n\n\n\n\n";
		}
		sprintf_s(order,"Round:%d    Scores:(%d,%d)    Population=%d Element_Num=%d Production_Num=%d\n",a.round,a.scores[0],a.scores[1],a.population,a.element_num,a.production_num);
		draw(order);
		for(int k=0;k<3;k++)
			for(int j=0;j<10;j++)
				for(int i=0;i<a.element_num;i++)
					if(a.elements[i]->team==k&&a.elements[i]->type==j)
						draw(*a.elements[i]);
		for(int i=0;i<a.production_num;i++)draw(a.production_list[i]);
		output<<"\n\n\n\n\n\n"<<endl;
	}
}DEBUGER;*/
void AIMain()
{  
	TryUpdate();
	//DEBUGER.draw(*INFO);
	//回合开始时的初始化
	if(!AI_Begin)
	{
		AI.AI_Start();
		AI_Begin=true;
	}
	//只允许攻击不允许移动
	/*{
		Attack.clear();
		Attack.init();
		for(int i=0;i<INFO->element_num;i++)
			if(INFO->elements[i]->team==MyTeam)
				Attack.check(INFO->elements[i]);
		Attack.solve();
		return;
	}*/

	AI.AI_Run();
}
