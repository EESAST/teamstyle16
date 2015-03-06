#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
from ui_webPlay import *
from deep_blue import *
import requests
import urllib

REPLAY_FILE_DIR = "."

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

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
		self.mapInfo = None

	def update_info(self, datas):
		self.data = datas
		print datas[1]
		if datas[1]:
			self.UserNameLabel.setText(_fromUtf8(datas[1]))
		else:
			self.UserNameLabel.setText("")
		self.IDLabel.setText("%d" %datas[0])
		if datas[3]:
			self.TeamLabel.setText(_fromUtf8(datas[3]))
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

	@pyqtSlot()
	def on_F1Button_clicked(self):
		if not self.AI1:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("未加载ai"), QMessageBox.Ok)
			return
		self.mapInfo = map_info.load('test_match_1.map')
		self.fileInfo = ai_battle.AIBattle(map_info = self.mapInfo, ai0_filename = str(self.AI1), ai1_filename = 'ai/empty_ai.exe', judge = TestGame[0][0])
		now_round = 0
		while now_round < self.mapInfo.max_round and self.fileInfo.state() not in [0,1]:
			self.fileInfo.feed_ai_commands()
			self.fileInfo.next_round()
			now_round += 1
			print
			print
			print "in WebPlay"
			print "the state = ",self.fileInfo.state()
			print
			print
		if self.fileInfo.state():
			QMessageBox.information(self, QString.fromUtf8("抱歉"), QString.fromUtf8("测试失败"), QMessageBox.Ok)
		else:
			QMessageBox.information(self, QString.fromUtf8("恭喜"), QString.fromUtf8("测试成功"), QMessageBox.Ok)
		choice = QMessageBox.question(self, QString.fromUtf8("储存"), QString.fromUtf8("您想储存回放文件吗？"), QMessageBox.Yes|QMessageBox.No)
		if choice == QMessageBox.Yes:
			saveFile = QFileDialog.getSaveFileName(self, QString.fromUtf8("储存回放文件"), REPLAY_FILE_DIR, "replay files(*.battle)")
			try:
				battle.Battle.save(self.fileInfo, saveFile)
			except:
				if saveFile != "":
					QMessageBox.critical(self, QString.fromUtf8("文件加载错误"), QString.fromUtf8("加载中出现问题,加载失败。"), QMessageBox.Ok, QMessageBox.NoButton)
		else:
			pass

	@pyqtSlot()
	def on_F2Button_clicked(self):
		if not self.AI2:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("未加载ai"), QMessageBox.Ok)
			return
		self.mapInfo = map_info.load('test_match_1-3.map')
		self.fileInfo = ai_battle.AIBattle(map_info = self.mapInfo, ai0_filename = str(self.AI2), ai1_filename = 'ai/empty_ai.exe', judge = TestGame[1][0])
		now_round = 0
		while now_round < self.mapInfo.max_round and self.fileInfo.state() not in [0,1]:
			self.fileInfo.feed_ai_commands()
			self.fileInfo.next_round()
			now_round += 1
		if self.fileInfo.state():
			QMessageBox.information(self, QString.fromUtf8("抱歉"), QString.fromUtf8("测试失败"), QMessageBox.Ok)
		else:
			QMessageBox.information(self, QString.fromUtf8("恭喜"), QString.fromUtf8("测试成功"), QMessageBox.Ok)
		choice = QMessageBox.question(self, QString.fromUtf8("储存"), QString.fromUtf8("您想储存回放文件吗？"), QMessageBox.Yes|QMessageBox.No)
		if choice == QMessageBox.Yes:
			saveFile = QFileDialog.getSaveFileName(self, QString.fromUtf8("储存回放文件"), REPLAY_FILE_DIR, "replay files(*.battle)")
			try:
				battle.Battle.save(self.fileInfo, saveFile)
			except:
				if saveFile != "":
					QMessageBox.critical(self, QString.fromUtf8("文件加载错误"), QString.fromUtf8("加载中出现问题,加载失败。"), QMessageBox.Ok, QMessageBox.NoButton)
		else:
			pass

	@pyqtSlot()
	def on_F3Button_clicked(self):
		if not self.AI3:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("未加载ai"), QMessageBox.Ok)
			return
		self.mapInfo = map_info.load('test_match_1-3.map')
		self.fileInfo = ai_battle.AIBattle(map_info = self.mapInfo, ai0_filename = str(self.AI3), ai1_filename = 'ai/empty_ai.exe', judge = TestGame[2][0])
		now_round = 0
		while now_round < self.mapInfo.max_round and self.fileInfo.state() not in [0,1]:
			self.fileInfo.feed_ai_commands()
			self.fileInfo.next_round()
			now_round += 1
		if self.fileInfo.state():
			QMessageBox.information(self, QString.fromUtf8("抱歉"), QString.fromUtf8("测试失败"), QMessageBox.Ok)
		else:
			QMessageBox.information(self, QString.fromUtf8("恭喜"), QString.fromUtf8("测试成功"), QMessageBox.Ok)
		choice = QMessageBox.question(self, QString.fromUtf8("储存"), QString.fromUtf8("您想储存回放文件吗？"), QMessageBox.Yes|QMessageBox.No)
		if choice == QMessageBox.Yes:
			saveFile = QFileDialog.getSaveFileName(self, QString.fromUtf8("储存回放文件"), REPLAY_FILE_DIR, "replay files(*.battle)")
			try:
				battle.Battle.save(self.fileInfo, saveFile)
			except:
				if saveFile != "":
					QMessageBox.critical(self, QString.fromUtf8("文件加载错误"), QString.fromUtf8("加载中出现问题,加载失败。"), QMessageBox.Ok, QMessageBox.NoButton)
		else:
			pass

	@pyqtSlot()
	def on_F4Button_clicked(self):
		if not self.AI4:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("未加载ai"), QMessageBox.Ok)
			return
		self.mapInfo = map_info.load('map_for_level_4-5.map')
		self.fileInfo = ai_battle.AIBattle(map_info = self.mapInfo, ai0_filename = str(self.AI4), ai1_filename = 'ai/sample_ai.exe', judge = TestGame[3][0])
		now_round = 0
		while now_round < self.mapInfo.max_round and self.fileInfo.state() not in [0,1]:
			self.fileInfo.feed_ai_commands()
			self.fileInfo.next_round()
			now_round += 1
		if self.fileInfo.state():
			QMessageBox.information(self, QString.fromUtf8("抱歉"), QString.fromUtf8("测试失败"), QMessageBox.Ok)
		else:
			QMessageBox.information(self, QString.fromUtf8("恭喜"), QString.fromUtf8("测试成功"), QMessageBox.Ok)
		choice = QMessageBox.question(self, QString.fromUtf8("储存"), QString.fromUtf8("您想储存回放文件吗？"), QMessageBox.Yes|QMessageBox.No)
		if choice == QMessageBox.Yes:
			saveFile = QFileDialog.getSaveFileName(self, QString.fromUtf8("储存回放文件"), REPLAY_FILE_DIR, "replay files(*.battle)")
			try:
				battle.Battle.save(self.fileInfo, saveFile)
			except:
				if saveFile != "":
					QMessageBox.critical(self, QString.fromUtf8("文件加载错误"), QString.fromUtf8("加载中出现问题,加载失败。"), QMessageBox.Ok, QMessageBox.NoButton)
		else:
			pass

	@pyqtSlot()
	def on_F5Button_clicked(self):
		if not self.AI5:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("未加载ai"), QMessageBox.Ok)
			return
		self.mapInfo = map_info.load('map_for_level_4-5.map')
		self.fileInfo = ai_battle.AIBattle(map_info = self.mapInfo, ai0_filename = str(self.AI5), ai1_filename = 'ai/sample_ai.exe', judge = TestGame[4][0])
		now_round = 0
		while now_round < self.mapInfo.max_round and self.fileInfo.state() not in [0,1]:
			self.fileInfo.feed_ai_commands()
			self.fileInfo.next_round()
			now_round += 1
		if self.fileInfo.state():
			QMessageBox.information(self, QString.fromUtf8("抱歉"), QString.fromUtf8("测试失败"), QMessageBox.Ok)
		else:
			QMessageBox.information(self, QString.fromUtf8("恭喜"), QString.fromUtf8("测试成功"), QMessageBox.Ok)
		choice = QMessageBox.question(self, QString.fromUtf8("储存"), QString.fromUtf8("您想储存回放文件吗？"), QMessageBox.Yes|QMessageBox.No)
		if choice == QMessageBox.Yes:
			saveFile = QFileDialog.getSaveFileName(self, QString.fromUtf8("储存回放文件"), REPLAY_FILE_DIR, "replay files(*.battle)")
			try:
				battle.Battle.save(self.fileInfo, saveFile)
			except:
				if saveFile != "":
					QMessageBox.critical(self, QString.fromUtf8("文件加载错误"), QString.fromUtf8("加载中出现问题,加载失败。"), QMessageBox.Ok, QMessageBox.NoButton)
		else:
			pass

	@pyqtSlot()
	def on_UPAIButton_clicked(self):
		ai_name = unicode(QFileDialog.getOpenFileName(self, QString.fromUtf8("加载ai文件"), REPLAY_FILE_DIR, "c files(*.c);;cpp files(*.cpp)"))
		if ai_name:
			f = open(ai_name)
		else:
			return
		payload = {'MAX_FILE_SIZE':50000}
		files = {'file':f}
		r = requests.post("http://deepblue.eesast.com/online_battle/uploads/"+self.data[3], data = payload, files = files)
		if r.status_code != 200:
			QMessageBox.critical(self, QString.fromUtf8("文件上传失败"), QString.fromUtf8("连接失败，ai未能正确上传。"), QMessageBox.Ok, QMessageBox.NoButton)
			return
		else:
			QMessageBox.critical(self, QString.fromUtf8("文件上传成功"), QString.fromUtf8("文件上传成功。"), QMessageBox.Ok, QMessageBox.NoButton)


	@pyqtSlot()
	def on_CompileButton_clicked(self):
		print "clicked"
		print urllib.urlopen("http://deepblue.eesast.com/online_battle/compile_action/"+str(self.data[3])).read()
