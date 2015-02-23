from copy import deepcopy

from ..logic import *
from .. import battle
from ..logic.basic import *
from random import choice

MAP_DIR = '/Users/lisihan969/src/projects/teamstyle16/src/deep_blue/maps/'
MAP = map_info.load(MAP_DIR + 'island.map')


def add(m, cls, team, pos, **kws):
    element = cls(team, Position(*pos), **kws)
    m.add_element(element)
    return element


def save(b, name):
    b.save(MAP_DIR + name + '.battle')


def test_move():
    m = deepcopy(MAP)
    # Block some routes
    m.types[44][40] = m.types[45][49] = 1
    for row in xrange(30):
        m.types[row][45] = m.types[89 - row][45] = 1
    # Add units
    add(m, Submarine, 0, (44, 0))
    add(m, Submarine, 1, (45, 0))
    add(m, Cargo, 0, (44, 0))
    add(m, Cargo, 1, (45, 0))
    add(m, Destroyer, 1, (44, 1))
    add(m, Destroyer, 0, (45, 1))
    add(m, Carrier, 0, (44, 2))
    add(m, Carrier, 1, (45, 2))
    add(m, Scout, 0, (44, 0))
    add(m, Scout, 1, (45, 0))
    add(m, Fighter, 1, (44, 1))
    add(m, Fighter, 0, (45, 1))

    b = battle.Battle(m, 'Test team 1', 'Test team 2')

    for e in b.gamebody.map_info.elements.values():
        if isinstance(e, Unit):
            b.set_command(e.team, command.ChangeDest(e.index, Position(45, 89)))

    # Run until they all stop
    while len(b.next_round()) > 0:
        pass

    save(b, "test_move")


def test_fix():
    m = deepcopy(MAP)

    m.elements[2351].metal = 9999999  # Provide enough metal
    # Add Units
    eles = []
    eles.append(add(m, Submarine, 0, (29, 30), health=1))
    eles.append(add(m, Cargo, 0, (29, 30), health=1))
    eles.append(add(m, Destroyer, 0, (30, 29), health=1))
    eles.append(add(m, Carrier, 0, (31, 29), health=1))
    eles.append(add(m, Scout, 0, (30, 30), health=1))
    eles.append(add(m, Fighter, 0, (31, 31), health=1))
    cargo = eles[1].index
    carrier = eles[3].index
    scout = eles[4].index

    b = battle.Battle(m, 'Test team 1', 'Test team 2')

    while True:
        if b.round() % 2 == 0:
            b.set_command(0, command.ChangeDest(cargo, Position(0, 0)))
            b.set_command(0, command.ChangeDest(scout, Position(50, 50)))
            b.set_command(0, command.ChangeDest(carrier, Position(32, 29)))
        else:
            b.set_command(0, command.ChangeDest(cargo, Position(29, 30)))
            b.set_command(0, command.ChangeDest(scout, Position(30, 30)))
            b.set_command(0, command.ChangeDest(carrier, Position(31, 29)))

        target = choice(eles)
        while target.health == target.health_max:
            target = choice(eles)

        b.set_command(0, command.Fix(2351, target.index))
        b.next_round()

        for e in eles:
            if e.health != e.health_max:
                break
        else:  # All done
            break

    save(b, "test_fix")


# TODO: Test Attack

if __name__ == '__main__':
    test_move()
    test_fix()
