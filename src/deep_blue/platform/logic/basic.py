# -*- coding: UTF-8 -*-
# basic.py

# 缩进: 4空格
# 命名: (参考 Google style)
# 类名: CapWords
# 模块名: lower_with_under
# 函数或方法名: firstLowerCapWords()
# 常量名: CAPS_WITH_UNDER
# 其他变量, 实例及函数形参: lower_with_under

# 变量名开头加一个下划线(_)能对保护模块(protected)中的变量及函数提供一些支持(不会被 import * from 导入)
# 在实例的变量和方法开头加两个下划线(__)能有效地帮助把该变量或方法变成类的私有内容(using name mangling)

# 以下所有数据暂时并无理论依据...

from copy import deepcopy
from math import ceil
from random import random, choice
from Queue import Queue
from event import *
import json

# 基础参数限制
ROUND_MAX = 500     # 最大回合数
MAP_SIZE_MAX = 100   # 地图最大边长
FORT_NUM_MAX = 10    # 据点最大数量
MINE_NUM_MAX = 12   # 矿场最大数量
OILFIELD_NUM_MAX = 12    # 油田最大数量
POPULATION_MAX = 60    # 单方最大人口数
INFINITY = float('inf')     # 正无穷, 大于任何有限数

# 积分规则
FORT_SCORE = 1      # 占领据点每回合奖励积分
DAMAGE_SCORE = 1    # 每点伤害奖励积分
COLLECT_SCORE = 1   # 采集一单位资源奖励积分

# 维修代价
METAL_PER_HEALTH = 0.2    # 恢复1点生命所需金属

# 补给底线
SUPPLY_LIMIT = 0.1  # 资源数少于 SUPPLY_LIMIT * 上限 即不可继续补给(留给自己用..)(抛锚不可以么？）
                    # 基地, 据点燃料不设底线
                    # 基地弹药无限, 故也不必设底线
                    # 运输舰无攻击能力, 弹药不设底线


# 地图分层
UNDERWATER = 0  # 水下
SURFACE = 1     # 水面 or 地面
AIR = 2         # 空中


# 伤害类型
""" 潜艇只能造成和接受鱼雷伤害
    陆地建筑只能造成和受到火力伤害
    飞机不能受到鱼雷伤害 """
FIRE = 0        # 火力伤害
TORPEDO = 1     # 鱼雷伤害


# 地形
OCEAN = 0       # 海洋
LAND = 1        # 陆地


# ElementType (与basic.h保持一致)
# 建筑
BASE = 0        # 基地
FORT = 1        # 据点
# 资源点
MINE = 2        # 矿场
OILFIELD = 3    # 油田
# 可移动单位(unit)
SUBMARINE = 4   # 潜艇
DESTROYER = 5   # 驱逐舰   
CARRIER = 6     # 航母
CARGO = 7       # 运输舰
FIGHTER = 8     # 战斗机
SCOUT = 9       # 侦察机

names = {
    SUBMARINE: '潜艇', DESTROYER: '驱逐舰', 
    CARRIER: '航空母舰', CARGO: '运输舰',
    FIGHTER: '战斗机', SCOUT: '侦察机'
}

# ElementProperty
"""sight_ranges = [UNDERWATER, SURFACE, AIR]
   fire_ranges = [UNDERWATER, SURFACE, AIR]
   attacks = [FIRE, TORPEDO]
   defences = [FIRE, TORPEDO] """

