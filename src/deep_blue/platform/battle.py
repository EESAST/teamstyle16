import time, ai_proxy
from logic import map_info
from logic import gamebody
import logging
import socket

logger = logging.getLogger(__name__)


class Battle(object):
    def __init__(self, map_info):
        self.gamebody = gamebody.GameBody(map_info, ['', ''])
        self.init_map_info = map_info
        self.command_list = []
        self.score_list = []
        self.unit_num_list = []
        self.population_list = []
        self.record_interval = 5    #can be switched
        self.replay_info = []

    def map_info(self):
        return self.gamebody.map_info

    def team_name(self, team):
        return self.gamebody.team_name(team)

    def round(self):
        return self.gamebody.round

    def status(self):
        return self.gamebody.status

    def score(self, team):
        return self.gamebody.score(team)

    def view_elements(self, perspective):
        return self.gamebody.view_elements(perspective)

    def production_list(self, team):
        return self.gamebody.production_list(team)

    def population(self, team):
        return self.gamebody.population(team)

    def score_history(self):
        return score_list

    def unit_num_history(self):
        return unit_num_list

    def population_history(self):
        return population_list

    def command_history(self):
        return command_list

    def commands(self, team):
        return self.gamebody.commands[team]

    def next_round(self):
        self.record_info()
        logger.info('Moving to the next round')
        return self.gamebody.run()

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

    def record_info(self):
        self.score_list.append([self.gamebody.score(0),
                                self.gamebody.score(1)])
        #self.unit_num_list.append(...)
        self.population_list.append([self.gamebody.population(0),
                                     self.gamebody.population(1)])
        self.command_list.append([self.gamebody.commands[0],
                                  self.gamebody.commands[1]])
        if self.round() % self.record_interval == 0:
            self.replay_info.append(self.gamebody.map_info.saves_elements())


class AIBattle(Battle):
    def __init__(self, map_info, port, ai0_filename=None, ai1_filename=None):
        super(AIBattle, self).__init__(map_info)

        # build the socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.bind(('', port))
        sock.listen(2)

        logger.debug('Building proxies for AIs')
        self.AI_0 = ai_proxy.AIProxy(0, sock, ai0_filename)
        self.AI_1 = ai_proxy.AIProxy(1, sock, ai1_filename)
        logger.debug('Proxies built')

        self.gamebody.set_team_name(0, self.AI_0.team_name)
        self.gamebody.set_team_name(1, self.AI_1.team_name)
        # Battle has been started, so send infos to AIs
        logger.debug('Sending infos of round 0 to AIs')
        self.AI_0.send_info(self)
        self.AI_1.send_info(self)
        logger.debug('Infos sent')
        self.AI_0.start()
        self.AI_1.start()

    def feed_ai_commands(self):
        logger.info('Feeding commands')
        time.sleep(self.map_info().time_per_round)
        ai0_cmds = self.AI_0.get_commands()
        ai1_cmds = self.AI_1.get_commands()
        for cmd in ai0_cmds:
            self.gamebody.set_command(self.AI_0.team_name, cmd)
        for cmd in ai1_cmds:
            self.gamebody.set_command(self.AI_1.team_name, cmd)

    def run_until_end(self):
        while self.gamebody.state() == gamebody.STATE_CONTINUE:
            self.feed_ai_commands()
            self.next_round()

    def next_round(self):
        events = Battle.next_round(self)
        self.AI_0.send_info(self)
        self.AI_1.send_info(self)
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
