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

from copy import copy
from random import random, choice

# 基础参数限制
ROUND_MAX = 500     # 最大回合数
MAP_SIZE_MAX = 100   # 地图最大边长
FORT_NUM_MAX = 10    # 据点最大数量
MINE_NUM_MAX = 12   # 矿场最大数量
OILFIELD_NUM_MAX = 12    # 油田最大数量
POPULATION_MAX = 60    # 单方最大人口数
INFINITY = float('inf')     # 正无穷, 大于任何有限数


SCORE = [0, 0]      # 两队积分
COMMANDS = [[], []] #两队指令
WEATHER = 0         # 天气

#积分规则
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


# ElementProperty
""" property = (sight_ranges, fire_ranges,
                health_max, fuel_max, ammo_max, ammo_once, metal_max,
                speed, population,
                attacks, defences)
    sight_ranges = [UNDERWATER, SURFACE, AIR]
    fire_ranges = [UNDERWATER, SURFACE, AIR]
    attacks = [FIRE, TORPEDO]
    defences = [FIRE, TORPEDO] """
# property枚举, 可简化属性的获取: 如基地的生命值上限为 PROPERTY[BASE][HEALTH_MAX]
SIGHT_RANGES = 0
FIRE_RANGES = 1
HEALTH_MAX = 2
FUEL_MAX = 3
AMMO_MAX = 4
AMMO_ONCE = 5
METAL_MAX = 6
SPEED = 7
POPULATION = 8
ATTACKS = 9
DEFENCES = 10

PROPERTY = [([4, 10, 8], [0, 7, 5], 
             2000, 1000, INFINITY, 6, 200, 
             None, None, 
             [40, 0], [15, INFINITY]),      # 基地
            ([3, 8, 6], [0, 5, 4], 
             800, 200, 300, 4, 200, 
             None, None, 
             [25, 0], [12, INFINITY]),      # 据点
            (None, None,
             INFINITY, None, None, None, 1000,
             None, None, 
             None, [INFINITY, INFINITY]),   # 矿场
            (None, None,
             INFINITY, 1000, None, None, None,
             None, None,
             None, [INFINITY, INFINITY]),   # 油田
            ([6, 5, 3], [5, 5, 0], 
             35, 120, 20, 2, None, 
             6, 2,
             [0, 40], [INFINITY, 7]),       # 潜艇
            ([4, 9, 7], [2, 8, 6], 
             70, 150, 40, 4, None, 
             7, 3,
             [22, 11], [13, 10]),           # 驱逐舰
            ([4, 9, 9], [0, 8, 6], 
             120, 200, 70, 2, None, 
             5, 4,
             [18, 0], [16, 10]),             # 航母
            ([3, 7, 6], None, 
             60, 300, 120, None, 50, 
             8, 1,
             None, [15, 8]),                # 运输舰
            ([0, 9, 10], [0, 3, 4],
             70, 100, 21, 3, None,
             9, 3,
             [30, 10], [10, INFINITY]),     # 战斗机
            ([2, 12, 16], [1, 3, 4],
             50, 120, 5, 1, None,
             10, 1,
             [10, 5], [7, INFINITY])]       # 侦察机

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

    @property
    def level(self):
        return self.z

    @property
    def size(self):
        return (1, 1)

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
        for y in xrange(self.y - range, self.y + range + 1):
            for x in xrange(self.x - range, self.x + range + 1):
                if self.distance(Position(x, y, level)) <= range:
                    region_points.append(Position(x, y, level))
        # 在一矩形范围内寻找符合条件的点
        return region_points


class Rectangle(object):
    """平面矩形区域, 由左上及右下两顶点坐标确定"""
    def __init__(self, upper_left, lower_right):
        super(Rectangle, self).__init__()
        self.upper_left = upper_left
        lower_right.z = upper_left.z
        self.lower_right = lower_right

    @property
    def size(self):
        return (abs(self.upper_left.x - self.lower_right.x), abs(self.upper_left.y - self.lower_right.y))

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
        for y in xrange(self.upper_left.y, self.lower_right.y + 1):
            for x in xrange(self.upper_left.x, self.lower_right.x + 1):
                region_points.append(Position(x, y, level))    # region_points = 矩形区域内所有点构成的list
        if range == 0:
            return region_points
        else:
            for point in self.bound():
                for pos in point.region(level, range):
                    if pos not in region_points:
                        region_points.append(pos)
            return region_points

