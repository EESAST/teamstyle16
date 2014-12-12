#!/usr/bin/env python
# -*- coding: UTF-8 -*-
#生产播放逻辑

from PyQt4.QtGui import *
from PyQt4.QtCore import *
from ui_ProductionPaint import *
import qrc_source
from platform import *

UNIT_WIDTH = 60
UNIT_HEIGHT = 45

FILE_UNIT = ["base", "fort", "mine", "oilfield", "submarine", "destroyer", "carrier", "cargo", "fighter", "scout"]
NUMBER = ["0a", "1a", "2a", "3a", "4a", "5a"]

class ProductionReplay(QGraphicsView, Ui_QProductionPaint):
	def __init__(self, scene, parent = None):
		super(ProductionReplay, self).__init__(parent)

		self.scene = scene
		self.setScene(self.scene)
		self.setupUi(self)
		self.setAutoFillBackground(False)
		self.list1 = []
		self.list2 = []
		
		self.objects1 = [self.object1_1, self.object1_2, self.object1_3, self.object1_4, self.object1_5, self.object1_6]
		self.objects2 = [self.object2_1, self.object2_2, self.object2_3, self.object2_4, self.object2_5, self.object2_6]

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

	def MousePressEvent(self, event):
		if not self.run:
			QGraphicsView.mousePressEvent(self, event)
			return

		if Qt.LeftButton:
			pos = event.pos()
			items = self.items(pos)
			focus = None

			for it in items:
				if it in ProductionWidget.objects1 and self.HUMAN_REPLAY in [0, 2]:
					index = ProductionWidget.objects1.index(it)
					new_command = command.Produce(index + 4)
					GameBody.set_command(1, new_command)
				elif it in ProductionWidget.objects2 and self.HUMAN_REPLAY in [1, 2]:
					index = ProductionWidget.objects2.index(it)
					new_command = command.Produce(index + 4)
					GameBody.set_command(2, new_command)
				else:
					return

		if Qt.RightButton:
			return

	def KeyPressEvent(self, event):
		"""默认人人对战键盘只控制左边"""

		key_list = [Qt.Key_1, Qt.Key_2, Qt.Key_3, Qt.Key_4, Qt.Key_5, Qt.Key_6]

		if not self.run:
			QGraphicsView.keyPressEvent(self, event)
			return

		if event.key() in key_list:
			if self.HUMAN_REPLAY in [0, 2]:
				index = key_list.index(event.key())
				new_command = command.Produce(index + 4)
				GameBody.set_command(1, new_command)
			elif self.HUMAN_REPLAY == 1:
				index = key_list.index(event.key())
				new_command = command.Produce(index + 4)
				GameBody.set_command(2, new_command)
			else:
				return

		else:
			return

	def Initialize(self,battle):
		if not self.list1:
			for i in range(4,10):
				scene1 = ProductionScene(i)
				scene1.use(0)
				self.list1.append(scene1)
				self.objects1[i-4].setScene(scene1)
				self.objects1[i-4].show()

		if not self.list2:
			for i in range(4,10):
				scene2 = ProductionScene(i)
				scene2.use(0)
				self.list2.append(scene2)
				self.objects2[i-4].setScene(scene2)
				self.objects2[i-4].show()	

		now_list = [[9999,9999,9999,9999,9999,9999],[9999,9999,9999,9999,9999,9999]]

		for prod in battle.production_list(0):
			if prod[1] < now_list[0][prod[0] - 4] and now_list[0][prod[0] - 4] != 0:
				now_list[0][prod[0] - 4] = prod[1]

		for prod in battle.production_list(1):
			if prod[1] < now_list[1][prod[0] - 4] and now_list[1][prod[0] - 4] != 0:
				now_list[1][prod[0] - 4] = prod[1]

		for i in range(6):
			if now_list[0][i] == 9999:
				self.list1[i].use(0)
				continue
			self.list1[i].use(now_list[0][i])

		for i in range(6):
			if now_list[1][i] == 9999:
				self.list2[i].use(0)
				continue
			self.list2[i].use(now_list[1][i])

	def reset(self):
		for i in self.list1:
			i.reset()
		for i in self.list2:
			i.reset()	


class ProductionScene(QGraphicsScene):
	def __init__(self, kind, parent = None):
		super(ProductionScene, self).__init__(parent)
		self.kind = kind

	def reset(self):
		self.clear()

	def use(self, Round_to_go):
		self.reset()
		filename = ":" + FILE_UNIT[self.kind] + ".png"
		image = QImage(filename).convertToFormat(QImage.Format_ARGB32)
		image = image.scaled(42, 14, Qt.IgnoreAspectRatio)
		self.addPixmap(QPixmap.fromImage(image))
		filename = ":" + NUMBER[Round_to_go] + ".png"
		image = QImage(filename).convertToFormat(QImage.Format_ARGB32)
		image = image.scaled(15, 30, Qt.IgnoreAspectRatio)
		self.addPixmap(QPixmap.fromImage(image))

