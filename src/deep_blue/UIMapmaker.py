#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
from ui_mapmaker import *
from UIHelpdlg2 import *


class MapMaker(QWidget, Ui_Mapmaker):
	def __init__(self, parent = None):
		super(MapMaker, self).__init__(parent)
		self.setupUi(self)
		self.helpdlg = HelpDialog(self)
		self.helpdlg.setVisible(False)
		palette = QPalette()
		palette.setBrush(QPalette.Window, QBrush(Qt.NoBrush))
		self.setPalette(palette)

	@pyqtSlot()
	def on_HelpButton_clicked(self):
		self.helpdlg.setVisible(True)
