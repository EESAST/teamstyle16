#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
from ui_GameInfoTabWidget import *


class InfoWidget(QTabWidget, Ui_InfoTabWidget):
	def __init__(self, parent = None):
		super(InfoWidget, self).__init__(parent)
		self.setupUi(self)


if __name__ == "__main__":
	app = QApplication(sys.argv)
	form = InfoWidget()
	form.show()
	app.exec_()