DEFAULT_PROPERTY = [{
            'sight_ranges': [4, 10, 8], 'fire_ranges': [0, 7, 5],
            'health_max': 2000, 'fuel_max': 1000, 'ammo_max': INFINITY, 'ammo_once': 6, 'metal_max': 200,
            'speed': None, 'population': None, 'cost': None, 'build_round': None,
            'attacks': [40, 0], 'defences': [15, INFINITY]
            },  # Base
            {
            'sight_ranges': [3, 8, 6], 'fire_ranges': [0, 5, 4],
            'health_max': 800, 'fuel_max': 200, 'ammo_max': 300, 'ammo_once': 4, 'metal_max': 200,
            'speed': None, 'population': None, 'cost': None, 'build_round': None,
            'attacks': [25, 0], 'defences': [12, INFINITY]
            },  # Fort
            {
            'sight_ranges': None, 'fire_ranges': None,
            'health_max': INFINITY, 'fuel_max': None, 'ammo_max': None, 'metal_max': 1000,
            'speed': None, 'population': None, 'cost': None, 'build_round': None,
            'attacks': None, 'ammo_once': None, 'defences': [INFINITY, INFINITY]
            },  # Mine
            {
            'sight_ranges': None, 'fire_ranges': None,
            'health_max': INFINITY, 'fuel_max': 1000, 'ammo_max': None, 'ammo_once': None, 'metal_max': None,
            'speed': None, 'population': None, 'cost': None, 'build_round': None,
            'attacks': None, 'defences': [INFINITY, INFINITY]
            },  # Oilfield
            {
            'sight_ranges': [6, 5, 3], 'fire_ranges': [5, 5, 0],
            'health_max': 35, 'fuel_max': 120, 'ammo_max': 20, 'ammo_once': 2, 'metal_max': None,
            'speed': 6, 'population': 2, 'cost': 7, 'build_round': 2,
            'attacks': [0, 40], 'defences': [INFINITY, 7]
            },  # Submarine
            {
            'sight_ranges': [4, 9, 7], 'fire_ranges': [2, 8, 6],
            'health_max': 70, 'fuel_max': 150, 'ammo_max': 40, 'ammo_once': 4, 'metal_max': None,
            'speed': 7, 'population': 3, 'cost': 14, 'build_round': 3,
            'attacks': [22, 11], 'defences': [13, 10]
            },  # Destroyer
            {
            'sight_ranges': [4, 9, 9], 'fire_ranges': [0, 8, 6],
            'health_max': 120, 'fuel_max': 200, 'ammo_max': 70, 'ammo_once': 2, 'metal_max': None,
            'speed': 5, 'population': 4, 'cost': 24, 'build_round': 5,
            'attacks': [18, 0], 'defences': [16, 10]
            },  # Carrier
            {
            'sight_ranges': [3, 7, 6], 'fire_ranges': None,
            'health_max': 60, 'fuel_max': 300, 'ammo_max': 120, 'ammo_once': None, 'metal_max': 50,
            'speed': 8, 'population': 1, 'cost': 12, 'build_round': 3,
            'attacks': None, 'defences': [15, 8]
            },  # Cargo
            {
            'sight_ranges': [0, 9, 10], 'fire_ranges': [0, 3, 4],
            'health_max': 70, 'fuel_max': 100, 'ammo_max': 21, 'ammo_once': 3, 'metal_max': None,
            'speed': 9, 'population': 3, 'cost': 14, 'build_round': 3,
            'attacks': [30, 10], 'defences': [10, INFINITY]
            },  # Fighter
            {
            'sight_ranges': [2, 12, 16], 'fire_ranges': [1, 3, 4],
            'health_max': 50, 'fuel_max': 120, 'ammo_max': 5, 'ammo_once': 1, 'metal_max': None,
            'speed': 10, 'population': 1, 'cost': 10, 'build_round': 2,
            'attacks':[10, 5], 'defences': [7, INFINITY]
            }]  # Scout

# get property from configure file
NAMES = ['Base', 'Fort', 'Mine', 'Oilfield', 'Submarine', 'Destroyer', 'Carrier', 'Cargo', 'Fighter', 'Scout'];
OBJ = json.loads(open('property.conf', 'r').read())
PROPERTY = [OBJ[NAMES[i]] for i in range(10)]

# cannot be changed
CONSTANT = {
    BASE: ['ammo_max', 'build_round', 'cost', 'population', 'speed']
    CARGO: ['ammo_once', 'attacks', 'fire_ranges']
    CARRIER: ['metal_max']
    DESTROYER: ['metal_max']
    FIGHTER: ['metal_max']
    FORT: ['build_round', 'cost', 'population', 'speed']
    SCOUT: ['metal_max']
    SUBMARINE: ['metal_max']
    }

