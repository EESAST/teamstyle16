#include <cstdlib>
#include "basic.h"
#include"iostream"
#include<cstdlib>
using std::abs;
// Remove this line if you konw C++, and don't want a dirty namespace
using namespace teamstyle16;

const char * GetTeamName()
{
    return "Hahn's Paradise";  // Name of your team
}

int distance(Position pos1,Position pos2)                             
{
	int a,b;
	if(pos1.x>pos2.x) a=pos1.x-pos2.x;
	else a=pos2.x-pos1.x;
	if(pos1.y>pos2.y) b=pos1.y-pos2.y;
	else b=pos2.y-pos1.y;
	return a+b;
}

int attackflag1=0,attackflag2=0;
int mybase;                                                 
int enemybase;  
int mybasex,mybasey,enemybasex,enemybasey;
int fort[20]={0};int fort_num=0; int myfort_num=0;
int mine[20]={0};int mine_num=0; 
int oilfield[20]={0};int oilfield_num=0;
int submarine[30]={0};int submarine_num=0;int mysubmarine_num=0;
int destroyer[30]={0};int destroyer_num=0; int mydestroyer_num=0;
int carrier[30]={0};int carrier_num=0; int mycarrier_num=0;
int cargo[30]={0};int cargo_num=0; int mycargo_num=0;
int fighter[30]={0};int fighter_num=0; int myfighter_num=0;
int scout[30]={0};int scout_num=0; int myscout_num=0;

void getindex()                                                                                    
{
	fort_num=0,mine_num=0,oilfield_num=0,submarine_num=0,destroyer_num=0,carrier_num=0,cargo_num=0,fighter_num=0,scout_num=0;
	myfort_num=0; mysubmarine_num=0; mydestroyer_num=0; mycarrier_num=0; mycargo_num=0; myfighter_num=0; myscout_num=0;
	for(int i=0;i<Info()->element_num;i++)
	{
		switch(Info()->elements[i]->type)
		{
		case(FORT):fort[fort_num]=Info()->elements[i]->index;fort_num++;
			if(GetState(Info()->elements[i]->index)->team==Info()->team_num) myfort_num++;
			break;
		case(MINE):
			if( (GetState(Info()->elements[i]->index)->visible==true && GetState(Info()->elements[i]->index)->metal>0   ) || ( GetState(Info()->elements[i]->index)->visible==false  ) )
				    { mine[mine_num]=Info()->elements[i]->index;mine_num++; }
			break;
		case(OILFIELD):
			if((GetState(Info()->elements[i]->index)->visible==true && GetState(Info()->elements[i]->index)->fuel>0   ) || (GetState(Info()->elements[i]->index)->visible==false) )
		        	{ oilfield[oilfield_num]=Info()->elements[i]->index;oilfield_num++; }
			break;
		case(SUBMARINE):submarine[submarine_num]=Info()->elements[i]->index;submarine_num++;
			if(GetState(Info()->elements[i]->index)->team==Info()->team_num) mysubmarine_num++;
			break;
		case(DESTROYER):destroyer[destroyer_num]=Info()->elements[i]->index;destroyer_num++;
			if(GetState(Info()->elements[i]->index)->team==Info()->team_num) mydestroyer_num++;
			break;
		case(CARRIER):carrier[carrier_num]=Info()->elements[i]->index;carrier_num++;
			if(GetState(Info()->elements[i]->index)->team==Info()->team_num) mycarrier_num++;
			break;
		case(CARGO):cargo[cargo_num]=Info()->elements[i]->index;cargo_num++;
			if(GetState(Info()->elements[i]->index)->team==Info()->team_num) mycargo_num++;
			break;
		case(FIGHTER):fighter[fighter_num]=Info()->elements[i]->index;fighter_num++;
			if(GetState(Info()->elements[i]->index)->team==Info()->team_num) myfighter_num++;
			break;
		case(SCOUT):scout[scout_num]=Info()->elements[i]->index;scout_num++;
			if(GetState(Info()->elements[i]->index)->team==Info()->team_num) myscout_num++;
			break;
		default:break;
		}
	}
}

