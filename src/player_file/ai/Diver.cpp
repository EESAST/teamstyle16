#include "basic.h"//Repeated LastCarPos!!!!!!!!!
#include<time.h>
#include<stdlib.h>
#include<iostream>
#include<vector>
#include<cstring>
#include<math.h>
using namespace std;
// Remove this line if you konw C++, and don't want a dirty namespace
using namespace teamstyle16;

const char * GetTeamName()
{
    return "Diver";  // Name of your team
}
enum CommandType
{
	//Produce
	PRODUCE,
	//Attack
	ATTACKUNIT,
	ATTACKFORT,
	ATTACKBASE,
	//Suppply
	SUPPLYUNIT,//??
	SUPPLYSUB,
	SUPPLYFIGHTER,
	SUPPLYCARRIER,
	//Fix
    FIX,
	//ChangeDest
	FORWARD,
	CARGOMETALSUPPLY,
	GOFORT,
	RETURNFORT,
	RETURNBASE,
	//Cancel
	CANCEL,
    kCommandTypes
};

const GameInfo* INFO=Info();
vector <int> Sub;
int sub_num=0;
vector <int> Carrier;
int car_num=0;
vector <int> Fighter;
int fight_num=0;
int Base1;
int GoSubRound=0;
bool GoCargo=0;
Position SupplyPos[8];//submarines will by their sides;
Position CargoPos[8];//If witten, replace SupplyPos;
Position CarrierPos[8];
vector <Position> PrimaryPos;
vector <Position> Dis8Pos;
vector <Position> Dis8Pos1;
int sub_state=0,car_state=0,fly_state=0;
Position InitSubPos[14];
Position LastSubPos[14];
Position LastCarPos[6];
vector<Position> FighterPos;
vector<Position> EnemyPort;
vector<int> Enemy;
vector<int>::iterator carrierit;
vector<int>::iterator cargoit;
int target[14]; 
int BaseMetal;
int ps=0,pc=0,pf=0;

int GetBase(int teamnum=INFO->team_num);
int GetBaseIndex(int teamnum=INFO->team_num);
void Init();
int Distance(int i,int j);//i,j are indexes; 
int Distance(int i,Position p);
int Distance(Position p1,Position p2);
int GetNearA(int index, ElementType type);
int BeIn(int index,int*a,int n);
int BeIn(int index,vector<int> a,int n);
int BeIn(Position p,Position* a,int n);
int Min(int x,int y);
int Max(int x,int y);
int LargeThanZero(double i);
Position FindSea(Position p);
Position FindSeaA(Position p);
void MinAttack();
int DistanceB(int index,int team_num=INFO->team_num );
int DistanceB(Position p,int team_num=INFO->team_num );
int if_alive(int index);
void BaseProduce();
void Submarine();
void CarrierM();
void FighterM();
//Bad Roasted Ducks
struct Cargo
{
	int index;
	Position dest;
	Position dest0;
    int type;
	int roundleft;
	int supply;
};
struct Oilfield
{
	int index;
	int fuel;
	Position pos;
};
struct Mine
{
	int index;
	int metal;
	Position pos;
};
struct Fort
{
	int index;
	int metal;
	int fuel;
	Position pos;
};
struct BasePort
{
	bool occupy;
	Position pos;
};
vector <Oilfield> oilfield;//save all fields
vector <Mine> mine; //save all mines
vector <Fort> fort; //save my forts
vector <Oilfield> sfield;
int sfieldinit=0;
Cargo cargo[9];
int cargonum;
int portnum;
int avail=1;

vector <Position> oilport;
vector <Position> mineport;
vector <BasePort> baseport;
vector <Position> tempft;
vector <Position> tempfl;
vector <Position> tempm;
vector <Position> LSP;
bool visited[100][100];
bool findsfield[8];

int b2b;
bool enough=0;
int BaseFuelF[6];
Position BasePos;
void CargoSupply();
const State* symmetry(int index);
int compare(const void*a,const void*b);
Position OnePort(Position pos);
void fPorts(int f);
void mPorts(int m);
void FindBasePort();
int NearPort(int index);
int GetBP(Position pos);
void FindAroundSea(Position pos);
void FortAttack();