for index, list in CONSTANT:
    for str in list:
        PROPERTY[index][str] = DEFAULT_PROPERTY[index][str]

TMP = PROPERTY[MINE]['metal_max']
PROPERTY[MINE] = DEFAULT_PROPERTY[MINE]
PROPERTY[MINE]['metal_max'] = TMP
TMP = PROPERTY[OILFIELD]['fuel_max']
PROPERTY[OILFIELD] = DEFAULT_PROPERTY[OILFIELD]
PROPERTY[OILFIELD]['fuel_max'] = TMP


# 攻击修正
def modifiedAttacks(distance, fire_range, attacks):
    """返回距离修正后的火力和鱼雷攻击力"""
    return [int((1 - float(distance - fire_range / 2) / (fire_range + 1)) * attack)
            for attack in attacks]    # 可能大于attack

# 对象

class Position(object):
    """三维坐标"""
    def __init__(self, x, y, z = 1):
        super(Position, self).__init__()
        self.x = x
        self.y = y
        self.z = z

    def __eq__(self, other):
        """判断两Position实例相等"""
        if isinstance(other, Position):
            return (self.x == other.x) and (self.y == other.y) and (self.z == other.z)
        else:
            return False

    def __hash__(self):
        return hash((Position, self.x, self.y, self.z))

    def __repr__(self):
        return '<Position (%d, %d, %d)>' % (self.x, self.y, self.z)

    def __str__(self):
        return '(%d, %d, %d)' % (self.x, self.y, self.z)

    @property
    def level(self):
        return self.z

    @level.setter
    def level(self, value):
        self.z = min(max(0, value), 2)

    @property
    def size(self):
        return (1, 1)

    def inMap(self, map):
        return (self.x >= 0 and self.x < map.x_max and
                self.y >= 0 and self.y < map.y_max and
                self.z >= 0 and self.z < 3)

    def distance(self, target):
        """返回该位置到target(点或矩形)的(最小)距离"""
        if isinstance(target, Position):
            return abs(target.x - self.x) + abs(target.y - self.y)
        elif isinstance(target, Rectangle):
            return target.distance(self)    # 调用Rectangle.distance()
        else:
            return -1;

    def region(self, level, range):
        """返回距离该位置range以内区域点集list"""
        region_points = []
        for i in xrange(- range, range + 1):
            region_points += [Position(self.x + i, self.y + j, level) for j in xrange(- range + abs(i), range - abs(i) + 1)]
        return region_points


class Rectangle(object):
    """平面矩形区域, 由左上及右下两顶点坐标确定"""
    def __init__(self, upper_left, lower_right):
        super(Rectangle, self).__init__()
        self.upper_left = upper_left
        lower_right.level = upper_left.level
        self.lower_right = lower_right

    def __repr__(self):
        size = self.size
        return '<Rectangle %s %d*%d>' % (self.upper_left, size[0], size[1])

    def __str__(self):
        size = self.size
        return '%s, %d*%d' % (self.upper_left, size[0], size[1])

    @property
    def size(self):
        return (abs(self.upper_left.x - self.lower_right.x) + 1, abs(self.upper_left.y - self.lower_right.y) + 1)

    @property
    def x(self):
        return self.upper_left.x

    @property
    def y(self):
        return self.upper_left.y

    @property
    def z(self):
        return self.upper_left.z

    @property
    def level(self):
        return self.upper_left.level

    @level.setter
    def level(self, value):
        self.upper_left.level = self.lower_right.level = value

    def bound(self):
        """返回矩形区域的边界点集list"""
        bound_points = []
        for x in xrange(self.upper_left.x, self.lower_right.x + 1):
            bound_points.append(Position(x, self.upper_left.y, self.level))
            bound_points.append(Position(x, self.lower_right.y, self.level))
        for y in xrange(self.upper_left.y + 1, self.lower_right.y):
            bound_points.append(Position(self.upper_left.x, y, self.level))
            bound_points.append(Position(self.lower_right.x, y, self.level))
        return bound_points

    def distance(self, target):
        """返回该矩形区域到target(点或矩形)的最小距离"""
        if isinstance(target, Position):
            return (min([point.distance(target) for point in self.bound()])
                    if not target in self.region(target.level) else 0)
        elif isinstance(target, Rectangle):
            return min([self.distance(point) for point in target.bound()])
        else:
            return -1

    def region(self, level, range = 0):
        """返回矩形区域向外延伸range范围的区域点集list"""
        region_points = []
        for i in xrange(self.x, self.x + self.size[0]):
            for j in xrange(self.y, self.y + self.size[1]):
                region_points.append(Position(i, j, level))
        if range == 0:
            return region_points
        region_points += Rectangle(Position(self.x, self.y - range), Position(self.lower_right.x, self.y - 1)).region(level, 0)
        region_points += Rectangle(Position(self.lower_right.x + 1, self.y), Position(self.lower_right.x + range, self.lower_right.y)).region(level, 0)
        region_points += Rectangle(Position(self.x, self.lower_right.y + 1), Position(self.lower_right.x, self.lower_right.y + range)).region(level, 0)
        region_points += Rectangle(Position(self.x - range, self.y), Position(self.x - 1, self.lower_right.y)).region(level, 0)
        region_points += self.upper_left.region(level, range)
        region_points += Position(self.lower_right.x, self.y).region(level, range)
        region_points += self.lower_right.region(level, range)
        region_points += Position(self.x, self.lower_right.y).region(level, range)
        return region_points

