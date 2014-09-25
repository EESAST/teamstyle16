"""Represent an AI"""

class AIProxy(object):
    """Proxy for AI"""
    def __init__(self, team_num, file_name):
        super(AIProxy, self).__init__()
        self.__team_num = team_num
        self.__file_name = file_name
        self.__team_name = None

        self.__start_ai_file()

    def team_num(self):
        """Getter"""
        return self.__team_num

    def team_name(self):
        """Getter"""
        return self.__team_name

    def get_commands(self, time_limit):
        """Send info to AI, accept commands under the time limit"""
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
