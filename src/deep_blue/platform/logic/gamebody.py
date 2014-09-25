# -*- coding: UTF-8 -*-
# gamebody.py
from basic import *
import map
import json
import event

#GOD STATE_CONTINUE STATUS_TIE这些东西要怎么定义

class TeamInfo(object):
    """team info"""
    def __init__(self, team, name):
        self.name = name
        self.score = 0
        self.elements = [Base(self.team, rectangle(team))]
        base = self.elements[0]
        self.vision = []
        for i in range(3):
            self.vision.append(base.region(i, base.sight_ranges[i]))
        self.production_list = []
        self.commands = []
        
    def saves(self):
        """save TeamInfo to string"""
        #save score
        team_str = ''
        team_str += json.dumps(self.score)
        team_str += '&\t'
        #save name
        team_str += json.dumps(self.name)
        team_str += '&\t'
        #save elements
        for Element in self.elements: 
            team_str += json.dumps(Element, default = lambda obj: obj.__dict__)
            team_str += '@\t'
        team_str += '&\t'
        #save vision
        for i in [0, 1, 2]:
            team_str += json.dumps(self.vision[i], default = lambda obj: obj.__dict__)
            team_str += '@\t'
        team_str += '&\t'
        #save production_list
        for production in self.produvtion_list:
            team_str += json.dumps(production, default = lambda obj: obj.__dict__)
            team_str += '@\t'
        team_str += '&\t'
        #save commands
        for Command in self.commands:
            team_str += json.dumps(Command, default = lambda obj: obj.__dict__)
            team_str += '@\t'
        return team_str

def dictPos(dct):
    """return pos object from dict"""
    if len(dct) == 3:
        return Position(dct['x'], dct['y'], dct['z'])
    else :
        return Rectangle(dictPos(dct['upper_left']), level = dct['level'], \
                                     dictPos(dct['lower_right']), size = dct['size'])
    
def dictUnitBase(dct):
    """return UnitBase object from dict"""
    return UnitBase(team = dct['team'], type = dct['type'], pos = dictPos(dct['pos']),\
                        sight_ranges = dct['sight_ranges'], fire_ranges = dct['fire_ranges'],\
                        health = dct['health'], fuel = dct['fuel'], ammo = dct['ammo'],\
                        ammo_once = dct['ammo_once'], metal = dct['metal'], attacks = dct['attacks'],\
                        defences = dct['defences'])

def dictUnit(dct):
    """return Unit object from dict"""
    return Unit(None, None, None, None, None, None, None, None, None, None,\
                None, None, None, dictUnitBase(dct['UnitBase']),
                speed = dct['speed'], population = dct['population'], \
                    cost = dct['cost'], dest = dictPos(dct['pos']))

def dictElement(dct):
    """return object from dict"""
    if '__Element__' in dct:
    elif dct['type'] == MINE:
        return Mine(dct['type'], dictPos(dct['pos']), dct['metal'])
    elif dct['type'] == OILFIELD:
        return Mine(dct['type'], dictPos(dct['pos']), dct['fuel'])
    elif dct['type'] == BASE:
        return Base(None, None, None, dictUnitBase(dct))
    elif dct['type'] == FORT:
        return Fort(None, None, None, dictUnitBase(dct))
    elif '__Submarine__' in dct:
        return Submarine(None, None, dictElement(dct))
    elif '__Submarine__' in dct:
        return Submarine(None, None, dictElement(dct))
    elif '__Destroyer__' in dct:
        return Destroyer(None, None, dictElement(dct))
    elif '__Cruiser__' in dct:
        return Cruiser(None, None, dictElement(dct))
    elif '__Battleship__' in dct:
        return Battleship(None, None, dictElement(dct))
    elif '__Carrier__' in dct:
        return Carrier(None, None, dictElement(dct))
    elif '__Cargo__' in dct:
        return Cargo(None, None, dictElement(dct))
    elif '__Formation__' in dct:
        return Formation(None, None, dct['plane_nums'], dictElement(dct.pop('plane_nums')))
    else:
        return None
    
def Load(team_str):
    """return team from string"""
    parts = team_str.split('&\t')
    #load score
    Score = json.loads(parts[0])
    #load name
    Name = json.loads(parts[1])
    #load elements
    part1 = parts[2].split('@\t')
    Elements = []
    for ele_str in part1:
        Elements.append(json.loads(ele_str, \
                                   object_hook = dictElement))
    #load vision
    part2 = parts[3].spilt('@\t')
    Vision = []
    for vision_str in part2   :
        Vision.append(json.loads(vision_str, \
                                   object_hook = dictElement))
    #load production_list
    part3 = parts[4].spilt('@\t')
    Production_list = []
    for production  in part3 :
        Production_list.append(json.loads(production, \
                                   object_hook = dictElement))
    #load commands
    part4 = parts[5].spilt('@\t')
    Commands = []
    for Command  in part4:
        Commands.append(json.loads(Command, \
                                   object_hook = dictElement))
    return TeamInfo(score = Score, name = Name, elements = Elements, \
                    vision = Vision, production_list = Production_list, \
                    commands = Commands)

