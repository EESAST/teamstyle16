/*一个上千行的代码一句注释都没有好像不太好……
 *于是我决定在文件的开头强行加入一些注释
 *就是这样……
 *    by 马栩杰
 */



#include "basic.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <algorithm>
#include <ctime>

#define POS(x) (GetState(x)->pos)
#define myTeam (INFO->team_num)
#define enemyTeam (1-INFO->team_num)

using namespace teamstyle16;
using std::vector;
using std::cout;
using std::endl;
using std::rand;


const char * GetTeamName()
{
	return "6th_destroyer_division";  // Name of your team
}
struct SRoute;
struct Army{
	Army() :time(0){}
	vector<int> carrier;
	vector<int> cargo;
	vector<int> fighter;
	vector<int> submarine;
	int time;
} army;

const GameInfo *INFO;
int state = 0;  //0 = peace; 1 = prepare; 2 = attack;
int subState = 0;
int carrierState = 0;
int boost = 0;

int myBase;
int enemyBase;

vector<int> oilfield;
vector<int> mine;
vector<int> fort;
vector<SRoute> sourceRoute;
vector<Position> baseNbr;
vector<Position> carrierAttackPos;
vector<const State *> enemyUnit;
vector<const State *> cargo;
vector<const State *> submarine;
vector<const State *> fighter;
vector<const State *> carrier;
vector<const State *> destroyer;
vector<const State *> myUnit;



int dis(Position a, Position b){
	return abs(a.x - b.x) + abs(a.y - b.y);
}

bool isAlive(int id) {
	return bool(GetState(id));
}

bool canSupply(int elem) {
	if (isAlive(elem)) {
		for (int i = 0; i != baseNbr.size(); ++i) {
			if (dis(POS(elem), baseNbr[i]) == 0)
				return true;
		}
	}
	return false;
}

bool cmpDisToBase(const int s1, const int s2) {
	if (GetState(s1) == 0 || GetState(s2) == 0) return true;
	return dis(POS(s1), POS(myBase)) < dis(POS(s2), POS(myBase));
}

bool cmpDisToBase2(const State *s1, const State *s2) {
	if (s1 == NULL || s2 == NULL) return true;
	return dis(s1->pos, POS(myBase)) < dis(s2->pos, POS(myBase));
}
bool cmpDisToBase3(Position p1, Position p2) {
	return dis(p1, POS(myBase)) < dis(p2, POS(myBase));
}

void init();

struct SRoute {
	SRoute(int sid1, int sid2);
	int id1;
	int id2;
	Position s1;
	Position s2;
	Position base;
	Position base2;
	int cargo1;
	int cargo2;
	int state1; //0= 去采集1， 1= 去采集2， 2= 返程
	int state2;
	int oilconsume;
};

SRoute::SRoute(int sid1, int sid2) :id1(sid1), id2(sid2), cargo1(-1), cargo2(-1), state1(0), state2(0){
	s1 = POS(id1);
	s2 = POS(id2);
	int minDis = 1000000;
	/*	for (int i = 0; i != baseNbr.size(); ++i){
			if (dis(baseNbr[i], s2) < minDis) {
			this->base = baseNbr[i];
			minDis = dis(baseNbr[i], s2);
			}
			}*/
	base = baseNbr[std::rand() % baseNbr.size()];
	base2 = baseNbr[std::rand() % baseNbr.size()];
	while (base2.x == base.x && base2.y == base.y && baseNbr.size() > 1) base2 = baseNbr[std::rand() % baseNbr.size()];
	oilconsume = dis(POS(id1), POS(id2)) + dis(POS(id2), base) + dis(base, base2) + 30;
}

void baseAction();
void cargoAction();
void fortAction();
void mainAction();
void evlState();
void evlStateAnother();
void baseActionAnother();
void fighterActionAnother();
void baseAction_sub();
void subAction();
void carrierAction();
void evlState_carrier();
void baseAction_carrier();
void baseActionAnotherC();
void subActionAnother();
void baseActionAnotherCC();
void fighterActionAnotherCC();
//main函数

void AIMain()
{
	int t0 = clock();
	init();
	if (dis(POS(myBase), POS(enemyBase)) > 110) {
		evlState_carrier();
		baseAction_carrier();
		fortAction();
		cargoAction();
		carrierAction();
	}
	else if (dis(POS(myBase), POS(enemyBase)) > 80) {
		evlState();
		//		baseAction();
		baseAction_sub();
		fortAction();
		cargoAction();
		subAction();
		carrierAction();
		//	mainAction();
		//	std::cout << ">>>time = " << clock() - t0 << "ms<<<" << endl;
	}
/*	else if (dis(POS(myBase), POS(enemyBase)) > 40){
		evlStateAnother();
		baseActionAnother();
		fortAction();
		cargoAction();
		fighterActionAnother();
	}*/
	else if (dis(POS(myBase), POS(enemyBase)) > 32){
		evlStateAnother();
		baseActionAnotherC();
		fortAction();
		cargoAction();
		fighterActionAnother();
		subActionAnother();
	}
	else {
		evlStateAnother();
		baseActionAnotherCC();
		fortAction();
		cargoAction();
		fighterActionAnotherCC();
	}
	return;
}

void initSourceRoute(){
	if (oilfield.empty() || mine.empty()) return;
	std::sort(oilfield.begin(), oilfield.end(), cmpDisToBase);
	std::sort(mine.begin(), mine.end(), cmpDisToBase);
	int usedo[30] = { 0 }, usedm[30] = { 0 };
	for (int i = 0; i != 8 && i != oilfield.size() && i != mine.size(); ++i){
		int oj = 0;
		while (usedo[oj]) oj++;
		usedo[oj] = 1;
		oj = oilfield[oj];
		int minDis = 1000000, mk = -1;
		for (int j = 0; j != mine.size(); ++j) {
			if (usedm[j]) continue;
			if (dis(POS(mine[j]), POS(myBase)) + dis(POS(mine[j]), POS(oj)) < minDis) {
				mk = j;
				minDis = dis(POS(mine[j]), POS(myBase)) + dis(POS(mine[j]), POS(oj)) < minDis;
			}
		}
		usedo[mk] = 1;
		mk = mine[mk];
		sourceRoute.push_back(SRoute(oj, mk));
	}
}

void initBaseNbr() {
	int i, j;
	i = -2;
	for (j = -1; j != 2; ++j) {
		Position temp = POS(myBase);
		temp.x += i;
		temp.y += j;
		if (Map(temp.x, temp.y) == OCEAN) baseNbr.push_back(temp);
	}
	i = 2;
	for (j = -1; j != 2; ++j) {
		Position temp = POS(myBase);
		temp.x += i;
		temp.y += j;
		if (Map(temp.x, temp.y) == OCEAN) baseNbr.push_back(temp);
	}
	j = -2;
	for (i = -1; i != 2; ++i) {
		Position temp = POS(myBase);
		temp.x += i;
		temp.y += j;
		if (Map(temp.x, temp.y) == OCEAN) baseNbr.push_back(temp);
	}
	j = 2;
	for (i = -1; i != 2; ++i) {
		Position temp = POS(myBase);
		temp.x += i;
		temp.y += j;
		if (Map(temp.x, temp.y) == OCEAN) baseNbr.push_back(temp);
	}
}

