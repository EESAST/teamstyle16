import json
import logging
import socket
import time

from logic import gamebody
import battle
import ai_proxy

from . import DEFAULT_PORT
from . import DEFAULT_TIMEOUT

logger = logging.getLogger(__name__)

class HumanAIBattle(battle.Battle):
    """Represent a battle between two AIs"""
    def __init__(self, map_info, port=DEFAULT_PORT, timeout=DEFAULT_TIMEOUT,
                 human_team_name=None, ai_filename=None, ai_team_num=1,
                 prev_info=None):
        """Construct an HumanAIBattle from a map, or from previous infos.
        port is the port number used for listening.
        If ai_filename is given, HumanAIBattle is responsible for
        starting & closing AI program.
        ai_team_num should be 0 or 1.
        If prev_info is given, restore from it instead of constructing a new
        one.
        """
        # Start battle
        super(HumanAIBattle, self).__init__(map_info, prev_info=prev_info)
        # build the socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(timeout)
        while True:
            try:
                sock.bind(('', port))
            except socket.error:
                logger.warning('Port %d has already been taken, trying port %d',
                               port, port + 1)
                port += 1
            else:  # binding succeeds
                break
        sock.listen(1)

        logger.debug('Building proxy for AI')
        if ai_team_num != 0 and ai_team_num != 1:
            raise ValueError(
                'ai_team_num should be 0 or 1 (%d is given)' % ai_team_num)

        self.ai = ai_proxy.AIProxy(ai_team_num, sock=sock, battle=self,
                                                filename=ai_filename)
        logger.info('Proxy built')

        # Set team names in Battle
        self.team_names[1 - ai_team_num] = human_team_name
        self.team_names[ai_team_num] = self.ai.team_name

        # Battle has been started, so send infos to AI
        self.ai.send_info(self)
        self.ai.start()

    def __del__(self):
        if hasattr(self, 'ai') and self.ai.is_alive():
            logger.debug('Stopping AI')
            self.ai.stop()
            self.ai.join()
            logger.info('AI stopped')

    # Prevent AI team name from being covered
    def team_name(self, team):
        if team == self.ai.team_num:
            return self.ai.team_name
        return self.team_names[team]

    def next_round(self):
        """Advance the game to the next round, return events happened"""
        events = battle.Battle.next_round(self)
        self.ai.send_info(self)
        return events

    def feed_ai_commands(self, sleep_time=None):
        """Feed commands from AI within time limit time_per_round"""
        logger.debug('Feeding commands')

        if sleep_time is None:
            sleep_time = self.gamebody.time_per_round
        time.sleep(sleep_time)

        cmds = self.ai.get_commands()
        for cmd in cmds:
            self.gamebody.set_command(ai.team_num, cmd)

        logger.info('AI commands feed')

    def add_command(self, command):
        """Add a command to the gamebody for human"""
        return self.gamebody.set_command(1 - self.ai.team_num, command)

def load(filename, port=DEFAULT_PORT, timeout=DEFAULT_TIMEOUT,
         human_team_name=None, ai_filename=None, ai_team_num=1):
    """Load save file, construct an HumanAIBattle based on it"""
    return HumanAIBattle(None, port=port, timeout=timeout,
                         human_team_name=human_team_name,
                         ai_filename=ai_filename, ai_team_num=ai_team_num,
                         prev_info=battle.load_prev_info(filename))
