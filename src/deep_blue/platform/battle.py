import copy
import gzip
import json
import logging
from logic import map_info, gamebody

VERSION = 1

logger = logging.getLogger(__name__)

class Battle(object):
    def __init__(self, map_info, team0_name=None, team1_name=None, prev_info=None):
        """Construct a battle based on map_info.
        If prev_info is given, restore from it & ignore map_info"""
        if prev_info is not None:
            # Restore
            logger.debug('Restoring battle from previous info')

            save_version = prev_info['version']
            if save_version < VERSION:
                logger.warning('Older save file detected (save: %d, '
                               'current: %d), errors may occur',
                                save_version, VERSION)
            elif save_version > VERSION:
                logger.warning('Newer save file detected (save: %d, '
                               'current: %d), errors may occur',
                               save_version, VERSION)

            self.team_names = prev_info['team_names']
            self.gamebody = gamebody.loads(prev_info['gamebody'])
            self.history = prev_info['history']
            self.key_frames = prev_info['key_frames']

            logger.info('Battle restored')
            return

        # Construct new battle
        # Just check record_interval in map_info, because invalid interval
        # may casue run time error (e.g. modulo by zero)
        if map_info.record_interval <= 0:
            raise ValueError('record_interval should be positive integer')

        self.team_names = [team0_name, team1_name]
        self.history = {
            'score': [],
            'unit_num': [],
            'population': [],
            'command': []
        }
        self.gamebody = gamebody.GameBody(map_info)
        self.key_frames = []

        self.record_history()
        self.record_key_frame()  # record beginning frame

    def map_info(self):
        return self.gamebody.map_info

    def team_name(self, team):
        return self.team_names[team]

    def round(self):
        return self.gamebody.round

    def status(self):
        return self.gamebody.status

    def score(self, team):
        return self.gamebody.score(team)

    def elements(self, team):
        return self.gamebody.elements(team)

    def vision(self, team):
        return self.gamebody.vision(team)

    def view_elements(self, perspective):
        return self.gamebody.view_elements(perspective)

    def production_list(self, team):
        return self.gamebody.production_list(team)

    def population(self, team):
        return self.gamebody.population(team)

    def score_history(self):
        return self.history['score']

    def unit_num_history(self):
        return self.history['unit_num']

    def population_history(self):
        return self.history['population']

    def command_history(self):
        return self.history['command']

    def set_command(self, team, command):
        return self.gamebody.set_command(team, command)

    def commands(self, team):
        return self.gamebody.commands[team]

    def next_round(self):
        """Advance game to the next round, return events happened"""
        logger.debug('Moving to next round (round %d)', self.gamebody.round + 1)

        self.record_commands()  # commands of this round become fixed
        events = self.gamebody.run()
        # Record history & key frame as early as possible, or information of
        # the last round will be lost (because no next_round will be called).
        self.record_history()
        if self.gamebody.round % self.gamebody.record_interval == 0:
            self.record_key_frame()

        logger.info('Moved to round %d', self.gamebody.round)
        return events

    def save(self, filename, compact=False, compress=False):
        """Save game to file"""
        logger.debug('Saving game to "%s"', filename)

        save_file = (gzip.open if compress else open)(filename, 'w')
        contents = {
            "version": 1,
            "team_names": self.team_names,
            'history': self.history,
            'gamebody': self.gamebody.saves(),
            'key_frames': self.key_frames
        }
        if compact:
            json.dump(contents, save_file, sort_keys=True, separators=(',', ':'))
        else:
            json.dump(contents, save_file, indent=4, sort_keys=True)

        logger.info('Game saved to "%s"', filename)

    def record_history(self):
        history = self.history
        game = self.gamebody
        history['score'].append(copy.copy(game.scores))
        history['unit_num'].append([len(game.elements(0)),
                              len(game.elements(1))])
        history['population'].append(copy.copy(game.populations))

    def record_commands(self):
        self.history['command'].append(copy.copy(self.gamebody.commands))

    def record_key_frame(self):
        frame = (copy.deepcopy(self.gamebody.map_info.saves_elements()),
                 copy.deepcopy(self.gamebody.production_lists))
        self.key_frames.append(frame)

def load_prev_info(filename):
    """Load previous battle information from file"""
    try:
        prev_info = json.load(open(filename))
    except ValueError:
        logger.debug('Failed to load file, trying to treat it as gzipped file')
        try:
            prev_info = json.load(gzip.open(filename))
        except IOError:
            logger.error('%s: Not a valid save file', filename)
            raise IOError('Invalid save file')
    return prev_info

def load(filename):
    return Battle(None, prev_info=load_prev_info(filename))
