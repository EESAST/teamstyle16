#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
from ui_gameinfo2 import *


class InfoWidget2(QWidget, Ui_GameInfo2):
	def __init__(self, parent = None):
		super(InfoWidget2, self).__init__(parent)
		self.setupUi(self)


if __name__ == "__main__":
	app = QApplication(sys.argv)
	form = InfoWidget2()
	form.show()
	app.exec_()