class Element(object):
    """所有地图元素, 派生出资源类和作战单位(UnitBase)类"""
    kind = None     # 类属性
    def __init__(self, pos, **kwargs):
        super(Element, self).__init__()
        self.pos = pos          # pos可以是一个点(Position类型), 也可以是矩形(Rectangle类型)
        self.index = kwargs['index'] if 'index' in kwargs else None       # 调用MapInfo.addElement()才会赋予相应的index值

    @property
    def level(self):
        return self.pos.level

    @property
    def position(self):
        return self.pos if isinstance(self.pos, Position) else self.pos.upper_left

    @property
    def size(self):
        return self.pos.size

    def ghost(self):
        """return limited info when viewed by enemy"""
        ghost = deepcopy(self)
        ghost.fuel = ghost.ammo = ghost.metal = 0   # no access
        return ghost

class Resource(Element):
    """资源类, 派生出矿场类和油田类"""
    def __repr__(self):
        return '<%s at %s>' % (self.__class__.__name__, self.pos)

class Mine(Resource):
    """矿场"""
    kind = MINE
    def __init__(self, pos, metal = PROPERTY[MINE]['metal_max'], **kwargs):
        pos.level = SURFACE
        super(Mine, self).__init__(pos, **kwargs)
        self.metal = metal

    def globalGhost(self):
        ghost = deepcopy(self)
        ghost.metal = 0
        return ghost

    def ghost(self):
        return self

class Oilfield(Resource):
    """油田"""
    kind = OILFIELD
    def __init__(self, pos, fuel = PROPERTY[OILFIELD]['fuel_max'], **kwargs):
        pos.level = SURFACE
        super(Oilfield, self).__init__(pos, **kwargs)
        self.fuel = fuel

    def globalGhost(self):
        ghost = deepcopy(self)
        ghost.fuel = 0
        return ghost

    def ghost(self):
        return self

