import threading, socket

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
            except timeout:
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
        pass

    def __send_stable_info(self, battle):
        """Send infos that do not change over rounds to the AI"""
        self.sock_AI.send(self.__encode_stable_info(battle))

    def __send_round_info(self, battle):
        """Send infos that change over rounds to the AI"""
        self.sock_AI.send(self.__encode_round_info(battle))

    def __decode_commands(self, data):
        """Decode incoming data into list of commands"""
        pass    #return [Commands]

    def __encode_stable_info(self, battle):
        """Encode stable information of battle into str"""
        pass

    def __encode_round_info(self, battle):
        """Encode round information of battle into str"""
        pass        