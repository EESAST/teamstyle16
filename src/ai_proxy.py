"""Represent an AI"""

class AIProxy(object):
    """Proxy for AI"""
    def __init__(self, team_num, file_name):
        super(AIProxy, self).__init__()
        self.team_num = team_num
        self.file_name = file_name

        self.__start_ai_file()

    def send_info(self, battle):
        """send infos to the AI"""
        pass

    def accept_commands(self, time_limit):
        """Accept commands under the time limit"""
        pass


    def __start_ai_file(self):
        """Start AI file, connect to it, and set the name of the team"""
        pass

    def __send_stable_info(self, battle):
        """Send infos that do not change over rounds to the AI"""
        pass

    def __send_round_info(self, battle):
        """Send infos that change over rounds to the AI"""
        pass