void init() {
	TryUpdate();
	INFO = Info();
	enemyUnit.clear();
	cargo.clear();
	submarine.clear();
	fighter.clear();
	carrier.clear();
	destroyer.clear();
	myUnit.clear();
	if (INFO->round == 0) {
		for (int i = 0; i != INFO->element_num; i++) {
			if (INFO->elements[i] == NULL) continue;
			if (INFO->elements[i]->type < 0 || INFO->elements[i]->type >= 10) continue;
			if (INFO->elements[i]->type == BASE) {
				if (INFO->elements[i]->team == myTeam) myBase = INFO->elements[i]->index;
				else if (INFO->elements[i]->team == enemyTeam) enemyBase = INFO->elements[i]->index;
			}
			else if (INFO->elements[i]->type == OILFIELD) oilfield.push_back(INFO->elements[i]->index);
			else if (INFO->elements[i]->type == MINE) mine.push_back(INFO->elements[i]->index);
			else if (INFO->elements[i]->type == FORT) fort.push_back(INFO->elements[i]->index);
		}
		srand(myBase);
		initBaseNbr();
		initSourceRoute();
		for (int i = -8; i != 9; ++i){
			for (int j = -8; j != 9; ++j){
				Position temp = POS(enemyBase);
				temp.x += i;
				temp.y += j;
				if (temp.x > 0 && temp.y > 0 && temp.x < 100 && temp.y < 100) {
					if (dis(temp, POS(enemyBase)) == 7 || dis(temp, POS(enemyBase)) == 8) {
						if (Map(temp.x, temp.y) == OCEAN){
							carrierAttackPos.push_back(temp);
						}
					}
				}
			}
		}
		std::sort(carrierAttackPos.begin(), carrierAttackPos.end(), cmpDisToBase3);
	}
	else {
		for (int i = 0; i != INFO->element_num; i++) {
			if (INFO->elements[i] == NULL) continue;
			if (INFO->elements[i]->type < 0 || INFO->elements[i]->type >= 10) continue;
			if (INFO->elements[i]->index < 0) continue;
			const State *unit = INFO->elements[i];
			if (unit->team == enemyTeam) enemyUnit.push_back(unit);
			else if (unit->team == myTeam){
				myUnit.push_back(unit);
				if (unit->type == CARGO) cargo.push_back(unit);
				else if (unit->type == FIGHTER) fighter.push_back(unit);
				else if (unit->type == CARRIER) carrier.push_back(unit);
				else if (unit->type == SUBMARINE) submarine.push_back(unit);
				else if (unit->type == DESTROYER) destroyer.push_back(unit);
			}
		}
		if (!enemyUnit.empty()) std::sort(enemyUnit.begin(), enemyUnit.end(), cmpDisToBase2);
	}
}

void evlState() {
	if (submarine.size() >= 9) {
		subState = 1;
	}
	if (carrier.size() >= 4) {
		carrierState = 1;
	}
}


void baseAction() {
	if (!fighter.empty()) {
		for (int i = 0; i != fighter.size(); ++i){
			if (fighter[i]->fuel < kProperty[FIGHTER].fuel_max / 2 || fighter[i]->ammo < kProperty[FIGHTER].ammo_max / 2) {
				if (dis(fighter[i]->pos, POS(myBase)) <= 2){
					Supply(myBase, fighter[i]->index, kProperty[FIGHTER].fuel_max - fighter[i]->fuel, kProperty[FIGHTER].ammo_max - fighter[i]->ammo, 0);
					break;
				}
			}
		}
	}
	if (!enemyUnit.empty()) {
		for (int i = 0; i != enemyUnit.size(); ++i) {
			if (enemyUnit[i]->type != SUBMARINE && dis(enemyUnit[i]->pos, POS(myBase)) <= kProperty[BASE].fire_ranges[enemyUnit[i]->pos.z]){
				AttackUnit(myBase, enemyUnit[i]->index);
				break;
			}
		}
	}
	int usedFuel = 0, producingNum = -1;
	int cargoNum = cargo.size(), fighterNum = fighter.size(), carrierNum = carrier.size(), subNum = submarine.size();
	while (++producingNum != INFO->production_num) {
		if (INFO->production_list[producingNum].unit_type == CARGO) cargoNum++;
		else if (INFO->production_list[producingNum].unit_type == FIGHTER) fighterNum++;
		else if (INFO->production_list[producingNum].unit_type == CARRIER) carrierNum++;
		else if (INFO->production_list[producingNum].unit_type == SUBMARINE) subNum++;
		int temp = kProperty[INFO->production_list[producingNum].unit_type].fuel_max;
		if (INFO->production_list[producingNum].unit_type == CARGO) temp >>= 1;
		usedFuel += temp;
	}
	int metal = GetState(myBase)->metal, fuel = GetState(myBase)->fuel - usedFuel;
	if (INFO->round == 0){
		for (int i = 0; i != 4; ++i) {
			Produce(CARGO); cargoNum++; metal -= kProperty[CARGO].cost; fuel -= kProperty[CARGO].fuel_max >> 1;
			Produce(FIGHTER); fighterNum++; metal -= kProperty[FIGHTER].cost; fuel -= kProperty[FIGHTER].fuel_max;
		}
	}
	while (metal >= 14 && fuel >= 60) {
		if (INFO->round <= 10 && cargoNum < 8) {
			Produce(CARGO); cargoNum++; metal -= kProperty[CARGO].cost; fuel -= kProperty[CARGO].fuel_max >> 1;
		}
		else if (cargoNum < 4){
			Produce(CARGO); cargoNum++; metal -= kProperty[CARGO].cost; fuel -= kProperty[CARGO].fuel_max >> 1;
		}
		else if (subNum < 2 && state == 0 && (fighterNum > 6)){
			Produce(SUBMARINE); subNum++; metal -= kProperty[SUBMARINE].cost; fuel -= kProperty[SUBMARINE].fuel_max;
		}
		else if (fighterNum > 6 && carrierNum < 5) {
			Produce(CARRIER); carrierNum++; metal -= kProperty[CARRIER].cost;
		}
		else{
			Produce(FIGHTER); fighterNum++; metal -= kProperty[FIGHTER].cost; fuel -= kProperty[FIGHTER].fuel_max;
		}
	}
}

