import argparse
import logging

from logic.testgame import TestGame
from logic import gamebody
import ai_battle

def run_test_match(args):
    """Run test match"""
    # check level & load match_info
    if args.level < 1 or args.level > len(TestGame):
        print 'Level %d does not exist' % args.level
        return

    judge, map_file, opponent_ai = TestGame[level - 1]  # level begins from 1

    kw = dict(ai0_filename=args.ai, ai1_filename=opponent_ai, judge=judge)
    if args.timeout:
        kw['timeout'] = args.timeout

    if map_file.endswith('.battle'):  # Load from pervious battle
        b = ai_battle.load(map_file, **kw)
    else:  # Construct a new battle
        m = map_info.load(map_file)
        b = ai_battle.AIBattle(m, **kw)

    if args.time_per_round:  # override time_per_round of the map
        b.gamebody.map_info.time_per_round = args.time_per_round

    if args.debug:
        while b.gamebody.state == gamebody.STATE_CONTINUE:
            raw_input('Round %d (press Enter to advance)' % g.round())
            b.feed_ai_commands()
            b.next_round()
    else:
        b.run_until_end()

    # Print result
    print b.round(), 'round(s) passed'
    print 'Your AI (%s)' % b.team_name(0),
    result = b.state()
    if result == 0:
        print 'won'
    elif result == 1:
        print 'lost'
    elif result == gamebody.STATE_TIE:
        print 'tied'
    else:
        raise RuntimeError('Invalid game state: %d', result)

    print 'Score (Your AI : opponent_ai):'
    print '%d : %d' % (b.score(0), b.score(1))

    # Save into files
    if args.save:
        b.save(args.save, compact=args.compact, compress=args.compress)
    if args.out:
        b.save_event_strings(args.out)

    del b  # Destroy the battle


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('level', help='level of test match')
    parser.add_argument('ai', help='file name of AI')
    parser.add_argument('-s', '--save', metavar='<file>',
                        help='Save the game to <file> after it ends')
    parser.add_argument('-o', '--out', metavar='<file>',
                        help='Place the output into <file>')
    parser.add_argument('--timeout', type=float,
                        help='Set timeout when waiting for AI connection')
    parser.add_argument('-t', '--time_per_round', type=float,
                        help='Override time interval of the map')
    parser.add_argument('--compact', action='store_true',
                        help='Make save file more compact')
    parser.add_argument('--compress', action='store_true',
                        help='Compress save file')
    parser.add_argument('-g', '--debug', action='store_true',
                        help='Use debug mode (move to next round manually)')

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

    run_test_match(args)
