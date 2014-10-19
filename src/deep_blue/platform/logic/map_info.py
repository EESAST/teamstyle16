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

    def pathfinding(self, origin, dest, plane = False):
        nodes = [origin, dest]
        if origin == dest:
            return [origin, dest]

        adjacent = {origin: origin}        # Adjacent字典存储到达这个点的点
        vector = [(0,1),(1,0),(0,-1),(-1,0)]
        queue = []
        queue.append(origin)
        nearest = origin

        while True:
            center = queue[0]
            for one in vector:
                pos = Position(center.x + one[0], center.y + one[1], center.z)
                if (pos in adjacent or pos.x < 0 or pos.x >= self.x_max or pos.y < 0 or pos.y >= self.y_max
                    or (plane is False and self.map_type(pos.x, pos.y) == LAND)):
                    continue
                queue.append(pos)
                adjacent[pos] = center
                if pos.distance(dest) < nearest.distance(dest):
                    nearest = pos
                if pos == dest or len(queue) == 0:
                    if len(queue) == 0:
                        pos = nearest
                    adj = adjacent[pos]
                    while adj is not origin:
                        nodes.insert(1, adj)
                        adj = adjacent[adj]
                    while True:
                        if len(nodes) == 1:
                            return [origin, origin]
                        elif self.element(nodes[-1]) != None: # 目的地有其他单位
                            nodes.pop(-1)
                            continue
                        else:
                            return nodes
            queue.pop(0)

def load(filename):
    """Read map from file"""
    return loads(open(filename).read())

def loads(map_str):
    """Read map from string"""
    map_info = MyDecoder().decode(map_str)
    map_info.elements = {int(index_str): element for index_str, element in map_info.elements.items()}
    return map_info
