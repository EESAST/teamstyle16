#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
from ui_webPlay import *
from deep_blue import *
import requests
import urllib, MySQLdb
import hashlib

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
		self.score = 0
		self.data = None
		self.md5_sample = '1b3ac9fc64bbf6b3be7efa270da5eca7'
		self.md5_empty = '47c8eb7d9899fd90c7805c0898d318d2'
		self.md5_map4_5 = '95757db098e9507849f17375b3f70b71'
		self.md5_map1_3 = '72ccf53a8d8b4b1b899c91cc0978209b'
		self.md5_map1 = 'b5b2870e20ee0b1ee0e160e3bfbdcf51'
		self.md5_pro = '59a26b25e8ddad4ee353274d3aef379d'

	def GetFileMd5(self, strFile):
	    file_ = None
	    bRet = False
	    strMd5 = ""
	    
	    try:
	        file_ = open(strFile, "rb")
	        md5 = hashlib.md5()
	        strRead = ""
	        
	        while True:
	            strRead = file_.read(8096)
	            if not strRead:
	                break
	            md5.update(strRead)
	        #read file finish
	        bRet = True
	        strMd5 = md5.hexdigest()
	    except:
	        bRet = False
	    finally:
	        if file_:
	            file_.close()

	    return strMd5

	def on_ReturnButton_clicked(self):
		self.AI1 = None
		self.AI2 = None
		self.AI3 = None
		self.AI4 = None
		self.AI5 = None
		self.mapInfo = None
		self.score = 0
		self.data = None
		self.UserNameLabel.setText("")
		self.TeamLabel.setText("")
		self.ScoreLabel.setText("")
		self.MapCombo.clear()
		self.EnemyCombo.clear()

	def update_info(self, datas):
		self.data = datas
		if datas[5]:
			self.score = datas[5]
		if datas[1]:
			self.UserNameLabel.setText(_fromUtf8(" " + datas[1].encode('utf-8')))
		else:
			self.UserNameLabel.setText("")
		self.IDLabel.setText(" %d" %int(datas[0]))
		if datas[3]:
			self.TeamLabel.setText( _fromUtf8(" " + datas[3].encode('utf-8')))
		else:
			self.TeamLabel.setText("")
		self.ScoreLabel.setText(" %d" %self.score)

	@pyqtSlot()
	def on_B1Button_clicked(self):
		QMessageBox.information(self, QString.fromUtf8("第一关"), QString.fromUtf8("选手生产出单位"), QMessageBox.Ok)

	@pyqtSlot()
	def on_B2Button_clicked(self):
		QMessageBox.information(self, QString.fromUtf8("第二关"), QString.fromUtf8("选手收集到资源"), QMessageBox.Ok)

	@pyqtSlot()
	def on_B3Button_clicked(self):
		QMessageBox.information(self, QString.fromUtf8("第三关"), QString.fromUtf8("选手攻打某个据点并占领"), QMessageBox.Ok)

	@pyqtSlot()
	def on_B4Button_clicked(self):
		QMessageBox.information(self, QString.fromUtf8("第四关"), QString.fromUtf8("选手在自己方单位没有死亡的前提下攻击敌方单位并使其死亡(对手为AI)"), QMessageBox.Ok)

	@pyqtSlot()
	def on_B5Button_clicked(self):
		QMessageBox.information(self, QString.fromUtf8("第五关"), QString.fromUtf8("胜利条件与正式赛相同(对手为AI)"), QMessageBox.Ok)

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
		if self.GetFileMd5('test_match_1.map') != self.md5_map1:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("请勿更改地图"), QMessageBox.Ok)
			return
		print self.GetFileMd5('ai/empty_ai.exe')
		if self.GetFileMd5('ai/empty_ai.exe') != self.md5_empty:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("请勿更改ai文件夹下的ai"), QMessageBox.Ok)
			return
		if self.GetFileMd5(str(self.AI1)) == self.md5_sample:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("请勿更改使用sample_ai"), QMessageBox.Ok)
			return
		if self.GetFileMd5('property.conf') != self.md5_pro:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("请勿更改配置文件"), QMessageBox.Ok)
			return
		self.mapInfo = map_info.load('test_match_1.map')
		self.fileInfo = ai_battle.AIBattle(map_info = self.mapInfo, ai0_filename = str(self.AI1), ai1_filename = 'ai/empty_ai.exe', judge = TestGame[0][0])
		now_round = 0
		while now_round < self.mapInfo.max_round and self.fileInfo.state() not in [0,1]:
			self.fileInfo.feed_ai_commands()
			self.fileInfo.next_round()
			now_round += 1
		if self.fileInfo.state():
			QMessageBox.information(self, QString.fromUtf8("抱歉"), QString.fromUtf8("测试失败"), QMessageBox.Ok)
		else:
			QMessageBox.information(self, QString.fromUtf8("恭喜"), QString.fromUtf8("测试成功"), QMessageBox.Ok)
			self.score |= 1
			self.update_score()
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
		if self.GetFileMd5('test_match_1-3.map') != self.md5_map1_3:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("请勿更改地图"), QMessageBox.Ok)
			return
		if self.GetFileMd5('ai/empty_ai.exe') != self.md5_empty:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("请勿更改ai文件夹下的ai"), QMessageBox.Ok)
			return
		if self.GetFileMd5(str(self.AI2)) == self.md5_sample:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("请勿更改使用sample_ai"), QMessageBox.Ok)
			return
		if self.GetFileMd5('property.conf') != self.md5_pro:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("请勿更改配置文件"), QMessageBox.Ok)
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
			self.score |= 2
			self.update_score()
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
		if self.GetFileMd5('test_match_1-3.map') != self.md5_map1_3:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("请勿更改地图"), QMessageBox.Ok)
			return
		if self.GetFileMd5('ai/empty_ai.exe') != self.md5_empty:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("请勿更改ai文件夹下的ai"), QMessageBox.Ok)
			return
		if self.GetFileMd5(str(self.AI3)) == self.md5_sample:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("请勿更改使用sample_ai"), QMessageBox.Ok)
			return
		if self.GetFileMd5('property.conf') != self.md5_pro:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("请勿更改配置文件"), QMessageBox.Ok)
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
			self.score |= 4
			self.update_score()
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
		if self.GetFileMd5('map_for_level_4-5.map') != self.md5_map4_5:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("请勿更改地图"), QMessageBox.Ok)
			return
		if self.GetFileMd5('ai/sample_ai.exe') != self.md5_sample:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("请勿更改ai文件夹下的ai"), QMessageBox.Ok)
			return
		if self.GetFileMd5(str(self.AI4)) == self.md5_sample:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("请勿更改使用sample_ai"), QMessageBox.Ok)
			return
		if self.GetFileMd5('property.conf') != self.md5_pro:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("请勿更改配置文件"), QMessageBox.Ok)
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
			self.score |= 8
			self.update_score()
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
		if self.GetFileMd5('map_for_level_4-5.map') != self.md5_map4_5:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("请勿更改地图"), QMessageBox.Ok)
			return
		if self.GetFileMd5('ai/sample_ai.exe') != self.md5_sample:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("请勿更改ai文件夹下的ai"), QMessageBox.Ok)
			return
		if self.GetFileMd5(str(self.AI5)) == self.md5_sample:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("请勿更改使用sample_ai"), QMessageBox.Ok)
			return
		if self.GetFileMd5('property.conf') != self.md5_pro:
			QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("请勿更改配置文件"), QMessageBox.Ok)
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
			self.score |= 16
			self.update_score()
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
		urllib.urlopen("http://deepblue.eesast.com/online_battle/compile_action/"+str(self.data[3]))

	def Initialize_online(self):
		db = MySQLdb.connect("teamstyle16.eesast.com","duishi16","******","info")
		cursor = db.cursor()
		sql="select round,name from maps;"
		try:
			cursor.execute(sql)
			maps_ = cursor.fetchall()
			print maps_
			for maps__ in maps_:
				self.MapCombo.addItem(QString.fromUtf8(maps__[1]+" 回合数：%d" %maps__[0]), maps__[1])
		except:
			QMessageBox.critical(self, QString.fromUtf8("连接错误"), QString.fromUtf8("未能正确获取信息"), QMessageBox.Ok, QMessageBox.NoButton)
			db.rollback()

		sql="select with_ai,team_name,score from teams;"
		try:
			cursor.execute(sql)
			teams_ = cursor.fetchall()
			print teams_
			for teams__ in teams_:
				if teams__[0]:
					self.EnemyCombo.addItem(QString.fromUtf8(teams__[1]+" 积分：%d" %teams__[2]), teams__[1])
		except:
			QMessageBox.critical(self, QString.fromUtf8("连接错误"), QString.fromUtf8("未能正确获取信息"), QMessageBox.Ok, QMessageBox.NoButton)
			db.rollback()
		db.close()

	@pyqtSlot()
	def on_FightButton_clicked(self):
		QMessageBox.critical(self, QString.fromUtf8("抱歉"), QString.fromUtf8("暂未开放"), QMessageBox.Ok, QMessageBox.NoButton)
		return
		print self.MapCombo.itemData(self.MapCombo.currentIndex()).toString(), self.EnemyCombo.itemData(self.EnemyCombo.currentIndex()).toString()
		payload = {'map':self.MapCombo.itemData(self.MapCombo.currentIndex()).toString(), 'player':self.EnemyCombo.itemData(self.EnemyCombo.currentIndex()).toString()}
		r = requests.post("http://deepblue.eesast.com/online_battle/battle/"+self.data[3],data = payload)

	def update_score(self):
		db = MySQLdb.connect("teamstyle16.eesast.com","duishi16","******","info")
		cursor = db.cursor()
		sql="update users set test_battle="+str(self.score)+" where user_id="+str(self.data[0])+";"
		try:
			cursor.execute(sql)
			self.ScoreLabel.setText(" %d" %self.score)
			db.commit()
		except:
			db.rollback()
		db.close()