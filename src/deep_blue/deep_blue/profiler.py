import cProfile
import logging
from sys import argv

from logic import  map_info
from ai_battle import AIBattle
import ai_proxy

logging.getLogger().setLevel(logging.INFO)

def main(m, ai0, ai1):
    b = AIBattle(map_info.load(m),
                 ai0_filename=ai0,
                 ai1_filename=ai1)
    b.run_until_end()


if __name__ == '__main__':

    if len(argv) != 4:
        print "Usage: python -m deep_blue.profiler <map> <ai0> <ai1>"
        sys.exit()

    cProfile.run("main(*argv[1:])")
