# -*- coding: UTF-8 -*-
# command.py
from basic import *
import event
from gamebody import *

class Command(object):
    """指令"""
    def __init__(self, operand):
        self.operand = operand

class Attack(Command):
    """攻击"""
    def __init__(self, operand, target):
        super(Attack, self).__init__(operand)
        self.target = target
    def result_event(self):
        Operand = ELEMENTS[self.operand]
        Target = ELEMENTS[self.target]
        Event = []
        hit_damage = Operand.attack(Target.pos)
        if hit_damage[0] < 0:
            hit = False
            damage = 0
        else:
            hit = True
            damage = hit_damage[1]
        Event.append(event.Attack("Attack", self.operand, self.target, hit, damage))
        if Target.health <= 0:
            if Target.type == FORT:
                Event.append(event.Capture("Capture", self.target, Operand.team))
            else:
                Event.append(event.Destroy("Destroy", self.target))
                if Target.type == FORMATION:
                    if map.element(Target.pos) == None:
                        Event.append(event.Crash("Crash", self.target))
                    else:
                        victim = map.element(Target.pos)
                        Event.append(event.Crash("Crash", self.target,\
                                                 victim.index, #Crash_damage))
                        if victim.health - #Crash_damage <=0:
                            Event.append(event.Destroy("Destroy", victim.index))
        return Event
class Collect(Command):
    """收集"""
    def __init__(self, operand, target):
        super(Collect, self).__init__(operand)
        self.target = target
    def result_event(self):
        Operand = ELEMENTS[self.operand]
        Target = ELEMENTS[self.target]
        Event = []
        metal = Operand.metal
        fuel = Operand.fuel
        if Operand.collect(Target) == -1:
            pass
        elif Target.type == MINE:
            supply = Operand.metal - metal
            Event.append(event.Collect("Collect", self.operand,\
                                       self.target, 0, supply)
        else:
            supply = Operand.fuel - fuel
            Event.append(event.Collect("Collect", self.operand,\
                                       self.target, supply, 0)
        return Event
class Fix(Command):
    """维修"""
    def __init__(self, operand, target):
        super(Fix, self).__init__(operand)
        self.target = target
    def result_event(self):
        Operand = ELEMENTS[self.operand]
        Target = ELEMENTS[self.target]
        Event = []
        metal = Operand.metal
        if Operand.repair(Target) < 0:
            pass
        else:
            provide_metal = metal - Operand.metal
            add_health = provide_metal / METAL_PER_HEALTH
            Event.append(event.Fix("Fix", self.operand, self.target, \
                                   provide_metal, add_health)
        return Event
class ChangeDest(Command):
    """更改目的地"""
    def __init__(self, operand, dest):
        super(ChangeDest, self).__init__(operand)
        self.dest = dest
    def result_event(self):
        Event = []
        Event.append(event.ChangeDest("ChangeDest", self.operand, self.dest)
        return Event
class Produce(Command):
    """生产"""
    def __init__(self, operand, kind):
        super(Produce, self).__init__(operand)
        self.kind = kind
    def result_event(self):
        Operand = ELEMENTS[self.operand]
        Event = []
        Event.append(event.AddProductionEntry("AddProductionEntry", \
                                              Operand.team, self.kind)
        return Event
class Supply(Command):
    """补给"""
    def __init__(self, operand, target, fuel = -1, metal = -1, ammo = -1):
        super(Supply, self).__init__(operand)
        self.target = target
        self.fuel = fuel
        self.metal = metal
        self.ammo = ammo
    def result_event(self):
        Event = []
        Event.append(event.Supply("Supply", self.operand, self.target, \
                                  self.fuel, self.metal, self.ammo)
        return Event
class Cancel(Command):
    """取消"""
    def __init__(self, operand):
        super(Cancel,self).__init__(operand)
    def result_event(self):
        Event = []
        return Event
        
