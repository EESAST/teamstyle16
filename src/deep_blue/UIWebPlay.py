#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
from ui_webPlay import *

REPLAY_FILE_DIR = "."

class WebPlay(QWidget, Ui_Form):
	def __init__(self, parent = None):
		super(WebPlay, self).__init__(parent)
		self.setupUi(self)
		self.setAutoFillBackground(False)
		palette = QPalette()
		palette.setBrush(QPalette.Window, QBrush(Qt.NoBrush))
		self.setPalette(palette)
		self.AI1 = None
		self.AI2 = None
		self.AI3 = None
		self.AI4 = None
		self.AI5 = None

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

	@pyqtSlot()
	def on_B1Button_clicked(self):
		QMessageBox.information(self, QString.fromUtf8("第一关"), QString.fromUtf8("选手生产出单位即为胜利"), QMessageBox.Ok)

	@pyqtSlot()
	def on_B2Button_clicked(self):
		QMessageBox.information(self, QString.fromUtf8("第二关"), QString.fromUtf8("选手收集到资源补充给基地使得基地各资源达到最大值"), QMessageBox.Ok)

	@pyqtSlot()
	def on_B3Button_clicked(self):
		QMessageBox.information(self, QString.fromUtf8("第三关"), QString.fromUtf8("选手攻打某个据点并占领"), QMessageBox.Ok)

	@pyqtSlot()
	def on_B4Button_clicked(self):
		QMessageBox.information(self, QString.fromUtf8("第四关"), QString.fromUtf8("选手在自己方单位没有死亡的前提下攻击敌方单位并使其死亡(对手为空AI)"), QMessageBox.Ok)

	@pyqtSlot()
	def on_B5Button_clicked(self):
		QMessageBox.information(self, QString.fromUtf8("第五关"), QString.fromUtf8("胜利条件与正式赛相同(对手为空AI)"), QMessageBox.Ok)

	@pyqtSlot()
	def on_U1Button_clicked(self):
		self.AI1 = unicode(QFileDialog.getOpenFileName(self, QString.fromUtf8("加载ai"), REPLAY_FILE_DIR, "ai files(*.exe)"))

	@pyqtSlot()
	def on_U2Button_clicked(self):
		self.AI2 = unicode(QFileDialog.getOpenFileName(self, QString.fromUtf8("加载ai"), REPLAY_FILE_DIR, "ai files(*.exe)"))

	@pyqtSlot()
	def on_U3Button_clicked(self):
		self.AI3 = unicode(QFileDialog.getOpenFileName(self, QString.fromUtf8("加载ai"), REPLAY_FILE_DIR, "ai files(*.exe)"))

	@pyqtSlot()
	def on_U4Button_clicked(self):
		self.AI4 = unicode(QFileDialog.getOpenFileName(self, QString.fromUtf8("加载ai"), REPLAY_FILE_DIR, "ai files(*.exe)"))

	@pyqtSlot()
	def on_U5Button_clicked(self):
		self.AI5 = unicode(QFileDialog.getOpenFileName(self, QString.fromUtf8("加载ai"), REPLAY_FILE_DIR, "ai files(*.exe)"))


