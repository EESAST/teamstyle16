# -*- coding: UTF-8 -*-
# event.py

from basic import *

class Event(object):
    def __init__(self, description):
        self.description = description
    def description(self):
        return self.description

class AddProductionEntry(Event):
    """添加生产条目"""
    def __init__(self, team, kind):
        d = {
            SUBMARINE: '潜艇', DESTROYER: '驱逐舰', 
            CARRIER: '航空母舰', CARGO: '运输舰',
            FIGHTER: '战斗机', SCOUT: '侦察机'
            }
        super(AddProductionEntry, self).__init__('%d号队伍开始生产新的%s' % (team, d[kind]))
        self.team = team
        self.kind = kind    # 生产单位的种类

class AttackMiss(Event):
    """攻击坐标未中(击中可全部归为AttackUnit)"""
    def __init__(self, index, target_pos):
        super(Attack, self).__init__('%d号单位没能击中坐标为(%d,%d,%d)的单位! 敌人太狡猾了!' % (index, target_pos.x, target_pos.y, target_pos.z))
        self.index = index              # attacker
        self.target_pos = target_pos    # position

class AttackUnit(Event):
    """攻击"""
    def __init__(self, index, target, damage):
        super(Attack, self).__init__('%d号单位对%d号单位造成了%d点伤害' % (index, target, damage))
        self.index = index      # attacker
        self.target = target    # index of target unit
        self.damage = damage

class Supply(Event):
    """补给"""
    def __init__(self, index, target, fuel, metal, ammo):
        super(Supply, self).__init__('%d号单位向%d号单位补给了 燃料:%d, 金属:%d, 弹药:%d' % (index, target, fuel, metal, ammo))
        self.index = index
        self.target = target    # index
        self.fuel = fuel
        self.metal = metal
        self.ammo = ammo

class Fix(Event):
    """修理"""
    def __init__(self, index, target, metal, health_increase):
        super(Fix, self).__init__('%d号单位消耗%d金属修理了%d号单位, 使其生命值恢复了%d点' % (index, metal, target, health_increase))
        self.index = index
        self.target = target
        self.metal = metal
        self.health_increase = health_increase

class Collect(Event):
    """收集"""
    def __init__(self, index, target, fuel, metal):
        super(Collect, self).__init__('%d号单位从%d号资源点那里收集了资源 燃料:%d, 金属:%d' % (index, target, fuel, metal))
        self.index = index
        self.target = target
        self.fuel = fuel
        self.metal = metal

class ChangeDest(Event):
    """修改目的地"""
    def __init__(self, index, dest):
        super(ChangeDest, self).__init__('%d号单位正在向坐标(%d,%d,%d)移动' % (index, dest.x, dest.y, dest.z))
        self.index = index
        self.dest = dest

class Move(Event):
    """移动"""
    def __init__(self, index, nodes): # nodes 为移动路径上的拐点, 包括起始点和终点
        super(Move, self).__init__('%d号单位' % index)
        self.index = index
        self.nodes = nodes
        self.steps = 0
        for i in xrange(len(self.nodes) - 1):
            self.steps += self.nodes[i].distance(self.nodes[i + 1])
        if len(nodes) == 2:
            self.description += '从(%d,%d,%d)直接杀向了(%d,%d,%d)' % (nodes[0].x, nodes[0].y, nodes[0].z, nodes[-1].x, nodes[-1].y, nodes[-1].z)
        else:
            self.description += '具有风骚的走位技巧! 从(%d,%d,%d)移动至(%d,%d,%d)' % (nodes[0].x, nodes[0].y, nodes[0].z, nodes[-1].x, nodes[-1].y, nodes[-1].z)

class Create(Event):
    """生产"""
    def __init__(self, index, kind, pos):
        d = {
            SUBMARINE: '潜艇', DESTROYER: '驱逐舰', 
            CARRIER: '航空母舰', CARGO: '运输舰',
            FIGHTER: '战斗机', SCOUT: '侦察机'
            }
        super(Create, self).__init__('%d号%s在(%d,%d,%d)出生!' % (index, d[kind], pos.x, pos.y, pos.z))
        self.index = index
        self.kind = kind
        self.pos = pos

class Destroy(Event):
    """单位损毁"""
    def __init__(self, index):
        super(Destroy, self).__init__('%d号单位被摧毁!' % index)
        self.index = index

class Capture(Event):
    """占领堡垒"""
    def __init__(self, index, team):
        super(Capture, self).__init__('%d号据点被%d号队伍攻占!' % (index, team))
        self.index = index
        self.team = team