void cargoAction(){
	if (cargo.empty() || sourceRoute.empty()) return;
	{
		for (int i = 0; i != sourceRoute.size(); ++i){
			if (sourceRoute[i].cargo1 == -1) continue;
			if (!isAlive(sourceRoute[i].cargo1)) {
				sourceRoute[i].cargo1 = -1;
				sourceRoute[i].state1 = 0;
			}
			else if (!army.cargo.empty() && std::find(army.cargo.begin(), army.cargo.end(), sourceRoute[i].cargo1) != army.cargo.end()) {
				sourceRoute[i].cargo1 = -1;
				sourceRoute[i].state1 = 0;
			}
		}
		for (int i = 0; i != sourceRoute.size(); ++i){
			if (sourceRoute[i].cargo2 == -1) continue;
			if (!isAlive(sourceRoute[i].cargo2)) {
				sourceRoute[i].cargo2 = -1;
				sourceRoute[i].state2 = 0;
			}
			else if (std::find(army.cargo.begin(), army.cargo.end(), sourceRoute[i].cargo2) != army.cargo.end()) {
				sourceRoute[i].cargo2 = -1;
				sourceRoute[i].state2 = 0;
			}
		}
		for (int i = 0; i != cargo.size() && i != 4; ++i) {
			int flag = 0;
			for (int j = 0; j != sourceRoute.size(); ++j) {
				if (cargo[i]->index == sourceRoute[j].cargo1){
					flag = 1;
					break;
				}
			}
			if (!flag){
				for (int j = 0; j != sourceRoute.size(); ++j) {
					if (sourceRoute[j].cargo1 == -1){
						sourceRoute[j].cargo1 = cargo[i]->index;
						break;
					}
				}
			}
		}
		if (cargo.size() > 4) {
			for (int i = 4; i != cargo.size(); ++i) {
				int flag = 0;
				for (int j = 0; j != sourceRoute.size(); ++j) {
					if (cargo[i]->index == sourceRoute[j].cargo2){
						flag = 1;
						break;
					}
				}
				if (!flag){
					for (int j = 0; j != sourceRoute.size(); ++j) {
						if (sourceRoute[j].cargo2 == -1){
							sourceRoute[j].cargo2 = cargo[i]->index;
							break;
						}
					}
				}
			}
		}
	}
	for (int i = 0; i != sourceRoute.size(); ++i){
		if (sourceRoute[i].cargo1 == -1) continue;
		if (sourceRoute[i].state1 == 0) {
			if (dis(POS(sourceRoute[i].cargo1), POS(sourceRoute[i].id1)) <= 1) {
				if (GetState(sourceRoute[i].cargo1)->metal < 10){
					sourceRoute[i].state1 = 1;
					ChangeDest(sourceRoute[i].cargo1, POS(sourceRoute[i].id2));
				}
				else{
					sourceRoute[i].state1 = 2;
					ChangeDest(sourceRoute[i].cargo1, sourceRoute[i].base);
				}
			}
			else ChangeDest(sourceRoute[i].cargo1, POS(sourceRoute[i].id1));
		}
		else if (sourceRoute[i].state1 == 1) {
			if (dis(POS(sourceRoute[i].cargo1), POS(sourceRoute[i].id2)) <= 1) {
				sourceRoute[i].state1 = 2;
				ChangeDest(sourceRoute[i].cargo1, sourceRoute[i].base);
			}
			else ChangeDest(sourceRoute[i].cargo1, sourceRoute[i].s2);
		}
		else if (sourceRoute[i].state1 == 2) {
			if (canSupply(sourceRoute[i].cargo1)) {
				sourceRoute[i].state1 = 0;
				Supply(sourceRoute[i].cargo1, GetState(myBase)->index, std::max(0, GetState(sourceRoute[i].cargo1)->fuel - sourceRoute[i].oilconsume), 0, GetState(sourceRoute[i].cargo1)->metal);
			}
			else ChangeDest(sourceRoute[i].cargo1, sourceRoute[i].base);
		}
	}
	for (int i = 0; i != sourceRoute.size(); ++i){
		if (sourceRoute[i].cargo2 == -1) continue;
		if (sourceRoute[i].state2 == 0) {
			if (GetState(sourceRoute[i].cargo2)->metal < 10){
				sourceRoute[i].state2 = 1;
				ChangeDest(sourceRoute[i].cargo2, POS(sourceRoute[i].id2));
			}
			else{
				sourceRoute[i].state2 = 2;
				ChangeDest(sourceRoute[i].cargo2, sourceRoute[i].base2);
			}
		}
		else if (sourceRoute[i].state2 == 1) {
			if (dis(POS(sourceRoute[i].cargo2), POS(sourceRoute[i].id2)) <= 1) {
				sourceRoute[i].state2 = 2;
				ChangeDest(sourceRoute[i].cargo2, sourceRoute[i].base2);
			}
			else ChangeDest(sourceRoute[i].cargo2, sourceRoute[i].s2);
		}
		else if (sourceRoute[i].state2 == 2) {
			if (canSupply(sourceRoute[i].cargo2)) {
				sourceRoute[i].state2 = 0;
				Supply(sourceRoute[i].cargo2, GetState(myBase)->index, std::max(0, GetState(sourceRoute[i].cargo2)->fuel - sourceRoute[i].oilconsume), 0, GetState(sourceRoute[i].cargo2)->metal);
			}
			else ChangeDest(sourceRoute[i].cargo2, sourceRoute[i].base2);
		}
	}
}

void fortAction(){
	if (fort.empty()) return;
	for (int i = 0; i != fort.size(); ++i){
		const State *frt = GetState(fort[i]);
		if (frt == NULL) continue;
		if (frt->team == myTeam) {
			if (!myUnit.empty() && (frt->ammo > 0 || frt->fuel > 0)){
				for (int j = 0; j != myUnit.size(); ++j) {
					if (myUnit[j]->type != CARGO && (myUnit[j]->ammo < kProperty[myUnit[j]->type].ammo_max / 2 || myUnit[j]->fuel < kProperty[myUnit[j]->type].fuel_max / 2)){
						if (myUnit[j]->type == FIGHTER && dis(frt->pos, myUnit[j]->pos) == 0) {
							Supply(frt->index, myUnit[j]->index, std::min(frt->fuel, kProperty[myUnit[j]->type].fuel_max - myUnit[j]->fuel), std::min(frt->ammo, kProperty[myUnit[j]->type].ammo_max - myUnit[j]->ammo), 0);
							break;
						}
						else if (dis(frt->pos, myUnit[j]->pos) == 1){
							Supply(frt->index, myUnit[j]->index, std::min(frt->fuel, kProperty[myUnit[j]->type].fuel_max - myUnit[j]->fuel), std::min(frt->ammo, kProperty[myUnit[j]->type].ammo_max - myUnit[j]->ammo), 0);
							break;
						}
					}
				}
			}
			if (dis(POS(enemyBase), frt->pos) <= kProperty[FORT].fire_ranges[1]) {
				AttackUnit(frt->index, enemyBase);
			}
			else if (!enemyUnit.empty()) {
				int minDis = 100000, tar = -1;
				for (int j = 0; j != enemyUnit.size(); ++j) {
					if (dis(frt->pos, enemyUnit[j]->pos) < minDis && enemyUnit[j]->type != SUBMARINE) {
						tar = enemyUnit[j]->index;
						minDis = dis(frt->pos, enemyUnit[j]->pos);
					}
				}
				if (tar != -1) {
					if (dis(POS(tar), frt->pos) <= kProperty[FORT].fire_ranges[GetState(tar)->pos.z]) {
						AttackUnit(frt->index, tar);
					}
				}
			}
		}
	}
}

