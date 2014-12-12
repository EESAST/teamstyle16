#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
from ui_helpdlg2 import *


class HelpDialog(QDialog, Ui_HelpDialog):
	def __init__(self, parent = None):
		super(HelpDialog, self).__init__(parent)
		self.setupUi(self)
		
		#self.setWindowFlags(Qt.FramelessWindowHint)


if __name__ == "__main__":
	app = QApplication(sys.argv)
	form = HelpDialog()
	form.show()
	app.exec_()