class UnitBase(Element):
    """作战单位抽象, 派生出建筑类以及可移动单位类"""
    def __init__(self, team, pos, sight_ranges, fire_ranges,
                 health_max, fuel_max, ammo_max, ammo_once, metal_max,
                 speed, population, cost, build_round,
                 attacks, defences, **kwargs):
        super(UnitBase, self).__init__(pos, **kwargs)
        self.team = team
        self.sight_ranges = sight_ranges
        self.fire_ranges = fire_ranges
        self.health = self.health_max = health_max
        self.fuel = self.fuel_max = fuel_max
        self.ammo = self.ammo_max = ammo_max
        self.ammo_once = ammo_once
        self.metal = self.metal_max = metal_max
        self.speed = speed
        self.population = population
        self.cost = cost
        self.build_round = build_round
        self.attacks = attacks
        self.defences = defences
        for kw in ['health', 'fuel', 'ammo', 'metal']:
            if kw in kwargs:
                maximum = getattr(self, kw + '_max')
                setattr(self, kw, min(max(0, kwargs[kw]), maximum))     # in case health < 0 or health > health_max

    def __repr__(self):
        return '<team %s %s at %s>' % (self.team, self.__class__.__name__, self.pos)

    def attack(self, game, target_pos):
        """攻击(火力与鱼雷伤害叠加计算)某坐标(可能出现射程大于视野的情况)"""
        distance = self.pos.distance(target_pos)
        range = self.fire_ranges[target_pos.z]
        target_unit = game.map_info.element(target_pos)
        result_events = []
        self.ammo -= self.ammo_once
        if target_unit == None or hasattr(target_unit, 'team') is False or target_unit.team == self.team:
            result_events.append(AttackMiss(self.index, target_pos))    # 坐标不存在敌军单位, miss
            return result_events
        modified_attacks = modifiedAttacks(distance, range, self.attacks)
        fire_damage = max(0, modified_attacks[FIRE] - target_unit.defences[FIRE])
        # 考虑到 defence = INFINITY 可能无法破防
        torpedo_damage = max(0, modified_attacks[TORPEDO] - target_unit.defences[TORPEDO])
        damage = fire_damage + torpedo_damage
        game.scores[self.team] += damage * DAMAGE_SCORE
        result_events.append(AttackUnit(self.index, target_unit.index, damage))
        if damage >= target_unit.health:
            if target_unit.kind == FORT:    # capture fort
                target_unit.team = self.team
                target_unit.health = target_unit.health_max
                result_events.append(Capture(target_unit.index, self.team))
            else:
                target_unit.health = 0  # killed
                del game.map_info.elements[target_unit.index]
                result_events.append(Destroy(target_unit.index))
        else:
            target_unit.health -= damage
        return result_events


def replenishFuelAmmo(giver, receiver, fuel, ammo):   # 补给燃料弹药
    if giver.kind == BASE:
        fuel_supply_limit = ammo_supply_limit = 0
    elif giver.kind == FORT:
        fuel_supply_limit = 0
        ammo_supply_limit = SUPPLY_LIMIT
    elif giver.kind == CARGO:
        fuel_supply_limit = SUPPLY_LIMIT
        ammo_supply_limit = 0
    elif receiver.kind == BASE:
        fuel_supply_limit = SUPPLY_LIMIT
        ammo_supply_limit = 1
    else:
        fuel_supply_limit = ammo_supply_limit = SUPPLY_LIMIT
    provides = [0, 0]    # 维修者提供的燃料, 弹药
    provides[0] = min(fuel, int(giver.fuel - giver.fuel_max * fuel_supply_limit),
                            receiver.fuel_max - receiver.fuel)
    if giver.ammo == INFINITY:
        provides[1] = min(ammo, receiver.ammo_max - receiver.ammo)
    else:
        provides[1] = min(ammo, max(0, int(giver.ammo - giver.ammo_max * ammo_supply_limit)),
                            receiver.ammo_max - receiver.ammo)
    giver.fuel -= provides[0]
    giver.ammo -= provides[1]
    receiver.fuel += provides[0]
    receiver.ammo += provides[1]
    return provides


class Building(UnitBase):
    """建筑类"""
    def supply(self, our_unit, fuel = INFINITY, ammo = INFINITY, metal = INFINITY):   # 补给操作
        """建筑对周围单位补给"""
        result_events = []
        provides = replenishFuelAmmo(self, our_unit, fuel, ammo)
        provide_metal = 0
        if our_unit.metal_max != None:
            provide_metal = min(metal, self.metal, our_unit.metal_max - our_unit.metal)
            self.metal -= provide_metal
            our_unit.metal += provide_metal            
        result_events.append(Supply(self.index, our_unit.index, provides[0], provides[1], provide_metal))
        return result_events

