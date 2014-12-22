#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
from ui_webWidget import *

class WebWidget(QWidget, Ui_WebWidget):
	def __init__(self, parent = None):
		super(WebWidget, self).__init__(parent)
		self.setupUi(self)
		palette = QPalette()
		palette.setBrush(QPalette.Window, QBrush(Qt.NoBrush))
		self.setPalette(palette)
