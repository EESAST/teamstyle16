#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
from ui_teamWidget import *


class TeamWidget(QWidget, Ui_TeamWidget):
	def __init__(self, parent = None):
		super(TeamWidget, self).__init__(parent)
		self.setupUi(self)
		self.setAutoFillBackground(False)
		palette = QPalette()
		palette.setBrush(QPalette.Window, QBrush(Qt.NoBrush))
		self.setPalette(palette)


if __name__ == "__main__":
	app = QApplication(sys.argv)
	form = TeamWidget()
	form.show()
	app.exec_()