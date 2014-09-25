#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
from ui_mapmaker import *


class MapMaker(QWidget, Ui_Mapmaker):
	def __init__(self, parent = None):
		super(MapMaker, self).__init__(parent)
		self.setupUi(self)
		self.setupUi(self)
		palette = QPalette()
		palette.setBrush(QPalette.Window, QBrush(Qt.NoBrush))
		self.setPalette(palette)

if __name__ == "__main__":
	app = QApplication(sys.argv)
	form = MapMaker()
	form.show()
	app.exec_()