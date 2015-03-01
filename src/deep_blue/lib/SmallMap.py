#!/usr/bin/env python
# -*- coding: UTF-8 -*-
#小地图定义

from PaintEvent import *
from platform import *

MAP_X = 100
MAP_Y = 100

class SmallMap(QGraphicsView):
	areaChanged = pyqtSignal(QPoint)
	def __init__(self, scene, parent = None):
		super(SmallMap, self).__init__(parent)

		self.scene = scene
		self.setScene(self.scene)

		self.chosenArea = ChosenIndUnit(0, 0, 0, 50)
		self.chosenArea.setVisible(False)
		self.scene.addItem(self.chosenArea)

		self.run = False
		self.left = 0
		self.right = 0
		self.up = 0
		self.down = 0
		self.init = False

		self.chosenarea = 50

		self.SmallMapList = []
		self.UnitBase = [[],[],[]]
		self.Unit_Info = [{},{},{}]

	def setSmallMap(self, m_x, m_y):
		self.resetSmallMap()
		self.run = True
		self.left = (MAP_X - self.width)/2
		self.right = (MAP_X + self.width)/2
		self.up = (MAP_Y - self.height)/2
		self.down = (MAP_Y + self.height)/2
		if m_x == -1:
			m_x = self.left
			m_y = self.up
		for i in range(self.left, self.right):
			for j in range(self.up, self.down):
				new_smallmap = SmallMapUnit(i - self.left, j - self.up, 2, self.Map_Info[i - self.left][j - self.up])
				self.scene.addItem(new_smallmap)
				new_smallmap.setPos(i,j,2,flag = False)
				self.SmallMapList.append(new_smallmap)
		if not self.init:
			if self.chosenArea in self.scene.items():
				self.scene.removeItem(self.chosenArea)
			self.chosenArea = ChosenIndUnit(m_x, m_y, 0, self.chosenarea)
			self.scene.addItem(self.chosenArea)
			self.chosenArea.setPos(m_x,m_y,0,flag = False)

	'''待修改'''
	def mousePressEvent(self, event):
		if not self.run:
			return
		pos = event.pos()
		if pos.x() < self.chosenarea/2 + self.left * 2:
			pos = QPoint(self.chosenarea/2 + self.left * 2, pos.y())
		elif pos.x() > self.right * 2 - self.chosenarea/2:
			pos = QPoint(self.right * 2 - self.chosenarea/2, pos.y())
		if pos.y() < self.chosenarea/2 + self.up * 2:
			pos = QPoint(pos.x(), self.chosenarea/2 + self.up * 2)
		elif pos.y() > self.down * 2 - self.chosenarea/2:
			pos = QPoint(pos.x(), self.down * 2- self.chosenarea/4)
		pos = QPoint(pos.x() - self.chosenarea/2, pos.y() - self.chosenarea/2)
		self.scene.removeItem(self.chosenArea)
		self.chosenArea = ChosenIndUnit(pos.x(), pos.y(), 0, self.chosenarea)
		self.scene.addItem(self.chosenArea)
		pos = (int(pos.x()/2), int(pos.y()/2))
		self.chosenArea.setPos(pos[0],pos[1],2,flag = False)
		pos = QPoint(pos[0] - self.left + (self.chosenarea - 50)/4, pos[1] - self.up + (self.chosenarea - 50)/4)
		self.emit(SIGNAL("areaChanged(QPoint)"), pos)

	def resetSmallMap(self):
		for item in self.SmallMapList:
			if item.scene() == self.scene:
				self.scene.removeItem(item)
		self.SmallMapList = []

	def resetUnit(self):
		for item in self.UnitBase[0]:
			if item.scene() == self.scene:
				self.scene.removeItem(item)
		for item in self.UnitBase[1]:
			if item.scene() == self.scene:
				self.scene.removeItem(item)
		for item in self.UnitBase[2]:
			if item.scene() == self.scene:
				self.scene.removeItem(item)
		self.UnitBase = [[],[],[]]

	def ScaleMap(self, factor):
		middle_x = self.chosenArea.corX + self.chosenarea/4
		middle_y = self.chosenArea.corY + self.chosenarea/4
		print "I get the signal:",self.chosenarea,
		self.chosenarea = self.chosenarea / factor
		print " to ",self.chosenarea
		self.init = False
		self.setSmallMap(middle_x - self.chosenarea/4, middle_y - self.chosenarea/4)
		self.init = True

	def reset(self):
		self.resetSmallMap()
		self.resetUnit()
		self.Unit_Info = [{},{},{}]
		self.run = False
		self.left = 0
		self.right = 0
		self.up = 0
		self.down = 0
		self.init = False
		self.scene.removeItem(self.chosenArea)

	def setUnit(self):
		self.resetUnit()
		for i in range(3):
			for j in self.Unit_Info[i].keys():
				new_unit = PointUnit(self.Unit_Info[i][j].position, i, self.Unit_Info[i][j].kind)
				self.UnitBase[i].append(new_unit)
				self.scene.addItem(new_unit)
				new_unit.setPos(new_unit.corX + self.left, new_unit.corY + self.up, new_unit.corZ, flag = False)

	def Initialize(self, battle, index):
		if not self.init:
			MapInfo = battle.map_info()
			self.width = MapInfo.x_max
			self.height = MapInfo.y_max
			self.Map_Info = [[0 for y in range(MapInfo.y_max)] for x in range(MapInfo.x_max)]
			for i in range(MapInfo.x_max):
				for j in range(MapInfo.y_max):
					self.Map_Info[i][j] = MapInfo.map_type(i, j)
			self.setSmallMap(-1,-1)
		self.Unit_Info = [{},{},{}]
		if index not in [0, 1]:
			Elements = [battle.elements(0), battle.elements(1)]
			for i in range(2):
				for element in Elements[i].values():
					self.Unit_Info[i][element.position] = element
		else:
			Elements = battle.view_elements(index)
			for element in Elements.values():
				if element.kind not in [2,3]:
					self.Unit_Info[element.team][element.position] = element
				else:
					self.Unit_Info[2][element.position] = element
		self.setUnit()
		if not self.init:
			rect = QRectF(0, 0, 200, 200)
			self.scene.setSceneRect(rect)
			self.fitInView(self.scene.sceneRect())
			self.init = True

