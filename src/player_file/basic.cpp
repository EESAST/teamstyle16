#include <cstdlib>

#include "basic.h"
#include "connection.h"

namespace teamstyle16 {

const GameInfo * Info()
{
    return Connection::Instance()->game_info();
}

MapType Map(int x, int y)
{
    return Connection::Instance()->map(x, y);
}

int Update()
{
    return Connection::Instance()->Update();
}

int TryUpdate()
{
    return Connection::Instance()->Update();
}

void AttackPos(int operand, const Position *target)
{
    using std::to_string;

    Connection::Instance()->Send("ap" + to_string(operand) + ' ' +
                                        to_string(target->x) + ' ' +
                                        to_string(target->y) + ' ' +
                                        to_string(target->z) + ',');
}

void AttackUnit(int operand, int target)
{
    using std::to_string;

    Connection::Instance()->Send("au" + to_string(operand) + ' ' +
                                        to_string(target) + ',');
}

void ChangeDest(int operand, const Position *dest)
{
    using std::to_string;

    Connection::Instance()->Send("cd" + to_string(operand) + ' ' +
                                        to_string(dest->x) + ' ' +
                                        to_string(dest->y) + ' ' +
                                        to_string(dest->z) + ',');
}

void Collect(int operand, int target)
{
    using std::to_string;

    Connection::Instance()->Send("cl" + to_string(operand) + ' ' +
                                        to_string(target) + ',');
}

void Fix(int operand, int target)
{
    using std::to_string;

    Connection::Instance()->Send("fx" + to_string(operand) + ' ' +
                                        to_string(target) + ',');
}

void Produce(int operand, Type type)
{
    using std::to_string;

    Connection::Instance()->Send("pd" + to_string(operand) + ' ' +
                                        to_string(type) + ',');
}

void Supply(int operand, int target, int fuel, int metal, int ammo)
{
    using std::to_string;

    Connection::Instance()->Send("sp" + to_string(operand) + ' ' +
                                        to_string(target) + ' ' +
                                        to_string(fuel) + ' ' +
                                        to_string(metal) + ' ' +
                                        to_string(ammo) + ',');
}

void Cancel(int operand)
{
    Connection::Instance()->Send("cc,");
}

const Property * GetProperty(int type)
{
    static Property property;

    if (type < 0)
        return NULL;
    if (type < kElementTypes)
        return &kElementInfo[type];

    // might be formation
    Formation formation;
    if (TypeToFormation(type, &formation))
    {
        // initialize to default
        property = kElementInfo[FORMATION];

        if (formation[SCOUT])
            std::memcpy(property.sight_range,
                        kScoutSightRange,
                        sizeof(property.sight_range));

        for (int i = 0; i < kMaxPlaneTypes; i++)
        {
            int num = formation[i];
            const Property &plane = kPlaneInfo[i];

            property.health_max += plane.health_max * num;
            property.fuel_max += plane.fuel_max * num;
            property.ammo_max += plane.ammo_max * num;
            property.ammo_once += plane.ammo_once * num;
            property.metal_max += plane.metal_max * num;
            property.attack[0] += plane.attack[0] * num;
            property.attack[1] += plane.attack[1] * num;
            property.defence[0] += plane.defence[0] * num;
            property.defence[1] += plane.defence[1] * num;
            property.cost += plane.cost * num;
            property.build_round += plane.build_round * num;
            property.population += plane.population * num;
        }
        return &property;
    }

    return NULL;
}


bool TypeToFormation(int type, Formation *formation)
{
    // TODO
    return true;
}

int FormationToType(const Formation *Formation)
{
    // TODO
    return -1;
}


}  // namespace teamstyle16
