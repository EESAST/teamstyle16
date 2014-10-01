# -*- coding: UTF-8 -*-
# map.py
import sys
from basic import *

class MapInfo(object):
    """地图"""
    def __init__(self, types, elements = ELEMENTS):
        self.types = types            
        self.row = len(types)
        self.col = len(types[0])
        self.elements = elements

    def row(self):
        """Return row number of the map"""
        return self.row
    
    def col(self):
        """Return column number of the map"""
        return self.col
    
    def type(self, x, y): 
        """Return map type(OCEAN or LAND) on (x, y)"""
        return self.types[x][y]

    def element(self, pos):
        """Return element at pos"""
        for element in self.elements:
            if element.pos.distance(pos) == 0:
                return element
        return None

    def elements(self):
        """Return all elements"""
        return self.elements

    def update(self):
        """Update self.elements from basic.ELEMENTS"""
        self.elements = ELEMENTS

def save(map, filename):
    """Save map to file"""
    map_str = saves(map)
    open(filename).write(map_str)

def saves(map):
    """Save map to string"""
    # save map_type
    map_str = ''
    for line in map.types:
        for type in line:
            map_str += str(type)
        map_str += '\n'
    map_str += '#\n'
    # save elements
    for tuple in map.elements():
        map_str += '%d %d %d %d %d\n' % (tuple[0], tuple[1].x, tuple[1].y, tuple[1].z, tuple[2])
    return map_str

def load(filename):
    """Read map from file"""
    map_str = open(filename).read()
    return loads(map_str)

def loads(map_str):
    """Read map from string"""
    parts = map_str.split('#\n')
    types = []
    for line in parts[0].split():
        types.append([int(type) for type in line])
    elements_info = parts[1].split()
    elements = []
    k = 0
    while k < len(elements_info):
        tuple = (int(elements_info[k]))
        tuple += (Position(int(elements_info[k+1]), int(elements_info[k+2]), int(elements_info[k+3])))
        tuple += (int(elements_info[k+4]))
        elements.append(tuple)
        k += 5
    return Map(types, elements)
