# -*- coding: UTF-8 -*-

from .. import basic

STATE_CONTINUE = -1
STATE_TIE = 2

#TEST_LEVEL
TEST_PRODUCE = 0
TEST_COLLECT = 1
TEST_CAPTURE = 2
TEST_ATTACK_UNIT = 3
TEST_REAL_GAME = 4


def testgame_produce(game, events):
    test_state = -1
    for event in events:
        if event.__class__.__name__ == 'Create':
            #test_state = 2 if test_state == 1 - game.map_info.elements[event.index].team or test_state == 2 \
            #   else game.map_info.elements[event.index].team
            if not game.map_info.elements[event.index].team:
                test_state = 0
    if test_state >= 0:
        if test_state in [0, 1]:
            return test_state
        else:
            return STATE_TIE
    if game.round >= game.max_round:
        return 0 if game.score(0) > game.score(1) else (1 if game.score(1) > game.score(0) else STATE_TIE)
    return STATE_CONTINUE


def testgame_collect(game, events):
    test_state = -1
    for event in events:
        if event.__class__.__name__ == 'Collect':
            test_state = 2 if test_state == 1 - game.map_info.elements[event.index].team or test_state == 2 \
                else game.map_info.elements[event.index].team
    if test_state >= 0:
        if test_state in [0, 1]:
            return test_state
        else:
            return STATE_TIE
    if game.round >= game.max_round:
        return STATE_TIE
    return STATE_CONTINUE


def testgame_capture(game, events):
    test_state = -1
    for event in events:
        if event.__class__.__name__ == 'Capture':
            test_state = 2 if test_state == 1 - game.map_info.elements[event.index].team or test_state == 2 \
                else game.map_info.elements[event.index].team
    if test_state >= 0:
        if test_state in [0, 1]:
            return test_state
        else:
            return STATE_TIE
    if game.round >= game.max_round:
        return STATE_TIE
    return STATE_CONTINUE


def testgame_attackunit(game, events):
    test_state = -1
    for event in events:
        if event.__class__.__name__ == 'AttackUnit' and not (event.target in game.map_info.elements.values()):
            test_state = 2 if (test_state == 1 - game.map_info.elements[event.index].team or test_state == 2) \
                else game.map_info.elements[event.index].team
    if test_state >= 0:
        if test_state in [0, 1]:
            return test_state
        else:
            return STATE_TIE
    if game.round >= game.max_round:
        return 0 if game.score(0) > game.score(1) else (1 if game.score(1) > game.score(0) else STATE_TIE)
    return STATE_CONTINUE


def testgame_realgame(game, events):
    dead_bases = 0
    rst = None
    for element in game.map_info.elements.values():
        if element.kind == basic.BASE and element.health <= 0:
            dead_bases += 1
            rst = 1 - element.team
    if dead_bases == 1:
        return rst
    if game.round >= game.max_round or dead_bases == 2:
        return 0 if game.score(0) > game.score(1) else (1 if game.score(1) > game.score(0) else STATE_TIE)
    return STATE_CONTINUE



#TestGame列表
TestGame = [[testgame_produce, 'deep_blue/samples/small.map', 'empty_ai'],
            [testgame_collect, 'deep_blue/samples/small.map', 'empty_ai'],
            [testgame_capture, 'deep_blue/samples/small.map', 'empty_ai'],
            [testgame_attackunit, 'deep_blue/samples/small.map', 'sample_ai'],
            [testgame_realgame, 'deep_blue/samples/small.map', 'sample_ai']]
