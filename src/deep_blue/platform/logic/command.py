# -*- coding: UTF-8 -*-
# command.py
from basic import *
from copy import copy
import event
import map_info


class Command(object):
    """指令"""
    def __init__(self, game, operand):
        self.game = game            # current gamebody
        self.operand = operand      # index of operator

class AttackPos(Command):
    """攻击坐标"""
    def __init__(self, game, operand, pos):
        super(AttackPos, self).__init__(game, operand)
        self.pos = pos

    def add_to(self, commands):
        elements = self.game.map_info.elements
        attacker = elements.get(operand)
        if attacker == None:
            return False
        if not (self.pos.x >= 0 and self.pos.x < self.game.map_info.x_max and
                self.pos.y >= 0 and self.pos.y < self.game.map_info.y_max and
                self.pos.z >= 0 and self.pos.z < 3):
            return False
        if attacker.attack(copy(self.game), self.pos)['valid'] is False:
            return False
        for command in commands:
            if self.operand == command.operand:
                commands.remove(command)
                break
        commands.append(self)
        return True

    def result_event(self):
        attacker = self.game.map_info.elements[operand]
        result_dict = attacker.attack(self.game, self.pos)
        return result_dict['events']

class AttackUnit(Command):
    """攻击单位"""
    def __init__(self, game, operand, target):
        super(AttackUnit, self).__init__(game, operand)
        self.target = target    # index of the target

    def add_to(self, commands):
        elements = self.game.map_info.elements
        attacker = elements.get(operand)
        defender = elements.get(target)
        if attacker == None or defender == None:
            return False
        if defender.team == attacker.team:
            return False
        if attacker.attack(copy(self.game), defender.pos)['valid'] is False:
            return False
        for command in commands:
            if self.operand == command.operand:
                commands.remove(command)
                break
        commands.append(self)
        return True

    def result_event(self):
        attacker = self.game.map_info.elements[operand]
        defender = elements.get(target)
        result_dict = attacker.attack(self.game, defender.pos)
        return result_dict['events']

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
                                   provide_metal, add_health))
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
