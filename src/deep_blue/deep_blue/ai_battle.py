import argparse
import json
import logging
import socket
import sys
import time

from logic import gamebody, map_info
import battle
import ai_proxy

from . import DEFAULT_PORT
from . import DEFAULT_TIMEOUT

logger = logging.getLogger(__name__)

class AIBattle(battle.Battle):
    """Represent a battle between two AIs"""
    def __init__(self, map_info, port=DEFAULT_PORT, timeout=DEFAULT_TIMEOUT,
                 ai0_filename=None, ai1_filename=None, prev_info=None,
                 fixed_port=False, **kw):
        """Construct an AIBattle from a map, or from previous infos.
        port is the port number used for listening.
        If filename is given, AIBattle is responsible for starting & closing
        the AI program.
        If prev_info is given, restore from it instead of constructing a new
        one.
        If fixed_port is True, will only listen on DEFAULT_PORT.
        """
        # Start battle
        super(AIBattle, self).__init__(map_info, prev_info=prev_info, **kw)
        # build the socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(timeout)
        while True:
            try:
                sock.bind(('', port))
            except socket.error:
                if fixed_port:
                    logger.error('Port %d has already been taken, aborting', port)
                    sys.exit(1)
                else:
                    logger.warning('Port %d has already been taken, trying port %d',
                                   port, port + 1)
                    port += 1
            else:  # binding succeeds
                break
        sock.listen(2)

        logger.debug('Building proxies for AIs')
        self.ais = []
        self.ais.append(ai_proxy.AIProxy(0, sock=sock, battle=self,
                                            filename=ai0_filename))
        self.ais.append(ai_proxy.AIProxy(1, sock=sock, battle=self,
                                            filename=ai1_filename))
        logger.debug('Proxies for AIs built')

        # Set team names in Battle
        self.team_names = [self.ais[0].team_name, self.ais[1].team_name]

        # Battle has been started, so send infos to AIs
        for ai in self.ais:
            ai.send_info(self)
        logger.info('Info sent to AIs')

        for ai in self.ais:
            ai.start()

        logger.info('Battle started')

    def __del__(self):
        for ai in self.ais:
            if ai.is_alive():
                logger.debug('Stopping AI %d proxy', ai.team_num)
                ai.stop()
                ai.join()
                logger.debug('AI %d proxy stopped', ai.team_num)

    # Override team_name(), to make sure team names of AI will be displayed,
    # instead of team names set at the beginning of the battle.
    # e.g. the battle may be started as a HumanAIBattle, but later taken over
    # as an AIBattle
    def team_name(self, team):
        return self.ais[team].team_name

    def next_round(self):
        """Advance the game to the next round, return events happened"""
        events = battle.Battle.next_round(self)
        for ai in self.ais:
            ai.send_info(self)
        logger.info('Info sent to AIs')

        return events

    def feed_ai_commands(self, sleep_time=None):
        """Feed commands from AIs within time limit time_per_round"""
        logger.debug('Feeding commands')
        if sleep_time is None:
            sleep_time = self.gamebody.time_per_round
        time.sleep(sleep_time)

        cmds = []
        # Collect commands for both teams first, or team 1 would have more
        # time to send commands
        for ai in self.ais:
            cmds.append(ai.get_commands())

        for team in xrange(2):
            for cmd in cmds[team]:
                if not self.gamebody.set_command(team, cmd):
                    # Invalid command
                    logger.error('AI %d provided an invalid command', team)
        logger.info('AIs commands feed')

    def run_until_end(self):
        """Run until the game ends"""
        while self.gamebody.state == gamebody.STATE_CONTINUE:
            self.feed_ai_commands()
            self.next_round()

def load(filename, port=DEFAULT_PORT, timeout=DEFAULT_TIMEOUT,
         ai0_filename=None, ai1_filename=None):
    """Load save file, construct an AIBattle based on it"""
    return AIBattle(None, port=port, timeout=timeout,
                    ai0_filename=ai0_filename, ai1_filename=ai1_filename,
                    prev_info=battle.load_prev_info(filename))

def black_box_run(args):
    """Run the game in a black box"""
    kw = dict(ai0_filename=args.ai_0, ai1_filename=args.ai_1)
    if args.timeout:
        kw['timeout'] = args.timeout

    if args.battle:  # Load from pervious battle
        b = load(args.map, **kw)
    else:  # Construct a new battle
        m = map_info.load(args.map)
        b = AIBattle(m, **kw)

    if args.time_per_round:  # override time_per_round of the map
        b.gamebody.map_info.time_per_round = args.time_per_round

    if args.debug:
        while b.gamebody.state == gamebody.STATE_CONTINUE:
            raw_input('Round %d (press Enter to advance)' % b.round())
            b.feed_ai_commands()
            b.next_round()
    else:
        b.run_until_end()

    # Print result
    print b.round(), 'round(s) passed'
    result = b.state()
    if result == 0 or result == 1:
        print 'Team %d wins' % result
    elif result == gamebody.STATE_TIE:
        print 'Tie (%d : %d)' % (b.score(0), b.score(1))
    else:
        raise RuntimeError('Invalid game state: %d', result)

    # Save into files
    if args.save:
        b.save(args.save, compact=args.compact, compress=args.compress)
    if args.out:
        b.save_event_strings(args.out)

    del b  # Destroy the battle
    return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('map', help='map file')
    parser.add_argument('ai_0', help='file name of AI 0')
    parser.add_argument('ai_1', help='file name of AI 1')
    parser.add_argument('-s', '--save', metavar='<file>',
                        help='Save the game to <file> after it ends')
    parser.add_argument('-o', '--out', metavar='<file>',
                        help='Place the output into <file>')
    parser.add_argument('--timeout', type=float,
                        help='Set timeout when waiting for AI connection')
    parser.add_argument('-t', '--time_per_round', type=float,
                        help='Override time interval of the map')
    parser.add_argument('-b', '--battle', action='store_true',
                        help='Treat map file as a battle')
    parser.add_argument('-g', '--debug', action='store_true',
                        help='Use debug mode (move to next round manually)')
    parser.add_argument('--compact', action='store_true',
                        help='Make save file more compact')
    parser.add_argument('--compress', action='store_true',
                        help='Compress save file')

    parser.add_argument('-v', '--verbose', action='count',
                        help='increase output verbosity')

    args = parser.parse_args()

    v = args.verbose
    root = logging.getLogger()
    if not v:
        root.setLevel(logging.WARNING)
    elif v == 1:
        root.setLevel(logging.INFO)
    else:
        root.setLevel(logging.DEBUG)

    sys.exit(black_box_run(args))

if __name__ == '__main__':
    main()
