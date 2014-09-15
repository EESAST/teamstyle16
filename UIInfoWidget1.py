#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
from ui_gameinfo1 import *


class InfoWidget1(QWidget, Ui_GameInfo1):
	def __init__(self, parent = None):
		super(InfoWidget1, self).__init__(parent)
		self.setupUi(self)


if __name__ == "__main__":
	app = QApplication(sys.argv)
	form = InfoWidget1()
	form.show()
	app.exec_()