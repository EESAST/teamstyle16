#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
from ui_aireplayer import *
from UIInfoWidget1 import *
from UIInfoWidget2 import *


class AIReplayerWidget(QWidget, Ui_AIReplayer):
	def __init__(self, parent = None):
		super(AIReplayerWidget, self).__init__(parent)
		self.setupUi(self)
		self.infoWidget = InfoWidget1(self)
		self.dockWidget.setWidget(self.infoWidget)
		self.dockWidget.setFeatures(QDockWidget.NoDockWidgetFeatures)
		self.infoWidget = InfoWidget2(self)
		self.dockWidget_2.setWidget(self.infoWidget)
		self.dockWidget_2.setFeatures(QDockWidget.NoDockWidgetFeatures)


if __name__ == "__main__":
	app = QApplication(sys.argv)
	form = AIReplayerWidget()
	form.show()
	app.exec_()