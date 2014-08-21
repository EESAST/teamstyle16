>>>
# -*- coding: UTF-8 -*-
# map.py
import sys
sys.path.append('F:\\teamstyle\\teasmtyle16\\src')
from basic import *

"""地图的初始数据"""
MapSize = 80
(Base_Metal,Mine_Metal,Oilfield_Fuel) = (100,30,30)  #瞎写的
F_M_O_pos=                       #Fort_Mine_Oilfield_position
   [
    [(),(),(),(),(),()],         #据点的position
     [(),(),(),(),(),(),(),()]   #矿场的position
     [(),(),(),(),(),(),(),()]   #油田的position
    ]

"""由初始的F_M_O_pos构造出Position列表"""
pos=[[],[],[]]
FMO_NUM_MAX=[FORT_NUM_MAX,MINE_NUM_MAX,OILFIELD_NUM_MAX]
for i in range(3) :
    for j in range(FMO_NUM_MAX[i]) :
        pos[i].append(Position(F_M_O_pos[i][j][0],F_M_O_pos[i][j][1]))
        
"""初始化Map_Elements"""
Map_Elements=[]
Base0 = Base(0,Rectangle(Position(0,3),Position(3,0)),Base_Metal)
Base1 = Base(1,Rectangle(Position(MapSize-3,MapSize),Position(MapSize,MapSize-3)),Base_Metal)
Map_Elements.append(Base0)
Map_Elements.append(Base1)
for fort in range(FORT_NUM_MAX) :
    Map_Elements.append(Fort(-1,pos[0][fort]))
for mine in range(MINE_NUM_MAX) :
    Map_Elements.append(Mine('MINE',pos[1][mine],Mine_Metal))
for oilfield in range(OILFIELD_NUM_MAX) :
    Map_Elements.append(Oilfield('OILFIELD',pos[2][oilfield],Oilfield_Fuel))

    """地图类"""
class Map(object):
    """地图"""
    def __init__(self,Mapsize,Map_Elements):
        self.Mapsize = Mapsize
        self.Map_Elements = Map_Elements

    def row():
        """Return row number of the map"""
        return self.Mapsize
    
    def col():
        """Return column number of the map"""
        return self.Mapsize
    
    def map_type(x,y):   #type以字符串的方式存储怎么返回int啊
        """Return map type on (x, y)"""
        for element in Map_Elements ：
            if element.type == 'BASE' or element.type == 'FORT' :
                upper_left = element.pos.upper_left
                lower_right = element.pos.lower_right
                if (x in range(upper_left.x,lower_right.x)
                   and y in range(lower_right.y,upper_left.y)):
                    return element.type 
            else :
                if element.pos.x == x and element.pos.y == y
                    return element.type
        return OCEAN
    
    def elements():
        """Return type and position of every element"""
        all_elements=[]
        for element in Map_Elements ：
            all_elements.append((element.type,element.pos))
        return all_elements