void getbase()                                         
{
	for(int i=0;i<Info()->element_num;i++)
	{
		if(Info()->elements[i]->type==BASE)
		{
			if(Info()->elements[i]->team==Info()->team_num)
			    mybase=Info()->elements[i]->index;
			else 
			    enemybase=Info()->elements[i]->index;
			
		}
	}
}

void attacknearby(int operand)                                                      
{
	for(int i=0;i<scout_num;i++)
	{
		if(GetState(scout[i])->team==(1-Info()->team_num))
		{
			if(distance(GetState(operand)->pos,GetState(scout[i])->pos)<=kProperty[GetState(operand)->type].fire_ranges[2] && GetState(operand)->type!=SUBMARINE)
				AttackUnit(operand,scout[i]);
		}
	}
	for(int i=0;i<cargo_num;i++)
	{
		if(GetState(cargo[i])->team==(1-Info()->team_num))
		{
			if(distance(GetState(operand)->pos,GetState(cargo[i])->pos)<=kProperty[GetState(operand)->type].fire_ranges[1])
				AttackUnit(operand,cargo[i]);
		}
	}
	for(int i=0;i<destroyer_num;i++)
	{
		if(GetState(destroyer[i])->team==(1-Info()->team_num))
		{
			if(distance(GetState(operand)->pos,GetState(destroyer[i])->pos)<=kProperty[GetState(operand)->type].fire_ranges[1])
				AttackUnit(operand,destroyer[i]);
		}
	}
	for(int i=0;i<submarine_num;i++)
	{
		if(GetState(submarine[i])->team==(1-Info()->team_num))
		{
			if(distance(GetState(operand)->pos,GetState(submarine[i])->pos)<=kProperty[GetState(operand)->type].fire_ranges[0] && GetState(operand)->type!=CARRIER)
				AttackUnit(operand,submarine[i]);
		}
	}
	for(int i=0;i<carrier_num;i++)
	{
		if(GetState(carrier[i])->team==(1-Info()->team_num))
		{
			if(distance(GetState(operand)->pos,GetState(carrier[i])->pos)<=kProperty[GetState(operand)->type].fire_ranges[1])
				AttackUnit(operand,carrier[i]);
		}
	}
	for(int i=0;i<fighter_num;i++)
	{
		if(GetState(fighter[i])->team==(1-Info()->team_num))
		{
			if(distance(GetState(operand)->pos,GetState(fighter[i])->pos)<=kProperty[GetState(operand)->type].fire_ranges[2] && GetState(operand)->type!=SUBMARINE)
				AttackUnit(operand,fighter[i]);
		}
	}

	if(distance(GetState(enemybase)->pos,GetState(operand)->pos)<kProperty[GetState(operand)->type].fire_ranges[1]-1 && GetState(operand)->type!=SUBMARINE)
		AttackPos(operand,GetState(enemybase)->pos);
}

void attackcargo(int operand)
{
	int target=0,mindistance=200;
	for(int i=0;i<cargo_num;i++)
	{
		if(distance(GetState(cargo[i])->pos,GetState(operand)->pos)<mindistance && distance(GetState(cargo[i])->pos,GetState(operand)->pos)<kProperty[GetState(operand)->type].fire_ranges[1] && GetState(cargo[i])->team==1-Info()->team_num)
		{ target=cargo[i]; mindistance=distance(GetState(cargo[i])->pos,GetState(operand)->pos); }
	}
	if(mindistance!=200)
	{ ChangeDest(operand,GetState(target)->pos); AttackUnit(operand,target); }
}

