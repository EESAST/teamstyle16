import time, gamebody, ai_proxy

class AIBattle(Battle):
    def __init__(self, map_info, ai0_filename, ai1_filename, port_AI):
        Battle.__init__(map_info)
        self.AI_0 = AIProxy(1, ai0_filename, port_AI)
        self.AI_1 = AIProxy(2, ai1_filename, port_AI)
        self.GameBody.set_team_name(0, self.AI_0.team_name)
        self.GameBody.set_team_name(1, self.AI_1.team_name)
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