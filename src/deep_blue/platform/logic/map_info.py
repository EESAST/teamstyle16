# -*- coding: UTF-8 -*-
# map.py

from copy import deepcopy
from basic import *
from custom_json import *

class MapInfo(object):
    """地图"""
    def __init__(self, x_max, y_max, max_population, max_round, record_interval, time_per_round, weather, **kwargs):
        """Create an empty map"""
        self.x_max = x_max
        self.y_max = y_max
        self.types = kwargs['types'] if 'types' in kwargs else [[0 for y in xrange(y_max)] for x in xrange(x_max)]
        self.elements = kwargs['elements'] if 'elements' in kwargs else {}
        self.max_round = max_round
        self.max_population = max_population
        self.record_interval = record_interval
        self.time_per_round = time_per_round
        self.weather = weather

    def map_type(self, x, y):
        """Return map type(OCEAN or LAND) on (x, y)"""
        return self.types[x][y]

    def set_map_type(self, x, y, map_type = LAND):
        """Set type on (x, y) to map_type"""
        if x >= self.x_max or y >= self.y_max:
            return False
        else:
            self.types[x][y] = min(map_type, LAND)      # in case map_type = 2, 3, etc..
            return True

    def element(self, pos):
        """Return element at pos"""
        for element in self.elements.values():
            # in case isinstance(element.pos, Rectangle), distance() is ok.
            if element.level == pos.level and element.pos.distance(pos) == 0:
                return element
        return None

    def add_element(self, new_element):
        """Add a new element to current map"""
        for point in new_element.pos.region(level = new_element.level, range = 0):
            if point.x >= self.x_max or point.y >= self.y_max:
                return None                     # 位置无效
            elif self.element(point) != None:
                return None                     # 位置被占用
        index = choice(xrange(10000))           # 10000以内随机生成index
        while index in self.elements.keys():    # 检查是否与已有index冲突
            index += 10000                      # 尝试解决冲突
        new_element.index = index
        self.elements[index] = new_element
        return index

    def save(self, filename):
        """Save map to file"""
        open(filename, 'w').write(self.saves())

    def saves(self):
        """Save map to string"""
        return MyEncoder(sort_keys=True, separators=(',', ':')).encode(self)

    def saves_elements(self):
        """Save elements to string"""
        return MyEncoder(sort_keys=True, separators=(',', ':')).encode(self.elements)

    def loads_elements(self, string):
        """Load elements to current map from string"""
        tmp = MyDecoder().decode(string)
        self.elements = {int(index_str): element for index_str, element in tmp.items()}
    def pathfinding(origin, dest):
        origin.x = max(min(self.x_max, origin.y), 0)
        origin.y = max(min(self.y_max, origin.y), 0)
        origin.z = max(min(origin.z, AIR), UNDERWATER)
        dest.x = max(min(self.x_max, dest.y), 0)
        dest.y = max(min(self.y_max, dest.y), 0)
        dest.z = origin.z
        nodes = [origin, dest]
        Adjacent = {}        #Adjacent字典存储到达这个点的点
        vector = [(0,1),(1,0),(0,-1),(-1,1)]
        List = []
        List.append(origin)
        while 1:
            center = List[0]
            for one in vector:
                pos = Position(center.x + one[0], center.y + one[1], center.z)
                if(pos == dest):
                    if(isinstance(self.element(pos), Building) or isinstance(self.element(pos), Resources)):
                        nodes.pop(-1)          #如果目的地是陆地单位或资源单位          
                    adj = Adjacent[dest]
                    while adj is not origin:
                        nodes.insert(1, adj)
                        adj = Adjacent[adj]
                    return nodes
                if not isinstance(self.element(pos), Building)   and not isinstance(self.element(pos), Resources)
                   and max(min(self.x_max, pos.x), 0) == pos.x and max(min(self.y_max, pos.y), 0)) == pos.y :           #最小坐标是0还是1      
                    List.append(pos)
                    Adjacent[pos] = center
            List.pop(0)

def load(filename):
    """Read map from file"""
    return loads(open(filename).read())

def loads(map_str):
    """Read map from string"""
    map_info = MyDecoder().decode(map_str)
    map_info.elements = {int(index_str): element for index_str, element in map_info.elements.items()}
    return map_info