Position stepTowards(Position now, Position des) {
	if (dis(now, des) <= 5) return des;
	Position result = des;
	int minDis = 100000;
	for (int i = -2; i != 3; ++i) {
		for (int j = -2; j != 3; ++j) {
			Position temp = now; temp.x += i; temp.y += j;
			if (dis(now, temp) > 5 || Map(temp.x, temp.y) == LAND || (dis(temp, des) - dis(now, des)) < 4) continue;
			if (Map(temp.x - 1, temp.y - 1) == OCEAN && Map(temp.x - 1, temp.y + 1) == OCEAN && Map(temp.x + 1, temp.y - 1) == OCEAN && Map(temp.x + 1, temp.y + 1) == OCEAN){
				if (dis(temp, des) < minDis) {
					result = temp;
					minDis = dis(temp, des);
				}
			}
		}
	}
	return result;
}

vector<const State *> baseEnemy;
vector<const State *> enemyCarrier;
vector<const State *> enemyBaseUnit;

void initEnemyList() {
	baseEnemy.clear();
	enemyCarrier.clear();
	enemyBaseUnit.clear();
	if (enemyUnit.empty()) return;
	for (int i = 0; i != enemyUnit.size(); ++i) {
		if (dis(enemyUnit[i]->pos, POS(myBase)) < 4 && enemyUnit[i]->type != SUBMARINE){
			baseEnemy.push_back(enemyUnit[i]);
		}
		if (enemyUnit[i]->type == CARRIER && dis(enemyUnit[i]->pos, POS(myBase)) <= 8){
			enemyCarrier.push_back(enemyUnit[i]);
		}
		if (dis(enemyUnit[i]->pos, POS(enemyBase)) < 3 && enemyUnit[i]->type == FIGHTER){
			enemyBaseUnit.push_back(enemyUnit[i]);
		}
	}
}

vector<Position> baseFighterPos;
bool cmpID(const State *f1, const State *f2) {
	return f1->index < f2->index;
}

void fighterMove(){
	if (baseFighterPos.empty()) {
		for (int i = 0; i != 20; ++i){
			baseFighterPos.push_back(POS(myBase));
			baseFighterPos[i].z += 1;
		}
		baseFighterPos[0].x += 0; baseFighterPos[0].y += 0;
		baseFighterPos[1].x += 0; baseFighterPos[1].y += 1;
		baseFighterPos[2].x -= 1; baseFighterPos[2].y += 0;
		baseFighterPos[3].x += 0; baseFighterPos[3].y -= 1;
		baseFighterPos[4].x += 1; baseFighterPos[4].y += 0;
		baseFighterPos[5].x += 2; baseFighterPos[5].y += 0;
		baseFighterPos[6].x += 0; baseFighterPos[6].y += 2;
		baseFighterPos[7].x -= 2; baseFighterPos[7].y += 0;
		baseFighterPos[8].x += 0; baseFighterPos[8].y -= 2;
		baseFighterPos[9].x += 1; baseFighterPos[9].y -= 2;
		baseFighterPos[10].x += 2; baseFighterPos[10].y -= 1;
		baseFighterPos[11].x += 1; baseFighterPos[11].y += 2;
		baseFighterPos[12].x -= 1; baseFighterPos[12].y += 2;
		baseFighterPos[13].x -= 2; baseFighterPos[13].y += 1;
		baseFighterPos[14].x -= 2; baseFighterPos[14].y -= 1;
		baseFighterPos[15].x += 0; baseFighterPos[15].y += 0;
		baseFighterPos[16].x += 0; baseFighterPos[16].y += 0;
		baseFighterPos[17].x += 0; baseFighterPos[17].y += 0;
		baseFighterPos[18].x += 0; baseFighterPos[18].y += 0;
		baseFighterPos[19].x += 0; baseFighterPos[19].y += 0;
	}
	if (!fighter.empty()){
		std::sort(fighter.begin(), fighter.end(), cmpID);
		for (int i = 0; i != fighter.size(); ++i){
			ChangeDest(fighter[i]->index, baseFighterPos[i]);
		}
	}
}


void mainAction(){
	if (state == 0) {
		if (!fighter.empty()) {
			for (int i = 0; i != fighter.size(); ++i) {

			}
		}
	}
}
/*
void foo(){
if (!carrier.empty()) {
for (int i = 0; i != carrier.size(); ++i){
if (dis(carrier[i]->pos, POS(enemyBase)) <= 8){
AttackUnit(carrier[i]->index, enemyBase);
std::cout << "ATTACKING\n";
}
else{
ChangeDest(carrier[i]->index, POS(enemyBase));
cout << "MOVING!! dis = " << dis(carrier[i]->pos, POS(enemyBase)) << endl;
}
}
}
}*/


void evlStateAnother(){
	initEnemyList();
	if (state == 2) {
		if (fighter.size() > 3) return;
		else if (GetState(enemyBase)->health > 1000 || !(GetState(enemyBase)->health < 200 && GetState(enemyBase)->health >=0))state = 0;
	}
	else if (state == 0) {
		if (fighter.size() >= 12){
			int cnt = 0;
			if (!enemyUnit.empty()) {
				for (int i = 0; i != enemyUnit.size(); ++i){
					if (dis(enemyUnit[i]->pos, POS(myBase)) < 9) cnt++;
				}
			}
			if (cnt == 0)
				state = 2;
		}
	}
}

