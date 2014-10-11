import copy
import logging
import json
import socket
import time
from logic import map_info, gamebody
import ai_proxy

logger = logging.getLogger(__name__)


class Battle(object):
    def __init__(self, map_info):
        # Just check record_interval in map_info, because invalid interval
        # may casue run time error (e.g. modulo by zero)
        if map_info.record_interval <= 0:
            raise ValueError('record_interval should be positive integer')

        self.gamebody = gamebody.GameBody(map_info)
        self.history = {
            'score': [],
            'unit_num': [],
            'population': [],
            'command': []
        }
        self.record_history()
        self.key_frames = []
        self.record_key_frame()  # record beginning frame

    def map_info(self):
        return self.gamebody.map_info

    def team_name(self, team):
        return self.ais[team].team_name

    def round(self):
        return self.gamebody.round

    def status(self):
        return self.gamebody.status

    def score(self, team):
        return self.gamebody.score(team)

    def elements(self, team):
        return self.gamebody.elements(team)

    def vision(self, team):
        return self.gamebody.vision(team)

    def view_elements(self, perspective):
        return self.gamebody.view_elements(perspective)

    def production_list(self, team):
        return self.gamebody.production_list(team)

    def population(self, team):
        return self.gamebody.population(team)

    def score_history(self):
        return self.history['score']

    def unit_num_history(self):
        return self.history['unit_num']

    def population_history(self):
        return self.history['population']

    def command_history(self):
        return self.history['command']

    def commands(self, team):
        return self.gamebody.commands[team]

    def next_round(self):
        """Advance the game to the next round, return events happened"""
        logger.info('Moving to the next round')

        self.record_commands()  # commands of this round become fixed
        events = self.gamebody.run()
        # Record history & key frame as early as possible, or infomation of
        # the last round will be lost (because no next_round will be called).
        self.record_history()
        if self.gamebody.round % self.gamebody.record_interval == 0:
            self.record_key_frame

        return events

    # def save_game(filename):
    #     save_file = open(filename, 'w')
    #     save_file.write(self.init_map_info.saves() + '\n' + str(self.replay_info) + '\n' + str(self.score_history()) + '\n' + str(self.unit_num_history()) + '\n' + str(self.population_history()) + '\n' + str(self.command_history()) + '\n' + self.map_info().saves())
    #     save_file.close()   #remember the sequence here is init_map_info -> replay_info -> score -> unit_num -> population -> commands -> map_info, each occupying a line

    # def load_game(filename):
    #     load_file = open(filename, 'r')
    #     self.init_map_info = map_info.loads(load_file.readline()[:-1])
    #     self.replay_info = eval(load_file.readline()[:-1])
    #     self.score_list = eval(load_file.readline()[:-1])
    #     self.unit_num_list = eval(load_file.readline()[:-1])
    #     self.population_list = eval(load_file.readline()[:-1])
    #     self.command_list = eval(load_file.readline()[:-1])
    #     self.gamebody = GameBody(map_info.loads(load_file.readline()))
    #     load_file.close()

    # def save_replay(filename, begin = 0, end = None):
    #     replayer_file = open(filename, 'w')
    #     replayer_file.write(self.init_map_info.saves() + '\n' + str(self.replay_info) + '\n' + str(self.score_history()) + '\n' + str(self.unit_num_history()) + '\n' + str(self.population_history()) + '\n' + str(self.command_history()))
    #     replayer_file.close()   #remember the sequence here is init_map_info -> replay_info -> score -> unit_num -> population -> commands, each occupying a line
    #     #But because of the unawareness of which form is saves() going to return in(may contain \n itself), so we could also use some other sep to separate informations
    #     #Such as by @, and func of loading could be load_file.read().split('@')...
    #     #Also we may use some other way as alternative, this part will be further revised later.

    def record_history(self):
        history = self.history
        game = self.gamebody
        history['score'].append(copy.copy(game.scores))
        history['unit_num'].append([len(game.elements(0)),
                              len(game.elements(1))])
        history['population'].append(copy.copy(game.populations))

    def record_commands(self):
        history['command'].append(copy.copy(game.commands))

    def record_key_frame(self):
        frame = (copy.deepcopy(self.gamebody.elements),
                 copy.deepcopy(self.gamebody.production_list))
        self.key_frames.append(frame)


class AIBattle(Battle):
    def __init__(self, map_info, port, ai0_filename=None, ai1_filename=None):
        super(AIBattle, self).__init__(map_info)

        # build the socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.bind(('', port))
        sock.listen(2)

        logger.debug('Building proxies for AIs')
        self.ais = []
        self.ais.append(ai_proxy.AIProxy(0, sock, ai0_filename))
        self.ais.append(ai_proxy.AIProxy(1, sock, ai1_filename))
        logger.debug('Proxies built')

        # Battle has been started, so send infos to AIs
        logger.debug('Sending infos of round 0 to AIs')
        for ai in self.ais:
            ai.send_info(self)
        logger.debug('Infos sent')

        for ai in self.ais:
            ai.start()

    def feed_ai_commands(self):
        logger.info('Feeding commands')
        time.sleep(self.map_info().time_per_round)

        cmds = []
        # Collect commands for both teams first, or team 1 would have more
        # time to send commands
        for ai in self.ais:
            cmds.append(ai.get_commands())

        for ai in self.ais:
            for cmd in cmds:
                self.gamebody.set_command(ai.team_num, cmd)

    def run_until_end(self):
        while self.gamebody.state() == gamebody.STATE_CONTINUE:
            self.feed_ai_commands()
            self.next_round()

    def next_round(self):
        events = Battle.next_round(self)
        for ai in self.ais:
            ai.send_info(self)
        return events


# class Replayer(Battle):
#     def __init__(self):
#         Battle.__init__(self)

#     def next_round(self):
#         commands_to_execute = self.command_list[self.round()]
#         for cmd in commands_to_execute[0]:
#             self.gamebody.set_command(0, cmd)
#         for cmd in commands_to_execute[1]:
#             self.gamebody.set_command(1, cmd)
#         return self.gamebody.run()

#     def go_to_round(self, round):
#         try:
#             self.gamebody = GameBody(map_info.loads_elements(self.replay_info[round // self.record_interval]))  #here a func which can translate the str returned by saves_elements() to map_info object, I call it loads_elements(str)
#             for i in range(round % self.record_interval):
#                 self.gamebody.run()
#         except IndexError:
#             #here may assert an error, to notify Replayer that go_to_round() failed because of round out of range.
#         except:
#             #here may assert another error, to notify Replayer that go_to_round() failed because of some unknown reason.

#     def begin(self):
#         self.gamebody = GameBody(self.init_map_info)

#     def end(self):
#         self.gamebody = GameBody(map_info.loads_elements(self.replay_info[-1]))
#         while self.gamebody.state() == gamebody.STATE_CONTINUE:
#             self.gamebody.run()

# def load_replayer(filename):
#     load_file = open(filename, 'r')
#     init_map_info = map_info.loads(load_file.readline()[:-1])
#     replay_info = eval(load_file.readline()[:-1])
#     score_list = eval(load_file.readline()[:-1])
#     unit_num_list = eval(load_file.readline()[:-1])
#     population_list = eval(load_file.readline()[:-1])
#     command_list = eval(load_file.readline())
#     load_file.close()
#     replayer = Replayer(init_map_info)
#     replayer.init_map_info = init_map_info
#     replayer.replay_info = replay_info
#     replayer.score_list = score_list
#     replayer.unit_num_list = unit_num_list
#     replayer.population_list = population_list
#     replayer.command_list = command_list
#     return replayer
