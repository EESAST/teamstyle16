# -*- coding: UTF-8 -*-
# gamebody.py
from basic import *
from map_info import *
from custom_json import *
import event

STATE_CONTINUE = -1
STATE_TIE = 2

class TeamInfo(object):
    """team info"""
    def __init__(self, team_index, name, **kwargs):
        self.team_index = team_index  # 0 or 1
        self.name = name 
        self.score = 0
        self.elements = {}
        self.production_list = []
        self.commands = []
        for kw in ['score', 'elements', 'production_list', 'commands']:
            if kw in kwargs:
                setattr(self, kw, kwargs[kw])
        self.score = max(self.score, 0)     # in case score < 0
    
    @property
    def vision(self):
        """shared 3-level vision of the whole team"""
        vision = []
        for level in xrange(3):
            for element in elements.values():
                vision.append(element.pos.region(level, element.sight_ranges[level]))
        return vision

    @property
    def population(self):
        """return total population"""
        return sum([element.population for element in self.elements.values() if element.population != None])

    def saves(self):
        """save TeamInfo to string"""
        tmp = copy(self)
        tmp.elements = {str(index): element for index, element in tmp.elements.items()}     # json does not allow int as a dict key
        return MyEncoder().encode(tmp)
    
def load_team_info(team_str):
    """return team_info from string"""
    team_info = MyDecoder().decode(map_str)
    team_info.elements = {int(index_str): element for index_str, element in team_info.elements.items()}
    return team_info

class GameBody(object):
    """docstring for GameBody"""
    def __init__(self, map_info, team_names = None, **kwargs):
        self.map_info = map_info
        self.round = kwargs['round'] if 'round' in kwargs else 0
        self.teams_info = kwargs['teams_info'] if 'teams_info' in kwargs else [TeamInfo(0, team_names), TeamInfo(1, team_names)]

    @property
    def map(self):
        return self.map_info.types

    @property
    def elements(self):
        return self.map_info.elements

    @property
    def max_population(self):
        return self.map_info.max_population

    @property
    def max_round(self):
        return self.map_info.max_round

    @property
    def record_interval(self):
        return self.map_info.record_interval

    @property
    def time_per_round(self):
        return self.map_info.time_per_round

    @property
    def weather(self):
        return self.map_info.weather

    @property
    def status(self):
        for element in self.elements:
            if element.kind == BASE and element.health <= 0:
                return 1 - element.team
        if self.round >= self.max_round:
            return 0 if self.score(0) > self.score(1) else (1 if self.score(1) > self.score(0) else 2)
        return -1

    def team_name(self, team):
        """return name of the team"""
        return self.teams_info[team].name

    def score(self, team):
        """return score of the team"""
        return self.teams_info[team].score

    def elements(self, team):
        """return elements of the team"""
        return self.teams_info[team].elements

    def view_elements(self, perspective):
        """return a dict containing all the elements in team vision"""
        can_see = {}
        for index, element in self.elements.items():
            for point in element.pos.region(element.level, 0):
                if point in self.teams_info[perspective].vision:
                    can_see[index] = element
                    break
        return can_see

    def production_list(self, team):
        """return current production list of the team"""
        return  self.teams_info[team].production_list

    def population(self, team):
        """return population of the team"""
        return self.teams_info[team].population

    def commands(self, team):
        """return commands of the team"""
        return self.teams_info[team].commands

    def set_command(self, team, command):
        """add a command and resolve conflicts"""
        pass
    
    def run(self):
        """run one round and return the events took place"""
        self.round = self.round + 1
        Event = []
        pass
        return Event
                                     
    def set_team_name(self, team, name):
        """set name of the team"""
        self.teams_info[team].name = name
                                     
    def save(self, filename):
        """Save game to file"""
        open(filename).write(self.saves())

    def saves(self):
        """Save game to string"""
        return MyEncoder().encode(self)

def load(filename):
    """Read map from file"""
    return loads(open(filename).read())

def loads(game_str):
    """Read game from string"""
    return MyDecoder().decode(game_str)