void baseActionAnother(){
	if (!fighter.empty()) {
		for (int i = 0; i != fighter.size(); ++i){
			if (fighter[i]->fuel < kProperty[FIGHTER].fuel_max / 2 || fighter[i]->ammo < kProperty[FIGHTER].ammo_max / 2) {
				if (dis(fighter[i]->pos, POS(myBase)) <= 2){
					Supply(myBase, fighter[i]->index, kProperty[FIGHTER].fuel_max - fighter[i]->fuel, kProperty[FIGHTER].ammo_max - fighter[i]->ammo, 0);
					break;
				}
			}
		}
	}
	if (!enemyUnit.empty()) {
		for (int i = 0; i != enemyUnit.size(); ++i) {
			if (enemyUnit[i]->type != SUBMARINE && dis(enemyUnit[i]->pos, POS(myBase)) <= kProperty[BASE].fire_ranges[enemyUnit[i]->pos.z]){
				AttackUnit(myBase, enemyUnit[i]->index);
				break;
			}
		}
	}	int usedFuel = 0, producingNum = -1;
	int cargoNum = cargo.size(), fighterNum = fighter.size(), carrierNum = carrier.size(), subNum = submarine.size();
	while (++producingNum != INFO->production_num) {
		if (INFO->production_list[producingNum].unit_type == CARGO) cargoNum++;
		else if (INFO->production_list[producingNum].unit_type == FIGHTER) fighterNum++;
		else if (INFO->production_list[producingNum].unit_type == CARRIER) carrierNum++;
		else if (INFO->production_list[producingNum].unit_type == SUBMARINE) subNum++;
		int temp = kProperty[INFO->production_list[producingNum].unit_type].fuel_max;
		if (INFO->production_list[producingNum].unit_type == CARGO) temp >>= 1;
		usedFuel += temp;
	}
	int metal = GetState(myBase)->metal, fuel = GetState(myBase)->fuel - usedFuel;
	if (INFO->round == 0){
		for (int i = 0; i != 4; ++i) {
			Produce(CARGO); cargoNum++; metal -= kProperty[CARGO].cost; fuel -= kProperty[CARGO].fuel_max >> 1;
//			Produce(FIGHTER); fighterNum++; metal -= kProperty[FIGHTER].cost; fuel -= kProperty[FIGHTER].fuel_max;
		}
	}
	if (fighterNum > 9 && boost < 2) {
		Produce(FIGHTER); fighterNum++; metal -= kProperty[FIGHTER].cost;
		Produce(FIGHTER); fighterNum++; metal -= kProperty[FIGHTER].cost;
		boost++;
	}
	while (metal >= 14 && fuel >= 60) {
		if (INFO->round <= 10 && cargoNum < 7) {
			Produce(CARGO); cargoNum++; metal -= kProperty[CARGO].cost; fuel -= kProperty[CARGO].fuel_max >> 1;
		}
		else if (cargoNum < 4){
			Produce(CARGO); cargoNum++; metal -= kProperty[CARGO].cost; fuel -= kProperty[CARGO].fuel_max >> 1;
		}
		else{
			Produce(FIGHTER); fighterNum++; metal -= kProperty[FIGHTER].cost; fuel -= kProperty[FIGHTER].fuel_max;
		}
	}
}

void fighterPeace() {
	if (!fighter.empty()){
		fighterMove();
		for (int i = 0; i != fighter.size(); ++i){
			if (i < 3 && baseEnemy.size() >0) {
				AttackUnit(fighter[i]->index, baseEnemy[0]->index);
			}
			else if (i < 6 && baseEnemy.size() >1) {
				AttackUnit(fighter[i]->index, baseEnemy[1]->index);
			}
			else if (i < 9 && baseEnemy.size() >2){
				AttackUnit(fighter[i]->index, baseEnemy[2]->index);
			}
			else if (!enemyCarrier.empty() && i > 4) {
				int minDis = 10000, tar = -1;
				for (int j = 0; j != enemyUnit.size(); ++j) {
					if (dis(enemyCarrier[j]->pos, fighter[i]->pos) < minDis) {
						minDis = dis(enemyCarrier[j]->pos, fighter[i]->pos);
						tar = enemyCarrier[j]->index;
					}
				}
				ChangeDest(fighter[i]->index, POS(tar));
				AttackUnit(fighter[i]->index, tar);
			}
			else if (!enemyUnit.empty()) {
				for (int j = 0; j != enemyUnit.size(); ++j) {
					if (enemyUnit[j]->type != SUBMARINE && dis(enemyUnit[j]->pos, fighter[i]->pos) <= kProperty[FIGHTER].fire_ranges[enemyUnit[j]->pos.z]){
						AttackUnit(fighter[i]->index, enemyUnit[j]->index);
						break;
					}
				}
			}
		}
	}
}

vector<Position> enemyBasePos;

bool cmpDisToEBase(const State *p1, const State *p2) {
	return dis(p1->pos, POS(enemyBase)) > dis(p2->pos, POS(enemyBase));
}



void fighterMove_attack(){
	if (enemyBasePos.empty()) {
		for (int i = 0; i != 20; ++i){
			enemyBasePos.push_back(POS(enemyBase));
			enemyBasePos[i].z += 1;
		}
		enemyBasePos[0].x += 0; enemyBasePos[0].y += 0;
		enemyBasePos[1].x += 0; enemyBasePos[1].y += 1;
		enemyBasePos[2].x += 1; enemyBasePos[2].y += 1;
		enemyBasePos[3].x += 1; enemyBasePos[3].y += 0;
		enemyBasePos[4].x += 1; enemyBasePos[4].y -= 1;
		enemyBasePos[5].x += 0; enemyBasePos[5].y -= 1;
		enemyBasePos[6].x -= 1; enemyBasePos[6].y -= 1;
		enemyBasePos[7].x -= 1; enemyBasePos[7].y += 0;
		enemyBasePos[8].x -= 1; enemyBasePos[8].y += 1;
		enemyBasePos[9].x += 0; enemyBasePos[9].y += 2;
		enemyBasePos[10].x += 2; enemyBasePos[10].y += 0;
		enemyBasePos[11].x += 0; enemyBasePos[11].y -= 2;
		enemyBasePos[12].x -= 2; enemyBasePos[12].y += 0;
		enemyBasePos[13].x += 0; enemyBasePos[13].y += 0;
		enemyBasePos[14].x += 0; enemyBasePos[14].y += 0;
		enemyBasePos[15].x += 0; enemyBasePos[15].y += 0;
		enemyBasePos[16].x += 0; enemyBasePos[16].y += 0;
		enemyBasePos[17].x += 0; enemyBasePos[17].y += 0;
		enemyBasePos[18].x += 0; enemyBasePos[18].y += 0;
		enemyBasePos[19].x += 0; enemyBasePos[19].y += 0;
	}
	if (!fighter.empty()){
		std::sort(fighter.begin(), fighter.end(), cmpDisToEBase);
		for (int i = 0; i != fighter.size(); ++i){
			ChangeDest(fighter[i]->index, enemyBasePos[i]);
		}
	}
}


