#!/usr/bin/env python
# -*- coding: UTF-8 -*-
#生产播放逻辑

from PyQt4.QtGui import *
from PyQt4.QtCore import *
from lib.ui_ProductionPaint import *
import lib.qrc_source
from lib.PaintEvent import *
from platform import *

UNIT_WIDTH = 60
UNIT_HEIGHT = 45

FILE_UNIT = ["base", "fort", "mine", "oilfield", "submarine", "destroyer", "carrier", "cargo", "fighter", "scout"]
NUMBER = ["0a", "1a", "2a", "3a", "4a", "5a"]

class CreateTab(QTabWidget):
	def __init__(self, scene1, scene2, parent = None):
		super(CreateTab, self).__init__(parent)

		self.scene1 = scene1
		self.scene2 = scene2

		self.team1 = ProductionReplay(self.scene1, 0)
		self.team2 = ProductionReplay(self.scene2, 1)

		self.addTab(self.team1, "Team 0")
		self.addTab(self.team2, "Team 1")


class ProductionReplay(QGraphicsView):
	def __init__(self, scene, index, parent = None):
		super(ProductionReplay, self).__init__(parent)

		self.scene = scene
		self.setScene(self.scene)
		self.setAutoFillBackground(False)

		self.index = index
		self.objects1 = []
		self.numberList = []

		self.battle = None
		self.now_state = None
		self.animation = None
		self.run = False
		self.HUMAN_REPLAY = -1 #4代表ai对战，0代表左人右机，1相反，2代表全人，3代表回放
		self.nowRound = 0
		self.MapInfo = None

		self.stateMachine = QStateMachine(self)
		self.State_Run = QState(self.stateMachine)
		self.stateMachine.setInitialState(self.State_Run)
		self.State_Final = QFinalState(self.stateMachine)

		self.statelist = []

	def mousePressEvent(self, event):
		print "in mousePressEvent!1"
		if not self.run:
			QGraphicsView.mousePressEvent(self, event)
			return
		print "in mousePressEvent!2"
		if event.button() == Qt.LeftButton:
			if self.index != self.HUMAN_REPLAY:
				return
			print "in mousePressEvent!3"
			pos = event.pos()
			items = self.items(pos)

			for it in items:
				if isinstance(it, SoldierMakerUnit):
					print "in mousePressEvent!4"
					new_command = command.Produce(it.obj.kind)
					self.battle.add_command(new_command)

	def keyPressEvent(self, event):
		"""默认人人对战键盘只控制左边"""

		key_list = [Qt.Key_1, Qt.Key_2, Qt.Key_3, Qt.Key_4, Qt.Key_5, Qt.Key_6]


		if not self.run:
			QGraphicsView.keyPressEvent(self, event)
			return

		if self.index != self.HUMAN_REPLAY:
			return

		if event.key() in key_list:
			new_command = command.Produce(event.key() - Qt.Key_1 + 4)
			self.battle.add_command(new_command)


	def Initialize(self, battle):
		self.run = True
		self.num()
		self.battle = battle
		if not self.objects1:
			new_unit = SoldierMakerUnit(basic.Submarine(self.index, basic.Position(0,0,0)), 50)
			self.objects1.append(new_unit)
			new_unit.setPos(0,0,2)
			self.scene.addItem(new_unit)
			new_unit = SoldierMakerUnit(basic.Destroyer(self.index, basic.Position(0,0,0)), 50)
			self.objects1.append(new_unit)
			new_unit.setPos(2,0,2)
			self.scene.addItem(new_unit)
			new_unit = SoldierMakerUnit(basic.Carrier(self.index, basic.Position(0,0,0)), 50)
			self.objects1.append(new_unit)
			new_unit.setPos(4,0,2)
			self.scene.addItem(new_unit)
			new_unit = SoldierMakerUnit(basic.Cargo(self.index, basic.Position(0,0,0)), 50)
			self.objects1.append(new_unit)
			new_unit.setPos(0,2,2)
			self.scene.addItem(new_unit)
			new_unit = SoldierMakerUnit(basic.Fighter(self.index, basic.Position(0,0,0)), 50)
			self.objects1.append(new_unit)
			new_unit.setPos(2,2,2)
			self.scene.addItem(new_unit)
			new_unit = SoldierMakerUnit(basic.Scout(self.index, basic.Position(0,0,0)), 50)
			self.objects1.append(new_unit)
			new_unit.setPos(4,2,2)
			self.scene.addItem(new_unit)


		now_list = [9999,9999,9999,9999,9999,9999]

		if self.HUMAN_REPLAY in [0, 1]:
			if self.index == self.HUMAN_REPLAY:
				for prod in battle.production_list(self.index):
					if prod[1] < now_list[prod[0] - 4] and now_list[prod[0] - 4] != 0:
						now_list[prod[0] - 4] = prod[1]

		else:
			for prod in battle.production_list(self.index):
					if prod[1] < now_list[prod[0] - 4] and now_list[prod[0] - 4] != 0:
						now_list[prod[0] - 4] = prod[1]

		for i in range(6):
			if now_list[i] == 9999:
				number = EffectIndUnit("0")
			else:
				number = EffectIndUnit("%d" %now_list[i])
			if i < 3:
				number.setPos(i*60, 30)
			else:
				number.setPos((i-3)*60, 80)
			number.setScale(1.2)
			number.setDefaultTextColor(QColor(Qt.blue))
			self.scene.addItem(number)
			self.numberList.append(number)


	def reset(self):
		self.num()
		for i in self.objects1:
			if i.scene() == self.scene:
				self.scene.removeItem(i)
		self.objects1 = []
	def num(self):
		for i in self.numberList:
			if i.scene() == self.scene:
				self.scene.removeItem(i)
		self.numberList = []	