void goforsupply(int operand)                                   
{
	
	
	int target=mybase,distance_min=distance(GetState(operand)->pos,GetState(mybase)->pos);
	for(int i=0;i<cargo_num;i++)
	{
		if((GetState(cargo[i])->team==Info()->team_num)&&(distance(GetState(cargo[i])->pos,GetState(operand)->pos)<distance_min)&&(GetState(operand)->type!=FIGHTER))
		{
			target=cargo[i];
			distance_min=distance(GetState(cargo[i])->pos,GetState(operand)->pos);
		}
	}
	
	for(int i=0;i<carrier_num;i++)
	{
		if((GetState(carrier[i])->team==Info()->team_num)&&(distance(GetState(carrier[i])->pos,GetState(operand)->pos)<distance_min))
		{
			target=carrier[i];
			distance_min=distance(GetState(carrier[i])->pos,GetState(operand)->pos);
		}
	

	for(int i=0;i<carrier_num;i++)
	{
		if(GetState(carrier[i])->team==Info()->team_num && distance(GetState(carrier[i])->pos,GetState(operand)->pos)<distance_min && GetState(operand)->type!=CARRIER)
		{
			target=carrier[i];
			distance_min=distance(GetState(carrier[i])->pos,GetState(operand)->pos);
		}
	}

	if((GetState(operand)->ammo<kProperty[GetState(operand)->type].ammo_once*3 && GetState(operand)->fuel>distance_min+10) && GetState(operand)->health>kProperty[GetState(operand)->type].health_max/3)
	{ ChangeDest(operand,GetState(target)->pos);  attacknearby(operand); }
	}
}

void goforerpair(int operand)                                      
{
	if(GetState(operand)->fuel>(distance(GetState(mybase)->pos,GetState(operand)->pos)+20) && GetState(operand)->health<15)
	        ChangeDest(operand,GetState(mybase)->pos);
}

void providesupply(int operand)                                  
{
	Position operandpos=GetState(operand)->pos;
	
	for(int i=0;i<destroyer_num;i++)
		if(distance(operandpos,GetState(destroyer[i])->pos)==1&&(GetState(destroyer[i])->team==Info()->team_num))
		{
			if(GetState(operand)->type!=CARRIER)
			    Supply(operand,destroyer[i],110,kProperty[DESTROYER].ammo_max,0);
			else Supply(operand,destroyer[i],GetState(operand)->fuel/2,GetState(operand)->ammo/3,0);
		}
		
	for(int i=0;i<submarine_num;i++)
		if(distance(operandpos,GetState(submarine[i])->pos)<=1&&(GetState(submarine[i])->team==Info()->team_num))
		{
			if(GetState(operand)->type!=CARRIER)
			   Supply(operand,submarine[i],kProperty[SUBMARINE].fuel_max,kProperty[SUBMARINE].ammo_max,0);
			else
				Supply(operand,submarine[i],GetState(operand)->fuel/3,GetState(operand)->ammo/3,0);
		}

	for(int i=0;i<carrier_num;i++)
		if((distance(operandpos,GetState(carrier[i])->pos)==1)&&(GetState(carrier[i])->team==Info()->team_num))
		{
			if(GetState(operand)->type!=CARRIER)
			    Supply(operand,carrier[i],170,kProperty[CARRIER].ammo_max,0);
			else 
				Supply(operand,carrier[i],GetState(operand)->fuel/2,GetState(operand)->ammo/2,0);
		}
	
	for(int i=0;i<fighter_num;i++)
		if(distance(operandpos,GetState(fighter[i])->pos)==0&&(GetState(fighter[i])->team==Info()->team_num) && GetState(operand)->type!=CARGO)
		{
			if(GetState(operand)->type!=CARRIER)
			    Supply(operand,fighter[i],kProperty[FIGHTER].fuel_max,kProperty[FIGHTER].ammo_max,0);
			else 
				Supply(operand,fighter[i],GetState(operand)->fuel*2/5,GetState(operand)->ammo*2/5,0);
		}

	for(int i=0;i<cargo_num;i++)
		if(distance(operandpos,GetState(cargo[i])->pos)<=1 && GetState(cargo[i])->team==Info()->team_num  && GetState(operand)->type!=CARGO)
			Supply(operand,cargo[i],25,0,GetState(operand)->metal);
}

