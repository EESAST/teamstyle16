# -*- coding: UTF-8 -*-
# gamebody.py
from basic import *
from map_info import *
from custom_json import *
from copy import copy

STATE_CONTINUE = -1
STATE_TIE = 2

class GameBody(object):
    """docstring for GameBody"""
    def __init__(self, map_info, **kwargs):
        self.map_info = map_info
        self.round = 0
        self.scores = [0, 0]
        self.populations = [sum([element.population for element in self.elements(0).values() if element.population != None]),
                            sum([element.population for element in self.elements(1).values() if element.population != None])]
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
        dead_bases = 0
        rst = None
        for element in self.map_info.elements.values():
            if element.kind == BASE and element.health <= 0:
                dead_bases += 1
                rst = 1 - element.team
        if dead_bases == 1:
            return rst
        if self.round >= self.max_round or dead_bases == 2:
            return 0 if self.score(0) > self.score(1) else (1 if self.score(1) > self.score(0) else STATE_TIE)
        return STATE_CONTINUE

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
            vision[level] = set(vision[level])
            tmp = []
            for point in vision[level]:
                if (point.x >= 0 and point.x < self.map_info.x_max and
                    point.y >= 0 and point.y < self.map_info.y_max and
                    point.z >= 0 and point.z < 3):
                    tmp.append(point)
            vision[level] = set(tmp)
        return vision

    def view_elements(self, perspective):
        """return a dict containing all the elements in team vision"""
        can_see = {}
        vision = self.vision(perspective)
        for index, element in self.map_info.elements.items():
            if element.kind == BASE or element.kind == FORT or element.kind == MINE or element.kind == OILFIELD:
                tmp = copy(element)
                setattr(tmp, 'visible', False)        # in fact, this element is not in sight
                can_see[index] = tmp.globalGhost()
            for point in element.pos.region(element.level, 0):
                if point in vision[element.level]:
                    if hasattr(element, 'team') is False or element.team != perspective:     # 非己方
                        can_see[index] = element.ghost()
                    else:
                        can_see[index] = element
                    break
        return can_see

    def production_list(self, team):
        """return current production list of the team"""
        return  self.production_lists[team]

    def population(self, team):
        """return total population of the team"""
        return self.populations[team]

    def set_command(self, team, command):
        """add a command and resolve conflicts"""
        operator = self.map_info.elements[command.operand]
        if team != operator.team:
            return False
        command.add_to(self)
        return True

    def run(self):
        """run one round and return the events took place"""
        self.round = self.round + 1
        events = []
        pass
        return events

    def save(self, filename):
        """Save game to file"""
        open(filename).write(self.saves())

    def saves(self):
        """Save game to string"""
        return MyEncoder(sort_keys=True, separators=(',', ':')).encode(self)

def load(filename):
    """Read map from file"""
    return loads(open(filename).read())

def loads(game_str):
    """Read game from string"""
    return MyDecoder().decode(game_str)
