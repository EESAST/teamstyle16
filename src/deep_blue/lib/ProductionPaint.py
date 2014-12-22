#!/usr/bin/env python
# -*- coding: UTF-8 -*-
#生产框绘制

from PyQt4.QtGui import *
from PyQt4.QtCore import *
from ui_ProductionPaint import *
import qrc_source

UNIT_WIDTH = 60
UNIT_HEIGHT = 45

FILE_UNIT = ["base", "fort", "mine", "oilfield", "submarine1", "destroyer", "carrier", "cargo", "fighter", "scout"]
NUMBER = ["0a", "1a", "2a", "3a", "4a", "5a"]

class ProductionWidget(QGraphicsView, Ui_QProductionPaint):
	def __init__ (self, scene, parent = None):
		super(ProductionWidget, self).__init__(parent)
		self.scene = scene
		self.setScene(self.scene)
		self.setupUi(self)
		self.setAutoFillBackground(False)
		self.list1 = []
		self.list2 = []
		
		objects1 = [self.object1_1, self.object1_2, self.object1_3, self.object1_4, self.object1_5, self.object1_6]
		objects2 = [self.object2_1, self.object2_2, self.object2_3, self.object2_4, self.object2_5, self.object2_6]
	
	def Initialize (self):
		print "PWInit"

		for i in range(4,10):
			scene1 = ProductionScene(i)
			scene1.use(0)
			scene2 = ProductionScene(i)
			scene2.use(0)
			self.list1.append(scene1)
			self.list2.append(scene2)
			"""这个地方应该接收一个所有单位生产剩余回合数的列表，然后把带数字的图覆盖在GraphicsView上，现在需要：1.要接口 2.把新的图放进qrc_source里面"""
			objects1[i-4].setScene(scene1)
			objects2[i-4].setScene(scene2)
			objects1[i-4].show()
			objects2[i-4].show()
		


class ProductionScene(QGraphicsScene):
	def __init__(self, kind, parent = None):
		super(ProductionScene, self).__init__(parent)
		self.kind = kind

	def use(self, Round_to_go, kind):
		filename = ":" + FILE_UNIT[self.kind] + ".png"
		image = QImage(filename).convertToFormat(QImage.Format_ARGB32)
		image = image.scaled(42, 14, Qt.IgnoreAspectRatio)
		self.addPixmap(QPixmap.fromImage(image))
		filename = ":" + NUMBER[Round_to_go] + ".png"
		image = QImage(filename).convertToFormat(QImage.Format_ARGB32)
		image = image.scaled(15, 30, Qt.IgnoreAspectRatio)
		#self.addPixmap(QPixmap.fromImage(image))