void providerepair()                         
{
	int target=0;
	for(int i=0;i<submarine_num;i++)
	{
		if(distance(GetState(mybase)->pos,GetState(submarine[i])->pos)==1 && GetState(submarine[i])->health<10)
			target=submarine[i];
	}
	for(int i=0;i<carrier_num;i++)
	{
		if(distance(GetState(mybase)->pos,GetState(carrier[i])->pos)==1 && GetState(carrier[i])->health<15)
			target=carrier[i];
	}
	for(int i=0;i<cargo_num;i++)
	{
		if(distance(GetState(mybase)->pos,GetState(cargo[i])->pos)==1 && GetState(cargo[i])->health<10)
			target=cargo[i];
	}
	for(int i=0;i<fighter_num;i++)
	{
		if(distance(GetState(mybase)->pos,GetState(fighter[i])->pos)==1 && GetState(fighter[i])->health<10)
			target=fighter[i];
	}
	Fix(mybase,target);
}

void sortmine(int *mine,int mine_num)                  
{
	int temp=0;
	for(int i=0;i<mine_num;i++)
	{
		for(int j=i+1;j<mine_num;j++)
		{
			if(distance(GetState(mine[i])->pos,GetState(mybase)->pos)>distance(GetState(mine[j])->pos,GetState(mybase)->pos))
				temp=mine[i],mine[i]=mine[j],mine[j]=temp;
		}
	}
}

void sortoilfield(int *oilfield,int oilfield_num)         
{
	int temp=0;
	for(int i=0;i<oilfield_num;i++)
	{
		for(int j=i+1;j<oilfield_num;j++)
		{
			if(distance(GetState(oilfield[i])->pos,GetState(mybase)->pos)>distance(GetState(oilfield[j])->pos,GetState(mybase)->pos))
				temp=oilfield[i],oilfield[i]=oilfield[j],oilfield[j]=temp;
		}
	}
}

void sortfort(int *fort,int fort_num)          
{
	int temp=0;
	for(int i=0;i<fort_num;i++)
	{
		for(int j=i+1;j<fort_num;j++)
		{
			if(distance(GetState(fort[i])->pos,GetState(mybase)->pos)>distance(GetState(fort[j])->pos,GetState(mybase)->pos))
				temp=fort[i],fort[i]=fort[j],fort[j]=temp;
		}
	}
}


void cargogetmetal()              
{
	int mycargo=-1;

	for(int i=0;i<cargo_num;i++)
	{
		if(GetState(cargo[i])->team==Info()->team_num) 
		{
			mycargo++;
		    if(GetState(cargo[i])->metal==0)
				ChangeDest(cargo[i],GetState(mine[mycargo%4])->pos);
			else if(GetState(cargo[i])->metal==kProperty[CARGO].metal_max)
			{
				if(distance(GetState(cargo[i])->pos,GetState(mybase)->pos)>3)
				{
					ChangeDest(cargo[i],GetState(mybase)->pos);
					 Supply(cargo[i],mybase,0,0,GetState(cargo[i])->metal);
				}
				else Supply(cargo[i],mybase,0,0,GetState(cargo[i])->metal);
			}
			else Supply(cargo[i],mybase,0,0,GetState(cargo[i])->metal);
         }
	}
}
 
void cargogetfuel()               
{
	int mycargo=-1;
	for(int i=0;i<cargo_num;i++)
	{
		if(GetState(cargo[i])->team==Info()->team_num)
		{
			mycargo++;
			if(GetState(cargo[i])->fuel<kProperty[CARGO].fuel_max*2/3)
				ChangeDest(cargo[i],GetState(oilfield[mycargo%4])->pos);
			else if(GetState(cargo[i])->fuel>275)
			{
				if(distance(GetState(cargo[i])->pos,GetState(mybase)->pos)>3)
				{
					ChangeDest(cargo[i],GetState(mybase)->pos);
					Supply(cargo[i],mybase,220,0,GetState(cargo[i])->metal);
				}
				
				else Supply(cargo[i],mybase,220,0,GetState(cargo[i])->metal);
			}
			else Supply(cargo[i],mybase,220,0,GetState(cargo[i])->metal);
         }
	}
}