void fighterAttack(){
	if (!fighter.empty()){
		fighterMove_attack();
		for (int i = 0; i != fighter.size(); ++i){
			if (enemyBaseUnit.size() < 3) {
				if (dis(POS(enemyBase), fighter[i]->pos) <= kProperty[FIGHTER].fire_ranges[1]){
					AttackUnit(fighter[i]->index, enemyBase);
				}
				else if (!enemyUnit.empty()) {
					for (int j = 0; j != enemyUnit.size(); ++j) {
						if (enemyUnit[j]->type != SUBMARINE && dis(enemyUnit[j]->pos, fighter[i]->pos) <= kProperty[FIGHTER].fire_ranges[enemyUnit[j]->pos.z]){
							AttackUnit(fighter[i]->index, enemyUnit[j]->index);
							break;
						}
					}
				}
			}
			if (i < 3 && enemyBaseUnit.size() >0 ) {
				AttackUnit(fighter[i]->index, enemyBaseUnit[0]->index);
			}
			else if (i < 6 && enemyBaseUnit.size() >1) {
				AttackUnit(fighter[i]->index, enemyBaseUnit[1]->index);
			}
			else if (i < 9 && enemyBaseUnit.size() >2){
				AttackUnit(fighter[i]->index, enemyBaseUnit[2]->index);
			}
			else if (dis(POS(enemyBase), fighter[i]->pos) <= kProperty[FIGHTER].fire_ranges[1]){
				AttackUnit(fighter[i]->index, enemyBase);
			}
			else if (!enemyUnit.empty()) {
				for (int j = 0; j != enemyUnit.size(); ++j) {
					if (enemyUnit[j]->type != SUBMARINE && dis(enemyUnit[j]->pos, fighter[i]->pos) <= kProperty[FIGHTER].fire_ranges[enemyUnit[j]->pos.z]){
						AttackUnit(fighter[i]->index, enemyUnit[j]->index);
						break;
					}
				}
			}
		}
	}
}

void fighterActionAnother() {
	initEnemyList();
	if (!fighter.empty()) {
		if (state == 0) fighterPeace();
		else fighterAttack();
	}
}

void baseAction_sub() {
	if (!fighter.empty()) {
		for (int i = 0; i != fighter.size(); ++i){
			if (fighter[i]->fuel < kProperty[FIGHTER].fuel_max / 2 || fighter[i]->ammo < kProperty[FIGHTER].ammo_max / 2) {
				if (dis(fighter[i]->pos, POS(myBase)) <= 2){
					Supply(myBase, fighter[i]->index, kProperty[FIGHTER].fuel_max - fighter[i]->fuel, kProperty[FIGHTER].ammo_max - fighter[i]->ammo, 0);
					break;
				}
			}
		}
	}
	if (!enemyUnit.empty()) {
		for (int i = 0; i != enemyUnit.size(); ++i) {
			if (enemyUnit[i]->type != SUBMARINE && dis(enemyUnit[i]->pos, POS(myBase)) <= kProperty[BASE].fire_ranges[enemyUnit[i]->pos.z]){
				AttackUnit(myBase, enemyUnit[i]->index);
				break;
			}
		}
	}	int usedFuel = 0, producingNum = -1;
	int cargoNum = cargo.size(), fighterNum = fighter.size(), carrierNum = carrier.size(), subNum = submarine.size();
	while (++producingNum != INFO->production_num) {
		if (INFO->production_list[producingNum].unit_type == CARGO) cargoNum++;
		else if (INFO->production_list[producingNum].unit_type == FIGHTER) fighterNum++;
		else if (INFO->production_list[producingNum].unit_type == CARRIER) carrierNum++;
		else if (INFO->production_list[producingNum].unit_type == SUBMARINE) subNum++;
		int temp = kProperty[INFO->production_list[producingNum].unit_type].fuel_max;
		if (INFO->production_list[producingNum].unit_type == CARGO) temp >>= 1;
		usedFuel += temp;
	}
	int metal = GetState(myBase)->metal, fuel = GetState(myBase)->fuel - usedFuel;
	if (INFO->round == 0){
		for (int i = 0; i != 4; ++i) {
			Produce(CARGO); cargoNum++; metal -= kProperty[CARGO].cost; fuel -= kProperty[CARGO].fuel_max >> 1;
		}
	}
	while (metal >= 14 && fuel >= 60) {
		if (INFO->round <= 10 && cargoNum < 7) {
			Produce(CARGO); cargoNum++; metal -= kProperty[CARGO].cost; fuel -= kProperty[CARGO].fuel_max >> 1;
		}
		else if (cargoNum < 4){
			Produce(CARGO); cargoNum++; metal -= kProperty[CARGO].cost; fuel -= kProperty[CARGO].fuel_max >> 1;
		}
		else if (subNum < 4){
			Produce(SUBMARINE); subNum++; metal -= kProperty[SUBMARINE].cost; fuel -= kProperty[SUBMARINE].fuel_max;
		}
		else if (metal >= 24 && carrierNum < 3){
			Produce(CARRIER); carrierNum++; metal -= kProperty[CARRIER].cost; fuel -= 120;
		}
		else if (subNum < 9){
			Produce(SUBMARINE); subNum++; metal -= kProperty[SUBMARINE].cost; fuel -= kProperty[SUBMARINE].fuel_max;
		}
		else {
			Produce(CARRIER); carrierNum++; metal -= kProperty[CARRIER].cost; fuel -= 120;
		}
	}
}

void subMove() {
	if (!submarine.empty()) {
		for (int i = 0; i != submarine.size(); ++i){
			if (dis(submarine[i]->pos, POS(myBase)) < 5) {
				Position temp = POS(enemyBase);
				temp.z--;
				temp.x += std::rand() % 10 - 5;
				temp.y += std::rand() % 10 - 5;
				ChangeDest(submarine[i]->index, temp);
			}
			else ChangeDest(submarine[i]->index, submarine[i]->pos);
		}
	}
}

void subAction() {
	initEnemyList();
	if (subState == 0){
		subMove();
		if (!submarine.empty()) {
			for (int i = 0; i != submarine.size(); ++i){
				if (!enemyUnit.empty()) {
					for (int j = 0; j != enemyUnit.size(); ++j) {
						if ((enemyUnit[j]->type == CARRIER || enemyUnit[j]->type == SUBMARINE || enemyUnit[j]->type == CARGO) && dis(enemyUnit[j]->pos, submarine[i]->pos) <= kProperty[SUBMARINE].fire_ranges[enemyUnit[j]->pos.z]){
							AttackUnit(submarine[i]->index, enemyUnit[j]->index);
							break;
						}
					}
					for (int j = 0; j != enemyUnit.size(); ++j) {
						if ((enemyUnit[j]->type == CARRIER) && dis(enemyUnit[j]->pos, submarine[i]->pos) <= kProperty[SUBMARINE].fire_ranges[enemyUnit[j]->pos.z]){
							AttackUnit(submarine[i]->index, enemyUnit[j]->index);
							ChangeDest(submarine[i]->index, enemyUnit[j]->pos);
							break;
						}
					}
				}
			}
		}
	}
	else {
		if (!submarine.empty()) {
			for (int i = 0; i != submarine.size(); ++i){
				if (dis(submarine[i]->pos, POS(enemyBase)) > 5)
					ChangeDest(submarine[i]->index, POS(enemyBase));
				else{
					Position temp = POS(enemyBase);
					temp.z--;
					temp.x += std::rand() % 5 - 2;
					temp.y += std::rand() % 5 - 2;
					ChangeDest(submarine[i]->index, temp);
				}
				if (!enemyUnit.empty()) {
					for (int j = 0; j != enemyUnit.size(); ++j) {
						if ((enemyUnit[j]->type == CARRIER || enemyUnit[j]->type == SUBMARINE || enemyUnit[j]->type == CARGO) && dis(enemyUnit[j]->pos, submarine[i]->pos) <= kProperty[SUBMARINE].fire_ranges[enemyUnit[j]->pos.z]){
							AttackUnit(submarine[i]->index, enemyUnit[j]->index);
							break;
						}
					}
					for (int j = 0; j != enemyUnit.size(); ++j) {
						if ((enemyUnit[j]->type == CARRIER) && dis(enemyUnit[j]->pos, submarine[i]->pos) <= kProperty[SUBMARINE].fire_ranges[enemyUnit[j]->pos.z]){
							AttackUnit(submarine[i]->index, enemyUnit[j]->index);
							ChangeDest(submarine[i]->index, enemyUnit[j]->pos);
							break;
						}
					}
				}
			}
		}
	}
}

