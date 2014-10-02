import ai_proxy
import socket
from logic import basic

class FakeElement(basic.UnitBase):
    def __init__(self):
        self.index = 42
        self.pos = basic.Position(1, 2, 3)
        self.kind = 8
        self.visible = True
        self.team = 1
        self.health = 100
        self.fuel = 50
        self.ammo = 70
        self.metal = 88
        self.dest = basic.Position(5, 4, 2)

class FakeMapInfo(object):
    def __init__(self):
        self.weather = 2
        self.max_population = 200
        self.max_round = 100
        self.time_per_round = 1.0
        self.elements = [FakeElement()]
    def x_max(self):
        return 70
    def y_max(self):
        return 60
    def map_type(self, x, y):
        if (x + y) % 5 == 0:
            return 1
        else:
            return 0

class FakeBattle(object):
    def __init__(self):
        self.__map_info = FakeMapInfo()
    def round(self):
        return 0
    def map_info(self):
        return self.__map_info
    def production_list(self, team):
        return [(4, 10), (2, 8), (3, 2)]
    def population(self, team):
        return 888
    def score(self, team):
        return 2048
    def elements(self, team):
        return self.__map_info.elements

battle = FakeBattle()

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind(('', 8099))
sock.listen(2)
AI_0 = ai_proxy.AIProxy(0, sock)
AI_1 = ai_proxy.AIProxy(1, sock)
AI_0.start()
AI_1.start()
AI_0.send_info(battle)
AI_1.send_info(battle)