class Base(Building):
    """基地, 继承自Building"""
    kind = BASE
    def __init__(self, team, pos, **kwargs):
        pos.level = SURFACE
        d = PROPERTY[BASE].copy()
        d.update(kwargs)
        super(Base, self).__init__(team, pos, **d)

    def globalGhost(self):
        ghost = deepcopy(self)
        ghost.fuel = ghost.ammo = ghost.metal = 0
        return ghost

    def repair(self, our_unit):
        """维修"""
        result_events = []

        provide_metal = min(self.metal, int(ceil((our_unit.health_max - our_unit.health) * METAL_PER_HEALTH)))
        self.metal -= provide_metal
        new_health = min(our_unit.health_max,
                         our_unit.health + int(provide_metal / METAL_PER_HEALTH))
        result_events.append(Fix(self.index, our_unit.index, provide_metal, new_health - our_unit.health))
        our_unit.health = new_health

        result_events += self.supply(our_unit)
        return result_events


class Fort(Building):
    """据点, 继承自Building"""
    kind = FORT
    def __init__(self, team, pos, **kwargs):
        pos.level = SURFACE
        d = PROPERTY[FORT].copy()
        d.update(kwargs)
        super(Fort, self).__init__(team, pos, **d)

    def globalGhost(self):
        ghost = deepcopy(self)
        ghost.team = 2  # Hide team info
        ghost.health = ghost.fuel = ghost.ammo = ghost.metal = 0
        return ghost

class Unit(UnitBase):
    """可移动单位"""
    def __init__(self, team, pos, sight_ranges, fire_ranges,
                 health_max, fuel_max, ammo_max, ammo_once, metal_max,
                 speed, population, cost, build_round,
                 attacks, defences, **kwargs):
        super(Unit, self).__init__(team, pos, sight_ranges, fire_ranges,
                                   health_max, fuel_max, ammo_max, ammo_once, metal_max,
                                   speed, population, cost, build_round,
                                   attacks, defences, **kwargs)
        self.dest = kwargs['dest'] if 'dest' in kwargs else self.pos    # 目的地(初始为自身位置)

    def ghost(self):
        ghost = deepcopy(self)
        ghost.fuel = ghost.ammo = ghost.metal = 0   # no access
        ghost.dest = None   # no access
        return ghost

    def move(self, game):
        events = []
        # cover = 0 # 走过的长度
        nodes = game.map_info.pathfinding(self.pos, self.dest, isinstance(self, Plane))

        if isinstance(self, Plane) and self.fuel <= 0:  # plane crashed
            self.health = 0
            del game.map_info.elements[self.index]
            events.append(Destroy(self.index))
            return events

        if self.fuel <= 0:  # cannot move
            return events

        # for i in range(len(nodes) - 1):
        #     can_move = self.speed - cover # 剩余的路程
        #     if(can_move > nodes[i].distance(nodes[i + 1])):
        #         cover += nodes[i].distance(nodes[i + 1])
        #     elif can_move == nodes[i].distance(nodes[i + 1]):
        #         nodes = nodes[0 : (i + 2)]
        #         break
        #     else:
        #         nodes = nodes[0 : (i + 2)]
        #         flag = 1 if (nodes[i].x + nodes[i].y) < (nodes[i + 1].x + nodes[i + 1].y) else -1
        #         if nodes[i].x == nodes[i + 1].x:
        #             nodes.append(Position(nodes[i].x + flag * can_move, nodes[i].y, nodes[i].z))
        #             break
        #         elif nodes[i].y == nodes[i + 1].y:
        #             nodes.append(Position(nodes[i].x, nodes[i].y + flag * can_move, nodes[i].z))
        #             break
        #         else:
        #             raise RuntimeError()
        if self.speed + 1 < len(nodes):
            nodes = nodes[:self.speed + 1]
        self.pos = nodes[-1]
        move_event = Move(self.index, nodes)
        events += [] if move_event.steps == 0 else [move_event]
        if self.kind == CARGO:
            for point in self.pos.region(level = SURFACE, range = 1):
                near_element = game.map_info.element(point)
                if isinstance(near_element, Resource):
                    collect_event = self.collect(near_element)
                    events += collect_event
                    game.scores[self.team] += (max(collect_event[0].fuel, collect_event[0].metal) * COLLECT_SCORE
                        if len(collect_event) != 0 else 0)

        # fuel
        if isinstance(self, Plane):
            self.fuel -= max(1, len(nodes) - 1)     # plane consumes at least 1 unit fuel each movement
        else:
            self.fuel -= len(nodes) - 1
        self.fuel = max(0, self.fuel)

        return events