void carrierAction() {
	if (carrierState == 0) {
		if (!carrier.empty()) {
			for (int i = 0; i != carrier.size(); ++i){
				if (dis(carrier[i]->pos, POS(myBase)) < 6) {
					Position temp = POS(enemyBase);
					temp.z--;
					temp.x += std::rand() % 10 - 5;
					temp.y += std::rand() % 10 - 5;
					ChangeDest(carrier[i]->index, temp);
				}
				else{
					ChangeDest(carrier[i]->index, carrier[i]->pos);
				}
			}
			for (int i = 0; i != carrier.size(); ++i) {
				if (!enemyUnit.empty()) {
					for (int j = 0; j != enemyUnit.size(); ++j) {
						if (enemyUnit[j]->type != SUBMARINE && dis(enemyUnit[j]->pos, carrier[i]->pos) <= kProperty[CARRIER].fire_ranges[enemyUnit[j]->pos.z]){
							AttackUnit(carrier[i]->index, enemyUnit[j]->index);
							break;
						}
					}
				}
			}
		}
	}
	if (carrierState == 1) {
		if (!carrier.empty()) {
			std::sort(carrier.begin(), carrier.end(), cmpID);
			for (int i = 0; i != carrier.size(); ++i){
				if (dis(carrier[i]->pos, POS(enemyBase)) > 8)
					ChangeDest(carrier[i]->index, carrierAttackPos[i % carrierAttackPos.size()]);
			}
			for (int i = 0; i != carrier.size(); ++i) {
				if (dis(carrier[i]->pos, POS(enemyBase)) <= 8){
					AttackUnit(carrier[i]->index, enemyBase);
				}
				else if (!enemyUnit.empty()) {
					for (int j = 0; j != enemyUnit.size(); ++j) {
						if (enemyUnit[j]->type != SUBMARINE && dis(enemyUnit[j]->pos, carrier[i]->pos) <= kProperty[CARRIER].fire_ranges[enemyUnit[j]->pos.z]){
							AttackUnit(carrier[i]->index, enemyUnit[j]->index);
							break;
						}
					}
				}
			}
		}
	}
}

void evlState_carrier(){
	if (carrier.size() > 7) carrierState = 1;
}

void baseAction_carrier() {
	if (!enemyUnit.empty()) {
		for (int i = 0; i != enemyUnit.size(); ++i) {
			if (enemyUnit[i]->type != SUBMARINE && dis(enemyUnit[i]->pos, POS(myBase)) <= kProperty[BASE].fire_ranges[enemyUnit[i]->pos.z]){
				AttackUnit(myBase, enemyUnit[i]->index);
				break;
			}
		}
	}	int usedFuel = 0, producingNum = -1;
	int cargoNum = cargo.size(), fighterNum = fighter.size(), carrierNum = carrier.size(), subNum = submarine.size();
	while (++producingNum != INFO->production_num) {
		if (INFO->production_list[producingNum].unit_type == CARGO) cargoNum++;
		else if (INFO->production_list[producingNum].unit_type == FIGHTER) fighterNum++;
		else if (INFO->production_list[producingNum].unit_type == CARRIER) carrierNum++;
		else if (INFO->production_list[producingNum].unit_type == SUBMARINE) subNum++;
		int temp = kProperty[INFO->production_list[producingNum].unit_type].fuel_max;
		if (INFO->production_list[producingNum].unit_type == CARGO) temp >>= 1;
		usedFuel += temp;
	}
	int metal = GetState(myBase)->metal, fuel = GetState(myBase)->fuel - usedFuel;
	if (INFO->round == 0){
		for (int i = 0; i != 6; ++i) {
			Produce(CARGO); cargoNum++; metal -= kProperty[CARGO].cost; fuel -= kProperty[CARGO].fuel_max >> 1;
		}
	}
	while (metal >= 14 && fuel >= 60) {
		if (INFO->round <= 10 && cargoNum < 7) {
			Produce(CARGO); cargoNum++; metal -= kProperty[CARGO].cost; fuel -= kProperty[CARGO].fuel_max >> 1;
		}
		else if (cargoNum < 4){
			Produce(CARGO); cargoNum++; metal -= kProperty[CARGO].cost; fuel -= kProperty[CARGO].fuel_max >> 1;
		}
		else if (INFO->round > 3 && metal >= 24) {
			Produce(CARRIER); carrierNum++; metal -= kProperty[CARRIER].cost; fuel -= 150;
		}
		else break;
	}
}