void cargoget(int operand)
{
	int targetmine=0,targetoilfield=0,minedistance=300,oilfielddistance=300;
	for(int i=0;i<mine_num;i++)
	{
		if(distance(GetState(operand)->pos,GetState(mine[i])->pos)<minedistance)
		{
			targetmine=mine[i];  minedistance=distance(GetState(operand)->pos,GetState(mine[i])->pos);
		}
	}

	for(int i=0;i<oilfield_num;i++)
	{
		if(distance(GetState(operand)->pos,GetState(oilfield[i])->pos)<oilfielddistance)
		{
			targetoilfield=oilfield[i]; oilfielddistance=distance(GetState(operand)->pos,GetState(oilfield[i])->pos);
		}
	}

	if(GetState(operand)->fuel<220)
		ChangeDest(operand,GetState(targetoilfield)->pos);
	else if(GetState(operand)->metal<40)
		ChangeDest(operand,GetState(targetmine)->pos);
	else
	{
		if(distance(GetState(operand)->pos,GetState(mybase)->pos)>3)
		{
			ChangeDest(operand,GetState(mybase)->pos);
			Supply(operand,mybase,GetState(operand)->fuel/5,0,GetState(operand)->metal);
		}
		else
			Supply(operand,mybase,GetState(operand)->fuel-distance(GetState(operand)->pos,GetState(targetoilfield)->pos)-distance(GetState(targetoilfield)->pos,GetState(targetmine)->pos)-10,0,GetState(operand)->metal);

	}



}


void attackfort(int operand)
{
	int target=0,distance_min=200,flag=0;
	for(int i=0;i<fort_num;i++)
	{
		if(GetState(fort[i])->team!=Info()->team_num && distance(GetState(fort[i])->pos,GetState(operand)->pos)<distance_min && (distance(GetState(fort[i])->pos,GetState(mybase)->pos)+distance(GetState(fort[i])->pos,GetState(enemybase)->pos))<160)
		{
			target=fort[i]; distance_min=distance(GetState(fort[i])->pos,GetState(operand)->pos); flag=1;
		}
	}

	if(flag)
	{
		Position temp={GetState(target)->pos.x+operand%3-1,GetState(target)->pos.y+distance_min%3-1,GetState(operand)->pos.z  };
        ChangeDest(operand,temp); AttackUnit(operand,target); attacknearby(operand);
	}
	else
	{
		ChangeDest(operand,GetState(enemybase)->pos); 
		AttackUnit(operand,enemybase);
		attacknearby(operand);
	}

	goforsupply(operand);

}

void attackcarrier(int operand)
{
	int target=0,flag=0,distance_min=200;
	for(int i=0;i<carrier_num;i++)
	{
		if(GetState(carrier[i])->team!=Info()->team_num && distance(GetState(operand)->pos,GetState(carrier[i])->pos)<distance_min)
		{
			target=carrier[i]; distance_min=distance(GetState(carrier[i])->pos,GetState(operand)->pos); flag=1;
		}
	}
	if(flag)
	{
		ChangeDest(operand,GetState(target)->pos); AttackUnit(operand,target);
	}
	else
	{
		Position temp={GetState(enemybase)->pos.x-enemybasex*(operand%3),GetState(enemybase)->pos.y-enemybasey*(distance_min%3),GetState(operand)->pos.z};
		ChangeDest(operand,temp);
		attacknearby(operand);
	}

	goforsupply(operand);

}


