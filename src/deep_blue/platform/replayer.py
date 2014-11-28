import json
import logging
import battle

logger = logging.getLogger(__name__)

class Replayer(battle.Battle):

    def __init__(self, prev_info):
        self.max_round = len(prev_info['history']['score']) - 1
        super(Replayer, self).__init__(None, prev_info=prev_info)

    def next_round(self):
        if self.gamebody.round == self.max_round:
            # Already at the end, won't move, just warn and return None
            logger.warning('Try to move past end round')
            return

        # Just return events recorded
        # Notice that indexes may have changed, so simulating is a bad idea
        events = self.history['event'][self.gamebody.round]

        logger.debug('Moving to next round')
        self.goto(self.gamebody.round + 1)

        return events

    def goto(self, round):
        """Go to specific round"""
        game = self.gamebody
        map_info = game.map_info

        if 0 <= round <= self.max_round:
            logger.debug('Going to round %d', round)
            # Find the key frame
            frame = self.key_frames[round]
            # Restore from the frame
            logger.debug('Restoring from key frame %d (round %d)', round,
                                                                   round)
            game.round = round
            game.map_info.loads_elements(frame[0])
            game.production_lists = frame[1]
            game.scores = self.history['score'][round]
            game.populations = self.history['population'][round]

            logger.info('Moved to round %d', self.gamebody.round)
        else:
            # round invalid, just warn
            logger.warning('Try to go to an invalid round (%d / %d)',
                           round, self.max_round)

    def goto_begin(self):
        logger.debug('Going to begin')
        self.goto(0)

    def goto_end(self):
        logger.debug('Going to end')
        self.goto(self.max_round)

def load(filename):
    return Replayer(battle.load_prev_info(filename))