void baseActionAnotherC(){
	if (!fighter.empty()) {
		for (int i = 0; i != fighter.size(); ++i){
			if (fighter[i]->fuel < kProperty[FIGHTER].fuel_max / 2 || fighter[i]->ammo < kProperty[FIGHTER].ammo_max / 2) {
				if (dis(fighter[i]->pos, POS(myBase)) <= 2){
					Supply(myBase, fighter[i]->index, kProperty[FIGHTER].fuel_max - fighter[i]->fuel, kProperty[FIGHTER].ammo_max - fighter[i]->ammo, 0);
					break;
				}
			}
		}
	}
	if (!enemyUnit.empty()) {
		for (int i = 0; i != enemyUnit.size(); ++i) {
			if (enemyUnit[i]->type != SUBMARINE && dis(enemyUnit[i]->pos, POS(myBase)) <= kProperty[BASE].fire_ranges[enemyUnit[i]->pos.z]){
				AttackUnit(myBase, enemyUnit[i]->index);
				break;
			}
		}
	}	int usedFuel = 0, producingNum = -1;
	int cargoNum = cargo.size(), fighterNum = fighter.size(), carrierNum = carrier.size(), subNum = submarine.size();
	while (++producingNum != INFO->production_num) {
		if (INFO->production_list[producingNum].unit_type == CARGO) cargoNum++;
		else if (INFO->production_list[producingNum].unit_type == FIGHTER) fighterNum++;
		else if (INFO->production_list[producingNum].unit_type == CARRIER) carrierNum++;
		else if (INFO->production_list[producingNum].unit_type == SUBMARINE) subNum++;
		int temp = kProperty[INFO->production_list[producingNum].unit_type].fuel_max;
		if (INFO->production_list[producingNum].unit_type == CARGO) temp >>= 1;
		usedFuel += temp;
	}
	int metal = GetState(myBase)->metal, fuel = GetState(myBase)->fuel - usedFuel;
	if (INFO->round == 0){
		for (int i = 0; i != 4; ++i) {
			Produce(CARGO); cargoNum++; metal -= kProperty[CARGO].cost; fuel -= kProperty[CARGO].fuel_max >> 1;
			Produce(SUBMARINE); subNum++; metal -= kProperty[SUBMARINE].cost; fuel -= kProperty[SUBMARINE].fuel_max;
		}
	}
	if (fighterNum > 9 && boost < 2) {
		Produce(FIGHTER); fighterNum++; metal -= kProperty[FIGHTER].cost;
		Produce(FIGHTER); fighterNum++; metal -= kProperty[FIGHTER].cost;
		boost++;
	}
	while (metal >= 14 && fuel >= 60) {
		if (INFO->round <= 10 && cargoNum < 7) {
			Produce(CARGO); cargoNum++; metal -= kProperty[CARGO].cost; fuel -= kProperty[CARGO].fuel_max >> 1;
		}
		else if (cargoNum < 4){
			Produce(CARGO); cargoNum++; metal -= kProperty[CARGO].cost; fuel -= kProperty[CARGO].fuel_max >> 1;
		}
		else{
			Produce(FIGHTER); fighterNum++; metal -= kProperty[FIGHTER].cost; fuel -= kProperty[FIGHTER].fuel_max;
		}
	}
}

void subActionAnother() {
	if (!submarine.empty()) {
		for (int i = 0; i != submarine.size(); ++i){
			if (dis(submarine[i]->pos, POS(enemyBase)) > 5)
				ChangeDest(submarine[i]->index, POS(enemyBase));
			else{
				Position temp = POS(enemyBase);
				temp.z--;
				temp.x += std::rand() % 5 - 2;
				temp.y += std::rand() % 5 - 2;
				ChangeDest(submarine[i]->index, temp);
			}
			if (!enemyUnit.empty()) {
				for (int j = 0; j != enemyUnit.size(); ++j) {
					if ((enemyUnit[j]->type == CARRIER || enemyUnit[j]->type == SUBMARINE || enemyUnit[j]->type == CARGO) && dis(enemyUnit[j]->pos, submarine[i]->pos) <= kProperty[SUBMARINE].fire_ranges[enemyUnit[j]->pos.z]){
						AttackUnit(submarine[i]->index, enemyUnit[j]->index);
						break;
					}
				}
				for (int j = 0; j != enemyUnit.size(); ++j) {
					if ((enemyUnit[j]->type == CARRIER) && dis(enemyUnit[j]->pos, submarine[i]->pos) <= kProperty[SUBMARINE].fire_ranges[enemyUnit[j]->pos.z]){
						AttackUnit(submarine[i]->index, enemyUnit[j]->index);
						ChangeDest(submarine[i]->index, enemyUnit[j]->pos);
						break;
					}
				}
			}
		}
	}
}

void baseActionAnotherCC() {
	if (!fighter.empty()) {
		for (int i = 0; i != fighter.size(); ++i){
			if (fighter[i]->fuel < kProperty[FIGHTER].fuel_max / 2 || fighter[i]->ammo < kProperty[FIGHTER].ammo_max / 2) {
				if (dis(fighter[i]->pos, POS(myBase)) <= 2){
					Supply(myBase, fighter[i]->index, kProperty[FIGHTER].fuel_max - fighter[i]->fuel, kProperty[FIGHTER].ammo_max - fighter[i]->ammo, 0);
					break;
				}
			}
		}
	}
	if (!enemyUnit.empty()) {
		for (int i = 0; i != enemyUnit.size(); ++i) {
			if (enemyUnit[i]->type != SUBMARINE && dis(enemyUnit[i]->pos, POS(myBase)) <= kProperty[BASE].fire_ranges[enemyUnit[i]->pos.z]){
				AttackUnit(myBase, enemyUnit[i]->index);
				break;
			}
		}
	}	int usedFuel = 0, producingNum = -1;
	int cargoNum = cargo.size(), fighterNum = fighter.size(), carrierNum = carrier.size(), subNum = submarine.size();
	while (++producingNum != INFO->production_num) {
		if (INFO->production_list[producingNum].unit_type == CARGO) cargoNum++;
		else if (INFO->production_list[producingNum].unit_type == FIGHTER) fighterNum++;
		else if (INFO->production_list[producingNum].unit_type == CARRIER) carrierNum++;
		else if (INFO->production_list[producingNum].unit_type == SUBMARINE) subNum++;
		int temp = kProperty[INFO->production_list[producingNum].unit_type].fuel_max;
		if (INFO->production_list[producingNum].unit_type == CARGO) temp >>= 1;
		usedFuel += temp;
	}
	int metal = GetState(myBase)->metal, fuel = GetState(myBase)->fuel - usedFuel;
	if (INFO->round == 0){
		for (int i = 0; i != 8; ++i) {
			Produce(FIGHTER); fighterNum++; metal -= kProperty[FIGHTER].cost; fuel -= kProperty[FIGHTER].fuel_max;
			state = 1;
		}
	}
	if (fighterNum > 9 && boost < 2) {
		Produce(FIGHTER); fighterNum++; metal -= kProperty[FIGHTER].cost;
		Produce(FIGHTER); fighterNum++; metal -= kProperty[FIGHTER].cost;
		boost++;
	}
	while (metal >= 14 && fuel >= 60) {
		if (INFO->round <= 10 && cargoNum < 7) {
			Produce(CARGO); cargoNum++; metal -= kProperty[CARGO].cost; fuel -= kProperty[CARGO].fuel_max >> 1;
		}
		else if (cargoNum < 4){
			Produce(CARGO); cargoNum++; metal -= kProperty[CARGO].cost; fuel -= kProperty[CARGO].fuel_max >> 1;
		}
		else{
			Produce(FIGHTER); fighterNum++; metal -= kProperty[FIGHTER].cost; fuel -= kProperty[FIGHTER].fuel_max;
		}
	}
}

void fighterActionAnotherCC(){
	initEnemyList();
	if (!fighter.empty()) {
		if (state == 0) fighterPeace();
		else fighterAttack();
	}
}