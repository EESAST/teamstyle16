class GameBody(object):
    """docstring for GameBody"""
    def __init__(self, map, team0_name, team1_name):
        pass
    
    def GetTeamGameInfo(self):
        """Return (team0_game_info, team1_game_info)"""
        pass

    def Run(self, team0_commands, team1_commands):
        """Return (winner, max_round_reached)
        winner: -1 if should continue, 0/1 if one team wins, 2 if tie
        """
        pass

    def GetScores(self):
        """Return (team0_score, team1_score)"""
        pass
