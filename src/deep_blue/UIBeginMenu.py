#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
from ui_beginMenu import *


class BeginMenu(QWidget, Ui_BeginMenu):
	def __init__(self, parent = None):
		super(BeginMenu, self).__init__(parent)
		self.setupUi(self)
		palette = QPalette()
		palette.setBrush(QPalette.Window, QBrush(Qt.NoBrush))
		self.setPalette(palette)


if __name__ == "__main__":
	app = QApplication(sys.argv)
	form = BeginMenu()
	form.show()
	app.exec_()