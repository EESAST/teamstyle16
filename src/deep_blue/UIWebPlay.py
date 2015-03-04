#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
from ui_webPlay import *

class WebPlay(QWidget, Ui_Form):
	def __init__(self, parent = None):
		super(WebPlay, self).__init__(parent)
		self.setupUi(self)
		self.setAutoFillBackground(False)
		palette = QPalette()
		palette.setBrush(QPalette.Window, QBrush(Qt.NoBrush))
		self.setPalette(palette)

	def update_info(self, datas):
		print datas[1]
		if datas[1]:
			self.UserNameLabel.setText(" %s" %datas[1])
		else:
			self.UserNameLabel.setText("")
		self.IDLabel.setText(" %s" %datas[0])
		if datas[3]:
			self.TeamLabel.setText(" %s" %datas[3])
		else:
			self.TeamLabel.setText("")
		self.ScoreLabel.setText("")
