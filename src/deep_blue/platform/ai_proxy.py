import threading, socket
import subprocess
import struct
from logic import basic

class AIProxy(threading.Thread):
    """Proxy for AI"""
    def __init__(self, team_num, file_name, port_AI):
        threading.Thread.__init__(self)
        self.lock = threading.RLock()
        self.team_num = team_num
        self.file_name = file_name
        self.commands = []
        self.sock_AI = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock_AI.bind(('', port_AI))
        self.sock_AI.listen(5)
        self.sock_AI.settimeout(10)
        self.stop_flag = False

        self.ai_program = None
        self.__run_ai()
        self.conn, self.addr = self.sock_AI.accept()
        self.team_name = self.__get_team_name()

    def stop(self):
        self.stop_flag = True

    def run(self):
        count = 0
        while not self.stop_flag:
            try:
                data = self.conn.recv(1024)
                cmd = self.__decode_commands(data.decode())
                self.lock.acquire()
                self.commands.extend(cmd)
                self.lock.release()
            except socket.timeout:
                continue

    def get_commands(self):
        """Get accepted commands"""
        commands = self.commands[:]
        self.lock.acquire()
        self.commands = []
        self.lock.release()
        return commands

    def send_info(self, battle):
        """Send infomations to AI"""
        if battle.GameBody.round() == 0:
            self.__send_stable_info()
            self.__send_round_info()
        else:
            self.__send_round_info()

    def __run_ai(self):
        self.ai_program = subprocess.Popen(self.file_name,
                                           stdout=subprocess.PIPE,
                                           stderr=subprocess.STDOUT)

    def __get_team_name(self):
        name = self.conn.recv(32)
        return name.strip()

    def __send_stable_info(self, battle):
        """Send infos that do not change over rounds to the AI"""
        self.sock_AI.send(self.__encode_stable_info(battle))

    def __send_round_info(self, battle):
        """Send infos that change over rounds to the AI"""
        self.sock_AI.send(self.__encode_round_info(battle))

    def __decode_commands(self, data):
        """Decode incoming data into list of commands"""
        cmd_strs = data.split(',')
        if not cmd_strs[-1]:  # last one is empty
            cmd_strs.pop()

        cmds = []
        for cmd_str in cmd_strs:
            name, args = cmd_str[:2], cmd_str[2:].split()
            args = map(int, args)  # map to ints

            if name == 'ap':
                cmd = basic.AttackPos(args[0], basic.Position(*args[1:]))
            elif name == 'au':
                cmd = basic.AttackUnit(*args)
            elif name == 'cd':
                cmd = basic.ChangeDest(args[0], basic.Position(*args[1:]))
            elif name == 'fx':
                cmd = basic.Fix(*args)
            elif name == 'pd':
                cmd = basic.Produce(*args)
            elif name == 'sp':
                cmd = basic.Supply(*args)
            elif name == 'cc':
                cmd = basic.Cancel()

            cmds.append(cmd)

        return cmds

    def __encode_stable_info(self, battle):
        """Encode stable information of battle into str"""
        map_info = battle.map_info()
        header = struct.pack('6if', map_info.x_max(),
                                    map_info.y_max(),
                                    map_info.weather,
                                    self.team_num,
                                    map_info.max_population,
                                    map_info.max_round,
                                    map_info.time_per_round)
        # serialize map
        map_array = []
        for x in xrange(map_info.x_max()):
            for y in xrange(map_info.y_max()):
                map_array.append(map_info.map_type(x, y))
        body = struct.pack(str(len(map_array)) + 'i', *map_array)

        return header + body

    def __encode_round_info(self, battle):
        """Encode round information of battle into str"""
        map_info = battle.map_info()
        production_list = battle.production_list(self.team_num)
        header = struct.pack('6i', battle.round(),
                                   len(map_info.element),
                                   battle.population(self.team_num),
                                   len(production_list),
                                   battle.score(0),
                                   battle.score(1))

        body = ''
        for entry in production_list:
            body = body + struct.pack('2i', entry.kind, entry.round_left)
        for element in battle.elements(self.team_num):
            body = body + self.__serialize_element(element)

        return header + body

    def __serialize_element(self, element):
        index = element.index
        pos = element.pos
        kind = element.kind
        visible = element.visible
        if isinstance(element, basic.UnitBase):
            team = element.team
            health = element.health
            fuel = element.fuel
            ammo = element.ammo
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
