import threading, socket
import subprocess
import struct
import logging
from logic import basic
from logic import command

logging.basicConfig(level=logging.INFO)

DEFAULT_PORT = 8067

class AIError(IOError):
    """Describe error caused by AI"""
    def __init__(self, what):
        super(AIError, self).__init__(what)

class AIFileError(AIError):
    """Failed to run AI file"""
    def __init__(self, what):
        super(AIFileError, self).__init__(what)

class AIConnectError(AIError):
    """Connection closed unexpectly"""
    def __init__(self, what):
        super(AIConnectError, self).__init__(what)

class ParseError(AIError):
    """Failed to parse commands sent by AI"""
    def __init__(self, what):
        super(ParseError, self).__init__(what)


class AIProxy(threading.Thread):
    """Proxy for AI"""
    def __init__(self, team_num, sock, battle, filename=None):
        threading.Thread.__init__(self)
        self.lock = threading.RLock()
        self.team_num = team_num
        self.commands = []
        self.stop_flag = False

        self.logger = logging.getLogger('%s.ai%d' % (__name__, team_num))

        port = sock.getsockname()[1]
        # start AI (if needed) and connect
        self.ai_program = None
        if filename is not None:
            self.logger.info('Starting AI (%s)', filename)
            self.__run_ai(filename, port)
            self.logger.info('AI started')

        self.logger.info('Waiting for connection at port %d', port)
        self.conn, self.addr = sock.accept()
        self.logger.debug('AI connected, getting team name')
        self.team_name = self.__get_team_name()
        self.logger.info('Connected to AI "%s"', self.team_name)

        # Send stable info
        self.logger.info('Sending stable info to AI')
        self.__send_stable_info(battle)
        self.logger.info('Info sent')

    def __del__(self):
        self.stop()

    def stop(self):
        # Terminate AI program if needed
        if self.ai_program:
            self.logger.info("Terminaing AI")
            self.ai_program.terminate()
        self.logger.info("Stop receiving commands from AI")
        self.stop_flag = True

    def run(self):
        self.logger.info('Starting loop for receiving commands')
        while not self.stop_flag:
            try:
                self.logger.debug('Receiving commands')
                data = self.conn.recv(1024)

                if len(data) == 0:
                    self.logger.error('Connection shutdown orderly by AI')

                    # If started the AI, check its return value if possible
                    if self.ai_program:
                        return_code = self.ai_program.returncode
                        if return_code is not None:
                            self.logger.info('AI returned wih value %d',
                                             return_code)
                        else:
                            self.logger.info('AI has not stopped yet')

                    raise AIConnectError('Connection shutdown orderly by AI %d'
                                         % self.team_num)

                self.logger.debug('Data received (size: %d)', len(data))
                cmds = self.__decode_commands(data.decode())
                self.logger.info('%d command(s) decoded', len(cmds))

                self.lock.acquire()
                self.commands.extend(cmds)
                self.lock.release()
            except socket.timeout:
                self.logger.debug('Receiving timeout, continue to next loop')
                continue
        self.logger.info('Receiving thread terminated')

    def get_commands(self):
        """Get accepted commands"""
        self.lock.acquire()
        commands = self.commands[:]
        self.commands = []
        self.lock.release()
        return commands

    def send_info(self, battle):
        """Send infomations to AI"""
        self.logger.info('Sending round info to AI')
        self.__send_round_info(battle)
        self.logger.info('Info sent')

    def __run_ai(self, filename, port):
        self.ai_program = subprocess.Popen([filename, 'localhost', str(port)],
                                           stdout=subprocess.PIPE,
                                           stderr=subprocess.STDOUT)

    def __get_team_name(self):
        name = self.conn.recv(32)
        return name.strip()

    def __send_stable_info(self, battle):
        """Send infos that do not change over rounds to the AI"""
        self.conn.send(self.__encode_stable_info(battle))

    def __send_round_info(self, battle):
        """Send infos that change over rounds to the AI"""
        self.conn.send(self.__encode_round_info(battle))

    def __decode_commands(self, data):
        """Decode incoming data into list of commands"""
        cmd_strs = data.split(',')
        if not cmd_strs[-1]:  # last one is empty
            cmd_strs.pop()

        cmds = []
        for cmd_str in cmd_strs:
            name, args = cmd_str[:2], cmd_str[2:].split()

            # Try to parse a command
            try:
                args = map(int, args)  # map to ints
                if name == 'ap':
                    cmd = command.AttackPos(args[0], basic.Position(*args[1:]))
                elif name == 'au':
                    cmd = command.AttackUnit(*args)
                elif name == 'cd':
                    cmd = command.ChangeDest(args[0], basic.Position(*args[1:]))
                elif name == 'fx':
                    cmd = command.Fix(*args)
                elif name == 'pd':
                    cmd = command.Produce(*args)
                elif name == 'sp':
                    # negative value means infinity
                    for i in [2, 3, 4]:
                        if args[i] < 0:
                            args[i] = basic.INFINITY
                    cmd = command.Supply(*args)
                elif name == 'cc':
                    cmd = command.Cancel(*args)
                else:  # not a valid command
                    self.logger.error('Unknown command name "%s"' % name)
                    continue
            except StandardError as e:
                self.logger.error('Failed to construct command from "%s": %s',
                                  cmd_str, e)
            else:
                cmds.append(cmd)

        return cmds

    def __encode_stable_info(self, battle):
        """Encode stable information of battle into str"""
        map_info = battle.map_info()
        header = struct.pack('6if', map_info.x_max,
                                    map_info.y_max,
                                    map_info.weather,
                                    self.team_num,
                                    map_info.max_population,
                                    map_info.max_round,
                                    map_info.time_per_round)
        # serialize map
        map_array = []
        for x in xrange(map_info.x_max):
            for y in xrange(map_info.y_max):
                map_array.append(map_info.map_type(x, y))
        body = struct.pack(str(len(map_array)) + 'i', *map_array)

        return header + body

    def __encode_round_info(self, battle):
        """Encode round information of battle into str"""
        map_info = battle.map_info()
        production_list = battle.production_list(self.team_num)
        view_elements = battle.view_elements(self.team_num)

        header = struct.pack('6i', battle.round(),
                                   len(view_elements),
                                   battle.population(self.team_num),
                                   len(production_list),
                                   battle.score(0),
                                   battle.score(1))

        body = ''
        for entry in production_list:
            body = body + struct.pack('2i', entry[0], entry[1])
        for element in view_elements.values():
            body = body + self.__serialize_element(element)

        return header + body

    def __serialize_element(self, element):
        index = element.index
        pos = element.pos
        kind = element.kind
        # Elements in sight might lack attribute 'visible'
        visible = element.visible if hasattr(element, 'visible') else True
        if isinstance(element, basic.UnitBase):
            team = element.team
            health = element.health
            fuel = element.fuel
            ammo = element.ammo if element.ammo != basic.INFINITY else -1
            metal = element.metal
            dest = element.dest if isinstance(element, basic.Unit) else pos
        else:
            team = 2
            health = fuel = ammo = metal = 0
            if isinstance(element, basic.Mine):
                metal = element.metal
            else:  # is a Oilfield
                fuel = element.fuel
            dest = pos

        return struct.pack('6i?7i', index, pos.x, pos.y, pos.z, kind, team,
                                    visible, health, fuel, ammo, metal,
                                    dest.x, dest.y, dest.z)
