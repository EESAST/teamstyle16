# -*- coding: UTF-8 -*-
# event.py

import basic
from custom_json import *

class Event(object):
    def __init__(self):
        super(Event, self).__init__()

    def saves(self):
        """Save event to string"""
        return MyEncoder(sort_keys=True, separators=(',', ':')).encode(self)

class AddProductionEntry(Event):
    """添加生产条目"""
    def __init__(self, team, kind):
        super(AddProductionEntry, self).__init__()
        self.team = team
        self.kind = kind    # 生产单位的种类

    def description(self):
        return '%d号队伍开始生产新的%s' % (self.team, basic.names[self.kind])

class AttackMiss(Event):
    """攻击坐标未中(击中可全部归为AttackUnit)"""
    def __init__(self, index, target_pos):
        super(AttackMiss, self).__init__()
        self.index = index              # attacker
        self.target_pos = target_pos    # position

    def description(self):
        return '%d号单位没能击中坐标为(%d,%d,%d)的单位! 敌人太狡猾了!' % (
            self.index, self.target_pos.x, self.target_pos.y, self.target_pos.z)

class AttackUnit(Event):
    """攻击"""
    def __init__(self, index, target, damage):
        super(AttackUnit, self).__init__()
        self.index = index      # attacker
        self.target = target    # index of target unit
        self.damage = damage

    def description(self):
        return '%d号单位对%d号单位造成了%d点伤害' % (self.index, self.target, self.damage)

class Supply(Event):
    """补给"""
    def __init__(self, index, target, fuel, ammo, metal):
        super(Supply, self).__init__()
        self.index = index
        self.target = target    # index
        self.fuel = fuel
        self.ammo = ammo
        self.metal = metal

    def description(self):
        return '%d号单位向%d号单位补给了 燃料:%d, 弹药:%d, 金属:%d' % (
            self.index, self.target, self.fuel, self.ammo, self.metal)

class Fix(Event):
    """修理"""
    def __init__(self, index, target, metal, health_increase):
        super(Fix, self).__init__()
        self.index = index
        self.target = target
        self.metal = metal
        self.health_increase = health_increase

    def description(self):
        return '%d号单位消耗%d金属修理了%d号单位, 使其生命值恢复了%d点' % (
            self.index, self.metal, self.target, self.health_increase)

class Collect(Event):
    """收集"""
    def __init__(self, index, target, fuel, metal):
        super(Collect, self).__init__()
        self.index = index
        self.target = target
        self.fuel = fuel
        self.metal = metal

    def description(self):
        return '%d号单位从%d号资源点那里收集了资源 燃料:%d, 金属:%d' % (
            self.index, self.target, self.fuel, self.metal)

class ChangeDest(Event):
    """修改目的地"""
    def __init__(self, index, dest):
        super(ChangeDest, self).__init__()
        self.index = index
        self.dest = dest

    def description(self):
        return '%d号单位正在向坐标(%d,%d,%d)移动' % (self.index, self.dest.x, self.dest.y, self.dest.z)

class Move(Event):
    """移动"""
    def __init__(self, index, nodes): # nodes 为移动路径上的拐点, 包括起始点和终点
        super(Move, self).__init__()
        self.index = index
        self.nodes = nodes
        self.steps = 0
        for i in xrange(len(self.nodes) - 1):
            self.steps += self.nodes[i].distance(self.nodes[i + 1])

    def description(self):
        s = '%d号单位' % self.index
        if len(self.nodes) == 2:
            s += '从(%d,%d,%d)直接杀向了(%d,%d,%d)' % (
                self.nodes[0].x, self.nodes[0].y, self.nodes[0].z,
                self.nodes[-1].x, self.nodes[-1].y, self.nodes[-1].z)
        else:
            s += '具有风骚的走位技巧! 从(%d,%d,%d)移动至(%d,%d,%d)' % (
                self.nodes[0].x, self.nodes[0].y, self.nodes[0].z,
                self.nodes[-1].x, self.nodes[-1].y, self.nodes[-1].z)
        return s


class Create(Event):
    """生产"""
    def __init__(self, index, kind, pos):
        super(Create, self).__init__()
        self.index = index
        self.kind = kind
        self.pos = pos

    def description(self):
        return '%d号%s在(%d,%d,%d)出生!' % (self.index, basic.names[self.kind],
                                           self.pos.x, self.pos.y, self.pos.z)

class Destroy(Event):
    """单位损毁"""
    def __init__(self, index):
        super(Destroy, self).__init__()
        self.index = index

    def description(self):
        return '%d号单位被摧毁!' % self.index

class Capture(Event):
    """占领堡垒"""
    def __init__(self, index, team):
        super(Capture, self).__init__()
        self.index = index
        self.team = team

    def description(self):
        return '%d号据点被%d号队伍攻占!' % (self.index, self.team)


def loads(event_str):
    """Read event from string"""
    return MyDecoder().decode(event_str)