void AIMain()
{
	Base1=GetBaseIndex(1-INFO->team_num);
    TryUpdate();
	Init();
	FortAttack();
	if(Distance(Base1,GetBaseIndex())>=75)
	{
		CarrierM();
		Submarine();
	}
	else
	{
		FighterM();
	}
	CargoSupply();
	BaseProduce();
}
void Init()
{
	b2b=DistanceB(GetBaseIndex(1-INFO->team_num));
	memset(visited,0,sizeof(visited));
	cout<<"Init()"<<endl;
	if(Distance(Base1 ,GetBaseIndex ())>=75)
	{
		int i,j,x,y,k,l;
		Position p;
		BaseMetal=GetState (GetBaseIndex ())->metal ;
		if(INFO->round ==0)
		{
			if(Distance(Base1,GetBaseIndex ())>=170)
			{
				if(GetState(Base1)->pos .x-GetState(GetBaseIndex ())->pos.x)
				{
					x=(GetState(Base1)->pos .x-GetState(GetBaseIndex ())->pos.x)/abs(GetState(Base1)->pos .x-GetState(GetBaseIndex ())->pos.x);
				}
				else x=0;
				if(GetState(Base1)->pos .y-GetState(GetBaseIndex ())->pos.y)
				{
					y=(GetState(Base1)->pos .y-GetState(GetBaseIndex ())->pos.y)/abs(GetState(Base1)->pos .y-GetState(GetBaseIndex ())->pos.y);
				}
				else y=0;
				for(p.x=GetState(GetBaseIndex ())->pos .x;p.x<=Max(GetState(GetBaseIndex ())->pos .x,GetState(Base1)->pos .x)&&p.x>=Min(GetState(GetBaseIndex ())->pos .x,GetState(Base1)->pos .x);p.x+=x)
				{
					for(p.y=(b2b-100-abs(p.x-GetState(GetBaseIndex ())->pos .x))*y+GetState(GetBaseIndex ())->pos .y;abs(p.y-GetState(GetBaseIndex())->pos.y)<=abs(b2b-90-abs(p.x-GetState(GetBaseIndex ())->pos .x))&&
							p.y<=Max(GetState(GetBaseIndex ())->pos .y,GetState(Base1)->pos .y)&&p.y>=Min(GetState(GetBaseIndex ())->pos .y,GetState(Base1)->pos .y);p.y+=y)
					{
						if(!Map(p.x,p.y)&&!Map(p.x+1,p.y)&&!Map(p.x+2,p.y)&&!Map(p.x,p.y+1)&&!Map(p.x+1,p.y+1)&&!Map(p.x+2,p.y+1))
						{
							p.z=1;
							PrimaryPos .push_back (p);
						}
					}
				}
				int d=200;
				Position p1,p2;
				for(i=0;i<PrimaryPos.size ();i++)
					for(j=i+1;j<PrimaryPos .size ();j++)
					{
						if(abs(PrimaryPos [i].y-PrimaryPos [j].y)>=2&&Distance (PrimaryPos [i],PrimaryPos [j])<d)
						{
							p1.x=PrimaryPos [i].x;
							p1.y=PrimaryPos [i].y;
							p1.z=1;
							p2.x=PrimaryPos [j].x;
							p2.y=PrimaryPos [j].y;
							p2.z=1;
							d=Distance (PrimaryPos [i],PrimaryPos [j]);
						}
					}
				if(GetState (Base1)->pos .y>GetState(GetBaseIndex ())->pos .y)
				{
					CargoPos[0].x=p1.x;CargoPos[0].y=p1.y;
					CargoPos[1].x=p1.x+1;CargoPos[1].y=p1.y;
					CargoPos[2].x=p1.x+2;CargoPos[2].y=p1.y;
					CargoPos[3].x=p2.x;CargoPos[3].y=p2.y;
					CargoPos[4].x=p2.x+1;CargoPos[4].y=p2.y;
					CargoPos[5].x=p2.x+2;CargoPos[5].y=p2.y;
					CarrierPos[0].x=p1.x;CarrierPos[0].y=p1.y+1;
					CarrierPos[1].x=p1.x+1;CarrierPos[1].y=p1.y+1;
					CarrierPos[2].x=p1.x+2;CarrierPos[2].y=p1.y+1;
					CarrierPos[3].x=p2.x;CarrierPos[3].y=p2.y+1;
					CarrierPos[4].x=p2.x+1;CarrierPos[4].y=p2.y+1;
					CarrierPos[5].x=p2.x+2;CarrierPos[5].y=p2.y+1;
				}
				else
				{
					CarrierPos[0].x=p1.x;CarrierPos[0].y=p1.y;
					CarrierPos[1].x=p1.x+1;CarrierPos[1].y=p1.y;
					CarrierPos[2].x=p1.x+2;CarrierPos[2].y=p1.y;
					CarrierPos[3].x=p2.x;CarrierPos[3].y=p2.y;
					CarrierPos[4].x=p2.x+1;CarrierPos[4].y=p2.y;
					CarrierPos[5].x=p2.x=2;CarrierPos[5].y=p2.y;
					CargoPos[0].x=p1.x;CargoPos[0].y=p1.y+1;
					CargoPos[1].x=p1.x+1;CargoPos[1].y=p1.y+1;
					CargoPos[2].x=p1.x+2;CargoPos[2].y=p1.y+1;
					CargoPos[3].x=p2.x;CargoPos[3].y=p2.y+1;
					CargoPos[4].x=p2.x+1;CargoPos[4].y=p2.y+1;
					CargoPos[5].x=p2.x+2;CargoPos[5].y=p2.y+1;
				}
				for(i=0;i<6;i++)
				{
					CargoPos[i].z=1;
					CarrierPos[i].z=1;
				}
				int m=6,n=6;
				for(k=0;k<40&&m<8;k++)
					for(i=0;i<20&&i<k&&m<8;i++)
					{
						p.z=1;
						p.x=p1.x+i;
						p.y=p1.y+k-i;
						if(!Map(p.x,p.y)&&!BeIn(p,CarrierPos,n)&&!BeIn(p,CargoPos,m)&&m<8)
						{
							CargoPos[m].x=p.x;CargoPos[m].y=p.y;CargoPos[m].z=p.z;
							m++;
						}
						p.x=p1.x+i;
						p.y=p1.y-k+i;
						if(!Map(p.x,p.y)&&!BeIn(p,CarrierPos,n)&&!BeIn(p,CargoPos,m)&&m<8)
						{
							CargoPos[m].x=p.x;CargoPos[m].y=p.y;CargoPos[m].z=p.z;
							m++;
						}
						p.x=p2.x+i;
						p.y=p2.y+k-i;
						if(!Map(p.x,p.y)&&!BeIn(p,CarrierPos,n)&&!BeIn(p,CargoPos,m)&&m<8)
						{
							CargoPos[m].x=p.x;CargoPos[m].y=p.y;CargoPos[m].z=p.z;
							m++;
						}
						p.x=p2.x+i;
						p.y=p2.y-k+i;
						if(!Map(p.x,p.y)&&!BeIn(p,CarrierPos,n)&&!BeIn(p,CargoPos,m)&&m<8)
						{
							CargoPos[m].x=p.x;CargoPos[m].y=p.y;CargoPos[m].z=p.z;
							m++;
						}
					}
				}
			else if(Distance(Base1,GetBaseIndex ())>=100)//LeftRightSymmetric or UpDownSymmetric not written
			{
				bool f=0;
				if(GetState(GetBaseIndex ())->pos .x-GetState(Base1)->pos .x&&GetState(GetBaseIndex ())->pos .y-GetState(Base1)->pos .y)
				{
					for(x=Min(GetState(GetBaseIndex ())->pos .x,GetState(Base1)->pos .x);x<=Max(GetState(GetBaseIndex ())->pos .x,GetState(Base1)->pos .x);x++)
					{
						for(y=b2b-100-x+Min(GetState(GetBaseIndex ())->pos .x,GetState(Base1)->pos .x)+Min(GetState(GetBaseIndex ())->pos .y,GetState(Base1)->pos .y);
								y<=b2b-90-x+Min(GetState(GetBaseIndex ())->pos .x,GetState(Base1)->pos .x)+Min(GetState(GetBaseIndex ())->pos .y,GetState(Base1)->pos .y)
									&&y<=Max(GetState(GetBaseIndex ())->pos .y,GetState(Base1)->pos .y);y++)
						{
							if(!Map(x,y)&&!Map(x+1,y)&&!Map(x+2,y)&&!Map(x+3,y)&&!Map(x,y+1)&&!Map(x+1,y+1)&&!Map(x+2,y-1)&&!Map(x+3,y+1))
							{
								p.x=x;
								p.y=y;
								p.z=1;
								f=1;
								break;
							}
						}
						if(f)
							break;
					}
				}
				else
				{
					if(GetState(GetBaseIndex ())->pos .x==GetState(Base1)->pos .x)
					{
						for(x=GetState(Base1)->pos .x-5;x<=GetState(Base1)->pos .x+5;x++)
						{
							for(y=b2b-100-x+Min(GetState(GetBaseIndex ())->pos .x,GetState(Base1)->pos .x)+Min(GetState(GetBaseIndex ())->pos .y,GetState(Base1)->pos .y);
								y<=b2b-90-x+Min(GetState(GetBaseIndex ())->pos .x,GetState(Base1)->pos .x)+Min(GetState(GetBaseIndex ())->pos .y,GetState(Base1)->pos .y)
								&&y<=Max(GetState(GetBaseIndex ())->pos .y,GetState(Base1)->pos .y);y++)
							{
								if(!Map(x,y)&&!Map(x+1,y)&&!Map(x+2,y)&&!Map(x+3,y)&&!Map(x,y+1)&&!Map(x+1,y+1)&&!Map(x+2,y+1)&&!Map(x+3,y+1))
								{
									p.x=x;
									p.y=y;
									p.z=1;
									f=1;
									break;
								}
							}
							if(f)
								break;
						}
					}
					else if(GetState(GetBaseIndex ())->pos .y==GetState(Base1)->pos .y)
					{
						for(y=GetState(Base1)->pos .y-5;x<=GetState(Base1)->pos .y+5;y++)
						{
							for(x=b2b-100-y+Min(GetState(GetBaseIndex ())->pos .x,GetState(Base1)->pos .x)+Min(GetState(GetBaseIndex ())->pos .y,GetState(Base1)->pos .y);
								x<=b2b-90-y+Min(GetState(GetBaseIndex ())->pos .x,GetState(Base1)->pos .x)+Min(GetState(GetBaseIndex ())->pos .y,GetState(Base1)->pos .y)
								&&x<=Max(GetState(GetBaseIndex ())->pos .x,GetState(Base1)->pos .x);x++)
							{
								if(!Map(x,y)&&!Map(x,y+1)&&!Map(x,y+2)&&!Map(x,y+3)&&!Map(x+1,y)&&!Map(x+1,y+1)&&!Map(x+1,y+2)&&!Map(x+1,y+3))
								{
									p.x=x;
									p.y=y;
									p.z=1;
									f=1;
									break;
								}
							}
							if(f)
								break;
						}
					}
				}
				if(f)
				{
					CargoPos[0].x=p.x;CargoPos[0].y=p.y;CargoPos[0].z=1;
					CargoPos[1].x=p.x+1;CargoPos[1].y=p.y;CargoPos[1].z=1;
					CargoPos[2].x=p.x+2;CargoPos[2].y=p.y;CargoPos[2].z=1;
					CargoPos[3].x=p.x+3;CargoPos[3].y=p.y;CargoPos[3].z=1;
					CargoPos[4].x=p.x;CargoPos[4].y=p.y+1;CargoPos[4].z=1;
					CargoPos[5].x=p.x+1;CargoPos[5].y=p.y+1;CargoPos[5].z=1;
					CargoPos[6].x=p.x+2;CargoPos[6].y=p.y+1;CargoPos[6].z=1;
					CargoPos[7].x=p.x+3;CargoPos[7].y=p.y+1;CargoPos[7].z=1;
				}
			}
		for(x=GetState(Base1)->pos .x-9;x<=GetState(Base1)->pos .x+9;x++)
			for(y=GetState(Base1)->pos .y-9;y<=GetState(Base1)->pos .y+9;y++)
			{
				p.x=x;
				p.y=y;
				p.z=1;
				if((!Map(x,y))&&(DistanceB(p,1-INFO->team_num)==8||DistanceB(p,1-INFO->team_num)==7))
				{
					Dis8Pos .push_back (p);
				}
			}
		for(i=0;i<Dis8Pos.size();i++)
			for(j=0;j<Dis8Pos.size()-1-i;j++)
			{
				if(DistanceB(Dis8Pos [j+1])<DistanceB(Dis8Pos [j]))
				{
					Position temp;
					temp.x=Dis8Pos [j].x;temp.y=Dis8Pos [j].y;temp.z=Dis8Pos [j].z;
					Dis8Pos [j].x=Dis8Pos [j+1].x;Dis8Pos [j].y=Dis8Pos [j+1].y;Dis8Pos [j].z=Dis8Pos [j+1].z;
					Dis8Pos [j+1].x=temp.x;Dis8Pos [j+1].y=temp.y;Dis8Pos [j+1].z=temp.z;
				}
			}
			for(i=0;i<6;i++)
			{
				LastCarPos[i].x=Dis8Pos [i].x;LastCarPos[i].y=Dis8Pos [i].y;LastCarPos[i].z=Dis8Pos [i].z;
			}
		for(x=GetState(GetBaseIndex())->pos .x-9;x<=GetState(GetBaseIndex())->pos .x+9;x++)
			for(y=GetState(GetBaseIndex())->pos .y-9;y<=GetState(GetBaseIndex())->pos .y+9;y++)
			{
				p.x=x;
				p.y=y;
				p.z=1;
				if((!Map(x,y))&&(DistanceB(p)==8||DistanceB(p)==7))
				{
					Dis8Pos1 .push_back (p);
				}
			}
		for(i=0;i<Dis8Pos1.size();i++)
			for(j=0;j<Dis8Pos1.size()-1-i;j++)
			{
				if(DistanceB(Dis8Pos1 [j+1],1-INFO->team_num)<DistanceB(Dis8Pos1 [j]),1-INFO->team_num)
				{
					Position temp;
					temp.x=Dis8Pos1 [j].x;temp.y=Dis8Pos1 [j].y;temp.z=Dis8Pos1 [j].z;
					Dis8Pos1[j].x=Dis8Pos1[j+1].x;Dis8Pos1[j].y=Dis8Pos1 [j+1].y;Dis8Pos [j].z=Dis8Pos1 [j+1].z;
					Dis8Pos1 [j+1].x=temp.x;Dis8Pos1 [j+1].y=temp.y;Dis8Pos1 [j+1].z=temp.z;
				}
			}
		}
		cout<<"1"<<endl;
		for(i=0;i<sub_num;i++)
		{
			if(!if_alive(Sub[i]))
			{
				Sub.erase(Sub.begin ()+i);
				sub_num--;
				i--;
			}
		}
		cout<<"2"<<endl;
		for(i=0;i<INFO->element_num ;i++)
		{
			const State* element=INFO->elements [i];
			if(element->team ==INFO->team_num &&element->type ==SUBMARINE&&!BeIn(element->index ,Sub,sub_num))
			{
				Sub.push_back (element->index );
				sub_num++;
			}
		}
		for(i=0;i<car_num;i++)
		{
			if(!if_alive(Carrier[i]))
			{
				Carrier.erase(Carrier.begin ()+i);
				car_num--;
				i--;
			}
		}
		cout<<"3"<<endl;
		for(i=0;i<INFO->element_num ;i++)
		{
			const State* element=INFO->elements [i];
			if(element->team ==INFO->team_num &&element->type ==CARRIER&&!BeIn(element->index ,Carrier,car_num))
			{
				Carrier.push_back (element->index );
				car_num++;
			}
		}
		cout<<"4"<<endl;
		if(Sub.size()||Carrier.size())
		{
			/*Enemy.clear ();
			carrierit=cargoit=Enemy.begin();
			for(i=0;i<INFO->element_num ;i++)
			{
				const State* element=INFO->elements [i];
				if(element->team ==1-INFO->team_num &&element->visible &&Distance(element->index ,Base1)<20)
				{
					if(element->type==SUBMARINE)
					{
						Enemy.insert(Enemy.begin (),element->index );
						if(carrierit!=Enemy.end()) carrierit++;
						if(cargoit!=Enemy.end()) cargoit++;
					}
					else if(element->type ==CARRIER)
					{
						carrierit=Enemy.insert (carrierit,element->index);
						if(cargoit!=Enemy.end()) cargoit++;
					}
					else if(element->type==CARGO||element->type ==FIGHTER||element->type ==DESTROYER||element->type ==SCOUT)
					{
						cargoit=Enemy.insert (cargoit,element->index);
					}
				}
			}*/
		}
		for(i=0;i<14;i++)
		{
			target[i]=-1;
		}
		FindAroundSea(GetState(GetBaseIndex(1-INFO->team_num))->pos);
		int flag14[14];
		memset(flag14,0,sizeof(flag14));
		//if((car_num==6||car_state==3))
		for(i=0;i<sub_num;i++)
		{
			if(car_state!=3&&DistanceB(Sub[i],1-INFO->team_num)>10)
			{
				LastSubPos[i]=GetState(GetBaseIndex(1-INFO->team_num))->pos;
			}
			else if(car_state!=3||DistanceB(Sub[i])>50)
			{
				flag14[i]=1;
			}
			else 
			{
				LastSubPos[i]=Dis8Pos1[i];
			}
		}
		vector <Position> EnemyPort;
		
		for(i=0;i<LSP.size();i++)
		{
			if(DistanceB(LSP[i],1-INFO->team_num)==1)
				EnemyPort.push_back(LSP[i]);
		}
		
		/*for(i=0;i<LSP.size();i++)
		{
			for(j=i+1;j<LSP.size();j++)
			{
				if(DistanceB(LSP[j])<DistanceB(LSP[i]))
				{
					Position p=LSP[i]; LSP[i]=LSP[j]; LSP[j]=p;
				}
			}
		}*/
		int n=EnemyPort.size();
		j=0;
		k=0;
		for(i=0;i<sub_num;i++)
		{
			for(j=0;j<EnemyPort.size();j++)
			{
				if(flag14[i])
				{
					LastSubPos[i]=EnemyPort[j%n];
				}
			}
		}
		if(INFO->round ==0)
		{
			if(GetState(Base1)->pos .x-GetState(GetBaseIndex ())->pos.x)
			{
				x=(GetState(Base1)->pos .x-GetState(GetBaseIndex ())->pos.x)/abs(GetState(Base1)->pos .x-GetState(GetBaseIndex ())->pos.x);
			}
			else x=0;
			if(GetState(Base1)->pos .y-GetState(GetBaseIndex ())->pos.y)
			{
				y=(GetState(Base1)->pos .y-GetState(GetBaseIndex ())->pos.y)/abs(GetState(Base1)->pos .y-GetState(GetBaseIndex ())->pos.y);
			}
			else y=0;
			for(i=1,k=0,l=0;i<=30&&(k<14);i++)
			{
				for(j=1;j<=30&&(k<14);j++)
				{
					Position p;
					if(x&&y)
					{
						p.x=GetState (Base1)->pos .x+x*i;
						p.y=GetState (Base1)->pos .y+y*j;
						if(Map(p.x,p.y)==0)
						{
							InitSubPos [k].x=p.x;
							InitSubPos [k].y=p.y;
							k++;
						}
					}
					else if(x==0)
					{
						p.x=GetState (Base1)->pos .x+i;
						p.y=GetState (Base1)->pos .y+y*j;
						if(Map(p.x,p.y)==0)
						{
							InitSubPos [k].x=p.x;
							InitSubPos [k].y=p.y;
							k++;
						}
						p.x=GetState (Base1)->pos .x-i;
						if(Map(p.x,p.y)==0)
						{
							InitSubPos [k].x=p.x;
							InitSubPos [k].y=p.y;
							k++;
						}
					}
					else if(y==0)
					{
						p.x=GetState (Base1)->pos .x+x*i;
						p.y=GetState (Base1)->pos .y+j;
						if(Map(p.x,p.y)==0)
						{
							InitSubPos [k].x=p.x;
							InitSubPos [k].y=p.y;
							k++;
						}
						p.y=GetState (Base1)->pos .y-j;
						if(Map(p.x,p.y)==0)
						{
							InitSubPos [k].x=p.x;
							InitSubPos [k].y=p.y;
							k++;
						}
					}
					/*if(l<14)
					{
						p.x=GetState (Base1)->pos .x+i;
						p.y=GetState (Base1)->pos .y+j;
						if(Map(p.x,p.y)==0)
						{
							LastSubPos [l].x=p.x;
							LastSubPos [l].y=p.y;
							l++;
						}
						p.x=GetState (Base1)->pos .x+i;
						p.y=GetState (Base1)->pos .y-j;
						if(Map(p.x,p.y)==0)
						{
							LastSubPos [l].x=p.x;
							LastSubPos [l].y=p.y;
							l++;
						}
						p.x=GetState (Base1)->pos .x-i;
						p.y=GetState (Base1)->pos .y+j;
						if(Map(p.x,p.y)==0)
						{
							LastSubPos [l].x=p.x;
							LastSubPos [l].y=p.y;
							l++;
						}
						p.x=GetState (Base1)->pos .x-i;
						p.y=GetState (Base1)->pos .y-j;
						if(Map(p.x,p.y)==0)
						{
							LastSubPos [l].x=p.x;
							LastSubPos [l].y=p.y;
							l++;
						}
					}*/
				}
			}
		}
	}
	else 
	{
		int i,j;
		for(i=0;i<fight_num;i++)
		{
			if(!if_alive(Fighter[i]))
			{
				Fighter.erase(Fighter.begin ()+i);
				fight_num--;
				i--;
			}
			else if(GetState(Fighter[i])->ammo==0)
			{
				ChangeDest(Fighter[i],GetState(GetBaseIndex())->pos);
				Fighter.erase(Fighter.begin ()+i);
				fight_num--;
				i--;
			}
		}
		for(i=0;i<INFO->element_num ;i++)
		{
			const State* element=INFO->elements [i];
			if(element->team ==INFO->team_num &&element->type ==FIGHTER&&!BeIn(element->index ,Fighter,fight_num))
			{
				Fighter.push_back (element->index );
				fight_num++;
			}
		}
		if(INFO->round==0)
		{
			Position b=INFO->elements[GetBase(1-INFO->team_num)]->pos;
			Position p1={b.x-1,b.y-1,2}; FighterPos.push_back(p1);
			Position p2={b.x,b.y-1,2}; FighterPos.push_back(p2);
			Position p3={b.x+1,b.y-1,2}; FighterPos.push_back(p3);
			Position p4={b.x+1,b.y,2}; FighterPos.push_back(p4);
			Position p5={b.x,b.y,2}; FighterPos.push_back(p5);
			Position p6={b.x-1,b.y,2}; FighterPos.push_back(p6);
			Position p7={b.x-1,b.y+1,2}; FighterPos.push_back(p7);
			Position p8={b.x-1,b.y+1,2}; FighterPos.push_back(p8);
			Position p9={b.x+1,b.y+1,2}; FighterPos.push_back(p9);
			Position p10={b.x+2,b.y+1,2}; FighterPos.push_back(p10);
			Position p11={b.x+2,b.y,2}; FighterPos.push_back(p11);
			Position p12={b.x+2,b.y-1,2}; FighterPos.push_back(p12);
			Position p13={b.x+1,b.y-2,2}; FighterPos.push_back(p13);
			Position p14={b.x,b.y-2,2}; FighterPos.push_back(p14);
			Position p15={b.x-1,b.y-2,2}; FighterPos.push_back(p15);
			Position p16={b.x-2,b.y-1,2}; FighterPos.push_back(p16);
			Position p17={b.x-2,b.y,2}; FighterPos.push_back(p17);
			Position p18={b.x-2,b.y+1,2};FighterPos.push_back(p18);
			/*for(i=-1;i<=1;i++)
				for(j=-1;j<=1;j++)
				{
					Position p={GetState(Base1)->pos.x+i,GetState(Base1)->pos.y+j,2};
					FighterPos.push_back(p);
				}
			for(i=-2;i<=2;i++)
				for(j=-2;j<=2;j++)
				{
					Position p={GetState(Base1)->pos.x+i,GetState(Base1)->pos.y+j,2};
					if(DistanceB(p,1-INFO->team_num)==1&&FighterPos.size()<17)
					{
						FighterPos.push_back(p);
					}
				}*/
		}
	}
	cout<<"6"<<endl;
	int i,j,k;
	if(b2b<75) cargonum=9;
	else cargonum=8;
	tempfl.clear();
	tempm.clear();
	tempft.clear();
	avail=1;
	for(i=0;i<portnum;i++) baseport[i].occupy=0;
	for(i=0;i<cargonum;i++)
	{
		if(if_alive(cargo[i].index))
		{
			for(j=0;j<portnum;j++)
			{
				if(cargo[i].dest.x==baseport[j].pos.x&&cargo[i].dest.y==baseport[j].pos.y&&(GetState(cargo[i].index)->pos.x!=cargo[i].dest.x||GetState(cargo[i].index)->pos.y!=cargo[i].dest.y))
				{
					baseport[j].occupy=1;
					break;
				}
			}
		}
	}
	




	fort.clear();
	for(i=0;i<INFO->element_num;i++)
	{
		const State* element=INFO->elements[i];
		if(element->type==FORT&&element->team==INFO->team_num)
		{
			Fort f={element->index,element->metal,element->fuel,element->pos};
			fort.push_back(f);
		}
	}
	if(INFO->round==0)
	{
		FindBasePort();
		memset((void*)BaseFuelF,0,sizeof(BaseFuelF));
		memset(findsfield,0,sizeof(findsfield));

		BasePos=GetState(GetBaseIndex())->pos;
		for(i=0;i<INFO->element_num;i++)
	    {
			const State* element=INFO->elements[i];
			if (element->type==OILFIELD)
			{
				Oilfield o={element->index,element->visible==1?element->fuel:1000,element->pos};
				oilfield.push_back(o);
			}
			else if (element->type==MINE)
			{
				Mine m={element->index,element->visible==1?element->metal:500,element->pos};
				mine.push_back(m);
			}
		}

		for(i=0;i<cargonum;i++) 
		{
			cargo[i].index=-1;
		}
	}

	cout<<"7"<<endl;
	for(i=0;i<oilfield.size();i++)
	{
		if(GetState(oilfield[i].index)->visible==1) oilfield[i].fuel=GetState(oilfield[i].index)->fuel;
	}
	for(i=0;i<mine.size();i++)
	{
		if(GetState(mine[i].index)->visible==1) mine[i].metal=GetState(mine[i].index)->metal;
	}
	

	for(i=0;i<cargonum;i++)
		if(!if_alive(cargo[i].index)) cargo[i].index=-1;
	if(GoCargo==0)
	{
		for(i=0;i<INFO->element_num;i++)
		{
			const State*element = INFO->elements[i];
			if(element->type==CARGO&&element->team==INFO->team_num)
			{
				for(j=0;j<cargonum;j++)
				{
					if(cargo[j].index==element->index) break;
				}
				if(j==cargonum)
				{
					for(k=0;k<cargonum;k++)
					{
						if(cargo[k].index==-1)
						{
							Cargo c;
							c.index=element->index;
							c.supply=0;
							cargo[k]=c;
							//baseport[GetBP(element->pos)].occupy=1;
							break;
						}

					}
				}
			}
		}
	}
}
int BeIn(int index,int*a,int n)
{
	int i;
	for(i=0;i<n;i++)
	{
		if(a[i]=index)
			return 1;
	}
	return 0;
};
int BeIn(int index,vector<int> a,int n)
{
	int i;
	for(i=0;i<n;i++)
	{
		if(a[i]==index)
			return 1;
	}
	return 0;
}
int BeIn(Position p,Position* a,int n)
{
	int i;
	for(i=0;i<n;i++)
	{
		if(a[i].x==p.x&&a[i].y==p.y&&a[i].z==p.z)
			return 1;
	}
	return 0;
}
int Distance(int i,int j)//i,j are indexes;
{
	int x,y;
	x=abs(GetState(i)->pos.x-GetState(j)->pos.x);
	y=abs(GetState(i)->pos.y-GetState(j)->pos.y);
	return (x+y);
}
int Distance(int i,Position p)
{
	int x,y;
	x=abs(GetState(i)->pos.x-p.x);
	y=abs(GetState(i)->pos.y-p.y);
	return (x+y);
}
int Distance(Position p1,Position p2)
{
	int x,y;
	x=abs(p1.x-p2.x);
	y=abs(p1.y-p2.y);
	return (x+y);
}
int GetBase(int teamnum)
{
	int i;
	for(i=0;i<INFO->element_num;i++)
		if(INFO->elements[i]->type==BASE&&INFO->elements[i]->team==teamnum)
			return i;
}
int GetBaseIndex(int teamnum)
{
	int i;
	for(i=0;i<INFO->element_num;i++)
		if(INFO->elements[i]->type==BASE&&INFO->elements[i]->team==teamnum)
			return INFO->elements[i]->index;
}
int if_alive(int operand)
{
	for(int i=0;i<INFO->element_num;i++)
		if(INFO->elements[i]->index == operand)
			return 1;
	return 0;
}
Position FindSea(Position p)
{
	int i,j,k;
	for(k=0;k<40;k++)
	{
		for(i=0;i<20;i++)
			for(j=0;j<20&&i+j<=k;j++)
			{
				if(p.x+i>=0&&p.y+j>=0&&Map(p.x+i,p.y+j)==OCEAN)
				{
					Position p1={p.x+i,p.y+j,1};
					return p1;
				}
				else if(p.x+i>=0&&p.y-j>=0&&Map(p.x+i,p.y-j)==OCEAN)
				{
					Position p1={p.x+i,p.y-j,1};
					return p1;
				}
				else if(p.x-i>=0&&p.y+j>=0&&Map(p.x-i,p.y+j)==OCEAN)
				{ 
					Position p1={p.x-i,p.y+j,1};
					return p1;
				}
				if(p.x-i>=0&&p.y-j>=0&&Map(p.x-i,p.y-j)==OCEAN)
				{
					Position p1={p.x-i,p.y-j,1};
					return p1;
				}
			}
	}
}
Position FindSeaA(Position p)//specifically for submarines
{
	int i,j,k,x,y;
	if(abs(GetState(Base1)->pos .x-p.x))
		x=(GetState(Base1)->pos .x-p.x)/abs(GetState(Base1)->pos .x-p.x);
	else x=0;
	if(abs(GetState(Base1)->pos .y-p.y))
		y=(GetState(Base1)->pos .y-p.y)/abs(GetState(Base1)->pos .y-p.y);
	else y=0;
	for(k=1;k<40;k++)
	{
		for(i=0;i<20&&i<=k;i++)
			if(Map(p.x+i*x,p.y+(k-i)*y)==OCEAN)
			{
				Position p1={p.x+x*i,p.y+y*(k-i),1};
				return p1;
			}
	}
}
int Min(int x,int y)
{
	return(x<y?x:y);
}
int Max(int x,int y)
{
	return(x>y?x:y);
}
int DistanceB(int index,int team_num)
{
	int m=1000,i,j;
	for(i=-1;i<=1;i++)
		for(j=-1;j<=1;j++)
		{
			int d;
			d=abs(GetState(index)->pos .x-GetState(GetBaseIndex(team_num))->pos.x-i)+abs(GetState(index)->pos .y-GetState(GetBaseIndex(team_num))->pos.y-j);
			m=m>d?d:m;
		}
	return m;
}
int DistanceB(Position p,int team_num)
{
	int m=1000,i,j;
	for(i=-1;i<=1;i++)
		for(j=-1;j<=1;j++)
		{
			int d;
			d=abs(p.x-GetState(GetBaseIndex(team_num))->pos.x-i)+abs(p.y-GetState(GetBaseIndex(team_num))->pos.y-j);
			m=m>d?d:m;
		}
	return m;
}

