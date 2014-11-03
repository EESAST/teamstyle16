# -*- coding: UTF-8 -*-
# command.py
from basic import *
from copy import deepcopy
import event
import map_info
from custom_json import *

class Command(object):
    """指令"""
    def __init__(self, operand):
        self.operand = operand      # index of operator

    def saves(self):
        """Save command to string"""
        return MyEncoder(sort_keys=True, separators=(',', ':')).encode(self)


class AttackPos(Command):
    """攻击坐标"""
    def __init__(self, operand, pos):
        super(AttackPos, self).__init__(operand)
        self.pos = pos

    def add_to(self, game):
        elements = game.map_info.elements
        attacker = elements.get(self.operand)
        if attacker == None or attacker.kind == CARGO:
            return False
        if self.pos.inMap(game.map_info) is False:
            return False
        if attacker.pos.distance(self.pos) > attacker.fire_ranges[self.pos.z]:
            return False
        if attacker.ammo < attacker.ammo_once:
            return False
        for command in game.commands[attacker.team]:
            if self.operand == command.operand and not isinstance(command, ChangeDest):
                game.commands[attacker.team].remove(command)
                break
        game.commands[attacker.team].append(self)
        return True

    def result_event(self, game):
        attacker = game.map_info.elements[self.operand]
        return attacker.attack(game, self.pos)

class AttackUnit(Command):
    """攻击单位"""
    def __init__(self, operand, target):
        super(AttackUnit, self).__init__(operand)
        self.target = target    # index of the target

    def add_to(self, game):
        elements = game.map_info.elements
        attacker = elements.get(self.operand)
        defender = elements.get(self.target)
        if attacker == None or defender == None or attacker.kind == CARRIER:
            return False
        if hasattr(defender, 'team') is False or defender.team == attacker.team:
            return False
        if attacker.pos.distance(defender.pos) > attacker.fire_ranges[defender.pos.z]:
            return False
        if attacker.ammo < attacker.ammo_once:
            return False
        for command in game.commands[attacker.team]:
            if self.operand == command.operand and not isinstance(command, ChangeDest):
                game.commands[attacker.team].remove(command)
                break
        game.commands[attacker.team].append(self)
        return True

    def result_event(self, game):
        attacker = game.map_info.elements[self.operand]
        defender = game.map_info.elements[self.target]
        return attacker.attack(game, defender.pos)

class Fix(Command):
    """维修"""
    def __init__(self, operand, target):
        super(Fix, self).__init__(operand)
        self.target = target    # index of target

    def add_to(self, game):
        elements = game.map_info.elements
        fixer = elements.get(self.operand)
        broken = elements.get(self.target)
        if fixer == None or broken == None or fixer.kind != BASE:
            return False
        if fixer.team != broken.team:
            return False
        if isinstance(broken, Plane) and fixer.pos.distance(broken.pos) > 0:
            return False
        if fixer.pos.distance(broken.pos) > 1:
            return False
        for command in game.commands[fixer.team]:
            if self.operand == command.operand:
                game.commands[fixer.team].remove(command)
                break
        game.commands[fixer.team].append(self)
        return True

    def result_event(self, game):
        elements = game.map_info.elements
        fixer = elements[self.operand]
        broken = elements[self.target]
        return fixer.repair(broken)
        
class ChangeDest(Command):
    """更改目的地"""
    def __init__(self, operand, dest):
        super(ChangeDest, self).__init__(operand)
        self.dest = dest

    def add_to(self, game):
        elements = game.map_info.elements
        mover = elements.get(self.operand)
        if mover == None or not isinstance(mover, Unit):
            return False
        for command in game.commands[mover.team]:
            if self.operand == command.operand and isinstance(command, ChangeDest):
                game.commands[mover.team].remove(command)
                break
        game.commands[mover.team].append(self)
        mover.dest = self.dest
        return True

    def result_event(self, game):
        mover = game.map_info.elements[self.operand]
        x_max = game.map_info.x_max
        y_max = game.map_info.y_max
        self.dest.x = min(x_max, max(0, self.dest.x))
        self.dest.y = min(y_max, max(0, self.dest.y))
        self.dest.z = mover.pos.z
        mover.dest = self.dest
        return [event.ChangeDest(self.operand, self.dest)]

class Produce(Command):
    """生产"""
    def __init__(self, kind, **kw):
        super(Produce, self).__init__(operand = -1)
        self.kind = kind

    def add_to(self, team, game):
        if self.kind not in xrange(SUBMARINE, SCOUT + 1):
            return False
        game.commands[team].append(self)
        self.team = team
        return True

    def result_event(self, game):
        game.production_lists[self.team].append([self.kind, basic.PROPERTY[self.kind]['build_round']])
        return [event.AddProductionEntry(self.team, self.kind)]

class Supply(Command):
    """补给"""
    def __init__(self, operand, target, fuel = INFINITY, ammo = INFINITY, metal = INFINITY):
        super(Supply, self).__init__(operand)
        self.target = target
        self.fuel = fuel
        self.ammo = ammo
        self.metal = metal

    def add_to(self, game):
        elements = game.map_info.elements
        giver = elements.get(self.operand)
        receiver = elements.get(self.target)
        if giver == None or receiver == None or giver.kind not in [BASE, FORT, CARRIER, CARGO]:
            return False
        if giver.team != receiver.team:
            return False
        if (isinstance(receiver, Plane) and giver.pos.distance(receiver.pos) > 0) or giver.pos.distance(receiver.pos) > 1:
            return False
        for command in game.commands[giver.team]:
            if self.operand == command.operand and not isinstance(command, ChangeDest):
                game.commands[giver.team].remove(command)
                break
        game.commands[giver.team].append(self)
        return True

    def result_event(self, game):
        elements = game.map_info.elements
        giver = elements.get(self.operand)
        receiver = elements.get(self.target)
        return giver.supply(receiver, self.fuel, self.ammo, self.metal)

class Cancel(Command):
    """取消"""
    def __init__(self, operand):
        super(Cancel, self).__init__(operand)

    def add_to(self, game):
        operator = game.map_info.elements.get(self.operand)
        if operator == None:
            return False
        for command in game.commands[operator.team]:
            if self.operand == command.operand:
                game.commands[operator.team].remove(command)
        return True

    def result_event(self, game):
        return []


def loads(cmd_str):
    """Read command from string"""
    return MyDecoder().decode(cmd_str)
