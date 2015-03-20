import argparse
import logging
import os
import sys

from deep_blue import *

logger = logging.getLogger(__name__)

def debug(args):
    """Run the game in a black box"""
    kw = dict(ai1_filename=args.opponent, fixed_port=True)
    if args.timeout:
        kw['timeout'] = args.timeout
    else:
        kw['timeout'] = None  # Just block


    try:
        print "Please start the AI you want to debug..."
        try:
            if args.map.endswith('.battle'):
                b = ai_battle.load(args.map, **kw)
            else:  # Construct a new battle
                m = map_info.load(args.map)
                b = ai_battle.AIBattle(m, **kw)
        except AIConnectError as e:
            print e
            print "Have you start your AI?"
            os.abort()
        except AIFileError as e:
            print e
            print "Can not find opponent AI (%s)" % args.opponent
            os.abort()

        # override time_per_round of the map
        b.gamebody.map_info.time_per_round = 0
        while b.gamebody.state == gamebody.STATE_CONTINUE:
            raw_input('Round %d (press Enter to advance)' % b.round())
            b.feed_ai_commands()
            b.next_round()
    except KeyboardInterrupt:
        logger.error("KeyboardInterrupt received, aborting")
        os.abort()

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
        b.save(args.save)

    del b  # Destroy the battle
    return result


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('map', help='map file')
    parser.add_argument('opponent', help='file name of opponent AI (team 1)')
    parser.add_argument('-s', '--save', metavar='<file>',
                        help='Save the game to <file> after it ends')
    parser.add_argument('-t', '--timeout', type=float,
                        help='Set timeout for AI connection (default: forever)')
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

    sys.exit(debug(args))
