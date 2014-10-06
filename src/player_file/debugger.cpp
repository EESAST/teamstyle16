#include <iostream>
#include <string>

#include <boost/algorithm/string.hpp>
#include "basic.h"
#include "connection.h"

namespace {

std::string PosString(const teamstyle16::Position &pos)
{
    using std::to_string;
    return "(" + to_string(pos.x) + ", "
               + to_string(pos.y) + ", "
               + to_string(pos.z) + ")";
}

void ShowElement(const teamstyle16::State &state)
{
    using std::cout;
    using std::endl;

    cout << "    index: " << state.index
         << "\n    pos: " << PosString(state.pos)
         << "\n    type: " << state.type
         << "\n    team: " << state.team
         << "\n    visible: " << state.visible
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

    auto info = teamstyle16::Info();

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

    auto info = teamstyle16::Info();

    cout << "round: " << info->round
         << "\nelement_num: " << info->element_num
         << "\npopulation: " << info->population
         << "\nproduction_num: " << info->production_num
         << "\nscores: " << info->scores[0] << ", "
                         << info->scores[1]
         << "\nproduction_list:\n";

    for (int i = 0; i < info->production_num; i++)
    {
        auto &entry = info->production_list[i];
        cout << "  " << i << ": " << entry.unit_type
                          << ", " << entry.round_left << endl;
    }
    cout << "elements:\n";
    for (int i = 0; i < info->element_num; i++)
    {
        auto &state = info->elements[i];
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
        boost::trim(line);
        boost::to_lower(line);

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
