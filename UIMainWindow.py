#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
from ui_mainWindow import *


class MainWindow(QWidget, Ui_MainWindow):
	def __init__(self, parent = None):
		super(MainWindow, self).__init__(parent)
		self.setupUi(self)


if __name__ == "__main__":
	app = QApplication(sys.argv)
	form = MainWindow()
	form.show()
	app.exec_()