void AIMain()
{
	TryUpdate();
    if(Info()->round==0)
	{
		
        getbase(); 

		if(distance(GetState(mybase)->pos,GetState(enemybase)->pos)>75)
             for(int i=0;i<6;i++)
			     Produce(CARGO);
		else
		{
			for(int i=0;i<3;i++)
				Produce(FIGHTER);
			for(int i=0;i<5;i++)
				Produce(CARGO);
			
		}
		

		if(GetState(mybase)->pos.x<50) 
			mybasex=-1;
		else mybasex=1;

		if(GetState(mybase)->pos.y<50) 
			mybasey=-1;
		else mybasey=1;

		if(GetState(enemybase)->pos.x<50) 
			enemybasex=-1;
		else enemybasex=1;

		if(GetState(enemybase)->pos.y<50) 
			enemybasey=-1;
		else enemybasey=1;
	}

	getindex();
	
	

	//distance<=75: CARGO+FIGHTER

	if(distance(GetState(mybase)->pos,GetState(enemybase)->pos)<=75)
	{
		//produce

		if(Info()->round>=3)
		{
			
			if(GetState(mybase)->fuel>340 && GetState(mybase)->metal>=50 )
				for(int i=0;i<4;i++)
					Produce(FIGHTER);
			
				
		}
		getindex();

		sortmine(mine,mine_num); sortoilfield(oilfield,oilfield_num); 

		if(myfighter_num>=10)
			attackflag1=1;

		//CARGO

		for(int i=0;i<cargo_num;i++)
			cargoget(cargo[i]);


		//FIGHTER

		for(int i=0;i<fighter_num;i++)
		{
			if(attackflag1==0)
			{
				if(Info()->production_num!=0 && distance(GetState(fighter[i])->pos,GetState(mybase)->pos)<3)
				{
					Position temp={GetState(fighter[i])->pos.x*2-GetState(mybase)->pos.x,GetState(fighter[i])->pos.y*2-GetState(mybase)->pos.y,2};
				    ChangeDest(fighter[i],temp);
				}
				attacknearby(fighter[i]);

			}
			else
			{
				Position temp={GetState(enemybase)->pos.x+(i%3-1),GetState(enemybase)->pos.y+GetState(fighter[i])->fuel%3-1,2  };
				ChangeDest(fighter[i],temp);
				AttackPos(fighter[i],GetState(enemybase)->pos);
			}

		}


		//BASE

		attacknearby(mybase);

		//FORT

		for(int i=0;i<fort_num;i++)
		{
			providesupply(fort[i]);
			attacknearby(fort[i]);
		}



	}


	//distance>75 : CARGO+SUBMARINE+CARRIER

	else
	{

		//PRODUCE

		if(Info()->round>=3)
		{
			if(mysubmarine_num<8)
			{
				if(GetState(mybase)->fuel>=100 && GetState(mybase)->metal>=7)
					Produce(SUBMARINE);
			}
			else
			{
				if(GetState(mybase)->fuel>=180 && GetState(mybase)->metal>=30)
					Produce(CARRIER);

			}
		}

		getindex();
		sortmine(mine,mine_num); sortoilfield(oilfield,oilfield_num); 

		if(mysubmarine_num>3 && mycarrier_num>3)
			attackflag2=1;

		//CARGO

		if(GetState(mybase)->fuel<850)
			cargogetfuel();
		else
			cargogetmetal();

		//SUBMARINE
        for(int i=0;i<submarine_num;i++)
			if(attackflag2)
		{
		    attackcarrier(submarine[i]);
		}
		else
		{
			Position temp={GetState(mybase)->pos.x*3/4+GetState(enemybase)->pos.x/4,GetState(mybase)->pos.y*2/3+GetState(enemybase)->pos.y/3,0};
			ChangeDest(submarine[i],temp);
			attacknearby(submarine[i]);
		}

		//CARRIER

		for(int i=0;i<carrier_num;i++)
		{
			if(attackflag2)
			{
			
			if(distance(GetState(enemybase)->pos,GetState(carrier[i])->pos)>7)
			{

				Position temp={GetState(enemybase)->pos.x+i%5-2,GetState(enemybase)->pos.y-7+(abs((i-2))%5),1};
				ChangeDest(carrier[i],temp);
				attacknearby(carrier[i]);
				AttackPos(carrier[i],GetState(enemybase)->pos);
			}
			

			else
			{
				AttackPos(carrier[i],GetState(enemybase)->pos);
			}
			}
			else
			{
				Position temp={GetState(mybase)->pos.x*3/4+GetState(enemybase)->pos.x/4,GetState(mybase)->pos.y*2/3+GetState(enemybase)->pos.y/3,1};
			    ChangeDest(carrier[i],temp);
			    attacknearby(carrier[i]);

			}


		}

		//BASE

		attacknearby(mybase);

		//FORT

		for(int i=0;i<fort_num;i++)
		{
			providesupply(fort[i]);
			attacknearby(fort[i]);
		}

	}

	return;
}