class GameBody(object):
    """docstring for GameBody"""
    def __init__(self, map, max_population, max_round, team_name):
        self.map = map
        self.max_population = max_population
        self.max_round = max_round
        # self.weather = [- randint(0,3), - randint(0,3), - randint(0,3)]
        #basic 里没有定义weather， 我也感觉weather的意义不大
        self.round = 0
        self.Team = []
        for i in [0,1]:
            self.Team.append(TeamInfo(team_name[i]))
    def map(self):
        """return map"""
        return self.map

    def max_population(self):
        """return current max_population"""
        return self.max_population

    def team_name(self, team):
        """return name of the team"""
        return self.Team[team].name

    def round(self):
        """return current round"""
        return self.round

    def score(self, team):
        """return score of the team"""
        self.Team[team].score

    def elements(self, perspective = GOD):
        """return all the elements in GOD/team vision"""
        if perspective == GOD:
            map.update()
            return ELEMENTS
        else:
            elements = []
            for element in ELEMENTS:
                if element.pos in self.Team[perspective].vision[element.pos.z]:
                    elements.append(element)
            return elements

    def production_list(self, team):
        """return current production list of the team"""
        return  self.Team[team].production_list

    def population(self, team):
        """return population of the team"""
        population = 0
        for element in self.Team[team].elements:
            population += element.population
        return population

    def weather(self):
        """return weather currently"""
        return self.weather

    def commands(self, team):
        """return commands of the team"""
        return self.Team[team].commands

    def set_command(self, team, command):
        """add a command and resolve conflicts"""
        flag = 0
        for Command in self.Team[team].commands:
            if Command.operand == command.operand:
                Command = command
                flag = 1
                break
        if flag == 0:
            self.Team[team].commands.append(command)
        
    def status(self):
        """return current status of the game"""
        for team in [0,1]:
            self.Team[team].elements[0].health <= 0
            return 1-team
        if round == ROUND_MAX:
            if self.Team[0].score == self.Team[1].score:
                return STATE_TIE
            else:
                return 0 if self.Team[0].score > self.Team[1].score else 1
        else:
            return STATE_CONTINUE
    
    def run(self):
        """run one round and return the event taked place"""
        self.round = self.round + 1
        pos = []
        for Element in ELEMENTS:
            if isinstance(Element, Unit):
                pos.append(Element, Element.pos)
        Event = []
        for team in [0,1]:
            for command in self.commands(team):
                Event.extend(command.result_event())
        i=0
        for Element in ELEMENTS:
            if isinstance(Element, Unit):
                for element, Pos in pos[i:]:
                    if Element.index == element.index and Pos != Element.pos:
                        Event.append(event.Move("Move", Element.index, Pos, Element.pos))
                    i = i + 1
        return Event
                                     
    def set_team_name(self, team, name):
        """set name of the team"""
        self.Team[team].name = name
                                     
    def save(self, filename):
        """Save game to file"""
        game_str = self.saves()
        open(filename).write(game_str)
    def saves(self):
        """Save game to string"""
        game_str = ''
        #save map
        game_str += json.dumps(map.saves(self.map))
        game_str += '&\n'
        #save max_population
        game_str += json.dumps(self.max_population)
        game_str += '&\n'
        #save max_round
        game_str += json.dumps(self.max_round)
        game_str += '&\n'
        #save weather
        game_str += json.dumps(self.weather)
        game_str += '&\n'
        #save round
        game_str += json.dumps(self.round)
        game_str += '&\n'
        #save team
        game_str += self.Team[0].saves()
        game_str += '@\n'
        game_str += self.Team[1].saves()
        return game_str

def load(filename):
    """Read map from file"""
    map_str = open(filename).read()
    return loads(map_str)

def loads(game_str):
    """Read game from string"""
    parts = game_str.split('&\n')
    map = map.loads(parts[0])                                 
    max_population = json.loads(parts[1])                                 
    max_round = json.loads(parts[2])                             
    weather = json.loads(parts[3])                                     
    round = json.loads(parts[4])
    parts = parts[5].split('@\n')
    Team0 = Load(parts[0])
    Team1 = Load(parts[1])
    return GameBody(map, max_population, max_round, weather, round, Team = [Team0, Team1])
    
            
