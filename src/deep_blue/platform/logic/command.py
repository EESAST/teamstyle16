# -*- coding: UTF-8 -*-
# command.py
from basic import *
import event
import map_info


class Command(object):
    """指令"""
    def __init__(self, operand):
        self.operand = operand

class AttackPos(Command):
    """攻击"""
    def __init__(self, operand, pos, hit = None):
        super(Attack, self).__init__(operand)
        self.pos = pos
        self.hit = hit
    def result_event(self):
        Operand = ELEMENTS[self.operand]
        Target = getElement(self.pos)
        Event = []
        hit_damage = Operand.AttackPos(self.pos) 
        if hit_damage == -3:
            Event.append(event.AttackPos("AttackPos", self.operand, self.pos, False, 0))
        elif hit_damage >= 0:
            Event.append(event.AttackPos("AttackPos", self.operand, self.target, True, hit_damage))
            if Target.health <= 0:
                if Target.type == FORT:
                    Event.append(event.Capture("Capture", self.target, Operand.team))
                else:
                    Event.append(event.Destroy("Destroy", self.target))
        return Event
                
class AttackUnit(Command):########
    """攻击"""
    def __init__(self, operand, target, hit = None):
        super(Attack, self).__init__(operand)
        self.target = target
        self.hit = None
    def result_event(self):
        Operand = ELEMENTS[self.operand]
        Target = ELEMENTS[self.target]
        Event = []
        damage = Operand.AttackUnit(self.target)
        if damage < 0:
            pass  ##打单位没打中的话产生什么event
        else:
            hit = True
            Event.append(event.Attack("Attack", self.operand, self.target, hit, damage))
            if Target.health <= 0:
                if Target.type == FORT:
                    Event.append(event.Capture("Capture", self.target, Operand.team))
                else:
                    Event.append(event.Destroy("Destroy", self.target))
            return Event

class Fix(Command):
    """维修"""
    def __init__(self, operand, target, new_kind = None):
        super(Fix, self).__init__(operand)
        self.target = target
        self.new_kind = new_kind
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
                                   provide_metal, add_health, self.new_kind))
        return Event
class ChangeDest(Command):
    """更改目的地"""
    def __init__(self, operand, dest):
        super(ChangeDest, self).__init__(operand)
        self.dest = dest
    def result_event(self):
        Event = []
        Operand = ELEMENTS[self.operand]
        ELEMENTS[self.operand].dest = self.dest
        Event.append(event.ChangeDest("ChangeDest", self.operand, self.dest))

        ##该回合的move效果
        Node = pathfinding(Operand.pos, self.dest)  ##寻路函数
        nodes = [Node[0]]
        distance = 0
        for i in range(len(Node)):
            if Node[i].distance(Node[i+1])+distance < speed:
                distance += Node[i].distance(Node[i+1]) + distance
                nodes.append(Node[i+1])
            elif Node[i].distance(Node[i+1])+distance == speed:
                distance += Node[i].distance(Node[i+1]) + distance
                nodes.append(Node[i+1])
                break
            else :
                rest = speed - distance
                if Node[i].x == Node[i+1].x:
                    pos = Position(Node[i].x, Node[i].y + rest, Node[i].z)
                    nodes.append(pos)
                elif Node[i].y == Node[i+1].y:
                    pos = Position(Node[i].x +rest, Node[i].y, Node[i].z)
                    nodes.append(pos)
                break
        Event.append(event.Move("Move", self.operand, nodes))

        ##move过程中可能产生的collect事件
        if Operand.kind == Cargo:
            metal_collected = 0
            metal_collected = 0
            for j in range(i+1):
                for Resource in ELEMENTS and Resource.kind == MINE:
                    if nodes[i].distance(Resource.pos) + nodes[i].distance(Resource.pos) == nodes[i].distance(nodes[i]) + 2:
                        metal = Operand.metal
                        Operand.collect(Resource)
                        supply = Operand.metal - metal
                        Event.append(event.Collect("Collect", self.operand,\
                                                   self.target, 0, supply))
                        metal_collected = True
                        break
                for Resource in ELEMENTS and Resource.kind == OILFIELD:
                    if nodes[i].distance(Resource.pos) + nodes[i].distance(Resource.pos) == nodes[i].distance(nodes[i]) + 2:
                        fuel = Operand.fuel
                        Operand.collect(Resource)
                        supply = Operand.metal - metal
                        Event.append(event.Collect("Collect", self.operand,\
                                                   self.target, supply, 0))
                        fuel_collected = True
                        break
                if fuel_collected and metal_collected:
                    break
                                 
        return Event
    
class Produce(Command):
    """生产"""
    def __init__(self, operand, kind):
        super(Produce, self).__init__(operand)
        self.kind = kind
    def result_event(self):
        Operand = ELEMENTS[self.operand]
        Operand.build(self.kind)
        Event = []
        Event.append(event.AddProductionEntry("AddProductionEntry", \
                                              Operand.team, self.kind))
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
        Operand = ELEMENTS[self.operand]
        Target = ELEMENTS[self.target]
        if Operand.supply(Target) < 0:
            pass
        else:
            Event.append(event.Supply("Supply", self.operand, self.target, \
                                  self.fuel, self.metal, self.ammo))
        return Event
    
class Cancel(Command):
    """取消"""
    def __init__(self, operand):
        super(Cancel,self).__init__(operand)
    def result_event(self):
        Event = []
        Operand = ELEMENTS[self.operand]
        for command in COMMANDS[Operand.team]:
            if command.operand == self.operand:
                COMMANDS[Operand.team].pop(command)
        return Event
        
