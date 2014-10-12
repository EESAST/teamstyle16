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
        if attacker == None or attacker.kind == CARRIER:
            return False
        if not (self.pos.x >= 0 and self.pos.x < self.game.map_info.x_max and
                self.pos.y >= 0 and self.pos.y < self.game.map_info.y_max and
                self.pos.z >= 0 and self.pos.z < 3):
            return False
        if attacker.attack(copy(self.game), self.pos)['valid'] is False:
            return False
        for command in commands:
            if self.operand == command.operand and not isinstance(command, ChangeDest):
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
        if attacker == None or defender == None or attacker.kind == CARRIER:
            return False
        if defender.team == attacker.team:
            return False
        if attacker.attack(copy(self.game), defender.pos)['valid'] is False:
            return False
        for command in commands:
            if self.operand == command.operand and not isinstance(command, ChangeDest):
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
    def __init__(self, game, operand, target):
        super(Fix, self).__init__(game, operand)
        self.target = target    # index of target

    def add_to(self, commands):
        elements = self.game.map_info.elements
        fixer = elements.get(operand)
        broken = elements.get(target)
        if fixer == None or broken == None or fixer.kind != BASE:
            return False
        if fixer.repair(copy(self.game), broken)['valid'] is False:
            return False
        for command in commands:
            if self.operand == command.operand and not isinstance(command, ChangeDest):
                commands.remove(command)
                break
        commands.append(self)
        return True

    def result_event(self):
        elements = self.game.map_info.elements
        fixer = elements[self.operand]
        broken = elements[self.target]
        result_dict = fixer.repair(broken)
        return result_dict['events'] 
        
class ChangeDest(Command):
    """更改目的地"""
    def __init__(self, game, operand, dest):
        super(ChangeDest, self).__init__(game, operand)
        self.dest = dest

    def add_to(self, commands):
        elements = self.game.map_info.elements
        mover = elements.get(operand)
        if mover == None or not isinstance(mover, Unit):
            return False
        for command in commands:
            if self.operand ==  command.operand and isinstance(command, ChangeDest):
                commands.remove(command)
                break
        commands.append(self)
        return True

    def result_event(self):
        x_max = self.game.map_info.x_max
        y_max = self.game.map_info.y_max
        dest.x = min(x_max, max(0, dest.x))
        dest.y = min(y_max, max(0, dest.y))
        dest.z = min(AIR, max(UNDERWATER, dest.z))
        mover = self.game.map_info.elements[operand]
        mover.dest = dest
        return [event.ChangeDest(self.operand, dest)]

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