void BaseProduce()
{
	cout<<"BaseProduce()"<<endl;
	if(Distance(Base1,GetBaseIndex ())>=75)
	{
		if(INFO->round ==0)
		{
				Produce(CARGO);
				Produce(CARGO);
				Produce(CARGO);
				Produce(CARGO);
				Produce(CARGO);
				Produce(CARGO);
				Produce(CARGO);	
				if(portnum<=7) Produce(CARGO);
		}
		else if(INFO->round==1&&portnum>7)
		{
			Produce(CARGO);
		}
		else if(INFO->round>=2)
		{
			int image=BaseFuelF[2];
			while(ps<14&&image>=120&&BaseMetal>=7)
			{
				Produce(SUBMARINE);
				image-=120;
				BaseMetal-=7;
				ps++;
			}
			if(ps>=14&&pc<=6)
			{
				image=BaseFuelF[5];
				while(pc<6&&image>=200&&BaseMetal>=30)
				{
					Produce(CARRIER);
					image-=200;
					BaseMetal-=30;
					pc++;
				}
			}
		}
	}
	else
	{
		if(INFO->round==0)
		{
			Produce(CARGO);
			Produce(CARGO);
			Produce(CARGO);
			Produce(CARGO);
			Produce(CARGO);
			Produce(CARGO);
			Produce(CARGO);
			if(portnum<=7) 
			{
				Produce(CARGO);
				Produce(CARGO);
			}
		}
		else if(INFO->round==1&&portnum>7)
		{
			Produce(CARGO);
			Produce(CARGO);
		}
		else
		{
			int image=BaseFuelF[3];
			while(pf<=17&&image>=100&&BaseMetal>=14)
			{
				Produce(FIGHTER);
				pf++;
				image-=100;
				BaseMetal-=14;
			}
			int i;
			for(i=0;i<INFO->production_num;i++)
			{
				cout<<INFO->production_list[i].unit_type<<endl;
			}
		}
	}
	/*if((car_num==6||car_state==3))
	{
		int image=BaseFuelF[2];
		cout<<BaseMetal<<image<<endl;
		while(BaseMetal>=7&&image>=120)
		{
			Produce(SUBMARINE);
			BaseMetal-=7;
			image-=120;
		}
	}*/
}
void Submarine()//where can submarine bw supplied distance 0 or 1
{
	cout<<"Submarine()"<<endl;
	int i,j,k,x,y;
	if(abs(GetState(Base1)->pos .x-GetState(GetBaseIndex())->pos.x))
		x=(GetState(Base1)->pos .x-GetState(GetBaseIndex())->pos.x)/abs(GetState(Base1)->pos .x-GetState(GetBaseIndex())->pos.x);
	else x=0;
	if(abs(GetState(Base1)->pos .y-GetState(GetBaseIndex())->pos.y))
		y=(GetState(Base1)->pos .y-GetState(GetBaseIndex())->pos.y)/abs(GetState(Base1)->pos .y-GetState(GetBaseIndex())->pos.y);
	else y=0;
	bool o=0,uf=0;
	for(i=0;i<sub_num;i++)
	{
		if(DistanceB(Sub[i])==1)
		{
			if(GetState(Sub[i])->fuel<119&&GetState(GetBaseIndex())->fuel-120+GetState(Sub[i])->fuel>0&&avail)
			{
				Supply(GetBaseIndex(),Sub[i],120-GetState(Sub[i])->fuel,40-GetState(Sub[i])->ammo,0);
				avail=0;
			}
			if(GetState(Sub[i])->fuel<110)
				uf=1;
			o=1;
		}
	}
	if(o)
		for(i=0;i<sub_num;i++)
		{
			if(DistanceB(Sub[i])>1||(DistanceB(Sub[i])==1&&GetState(Sub[i])->fuel>=119)&&DistanceB(Sub[i])<=10)
			{
				ChangeDest(Sub[i],FindSeaA(GetState(Sub[i])->pos ));
			}
		}
	if(sub_num==14&&sub_state==0&&!uf)//Change Find Sea for cargo  supply  Around Sea
	{
		for(i=0;i<sub_num;i++)
		{
			if(Distance(Base1,GetBaseIndex ())>=170)
			{
				if(i<8)
				{
					ChangeDest(Sub[i],CargoPos[i]);
				}
				else
				{
					ChangeDest (Sub[i],CarrierPos[i-8]);
				}
			}
			else if(Distance(Base1,GetBaseIndex ())>=90)
			{
				if(i<8)
				{
					ChangeDest(Sub[i],CargoPos[i]);
				}
				else
				{
					if(x)
					{
						Position p={CargoPos[i-8].x-4*x,CargoPos[i-8].y,0};
						ChangeDest(Sub[i],p);
					}
					else
					{
						Position p={CargoPos[i-8].x,CargoPos[i-8].y-2*y,0};
						ChangeDest(Sub[i],p);
					}
				}
			}
		}
		if(Distance(Base1,GetBaseIndex ())>=90)
		{
			bool s=1;
			for(i=0;i<sub_num&&i<8;i++)
			{
				if(GetState(Sub[i])->pos.x==CargoPos[i].x&&GetState(Sub[i])->pos.y==CargoPos[i].y);
				else {s=0;break;}
			}
			if(s)
			{
					sub_state=1;
					goto mark1;
			}
		}
		else
		{
			for(i=0;i<sub_num ;i++)
			{
				ChangeDest(Sub[i],InitSubPos[i]);
			}
			sub_state=3;
			//goto mark;
		}
	}
	else if(sub_state==1)
	{
		mark1:
		bool s=1;
		for(i=0;i<sub_num&&i<8 ;i++)
		{
			if(GetState(Sub[i])->fuel <=110)
			{
					s=0;
					break;
			}
		}
		if(s)
		{
			for(i=0;i<sub_num&&i<8;i++)
			{
				if(Distance(Base1,GetBaseIndex ())>=170)
				{
					if(i<6)
					{
						Position p={GetState(Sub[i])->destination.x+3,GetState(Sub[i])->destination .y,0};
						ChangeDest(Sub[i],p);
					}
					else
					{
						j=(GetState(Base1)->pos.y-GetState(GetBaseIndex ())->pos.y)/abs(GetState(Base1)->pos.y-GetState(GetBaseIndex ())->pos.y);
						Position p={GetState(Sub[i])->destination.x,GetState(Sub[i])->destination .y-j,0};
						ChangeDest(Sub[i],p);
					}
				}
				else 
				{
					Position p;
					if(GetState(Base1)->pos.y-GetState(GetBaseIndex ())->pos.y)
					{
						p.x=GetState(Sub[i])->destination.x+4*x;
						p.y=GetState(Sub[i])->destination.y;
					}
					else 
					{
						p.x=GetState(Sub[i])->destination.x;
						p.y=GetState(Sub[i])->destination.y+2*y;
					}
					ChangeDest(Sub[i],p);
				}
			}
			sub_state=2;
			if(Distance(Base1,GetBaseIndex ())>=170)
			{
				i=8;
				while(i<sub_num)
				{
					switch(i)
					{
					case 8:ChangeDest (Sub[8],CargoPos[0]);break;
					case 9:ChangeDest (Sub[9],CargoPos[2]);break;
					case 10:ChangeDest (Sub[10],CargoPos[3]);break;
					case 11:ChangeDest (Sub[11],CargoPos[5]);break;
					case 12:ChangeDest (Sub[12],CargoPos[6]);break;
					case 13:ChangeDest (Sub[13],CargoPos[7]);break;
					}
				}
			}
			else if(Distance(Base1,GetBaseIndex ())>=90)
			{
				for(i=0;i<sub_num;i++)
				{
					ChangeDest(Sub[i],CargoPos[i-8]);
				}
			}
		}
	}

	else if(sub_state==2)
	{
		cout<<"2\n";
		bool s=1;
		for(i=0;i<sub_num&&i<8 ;i++)
		{
			if(GetState(Sub[i])->fuel <=100)
			{
					s=0;
					break;
			}
		}
		if(s)
		{
			for(i=0;i<sub_num&&i<8;i++)
			{
				ChangeDest(Sub[i],InitSubPos[i]);
			}
			sub_state=3;
		}
	}
	else if(sub_state==3)
	{
		cout<<"3\n";
		int s,d=200;
		for(i=0;i<sub_num;i++)
		{
			for(j=0;j<INFO->element_num;j++)
			{
				const State*target=INFO->elements[j];
				if(target->team==1-INFO->team_num&&target->type!=BASE&&target->type!=FORT&&target->type!=FIGHTER&&target->type!=SCOUT)
					if(Distance(Sub[i],target->index)<=kProperty[SUBMARINE].fire_ranges[kProperty[target->type].level])
						{
							AttackUnit(Sub[i],target->index);
							break;
					    }
			}
			ChangeDest(Sub[i],LastSubPos[i]);
		}
		//if(Enemy.size()==0)
		//{
			//for(i=0;i<sub_num;i++)
			 //{
				
			//}
		//}
		/*for(i=0;i<Enemy.size();i++)
		{
			for(j=0;j<sub_num;j++)
			{
				if(target[j]==-1)
				{
					if(Distance(Enemy[i],Sub[j])<d)
					{
						d=Distance(Enemy[i],Sub[j]);
						s=j;
					}
				}
			}
			if(d<=5)
			{
				target[j]=Enemy[i];
			}
		}
		for(i=0;i<sub_num;i++)
		{
			if(target[i]!=-1)
			{
				AttackUnit (Sub[i],target[i]);
			}
		}
		for(i=0;i<Enemy.size();i++)
		{
			bool attack=0,su;
			for(j=0,k=sub_num;j<sub_num;j++)
			{
				if(target[j]==Enemy[i])
				{
					attack=1;
					break;
				}
			}
			if(attack==0)
			{
				su=0;
				for(j=0;j<sub_num;j++)
				{
					if(target[j]==-1)
					{
						su=1;
						break;
					}
				}
				if(su)
				{
					target[j]=-2;
					ChangeDest(Sub[j],GetState(Enemy[i])->pos);
				}
				else
				{
					for(;k>=0;)
					{
						if(target[k]!=-2)
						{
							ChangeDest(Sub[j],GetState(Enemy[i])->pos);
							k--;
							break;
						}
					}
				}

			}
		}*/
	}
	for(i=0;i<sub_num;i++)
		{
			for(j=0;j<INFO->element_num;j++)
			{
				const State*target=INFO->elements[j];
				if(target->team==1-INFO->team_num&&target->type!=BASE&&target->type!=FORT&&target->type!=FIGHTER&&target->type!=SCOUT)
					if(Distance(Sub[i],target->index)<=kProperty[SUBMARINE].fire_ranges[kProperty[target->type].level])
						{
							AttackUnit(Sub[i],target->index);
							break;
					    }
			}
		}
}
void CarrierM()
{
	cout<<"CarrierM()"<<endl;
	int i,j;
	bool o=0,uf=0;
	for(i=0;i<car_num;i++)
	{
		if(DistanceB(Carrier[i])==1)
		{
			if(GetState(Carrier[i])->fuel<199&&avail&&GetState(GetBaseIndex())->fuel>=200-GetState(Carrier[i])->fuel)
			{
					Supply(GetBaseIndex(),Carrier[i],200,0,0);
					avail=0;
			}
			if(GetState(Carrier[i])->fuel<=190)
				uf=1;
			o=1;
		}
	}
	if(o)
		for(i=0;i<car_num;i++)
		{
			if((DistanceB(Carrier[i])>1||(DistanceB(Carrier[i])==1&&GetState(Carrier[i])->fuel>=199))&&DistanceB(Carrier[i])<=10)
			{
				ChangeDest(Carrier[i],FindSeaA(GetState(Carrier[i])->pos ));
			}
		}
		cout<<car_num<<"  "<<car_state<<"  "<<uf<<endl;
	if(car_num>=6&&car_state==0&&!uf)
	{
		GoCargo=1;
		if(Distance(Base1,GetBaseIndex())>=170)
			for(i=0;i<6;i++)
			{
				ChangeDest(Carrier[i],CarrierPos [i]);
			}
		for(i=0;i<car_num;i++)
		{
			for(j=0;j<INFO->element_num ;j++)
			{
				const State* t=INFO->elements [j];
				if(t->team ==1-INFO->team_num&&t->visible &&Distance (t->index ,Carrier[i])<=8&&t->type!=FORT&&t->type!=SUBMARINE)
				{
					AttackUnit(Carrier[i],t->index);
					break;
				}
			}
		}
		car_state=1;
	}
	if(car_state==1)
	{
		bool c=1;
		if(Distance(Base1,GetBaseIndex ())>=170)
		{
			for(i=0;i<car_num ;i++)
			{
				if(GetState(Carrier[i])->fuel <=102)
				{
						c=0;
						break;
				}
			}
		}
		for(i=0;i<car_num;i++)
		{
			for(j=0;j<INFO->element_num ;j++)
			{
				const State* t=INFO->elements [j];
				if(t->team ==1-INFO->team_num&&t->visible &&Distance (t->index ,Carrier[i])<=8&&t->type!=FORT&&t->type!=SUBMARINE)
				{
					AttackUnit(Carrier[i],t->index);
					break;
				}
			}
		}
		if(c)
		{
			for(i=0;i<car_num ;i++)
			{
				ChangeDest(Carrier[i],LastCarPos[i]);
			}
			car_state=2;
		}
	}
	else if(car_state==2)
	{
		for(i=0;i<car_num;i++)
		{
			if(Distance (Carrier[i],LastCarPos[i])==0)
				AttackUnit(Carrier[i],Base1);
			else if(Distance (Carrier[i],LastCarPos[i])&&(Distance (Carrier[i],LastCarPos[i])<=8))
			{
				for(j=0;j<INFO->element_num ;j++)
				{
					if(INFO->elements [j]->pos.x==LastCarPos[i].x&&INFO->elements [j]->pos.y==LastCarPos[i].y&&INFO->elements [j]->team ==1-INFO->team_num&&INFO->elements [j]->type!=SUBMARINE )
					{
						AttackUnit (Carrier[i],INFO->elements [j]->index);
						break;
					}
				}
			}
			else
			{
				for(j=0;j<INFO->element_num ;j++)
				{
					const State* target= INFO->elements[i];
					if(target->team==1-INFO->team_num&&target->type!=SUBMARINE)
						if(Distance(Carrier[i],target->index)<=8)
						{
							AttackUnit(Carrier[i],target->index);
							break;
						}
				}
			}
		}
		bool m=1;
		for(i=0;i<car_num;i++)
		{
			if(DistanceB(Carrier [i],1-INFO->team_num )>8)
			{
				m=0;
				break;
			}
		}
		if(m)
			car_state=3;
	}
	else if(car_state==3)
	{
		for(i=0;i<car_num;i++)
		{
			AttackUnit(Carrier[i],Base1);
		}
	}

	for(i=0;i<portnum;i++)
	{
		for(j=0;j<car_num;j++)
		{
			if(GetState(Carrier[j])->destination.x==baseport[i].pos.x&&GetState(Carrier[j])->destination.y==baseport[i].pos.y)
			{
				baseport[i].occupy=1;
			}
		}
	}
}
void FighterM()
{
	cout<<"Fighter()"<<endl;
	int i,j,k,of=0,fp=0,x,y;
	if(Fighter.size()>=16)
		fly_state=1;
	if(Fighter.size()<16&&fly_state==0)
	{
		x=0;
		y=0;
		for(i=0;i<Fighter.size();i++)
		{
			if(DistanceB(GetState(Fighter[i])->pos)==0)
				of++;
		}
		for(i=0;i<INFO->production_num;i++)
		{
			if(INFO->production_list[i].unit_type==FIGHTER&&INFO->production_list[i].round_left==1)
				fp++;
		}
		if(GetState(Base1)->pos.x-GetState(GetBaseIndex())->pos.x)
		{
			x=(GetState(Base1)->pos.x-GetState(GetBaseIndex())->pos.x)/abs(GetState(Base1)->pos.x-GetState(GetBaseIndex())->pos.x);
		}
		else 
		{
			y=(GetState(Base1)->pos.y-GetState(GetBaseIndex())->pos.y)/abs(GetState(Base1)->pos.y-GetState(GetBaseIndex())->pos.y);
		}
		if(of+fp>9)
		{
			for(i=0;i<Fighter.size();i++)
			{
				Position p;
				p.z=2;
				if(fp<=3)
				{
					p.x=GetState(Fighter[i])->pos.x+x;
					p.y=GetState(Fighter[i])->pos.y+y;	
				}
				else if(fp<=6)
				{
					p.x=GetState(Fighter[i])->pos.x+x*2;
					p.y=GetState(Fighter[i])->pos.y+y*2;
				}
				else
				{
					p.x=GetState(Fighter[i])->pos.x+x*3;
					p.y=GetState(Fighter[i])->pos.y+y*3;
				}
				ChangeDest(Fighter[i],p);
			}
		}
		/*int fa=0;
		const State* target;
		for(i=0;i<INFO->element_num ;i++)
		{
			target=INFO->elements[i];
			if(target->team==1-INFO->team_num&&DistanceB(target->index)<=8)
				break;
		}
		if(i!=INFO->element_num )
		{
			for(j=0;j<Fighter.size()&&fa<2;j++)
			{
				if(DistanceB(Fighter[j])==0&&Distance(target->index,Fighter[j])<=3)
				{
					AttackUnit(Fighter[j],target->index);
					Supply(GetBaseIndex(),Fighter[j],100-GetState(Fighter[j])->fuel,21-GetState(Fighter[j])->ammo,0);
					fa++;
				}
			}
			AttackUnit(GetBaseIndex(),target->index);
			for(j=0;j<INFO->element_num;j++)
			{
				if(INFO->elements[j]->type==FORT&&INFO->elements[j]->team==INFO->team_num&&Distance(target->index,INFO->elements[j]->index)<=8)
				{
					AttackUnit(INFO->elements[j]->index,target->index);
				}
			}
		}*/
	}
	if(fly_state==1)
	{
		for(i=0;i<Fighter.size();i++)
		{
			ChangeDest(Fighter[i],FighterPos[i]);
		}
	}
	for(i=0;i<Fighter.size();i++)
	{
		if(GetState(Fighter[i])->ammo<=4) continue;
		for(j=0;j<INFO->element_num;j++)
		{
			const State* target=INFO->elements[j];
			if(target->team==1-INFO->team_num&&target->type!=FORT&&(Distance(Fighter[i],target->index)<=kProperty[FIGHTER].fire_ranges[kProperty[target->type].level]))
			{
				AttackUnit(Fighter[i],target->index);
			}
		}
	}
	for(i=0;i<Fighter.size();i++)
	{
		if(DistanceB(Fighter[i],1-INFO->team_num)<=3)
			AttackUnit(Fighter[i],Base1);
	}
	/*if(fly_state==1)
	{
		for(i=0;i<Fighter.size();i++)
		{
			ChangeDest(Fighter[i],FighterPos[i]);
			if(DistanceB(Fighter[i],1-INFO->team_num)<=3)
				AttackUnit(Fighter[i],Base1);
		}
		int f[3];
		for(i=0;i<INFO->element_num;i++)
		{
			const State* enemy=INFO->elements[i];
			for(j=0;j<Fighter.size();j++)
			{
				if(enemy->team==1-INFO->team_num&&FighterPos[j].x==enemy->pos.x&&FighterPos[j].y==enemy->pos.y&&FighterPos[j].z==enemy->pos.z)
				{
					f[0]=f[1]=f[2]=-1;
					vector<int> pref;
					for(k=0;k<Fighter.size();k++)
					{
						if(Distance(Fighter[k],FighterPos[j])<=3)
							pref.push_back(k);
					}
					if(pref.size())
					{
						int m,n;
						for(m=0;m<pref.size();m++)
							for(n=0;n<pref.size()-1-m;n++)
							{
								if(Distance(Fighter[pref[n+1]],FighterPos[j])<Distance(Fighter[pref[n]],FighterPos[j]))
								{
									int temp;
									temp=pref[n];
									pref[n]=pref[n+1];
									pref[n+1]=temp;
								}
							}
						for(k=0;k<3&&k<pref.size();k++)
						{
							f[k]=pref[k];
						}
					}
					for(k=0;k<3;k++)
					{
						if(f[k]!=-1)
						{
							AttackUnit(f[k],enemy->index);
						}
					}
				}
			}
		}
	}*/
}
//Bad Roasted Ducks
void CargoSupply()
{
	int i,j,k;
	avail=1;
	if(GoCargo==0||b2b<=100)
	{
		for(i=0;i<fort.size();i++)
		{
			if(fort[i].metal>=50||fort[i].fuel>=150)
				tempft.push_back(OnePort(fort[i].pos));
		}
		for(i=0;i<oilfield.size();i++)
		{
			if(oilfield[i].fuel>=150)
				fPorts(i);
		}
		for(i=0;i<mine.size();i++)
		{
			if(mine[i].metal>=50)
				mPorts(i);
		}
	
		for(i=0;i<tempfl.size();i++)//where to define them?
			for(j=i+1;j<tempfl.size();j++)
			{
				if(DistanceB(tempfl[j])<DistanceB(tempfl[i]))
				{
					Position p=tempfl[i]; tempfl[i]=tempfl[j]; tempfl[j]=p;
				}
			}
        for(i=0;i<tempm.size();i++)
			for(j=i+1;j<tempm.size();j++)
			{
				if(DistanceB(tempm[j])<DistanceB(tempm[i]))
				{
					Position p=tempm[i]; tempm[i]=tempm[j]; tempm[j]=p;
				}
			}
			cout<<tempfl.size()<<"  "<<cargonum<<"   ";
			cout<<tempm.size();
			int abc=cargonum-3;
		if(tempfl.size()>abc) tempfl.erase(tempfl.begin()+abc,tempfl.end());
		if(tempm.size()>3) tempm.erase(tempm.begin()+3,tempm.end());

		for(i=0;i<tempft.size();i++)
		{
			for(j=0;j<tempm.size();j++)
			{
				if(DistanceB(tempft[i])/11<DistanceB(tempm[j])/11)
				{
					tempm.pop_back();
					tempm.push_back(tempft[i]);
					if (tempm.size()>=3&&DistanceB(tempm[1])>DistanceB(tempm[2]))
					{
						Position p=tempm[1]; tempm[1]=tempm[2]; tempm[2]=p;
					}
					if (DistanceB(tempm[0])>DistanceB(tempm[1]))
					{
						Position p=tempm[0]; tempm[0]=tempm[1]; tempm[1]=p;
					}
					break;
				}
			}
		}
		for(i=0;i<tempft.size();i++)
		{
			for(j=0;j<tempfl.size();j++)
			{
				if(tempft[i].x==tempfl[j].x&&tempft[i].y==tempfl[j].y) break;
			}
			if(j<tempfl.size()) continue;
			for(j=0;j<tempfl.size();j++)
			{
				if(DistanceB(tempft[i])/11<DistanceB(tempfl[j])/11)
				{
					tempfl.pop_back();
					tempfl.push_back(tempft[i]);
					if (tempfl.size()>=6)
					{
						if(DistanceB(tempfl[4])>DistanceB(tempfl[5]))
						{
						Position p=tempfl[4]; tempfl[4]=tempfl[5]; tempfl[5]=p;
						}
					}
					if (tempfl.size()>=5)
					{
						if(DistanceB(tempfl[3])>DistanceB(tempfl[4]))
						{
						Position p=tempfl[3]; tempfl[3]=tempfl[4]; tempfl[4]=p;
						}
					}
					if (tempfl.size()>=4)
					{
						if(DistanceB(tempfl[2])>DistanceB(tempfl[3]))
						{
						Position p=tempfl[2]; tempfl[2]=tempfl[3]; tempfl[3]=p;
						}
					}
					if (tempfl.size()>=3)
					{
						if(DistanceB(tempfl[1])>DistanceB(tempfl[2]))
						{
						Position p=tempfl[1]; tempfl[1]=tempfl[2]; tempfl[2]=p;
						}
					}
					if (tempfl.size()>=2)
					{
						if(DistanceB(tempfl[0])>DistanceB(tempfl[1]))
						{
						Position p=tempfl[0]; tempfl[0]=tempfl[1]; tempfl[1]=p;
						}
					}
					break;
				}
			}
		}
		for(i=0;i<tempm.size();i++)
		{
			for(j=0;j<portnum;j++)
			{
				if(tempm[i].x==baseport[j].pos.x&&tempm[i].y==baseport[j].pos.y)
				{
					baseport[j].occupy=1;
					break;
				}
			}
		}
		for(i=0;i<tempfl.size();i++)
		{
			for(j=0;j<portnum;j++)
			{
				if(tempfl[i].x==baseport[j].pos.x&&tempfl[i].y==baseport[j].pos.y)
				{
					baseport[j].occupy=1;
					break;
				}
			}
		}
    
		oilport=tempfl;
		mineport=tempm;
		int m=0;
		int n=0;
		for(k=0;k<6;k++) BaseFuelF[k]=GetState(GetBaseIndex())->fuel;
		for(i=0;i<cargonum;i++)
		{
			if(cargo[i].index!=-1)
			{
				if(DistanceB(cargo[i].index)==1)
				{
					int o=oilport.size();
					int m=mineport.size();
					if(ps>=14&&INFO->elements[GetBase()]->metal>(30*(6-pc)))
					{
						if(cargo[i].type==0)
						{
						    cargo[i].dest=oilport[i%o];
						}
						else
						{
							cargo[i].dest=oilport[i%o];
							if((GetState(cargo[i].index)->fuel-Distance(cargo[i].index,cargo[i].dest))<15)
							{
								if(avail==1)
								{
							        Supply(GetBaseIndex(),cargo[i].index,max(0,GetState(cargo[i].index)->fuel-Distance(cargo[i].index,cargo[i].dest)+15),0,0);
									cargo[i].type=0;
								}
								else
								{
							        cargo[i].dest=GetState(cargo[i].index)->pos;
							        baseport[GetBP(cargo[i].dest)].occupy=1;
								}
							}
						}
					}
					else
					{
						if(i==0||i==1)
						{
							cargo[i].dest=oilport[i%o]; cargo[i].type=0;
						}
						else if(i==2||i==3)
						{
							cargo[i].dest=mineport[(i-2)%m]; cargo[i].type=1;
						}
						else if(i==4)
						{
							cargo[i].dest=oilport[(i-2)%o];cargo[i].type=0;
						}
						else if(i==5)
						{
							cargo[i].dest=mineport[(i-3)%m]; cargo[i].type=1;
						}
						else
						{
							cargo[i].dest=oilport[(i-3)%o]; cargo[i].type=0;
						}
					}


					if(cargo[i].type==1) cargo[i].roundleft=1000; 
					else  cargo[i].roundleft=Distance(cargo[i].index,cargo[i].dest)/11*2+4;
					
					if(cargo[i].type==0&& GetState(cargo[i].index)->fuel-2*Distance(cargo[i].index,cargo[i].dest)-15>0)
	                    Supply(cargo[i].index,GetBaseIndex(),GetState(cargo[i].index)->fuel-2*Distance(cargo[i].index,cargo[i].dest)-15,0,GetState(cargo[i].index)->metal);
					else if(cargo[i].type==0&& GetState(cargo[i].index)->fuel-2*Distance(cargo[i].index,cargo[i].dest)-15<0)
					{
						if(avail==1)
						{
							Supply(GetBaseIndex(),cargo[i].index,2*Distance(cargo[i].index,cargo[i].dest)+15-GetState(cargo[i].index)->fuel,0,0);
							avail=0;
						}
						else
						{
							cargo[i].dest=GetState(cargo[i].index)->pos;
							baseport[GetBP(cargo[i].dest)].occupy=1;
						}
					}
					else if(cargo[i].type==1)
					{
						if(GetState(cargo[i].index)->fuel<2*Distance(cargo[i].index,cargo[i].dest)+50)					
						{
							if(avail==1)
							{	Supply(GetBaseIndex(),cargo[i].index,2*Distance(cargo[i].index,cargo[i].dest)+50-GetState(cargo[i].index)->fuel,0,0);
							    avail=0;
							}
							else
							{
								cargo[i].dest=GetState(cargo[i].index)->pos;
								baseport[GetBP(cargo[i].dest)].occupy=1;
							}
							Supply(cargo[i].index,GetBaseIndex(),0,0,GetState(cargo[i].index)->metal);
						}
						else 
						{
							Supply(cargo[i].index,GetBaseIndex(),GetState(cargo[i].index)->fuel-2*Distance(cargo[i].index,cargo[i].dest)-50,0,GetState(cargo[i].index)->metal);
							cargo[i].type=1;
						}
					}
				}
				else if(Distance(cargo[i].index,cargo[i].dest)==0||(cargo[i].dest.x==GetState(GetBaseIndex())->pos.x&&cargo[i].dest.y==GetState(GetBaseIndex())->pos.y))
				{
					int p=NearPort(cargo[i].index);
					if(p==-1) cargo[i].dest=GetState(GetBaseIndex())->pos;
					else if(Distance(cargo[i].index,baseport[p].pos)<=11)  
					{
						cargo[i].dest=baseport[p].pos;
						baseport[p].occupy=1;
					}
					else
						cargo[i].dest=GetState(GetBaseIndex())->pos;
					cargo[i].roundleft--;
				}
				else cargo[i].roundleft--;

				ChangeDest(cargo[i].index,cargo[i].dest);

				BaseMetal=GetState(GetBaseIndex())->metal;
				
				for(k=1;k<6;k++)
				{
					if(cargo[i].roundleft<=k) BaseFuelF[k]+=300-2*DistanceB(oilport[4%(oilport.size())])-15;
				}
		    }
		}
    }
	else if(b2b>100)
	{
			k=0;
		if(sfieldinit==0)
		{
			for(i=0;i<oilfield.size();i++) sfield.push_back(oilfield[i]);
			for(i=0;i<fort.size();i++)
			{
				if(fort[i].fuel>0) 
				{
					Oilfield o={fort[i].fuel,fort[i].index,fort[i].pos};
					sfield.push_back(o);
				}
			}
			if(INFO->elements[GetBase()]->fuel>0)
			{
				Oilfield o={GetState(GetBaseIndex())->fuel,GetBaseIndex(),BasePos};
				sfield.push_back(o);
			}
			sfieldinit=1;
			for(i=0;i<8;i++)
			{
				if(cargo[i].index==-1) continue;
				for(j=i+1;j<8;j++)
				{
					if(cargo[j].index==-1) continue;
					if(GetState(cargo[i].index)->fuel>GetState(cargo[j].index)->fuel)
					{
						Cargo c=cargo[i]; cargo[i]=cargo[j]; cargo[j]=c;
					}
				}
			}
		}
		

		for(i=0;i<8;i++)
		{
			if(cargo[i].index!=-1)
			{
				for(j=0;j<baseport.size();j++)
				{
					if(((cargo[i].dest.x==baseport[j].pos.x&&cargo[i].dest.y==baseport[j].pos.y)||cargo[i].dest.x==BasePos.x&&cargo[i].dest.y==BasePos.y)&&GetState(cargo[i].index)->fuel>=(2*(b2b-100)+Distance(cargo[i].index,CargoPos[i])))
					{
						cargo[i].dest=CargoPos[i];
						cargo[i].supply=1;
						break;
					}
				}
			}
		}
		for(i=0;i<8;i++)
		{
			if(cargo[i].index!=-1&&cargo[i].supply==0&&findsfield[i]==0)
			{
			
				
					
						vector <Oilfield> tempfield;
						tempfield=sfield;
						/*for(j=0;j<tempfield.size();j++)
							for(k=j+1;k<tempfield.size();k++)
							{
								if(Distance(cargo[i].index,tempfield[j].pos)>Distance(cargo[i].index,tempfield[k].pos))
								{
									Oilfield o=tempfield[j];tempfield[j]=tempfield[k];tempfield[k]=o;
								}
							}*/
						int Mindistance=1000;
						int minj=-1;
						for(j=0;j<tempfield.size();j++)
						{
							if(((b2b<170&&tempfield[j].fuel-Distance(CargoPos[i],tempfield[j].pos)>=2*(b2b-100))||(b2b>=170&&tempfield[j].fuel-Distance(CargoPos[i],tempfield[j].pos)>=2*(b2b-90)))&&(GetState(cargo[i].index)->fuel-10)>Distance(cargo[i].index,tempfield[j].pos)&&Distance(tempfield[j].pos,CargoPos[i])<Mindistance)
							{
								Mindistance=Distance(tempfield[j].pos,CargoPos[i]);
								minj=j;
							}
						}
						if(minj==-1) 
						{
							cargo[i].index=-1;
							minj=tempfield.size()-1;
						}
						cargo[i].dest=tempfield[minj].pos;
						for(j=0;j<sfield.size();j++)
						{
							if(tempfield[minj].pos.x==sfield[j].pos.x&&tempfield[minj].pos.y==sfield[j].pos.y&&sfield[j].index!=GetBaseIndex())
								sfield[j].fuel-=300-(GetState(cargo[i].index)->fuel-Distance(cargo[i].index,sfield[j].pos));
							else if(tempfield[minj].pos.x==sfield[j].pos.x&&tempfield[minj].pos.y==sfield[j].pos.y&&sfield[j].index!=GetBaseIndex())
								sfield[j].fuel-=2*(b2b-90)+Distance(cargo[i].index,CargoPos[i]);
						}
						findsfield[i]=1;
									
			    }
		}
		
		for(i=0;i<8;i++)
		{
			if(cargo[i].index!=-1)
			{
				if(findsfield[i]==1&&GetState(cargo[i].index)->fuel>=Distance(CargoPos[i],GetState(cargo[i].index)->pos)+2*(b2b-100))
					{
						cargo[i].dest=CargoPos[i];
						cargo[i].supply=1;
					}
				}

				
				ChangeDest(cargo[i].index,cargo[i].dest);


				int can1=1,can4=1;
				if(cargo[i].supply==1&&(!Distance(cargo[i].index,CargoPos[i])))
				{
					for(k=0;k<INFO->element_num;k++)
					{
						const State* element=INFO->elements[k];
						if(Distance(cargo[i].index,element->index)<=1&&element->team==INFO->team_num&&(element->type==CARRIER||element->type==SUBMARINE))
						{
							if(b2b>=100&&element->type==SUBMARINE&&Distance(cargo[i].index,element->index)==0) 
							{
								Supply(cargo[i].index,element->index, 120-element->fuel, 20, 0);
								if(i==1) can1=0; if(i==4) can4=0;
							}
							if(b2b>=170&&element->type==CARRIER) 
							{
								Supply(cargo[i].index,element->index,20,70,0);
								if(i==1) can1=0; if(i==4) can4=0;
							}
						}
					}

					if(i==1&&Distance(cargo[i].index,cargo[i].dest)==0&&can1==1)
					{
						if(can1&&if_alive(cargo[0].index)&&Distance(cargo[1].index,cargo[0].index)==1&&GetState(cargo[0].index)->fuel<30&&GetState(cargo[1].index)->fuel>=30)
					    {
							Supply(cargo[1].index,cargo[0].index,30-GetState(cargo[0].index)->fuel,0,0);
							can1=0;
						}
						if(can1&&if_alive(cargo[2].index)&&Distance(cargo[1].index,cargo[2].index)==1&&GetState(cargo[2].index)->fuel<30&&GetState(cargo[1].index)->fuel>=30)
						{
							Supply(cargo[1].index,cargo[2].index,30-GetState(cargo[2].index)->fuel,0,0);
							can1=0;
						}
					}
					if(i==4&&Distance(cargo[i].index,cargo[i].dest)==0&&can4==1)
					{
						if(can4&&if_alive(cargo[3].index)&&Distance(cargo[4].index,cargo[3].index)==1&&GetState(cargo[3].index)->fuel<30&&GetState(cargo[4].index)->fuel>=30)
						{
							Supply(cargo[4].index,cargo[3].index,30-GetState(cargo[3].index)->fuel,0,0);
							can4=0;
						}
						if(can4&&if_alive(cargo[5].index)&&Distance(cargo[4].index,cargo[5].index)==1&&GetState(cargo[5].index)->fuel<30&&GetState(cargo[4].index)->fuel>=30)
						{
							Supply(cargo[4].index,cargo[5].index,30-GetState(cargo[5].index)->fuel,0,0);
							can4=0;
						}
					}
				}
			}
		}


	for(i=0;i<INFO->element_num;i++)
	{
		const State* element= INFO->elements[i];
		if(element->type==FORT&&element->team==INFO->team_num)
		{
			for(j=0;j<INFO->element_num&&j!=i;j++)
			{
				const State* target= INFO->elements[j];
				if(Distance(element->index,target->index)<=1&&target->team==INFO->team_num&&target->type!=BASE&&target->type!=CARGO)
					Supply(element->index,target->index,kProperty[target->type].fuel_max-target->fuel,kProperty[target->type].ammo_max-target->ammo,0);
				else if(Distance(element->index,target->index)<=1&&target->team==INFO->team_num&&target->type==BASE) 
					Supply(element->index,target->index,200,0,200);
				else if(Distance(element->index,target->index)<=1&&target->team==INFO->team_num&&target->type==CARGO) 
				{
					Supply(element->index,target->index,200,0,50);
					if(DistanceB(target->index)==1&&((target->fuel+element->fuel>=250)&&(target->metal+Min(element->metal,50)>=50))&&target->fuel<200)
					{
						int p=NearPort(target->index);
						if(p!=-1) ChangeDest(target->index,baseport[p].pos);
						baseport[p].occupy=1;
					}
				}
			}
		}
	}
	if(avail==1)
	{
		for(i=0;i<cargonum;i++)
		{
			if(cargo[i].index!=-1)
			{
				if(cargo[i].supply==0&&findsfield[i]==1&&cargo[i].dest.x==BasePos.x&&cargo[i].dest.y==BasePos.y)
				{
					Supply(GetBaseIndex(),cargo[i].index,Distance(cargo[i].index,CargoPos[i])+2*(b2b-90),120,0);
					avail=0;
				}
			}
		}
	}
	int l;
	for(l=0;l<INFO->production_num;l++)
		{
			switch(INFO->production_list[l].unit_type)
			{
			case SUBMARINE:for(i=INFO->production_list[l].round_left;i<6;i++)BaseFuelF[i]-=120;break;
			case CARRIER:for(i=INFO->production_list[l].round_left;i<6;i++)BaseFuelF[i]-=200;break;
			case CARGO:for(i=INFO->production_list[l].round_left;i<6;i++)BaseFuelF[i]-=150;break;
			}
		}
		for(k=0;k<6;k++)
		{
			BaseFuelF[k]=Min(BaseFuelF[k],1000);
		}
		
}
void FindBasePort()
{
	portnum=0;
    int p=GetBase();
	int x=INFO->elements[p]->pos.x,y=INFO->elements[p]->pos.y;
	for(int i=-1;i<2;i++)
	{
		if(Map(x+i,y+2)==OCEAN)
		{
			Position p={x+i,y+2,1};
			BasePort b={0,p};
			baseport.push_back(b);
			portnum++;
		}
		if(Map(x+i,y-2)==OCEAN)
		{
			Position p={x+i,y-2,1};
			BasePort b={0,p};
			baseport.push_back(b);
			portnum++;
		}
		if(Map(x+2,y+i)==OCEAN)
		{
			Position p={x+2,y+i,1};
			BasePort b={0,p};
			baseport.push_back(b);
			portnum++;
		}
		if(Map(x-2,y+i)==OCEAN)
		{
			Position p={x-2,y+i,1};
			BasePort b={0,p};
			baseport.push_back(b);
			portnum++;
		}
	}
}
Position OnePort(Position pos)
{
	int i,j;
	vector <Position> ports;
	if(Map(pos.x-1,pos.y)==OCEAN)
	{
		Position p={pos.x-1,pos.y,1};
		ports.push_back(p);
	}
	if(Map(pos.x+1,pos.y)==OCEAN)
	{
		Position p={pos.x+1,pos.y,1};
		ports.push_back(p);
	}
	if(Map(pos.x,pos.y-1)==OCEAN)
	{
		Position p={pos.x,pos.y-1,1};
		ports.push_back(p);
	}
	if(Map(pos.x,pos.y+1)==OCEAN)
	{
		Position p={pos.x-1,pos.y,1};
		ports.push_back(p);
	}
		int Min=1000;
		Position p1;
		for(i=0;i<ports.size();i++)
		{
			if(DistanceB(ports[i])<Min) 
			{
				Min=DistanceB(ports[i]);
				p1=ports[i];
			}
		}
		return p1;
}
void  fPorts(int f)
{
	int n=(oilfield[f].fuel+150)/300;
	int k=0;
	if(Distance(GetBaseIndex(1-INFO->team_num),oilfield[f].pos)<30&&(Distance(GetBaseIndex(),oilfield[f].pos)>Distance(GetBaseIndex(1-INFO->team_num),oilfield[f].pos))) return;
	if(Map(oilfield[f].pos.x-1,oilfield[f].pos.y)==OCEAN) 
	{
		Position p={oilfield[f].pos.x-1,oilfield[f].pos.y,1};
		tempfl.push_back(p);
		k++;
	}
	if(k==n) return;
	if(Map(oilfield[f].pos.x+1,oilfield[f].pos.y)==OCEAN) 
	{
		Position p={oilfield[f].pos.x+1,oilfield[f].pos.y,1};
		tempfl.push_back(p);
		k++;
	}
	if(k==n) return;
	if(Map(oilfield[f].pos.x,oilfield[f].pos.y-1)==OCEAN) 
	{
		Position p={oilfield[f].pos.x,oilfield[f].pos.y-1,1};
		tempfl.push_back(p);
		k++;
	}
	if(k==n) return;
	if(Map(oilfield[f].pos.x,oilfield[f].pos.y+1)==OCEAN) 
	{
		Position p={oilfield[f].pos.x,oilfield[f].pos.y+1,1};
		tempfl.push_back(p);
		k++;
	}
	if(k==n) return;
}
void  mPorts(int m)
{
	int n=mine[m].metal/50;
	if(n==0) return;
	if(Distance(GetBaseIndex(1-INFO->team_num),mine[m].pos)<30&&(Distance(GetBaseIndex(),mine[m].pos)>Distance(GetBaseIndex(1-INFO->team_num),mine[m].pos))) return;
	int k=0;
	if(Map(mine[m].pos.x-1,mine[m].pos.y)==OCEAN) 
	{
		Position p={mine[m].pos.x-1,mine[m].pos.y,1};
		tempm.push_back(p);
		k++;
	}
	if(k==n) return;
	if(Map(mine[m].pos.x+1,mine[m].pos.y)==OCEAN) 
	{
		Position p={mine[m].pos.x+1,mine[m].pos.y,1};
		tempm.push_back(p);
		k++;
	}
	if(k==n) return;
	if(Map(mine[m].pos.x,mine[m].pos.y-1)==OCEAN) 
	{
		Position p={mine[m].pos.x,mine[m].pos.y-1,1};
		tempm.push_back(p);
		k++;
	}
	if(k==n) return;
	if(Map(mine[m].pos.x,mine[m].pos.y+1)==OCEAN) 
	{
		Position p={mine[m].pos.x,mine[m].pos.y+1,1};
		tempm.push_back(p);
		k++;
	}
	if(k==n) return;
}
int NearPort(int index)
{
	int i,j;
	vector <BasePort> temp;
	for(i=0;i<baseport.size();i++) temp.push_back(baseport[i]);
	for(i=0;i<portnum;i++)
		for(j=i+1;j<portnum;j++)
		{
			if(Distance(index,temp[i].pos)>Distance(index,temp[j].pos))
			{
				BasePort p =temp[i]; temp[i]=temp[j]; temp[j]=p;
			}
		}

		for(i=0;i<temp.size();i++)
		{
			int flag=1;
			if(temp[i].occupy==0) 
			{
				for(j=0;j<cargonum;j++)
				{
					if(cargo[j].index==index) continue;
					if(cargo[j].index!=-1)
					{
						if((cargo[j].dest.x==temp[i].pos.x)&&(cargo[j].dest.y==temp[i].pos.y))
						{
							flag=0;
							break;
						}
					}
				}
				
				if(flag==1) return GetBP(temp[i].pos);
			}
		}
		if(i>=temp.size())
		{
			return -1;
		}
}

int GetBP(Position pos)
{
	int i;
	for(i=0;i<baseport.size();i++)
	{
		if(baseport[i].pos.x==pos.x&&baseport[i].pos.y==pos.y) return i;
	}
}
void FindAroundSea(Position pos)
{
	for(int i=-1;i<2;i++)
		for(int j=-1;j<2;j++)
		{
			if(visited[pos.x+i][pos.y+j]) continue;
			if((i*j)||(i==j)) continue;
			visited[pos.x+i][pos.y+j]=1;
			Position p={pos.x+i,pos.y+j,0};
			if(!Map(pos.x+i,pos.y+j))
			{
				LSP.push_back(p);
			}
			else
			{
				FindAroundSea(p);
			}
		}
}
void FortAttack()
{
	int i,j;
	for(i=0;i<fort.size();i++)
	{
		for(j=0;j<INFO->element_num;j++)
		{
			const State* target=INFO->elements[j];
			if(target->team==INFO->team_num||target->type==SUBMARINE||target->type==OILFIELD||target->type==MINE) continue;
			AttackUnit(fort[i].index,target->index);
			break;
		}
	}
}
