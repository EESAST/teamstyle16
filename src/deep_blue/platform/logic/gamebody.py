# -*- coding: UTF-8 -*-
# gamebody.py
from basic import *
from map_info import *
from custom_json import *

STATE_CONTINUE = -1
STATE_TIE = 2

class GameBody(object):
    """docstring for GameBody"""
    def __init__(self, map_info, team_names, **kwargs):
        self.map_info = map_info
        self.team_names = team_names
        self.round = 0
        self.scores = [0, 0]
        self.production_lists = [[], []]
        self.commands = [[], []]
        for kw in ['round', 'scores', 'production_lists', 'commands']:
            if kw in kwargs:
                setattr(self, kw, kwargs[kw])
        self.round = max(self.round, 0)         # in case round < 0
        self.scores = [max(score, 0) for score in self.scores]      # in case score < 0

    @property
    def map(self):
        return self.map_info.types

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
        for element in self.map_info.elements.values():
            if element.kind == BASE and element.health <= 0:
                return 1 - element.team
        if self.round >= self.max_round:
            return 0 if self.score(0) > self.score(1) else (1 if self.score(1) > self.score(0) else STATE_TIE)
        return STATE_CONTINUE

    def team_name(self, team):
        """return name of the team"""
        return self.team_names[team]

    def score(self, team):
        """return score of the team"""
        return self.scores[team]

    def elements(self, team):
        """return elements of the team"""
        our_elements = {}
        for index, element in self.map_info.elements.items():
            if isinstance(element, UnitBase) and element.team == team:
                our_elements[index] = element
        return our_elements

    def vision(self, team):
        """shared 3-level vision of the whole team"""
        vision = [[], [], []]
        for level in xrange(3):
            for element in self.elements(team).values():
                vision[level].extend(element.pos.region(level, element.sight_ranges[level]))
        return vision

    def view_elements(self, perspective):
        """return a dict containing all the elements in team vision"""
        can_see = {}
        vision = self.vision(perspective)
        for index, element in self.map_info.elements.items():
            for point in element.pos.region(element.level, 0):
                if point in vision[element.level]:
                    can_see[index] = element
                    break
        return can_see

    def production_list(self, team):
        """return current production list of the team"""
        return  self.production_lists[team]

    def population(self, team):
        """return total population of the team"""
        return sum([element.population for element in self.elements(team).values() if element.population != None])

    def set_command(self, team, command):
        """add a command and resolve conflicts"""
        pass
    
    def run(self):
        """run one round and return the events took place"""
        self.round = self.round + 1
        events = []
        pass
        return events
                                     
    def set_team_name(self, team, name):
        """set name of the team"""
        self.team_names[team] = name
                                     
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