ELEMENTS = {}   # 地图上所有元素的字典 { int:Element }

def appendElement(new_element):
    """向ELEMENTS字典中加入新element, 返回新element的索引号(随机生成索引号, 防止选手根据索引号推测对方生产的单位数)"""
    index = choice(xrange(1000))    # 随机范围1000以内
    while index in ELEMENTS.keys(): # 是否与已有index冲突
        index += 1
    ELEMENTS[index] = new_element
    return index

def getElement(pos):
    for element in ELEMENTS.values():
        if element.pos == pos:
            return element
    return None

class Element(object):
    """所有地图元素, 派生出资源类和作战单位(UnitBase)类"""
    kind = None     # 类属性
    def __init__(self, pos):
        super(Element, self).__init__()
        self.pos = pos          # pos可以是一个点(Position类型), 也可以是矩形(Rectangle类型)
        self.index = None       # 调用MapInfo.addElement()才会赋予相应的index值
        self.visible = False    # 每回合更新所有element的visible值

    @property
    def level(self):
        return self.pos.level

    @property
    def position(self):
        return self.pos if isinstance(self.pos, Position) else self.pos.upper_left

    @property
    def size(self):
        return self.pos.size

class Resource(Element):
    """资源类, 派生出矿场类和油田类"""
    pass        

class Mine(Resource):
    """矿场"""
    kind = MINE
    def __init__(self, pos, metal = PROPERTY[MINE][METAL_MAX]):
        super(Mine, self).__init__(pos)
        self.metal = metal
        self.visible = True

class Oilfield(Resource):
    """油田"""
    kind = OILFIELD
    def __init__(self, pos, fuel = PROPERTY[OILFIELD][FUEL_MAX]):
        super(Oilfield, self).__init__(pos)
        self.fuel = fuel
        self.visible = True
        
        
class UnitBase(Element):
    """作战单位抽象, 派生出建筑类以及可移动单位类"""
    def __init__(self, team, pos, sight_ranges, fire_ranges, 
                 health, fuel, ammo, ammo_once, metal, 
                 speed, population, 
                 attacks, defences):
        super(UnitBase, self).__init__(pos)
        self.team = team
        self.sight_ranges = sight_ranges
        self.fire_ranges = fire_ranges
        self.health = self.health_max = health
        self.fuel = self.fuel_max = fuel
        self.ammo = self.ammo_max = ammo
        self.ammo_once = ammo_once
        self.metal = self.metal_max = metal
        self.attacks = attacks
        self.defences = defences

    def view(self, target_pos):
        """查看目标点的状态"""
        if self.pos.distance(target_pos) > self.sight_ranges[target_pos.level]:
            return -1   # 不在视野范围内, 不可见
        else:
            return getElement(target_pos)

    def attack(self, target_pos):
        """攻击(火力与鱼雷伤害叠加计算)某坐标(可能出现射程大于视野的情况)"""
        distance = self.pos.distance(target_pos)
        range = self.fire_ranges[target_pos.z]
        target_unit = getElement(target_pos)
        if distance > range:
            return -1   # 不在攻击范围内
        elif self.ammo <= 0:
            return -2    # 无弹药
        elif target_unit == None or target_unit.team == self.team:
            self.ammo -= self.ammo_once
            return -3    # 坐标不存在敌军单位, miss
        else:
            self.ammo -= self.ammo_once  # 减少弹药数目
            modified_attacks = modifiedAttacks(distance, range, self.attacks)
            fire_damage = max(0, modified_attacks[FIRE] - target_unit.defences[FIRE]) 
            # 考虑到 defence = INFINITY 可能无法破防
            torpedo_damage = max(0, modified_attacks[TORPEDO] - target_unit.defences[TORPEDO])
            damage = fire_damage + torpedo_damage
            # scout influence required.
            SCORE[self.team] += damage * DAMAGE_SCORE
            if damage >= target_unit.health:
                target_unit.health = 0  # killed
                target_unit.destroy()
            else:
                target_unit.health -= damage
            return damage

    def destroy(self):
        """单位阵亡"""
        del ELEMENTS[self.index]


