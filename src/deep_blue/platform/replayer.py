import json
import logging
import battle

logger = logging.getLogger(__name__)

class Replayer(battle.Battle):

    def __init__(self, prev_info):
        self.max_round = len(prev_info['history']['score']) - 1
        super(Replayer, self).__init__(None, prev_info=prev_info)

    def next_round(self):
        logger.debug('Moving to next round (round %d)', self.gamebody.round + 1)

        cmds = self.history['command'][self.gamebody.round]
        for team in [0, 1]:
            for cmd in cmds[team]:
                self.gamebody.set_command(cmd)
        # Call run() directly to avoid infos recording in base class
        events = self.gamebody.run()

        logger.info('Moved to round %d', self.gamebody.round)
        return events

    def goto(self, round):
        """Go to specific round"""
        game = self.gamebody
        map_info = game.map_info

        if 0 <= round <= self.max_round:
            logger.debug('Going to round %d', round)
            # Find nearest key frame
            frame_index = round // map_info.record_interval
            frame_round = frame_index * map_info.record_interval
            frame = self.key_frames[frame_index]
            # Restore to the frame
            logger.debug('Restoring from key frame %d (round %d)', frame_index,
                                                                   frame_round)
            game.round = frame_round
            game.map_info.loads_elements(frame[0])
            game.production_list = frame[1]
            game.scores = self.history['score'][frame_round]
            game.populations = self.history['population'][frame_round]

            # Run till round
            while game.round < round:
                self.next_round()

            logger.info('Moved to round %d', self.gamebody.round)
        # round invalid, just warn
        logger.warning('Try to go to an invalid round (%d / %d)',
                       round, self.max_round)

    def goto_begin(self):
        logger.debug('Going to begin')
        self.goto(0)
        logger.info('Moved to round %d', self.gamebody.round)

    def goto_end(self):
        logger.debug('Going to end')
        self.goto(self.max_round)
        logger.info('Moved to round %d', self.gamebody.round)

def load(filename):
    return Replayer(battle.load_prev_info(filename))
