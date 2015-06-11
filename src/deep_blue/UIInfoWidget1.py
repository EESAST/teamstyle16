#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
from ui_gameinfo1 import *
from deep_blue import *


class InfoWidget1(QWidget, Ui_GameInfo1):
	def __init__(self, parent = None):
		super(InfoWidget1, self).__init__(parent)
		self.setupUi(self)

	def setText(self, battle):
		self.RoundNumberLineEdit.setText("%d" %battle.round())
		self.PeopleNumberLineEdit1.setText("%d/%d" %(battle.population(0),battle.map_info().max_population))
		self.PeopleNumberLineEdit2.setText("%d/%d" %(battle.population(1),battle.map_info().max_population))
		self.ScoreLineEdit1.setText("%d" %battle.score(0))
		self.ScoreLineEdit2.setText("%d" %battle.score(1))
		airUnit = 0
		waterUnit = 0
		underWaterUnit = 0
		FortNumber = 0
		for element in battle.elements(0).values():
			if isinstance(element, basic.Base):
				self.BaseLifeLineEdit1.setText("%d/%d" %(element.health,element.health_max))
				self.SourceNumberLineEdit1.setText("%d/%d" %(element.metal,element.fuel))
			if isinstance(element, basic.Fort):
				FortNumber += 1
			if isinstance(element, basic.Submarine):
				underWaterUnit += 1
			if isinstance(element, basic.Ship):
				waterUnit += 1
			if isinstance(element, basic.Plane):
				airUnit += 1
		self.SubmarineNumberLineEdit1.setText("%d" %underWaterUnit)
		self.ShipNumberLineEdit1.setText("%d" %waterUnit)
		self.PlaneNumberLineEdit1.setText("%d" %airUnit)
		self.FortNumberLineEdit1.setText("%d" %FortNumber)
		airUnit = 0
		waterUnit = 0
		underWaterUnit = 0
		FortNumber = 0
		for element in battle.elements(1).values():
			if isinstance(element, basic.Base):
				self.BaseLifeLineEdit2.setText("%d/%d" %(element.health,element.health_max))
				self.SourceNumberLineEdit2.setText("%d/%d" %(element.metal,element.fuel))
			if isinstance(element, basic.Fort):
				FortNumber += 1
			if isinstance(element, basic.Submarine):
				underWaterUnit += 1
			if isinstance(element, basic.Ship):
				waterUnit += 1
			if isinstance(element, basic.Plane):
				airUnit += 1
		self.SubmarineNumberLineEdit2.setText("%d" %underWaterUnit)
		self.ShipNumberLineEdit2.setText("%d" %waterUnit)
		self.PlaneNumberLineEdit2.setText("%d" %airUnit)
		self.FortNumberLineEdit2.setText("%d" %FortNumber)

	def reset(self):
		self.RoundNumberLineEdit.setText("")
		self.PeopleNumberLineEdit1.setText("")
		self.PeopleNumberLineEdit2.setText("")
		self.BaseLifeLineEdit1.setText("")
		self.SourceNumberLineEdit1.setText("")
		self.SubmarineNumberLineEdit1.setText("")
		self.ShipNumberLineEdit1.setText("")
		self.PlaneNumberLineEdit1.setText("")
		self.FortNumberLineEdit1.setText("")
		self.BaseLifeLineEdit2.setText("")
		self.SourceNumberLineEdit2.setText("")
		self.SubmarineNumberLineEdit2.setText("")
		self.ShipNumberLineEdit2.setText("")
		self.PlaneNumberLineEdit2.setText("")
		self.FortNumberLineEdit2.setText("")
		self.ScoreLineEdit1.setText("")
		self.ScoreLineEdit2.setText("")



if __name__ == "__main__":
	app = QApplication(sys.argv)
	form = InfoWidget1()
	form.show()
	app.exec_()