def replenishFuelAmmo(giver, receiver):   # 补给燃料弹药
    if giver.kind == BASE:
        fuel_supply_limit = ammo_supply_limit = 0
    elif giver.kind == FORT:
        fuel_supply_limit = 0
        ammo_supply_limit = SUPPLY_LIMIT
    elif giver.kind == CARGO:
        fuel_supply_limit = SUPPLY_LIMIT
        ammo_supply_limit = 0
    else:
        fuel_supply_limit = ammo_supply_limit = SUPPLY_LIMIT
    provides = [0, 0]    # 维修者提供的燃料, 弹药
    provides[0] = max(giver.fuel - giver.fuel_max * fuel_supply_limit, 
                      receiver.fuel_max - receiver.fuel)
    provides[1] = max(giver.ammo - giver.ammo_max * ammo_supply_limit, 
                      receiver.ammo_max - receiver.ammo)
    giver.fuel -= provides[0]
    giver.ammo -= provides[1]
    receiver.fuel += provides[0]
    receiver.ammo += provides[1]


class Building(UnitBase):
    """建筑类"""
    def supply(self, our_unit):   # 补给操作
        """建筑对周围单位补给, 不对外提供金属"""
        if not self.team == our_unit.team:
            return -1   # 非友军
        elif ((isinstance(our_unit, Plane) and self.pos.distance(our_unit) > 0)
              or self.pos.distance(our_unit) > 1):
            return -2   # 不在范围内
        else:
            replenishFuelAmmo(self, our_unit)
            return 0

class Base(Building):
    """基地, 继承自Building"""
    kind = BASE
    def __init__(self, team, pos, unitbase = None):
        if unitbase == None:
            super(Base, self).__init__(team, pos, *PROPERTY[BASE])
                                   # 从元组解析出数据后传入 Building.__init__()
        else :
            self.__dict__ = unitbase.__dict__.copy

    def repair(self, our_unit):  
        """维修"""
        if not self.team == our_unit.team:
            return -1   # 非友军
        elif isinstance(our_unit, Plane):  
            if self.pos.distance(our_unit) > 0:
                return -2   # 不在范围内
            else:
                while self.metal >= METAL_PER_HEALTH:
                    pass
                replenishFuelAmmo(self, our_unit)
                return 0
        else:
            if self.pos.distance(our_unit) > 1:
                return -2   # 不在范围内
            else:
                provide_metal = max(self.metal, (our_unit.health_max - our_unit.health) * METAL_PER_HEALTH)
                self.metal -= provide_metal
                our_unit.health += provide_metal / METAL_PER_HEALTH
                replenishFuelAmmo(self, our_unit)
                return 0

    def build(self, kind):
        """生产单位, 新单位出生地在基地陆地周围一圈"""
        pass    ##


class Fort(Building):
    """据点, 继承自Building"""
    kind = FORT
    def __init__(self, team, pos, unitbase = None):
        if unitbase == None:
            super(Fort, self).__init__(team, pos, *PROPERTY[FORT])
        else :
            self.__dict__ = unitbase.__dict__.copy()

class Unit(UnitBase):
    """可移动单位"""
    def __init__(self, team, pos, sight_ranges, fire_ranges, 
                 health, fuel, ammo, ammo_once, metal, 
                 speed, population, 
                 attacks, defences, unitbase = None):
        if unitbase == None:
            super(Unit, self).__init__(team, pos, sight_ranges, fire_ranges, 
                                       health, fuel, ammo, ammo_once, metal, 
                                       speed, population, 
                                       attacks, defences)
        else:
            self.__dict__ = unitbase.__dict__.copy()
        self.speed = speed
        self.dest = self.pos    # 目的地(初始为自身位置)
        self.cost = int(self.health_max * METAL_PER_HEALTH)
        self.build_round = self.cost / 10
        self.population = population

