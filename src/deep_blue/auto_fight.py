import logging

from deep_blue import *
from sys import argv

logging.getLogger().setLevel(logging.INFO)

def main():
    if len(argv) < 5:
        print "Usage: %s <map> <skip_num> <ai0> <ai1> ..." % argv[0]
        return

    m = map_info.load(argv[1])
    skip_num = int(argv[2])
    ais = argv[3:]

    print "Map: %s" % argv[1]
    print "Skip: %d" % skip_num
    print "AIs:"
    for index, ai in enumerate(ais):
        print "%d: %s" % (index, ai)

    summary = []

    for i, ai0 in enumerate(ais):
        for j, ai1 in enumerate(ais):
            if skip_num > 0:  # Skip this one
                skip_num -= 1
                continue

            if ai0 == ai1:  # Same AI
                continue

            print "(%d, %d)" % (i, j),
            b = ai_battle.AIBattle(m, ai0_filename=ai0,
                                      ai1_filename=ai1)
            print "%s vs %s" % (b.team_name(0), b.team_name(1))
            b.run_until_end()

            # Show result
            result = "(%d: %s, %d: %s)  %3d round (exit: %d, %d), %d win, (%d, %d)" % (
                i, b.team_name(0), j, b.team_name(1),
                b.round(),
                b.ais[0].exit_round, b.ais[1].exit_round,
                b.state(),
                b.score(0), b.score(1))
            summary.append(result)
            print result

            # Save to file
            b.save("%d_vs_%d.battle" % (i, j))

    print "================== Summary =================="
    for result in summary:
        print result

if __name__ == '__main__':
    main()
