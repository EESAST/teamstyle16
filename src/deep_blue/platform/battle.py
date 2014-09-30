import time, gamebody, ai_proxy, map_info

class AIBattle(Battle):
    def __init__(self, map_info, ai0_filename, ai1_filename, port_AI):
        Battle.__init__(map_info)
        self.AI_0 = AIProxy(1, ai0_filename, port_AI)
        self.AI_1 = AIProxy(2, ai1_filename, port_AI)
        self.game_body.set_team_name(0, self.AI_0.team_name)
        self.game_body.set_team_name(1, self.AI_1.team_name)
        self.first_time_flag = True

    def feed_ai_commands(self):
        time.sleep(10)  #can be switched
        ai0_cmds = self.AI_0.get_commands()
        ai1_cmds = self.AI_1.get_commands()
        for i in len(ai0_cmds):
            self.battle.set_command(self.AI_0.team_name, self.ai0_cmds[i])
            self.battle.set_command(self.AI_1.team_name, self.ai1_cmds[i])

    def run_until_end(self):
        while self.gamebody.STATE_CONTINUE:
            self.feed_ai_commands()
            self.next_round()

    def next_round(self):
        if self.first_time_flag:
            AI_0.send_info(self)
            AI_1.send_info(self)
            self.first_time_flag = False
        Battle.next_round(self)
        AI_0.send_info(self)
        AI_1.send_info(self)

class Battle(object):
    def __init__(self, map_info):
        self.game_body = GameBody(map_info)
        self.command_list = []
        self.score_list = []
        self.unit_num_list = []
        self.population_list = []

    def map_info(self):
        return self.game_body.map_info()

    def team_name(self, team):
        return self.game_body.team_name(team)

    def round(self):
        return self.game_body.round()

    def status(self):
        return self.game_body.status()

    def score(self, team):
        return self.game_body.score(team)

    def elements(self, perspective):
        return self.game_body.elements(perspective)

    def production_list(self, team):
        return self.game_body.production_list(team)

    def population(self, team):
        return self.game_body.population(team)

    def score_history(self):
        return score_list

    def unit_num_history(self):
        return unit_num_list

    def population_history(self):
        return population_list

    def command_history(self):
        return command_list

    def commands(self, team):
        return self.game_body.commands(team)

    def next_round(self):
        event = self.game_body.run()
        self.score_list.append([score(0), score(1)])
        #self.unit_num_list.append(...)
        self.population_list.append([population(0), population(1)])
        self.command_list.append([commands(0), commands(1)])
        return event

    def save_game(filename):
        save_file = open(filename, 'w')
        save_file.write(str(self.score_history()) + '\n' + str(self.unit_num_history()) + '\n' + str(self.population_history()) + '\n' + str(self.command_history()) + '\n' + self.map_info().saves())
        save_file.close()   #remember the sequence here is score -> unit_num -> population -> commands -> map_info, each occupying a line

    def load_game(filename):
        load_file = open(filename, 'r')
        self.score_list = eval(load_file.readline()[:-1])
        self.unit_num_list = eval(load_file.readline()[:-1])
        self.population_list = eval(load_file.readline()[:-1])
        self.command_list = eval(load_file.readline()[:-1])
        self.game_body = GameBody(map_info.loads(load_file.readline()))
        load_file.close()

    def save_replay(filename, begin = 0, end = None):
        pass