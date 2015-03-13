#include <iostream>
#include <sstream>
#include <string>

#include "basic.h"
#include "connection.h"

namespace {

std::string PosString(const teamstyle16::Position &pos)
{
    std::ostringstream oss;
    oss << '(' << pos.x << ", " << pos.y << ", " << pos.z << ')';

    return oss.str();
}

void ShowElement(const teamstyle16::State &state)
{
    using std::cout;
    using std::endl;

    cout << "    index: " << state.index
         << "\n    type: " << state.type
         << "\n    pos: " << PosString(state.pos)
         << "\n    size: " << state.size.x_length << " * " << state.size.y_length
         << "\n    visible: " << state.visible
         << "\n    team: " << state.team
         << "\n    health: " << state.health
         << "\n    fuel: " << state.fuel
         << "\n    ammo: " << state.ammo
         << "\n    metal: " << state.metal
         << "\n    destination: " << PosString(state.destination) << endl;
}

void ShowStableInfo()
{
    using std::cout;
    using std::endl;

    const teamstyle16::GameInfo *info = teamstyle16::Info();

    cout << "x_max: " << info->x_max
         << "\ny_max: " << info->y_max
         << "\nweather: " << info->weather
         << "\nteam_num: " << info->team_num
         << "\npopulation_limit: " << info->population_limit
         << "\nround_limit: " << info->round_limit
         << "\ntime_per_round: " << info->time_per_round
         << "\nmap:\n";

    for (int x = 0; x < info->x_max; x++)
    {
        for (int y = 0; y < info->y_max; y++)
        {
            cout << teamstyle16::Map(x, y);
        }
        cout << endl;
    }
}

void ShowRoundInfo()
{
    using std::cout;
    using std::endl;

    const teamstyle16::GameInfo *info = teamstyle16::Info();

    cout << "round: " << info->round
         << "\nelement_num: " << info->element_num
         << "\npopulation: " << info->population
         << "\nproduction_num: " << info->production_num
         << "\nscores: " << info->scores[0] << ", "
                         << info->scores[1]
         << "\nproduction_list:\n";

    for (int i = 0; i < info->production_num; i++)
    {
        const teamstyle16::ProductionEntry &entry = info->production_list[i];
        cout << "  " << i << ": " << entry.unit_type
                          << ", " << entry.round_left << endl;
    }
    cout << "elements:\n";
    for (int i = 0; i < info->element_num; i++)
    {
        const teamstyle16::State &state = *info->elements[i];
        cout << "  " << i << ": \n";
        ShowElement(state);
    }
}

}  // namespace

const char * GetTeamName()
{
    return "0xFFFFFF";
}

void AIMain()
{
    using namespace teamstyle16;

    std::string line;
    while (true)
    {
        std::cout << "round " << Info()->round << ": ";
        std::getline(std::cin, line);

        if (line == "n" || line == "next")
            Update();
        else if (line == "t" || line == "try")
            TryUpdate();
        else if (line == "s" || line == "sinfo")
            ShowStableInfo();
        else if (line == "r" || line == "rinfo")
            ShowRoundInfo();
        else  // send this line
            Connection::Instance()->Send(line);
    }
}