class Submarine(Unit):
    """潜艇"""
    kind = SUBMARINE
    def __init__(self, team, pos, **kwargs):
        pos.level = UNDERWATER
        d = PROPERTY[SUBMARINE].copy()
        d.update(kwargs)
        super(Submarine, self).__init__(team, pos, **d)

class Ship(Unit):
    """水面舰"""
    pass

class Destroyer(Ship):
    """驱逐舰"""
    kind = DESTROYER
    def __init__(self, team, pos, **kwargs):
        pos.level = SURFACE
        d = PROPERTY[DESTROYER].copy()
        d.update(kwargs)
        super(Destroyer, self).__init__(team, pos, **d)

class Carrier(Ship):
    """航母"""
    kind = CARRIER
    def __init__(self, team, pos, **kwargs):
        pos.level = SURFACE
        d = PROPERTY[CARRIER].copy()
        d.update(kwargs)
        super(Carrier, self).__init__(team, pos, **d)

    def supply(self, our_unit, fuel = INFINITY, ammo = INFINITY, metal = INFINITY):
        """航母对周围单位补给燃料弹药, 可向飞机补给"""
        result_events = []
        provides = replenishFuelAmmo(self, our_unit, fuel, ammo)
        result_events.append(Supply(self.index, our_unit.index, provides[0], provides[1], 0))
        return result_events

class Cargo(Ship):
    """运输舰"""
    kind = CARGO
    def __init__(self, team, pos, **kwargs):
        pos.level = SURFACE
        d = PROPERTY[CARGO].copy()
        d.update(kwargs)
        super(Cargo, self).__init__(team, pos, **d)

    def supply(self, our_unit, fuel = INFINITY, ammo = INFINITY, metal = INFINITY):
        """运输舰对周围单位补给燃料弹药, 可向基地, 据点, 运输舰补充金属"""
        result_events = []
        provides = replenishFuelAmmo(self, our_unit, fuel, ammo)
        provide_metal = 0
        if our_unit.metal_max != None:
            provide_metal = min(metal, self.metal, our_unit.metal_max - our_unit.metal)
            self.metal -= provide_metal
            our_unit.metal += provide_metal
        result_events.append(Supply(self.index, our_unit.index, provides[0], provides[1], provide_metal))
        return result_events

    def collect(self, resource):
        """运输舰从资源点采集资源"""
        if resource.kind == MINE and resource.metal > 0:
            supply = min(self.metal_max - self.metal, resource.metal)
            self.metal += supply
            resource.metal -= supply
            return [Collect(self.index, resource.index, 0, supply)]
        elif resource.kind == OILFIELD and resource.fuel > 0:
            supply = min(self.fuel_max - self.fuel, resource.fuel)
            self.fuel += supply
            resource.fuel -= supply
            return [Collect(self.index, resource.index, supply, 0)]
        else:
            return []

class Plane(Unit):
    """飞机"""
    pass

class Fighter(Plane):
    """战斗机"""
    kind = FIGHTER
    def __init__(self, team, pos, **kwargs):
        pos.level = AIR
        d = PROPERTY[FIGHTER].copy()
        d.update(kwargs)
        super(Fighter, self).__init__(team, pos, **d)

class Scout(Plane):
    """侦察机"""
    kind = SCOUT
    def __init__(self, team, pos, **kwargs):
        pos.level = AIR
        d = PROPERTY[SCOUT].copy()
        d.update(kwargs)
        super(Scout, self).__init__(team, pos, **d)
