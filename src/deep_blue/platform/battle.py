import copy
import logging
import json
import socket
from logic import map_info, gamebody

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

    def set_command(self, team, command):
        return self.gamebody.set_command(team, command)

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


