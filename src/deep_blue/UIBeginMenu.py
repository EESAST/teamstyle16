#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
from ui_beginMenu import *
import qrc_source

class BeginMenu(QWidget, Ui_BeginMenu):
	def __init__(self, parent = None):
		super(BeginMenu, self).__init__(parent)
		self.setupUi(self)
		palette = QPalette()
		palette.setBrush(QPalette.Window, QBrush(Qt.NoBrush))
		self.setPalette(palette)
		self.SinglePlayerButton.setStyleSheet("#SinglePlayerButton{border-image:url(:1.png);}")
		self.OnlinePlayButton.setStyleSheet("#OnlinePlayButton{border-image:url(:2.png);}")
		self.MapEditorButton.setStyleSheet("#MapEditorButton{border-image:url(:3.png);}")
		self.AboutUsButton.setStyleSheet("#AboutUsButton{border-image:url(:4.png);}")
		self.ExitGameButton.setStyleSheet("#ExitGameButton{border-image:url(:5.png);}")


if __name__ == "__main__":
	app = QApplication(sys.argv)
	form = BeginMenu()
	form.show()
	app.exec_()