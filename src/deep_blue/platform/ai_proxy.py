import threading, time, socket

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
        pass

    def __run_ai(self):
        pass

    def __send_stable_info(self, battle):
        """Send infos that do not change over rounds to the AI"""
        pass

    def __send_round_info(self, battle):
        """Send infos that change over rounds to the AI"""
        pass

    def __decode_commands(self, data):
        """Decode incoming data into list of commands"""
        pass    #return [Commands]

    def __encode_stable_info(self, battle):
        """Encode stable information of battle into str"""
        pass

    def __encode_round_info(self, battle):
        """Encode round information of battle into str"""
        pass

class AIBattle(Battle):
    def __init__(self, map_info, ai0_filename, ai1_filename, port_AI, port):
        self.battle = Battle(map_info)
        self.AI_0 = AIProxy(1, ai0_filename, port_AI)
        self.AI_1 = AIProxy(2, ai1_filename, port_AI)

    def feed_ai_commands():
        time.sleep(10)  #can be switched
        ai0_cmds = self.AI_0.get_commands()
        ai1_cmds = self.AI_1.get_commands()

    def run_until_end():
        try:
            self.AI_0.start()
            self.AI_1.start()   #I don't know how to call send_stable_info or send_round_info here
            #if game is over:
                #AI_0.stop()
                #AI_1.stop()
        except:
            raise ...