class Submarine(Unit):
    """潜艇"""
    kind = SUBMARINE
    def __init__(self, team, pos, unit = None):
        if unit == None:
            super(Submarine, self).__init__(team, pos, *PROPERTY[SUBMARINE])
        else:
            self.__dict__ = unit.__dict__.copy()

class Ship(Unit):
    """水面舰"""        
    pass

class Destroyer(Ship):
    """驱逐舰"""
    kind = DESTROYER
    def __init__(self, team, pos, ship = None):
        if ship == None:
            super(Destroyer, self).__init__(team, pos, *PROPERTY[DESTROYER])
        else:
            self.__dict__ = ship.__dict__.copy()

class Carrier(Ship):
    """航母"""
    kind = CARRIER
    def __init__(self, team, pos, ship = None):
        if ship == None:
            super(Carrier, self).__init__(team, pos, *PROPERTY[CARRIER])
        else:
            self.__dict__ = ship.__dict__.copy()

    def supply(self, our_unit):
        """航母对周围单位补给燃料弹药, 可向基地, 运输舰以及航母补充金属"""
        if not self.team == our_unit.team:
            return -1   # 非友军
        elif ((isinstance(our_unit, Plane) and self.pos.distance(our_unit) > 0)
              or self.pos.distance(our_unit) > 1):
            return -2   # 不在范围内
        else:
            replenishFuelAmmo(self, our_unit)
            if our_unit.kind == BASE or our_unit.kind == CARGO or our_unit.kind == CARRIER:
                provide_metal = min(self.metal, our_unit.metal_max - our_unit.metal)
                self.metal -= provide_metal
                our_unit.metal += provide_metal
            return 0 

class Cargo(Ship):
    """运输舰"""
    kind = CARGO
    def __init__(self, team, pos, ship = None):
        if ship == None:
            super(Cargo, self).__init__(team, pos, *PROPERTY[CARGO])
        else:
            self.__dict__ = ship.__dict__.copy()

    def supply(self, our_unit):
        """运输舰对周围单位补给燃料弹药, 可向基地, 运输舰以及航母补充金属"""
        if not self.team == our_unit.team:
            return -1   # 非友军
        elif ((isinstance(our_unit, Plane) and self.pos.distance(our_unit) > 0)
              or self.pos.distance(our_unit) > 1):
            return -2   # 不在范围内
        else:
            replenishFuelAmmo(self, our_unit)
            if our_unit.kind == BASE or our_unit.kind == CARGO or our_unit.kind == CARRIER:
                provide_metal = min(self.metal, our_unit.metal_max - our_unit.metal)
                self.metal -= provide_metal
                our_unit.metal += provide_metal
            return 0 

    def collect(self, resource):
        """运输舰从资源点采集资源"""
        if resource.kind == MINE and resource.metal > 0:
            supply = min(self.metal_max - self.metal, resource.metal)
            self.metal += supply
            resource.metal -= supply
        elif resource.kind == OILFIELD and resource.fuel > 0:
            supply = min(self.fuel_max - self.fuel, resource.fuel)
            self.fuel += supply
            resource.fuel -= supply
        else:
            return -1

class Plane(Unit):
    """飞机"""
    pass

class Fighter(Plane):
    """战斗机"""
    kind = FIGHTER
    def __init__(self, team, pos, plane = None):
        if plane == None:
            super(Fighter, self).__init__(team, pos, *PROPERTY[FIGHTER])
        else:
            self.__dict__ = plane.__dict__.copy()

class Scout(Plane):
    """侦察机"""
    kind = SCOUT
    def __init__(self, team, pos, plane = None):
        if plane == None:
            super(Scout, self).__init__(team, pos, *PROPERTY[SCOUT])
        else:
            self.__dict__ = plane.__dict__.copy()
