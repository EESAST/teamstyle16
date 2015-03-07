#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
from ui_aireplayer import *
from UIInfoWidget1 import *
from UIInfoWidget2 import *
from UIHelpdlg import *
from ReplayEvent import *
from lib.SmallMap import *
from deep_blue import *
from functools import partial
from ProductionEvent import *

REPLAY_FILE_DIR = "."

class AIReplayerWidget(QWidget, Ui_AIReplayer):
	willreturn = pyqtSignal()
	def __init__(self, parent = None):
		super(AIReplayerWidget, self).__init__(parent)
		self.setupUi(self)
		self.helpdlg = HelpDialog(self)
		#self.CreateLayout = ProductionWidget(self)
		self.helpdlg.setVisible(False)
		self.infoWidget1 = InfoWidget1(self)
		self.LeftLayout.addWidget(self.infoWidget1)
		self.infoWidget2 = InfoWidget2(self)
		self.RightLayout.addWidget(self.infoWidget2)
		palette = QPalette()
		self.setFixedSize(self.size())
		palette.setBrush(QPalette.Window, QBrush(Qt.NoBrush))
		self.setPalette(palette)
		self.scene1 = QGraphicsScene()
		self.scene2 = QGraphicsScene()
		self.scene3 = QGraphicsScene()
		self.scene4 = QGraphicsScene()

		self.CenterWidget = Replay(self.scene1)
		self.CenterLayout.addWidget(self.CenterWidget)
		self.SmallMap = SmallMap(self.scene2)
		self.SmallMapLayout.addWidget(self.SmallMap)
		self.CreateWidget = CreateTab(self.scene3, self.scene4)
		self.CreateLayout.addWidget(self.CreateWidget)
		self.OpenFileComboBox1.setMaxCount(10)
		self.OpenFileComboBox2.setMaxCount(10)

		self.isPaused = False
		self.started = False
		self.playing = False
		self.fileInfo = None
		self.mapInfo = None
		self.repFileName = ""
		self.mapFileName = ""
		self.aiFileName1 = ""
		self.aiFileName2 = ""
		self.timer = None
		self.loadRepFile = False
		self.loadMap = False
		self.loadAi1 = False
		self.loadAi2 = False
		self.totalround = 0

		self.connect(self.CenterWidget, SIGNAL("unitSelected"), self.infoWidget2.newUnitInfo)
		self.connect(self.CenterWidget, SIGNAL("mapSelected"), self.infoWidget2.newMapInfo)
		self.connect(self, SIGNAL("toPause()"),  partial(self.PlayPushButton.setChecked, False), Qt.QueuedConnection)
		self.CenterWidget.moveAnimEnd.connect(self.on_animEnd)
		self.connect(self.SmallMap, SIGNAL("areaChanged(QPoint)"), self.CenterWidget.setMapArea)
		self.connect(self.CenterWidget, SIGNAL("Scaled"), self.SmallMap.ScaleMap)

		self.updateUi()


	def updateUi(self):
		if self.loadRepFile:
			self.CenterWidget.HUMAN_REPLAY = 3
			self.CreateWidget.team1.HUMAN_REPLAY = 3
			self.CreateWidget.team2.HUMAN_REPLAY = 3
		elif self.loadMap and self.loadAi1 and self.HumanCheckBox2.isChecked():
			self.CenterWidget.HUMAN_REPLAY = 1
			self.CreateWidget.team1.HUMAN_REPLAY = 1
			self.CreateWidget.team2.HUMAN_REPLAY = 1
		elif self.loadMap and self.loadAi2 and self.HumanCheckBox1.isChecked():
			self.CenterWidget.HUMAN_REPLAY = 0
			self.CreateWidget.team1.HUMAN_REPLAY = 0
			self.CreateWidget.team2.HUMAN_REPLAY = 0
		elif self.loadMap and self.HumanCheckBox1.isChecked() and self.HumanCheckBox2.isChecked():
			self.CenterWidget.HUMAN_REPLAY = 2
			self.CreateWidget.team1.HUMAN_REPLAY = 2
			self.CreateWidget.team2.HUMAN_REPLAY = 2
		elif self.loadMap and self.loadAi2 and self.loadAi1:
			self.CenterWidget.HUMAN_REPLAY = 4
			self.CreateWidget.team1.HUMAN_REPLAY = 4
			self.CreateWidget.team2.HUMAN_REPLAY = 4
		if self.CenterWidget.HUMAN_REPLAY != -1:
			self.PlayPushButton.setCheckable(True)
			self.PlayPushButton.setEnabled(True)
			self.OneStepButton.setCheckable(True)
			self.OneStepButton.setEnabled(True)
		else:
			self.PlayPushButton.setCheckable(False)
			self.PlayPushButton.setEnabled(False)
			self.OneStepButton.setCheckable(False)
			self.OneStepButton.setEnabled(False)
		if self.CenterWidget.HUMAN_REPLAY in [3, 4]:
			self.Frog2RadioButton.setEnabled(self.started)
			self.Frog1RadioButton.setEnabled(self.started)
			self.GodVisionRadioButton.setEnabled(self.started)
		self.OpenFileButton1.setEnabled(not self.started)
		self.OpenFileButton2.setEnabled(not self.started)
		self.LoadFilePushButton.setEnabled(not self.started)
		self.LoadMapPushButton.setEnabled(not self.started)
		self.HumanCheckBox1.setEnabled(not self.started)
		self.HumanCheckBox2.setEnabled(not self.started)
		self.StopPushButton.setEnabled(self.started)
		self.UpPushButton.setEnabled(self.started)
		self.DownPushButton.setEnabled(self.started)
		self.RoundSlider.blockSignals(not self.started)
		if self.CenterWidget.HUMAN_REPLAY in [0,1,2,4]:
			self.RoundSlider.blockSignals(True)
			if self.CenterWidget.HUMAN_REPLAY == 1:
				self.Frog1RadioButton.setChecked(False)
				self.Frog2RadioButton.setChecked(True)
				self.Frog2RadioButton.setEnabled(False)
				self.Frog1RadioButton.setEnabled(False)
				self.GodVisionRadioButton.setEnabled(False)
				self.CenterWidget.frogIndex = 1
			elif self.CenterWidget.HUMAN_REPLAY == 0:
				self.Frog1RadioButton.setChecked(True)
				self.Frog2RadioButton.setChecked(False)
				self.Frog2RadioButton.setEnabled(False)
				self.Frog1RadioButton.setEnabled(False)
				self.GodVisionRadioButton.setEnabled(False)
				self.CenterWidget.frogIndex = 0


	@pyqtSlot(int)
	def on_SpeedSlider_valueChanged(self, speed):
		self.CenterWidget.TIME_PER_STEP = 100 - speed * 1

	@pyqtSlot(QString)
	def on_OpenFileComboBox1_currentIndexChanged(self, text):
		ainame1 = text
		if ainame1 and ainame1 != self.aiFileName1:
			self.aiFileName1 = ainame1
			self.loadAi1 = True
			self.OpenFileComboBox1.setCurrentIndex(self.OpenFileComboBox1.count() - 1)
			self.ReplayComboBox.setCurrentIndex(-1)
			self.loadRepFile = False
			self.fileInfo = None
			self.repFileName = ""
			self.HumanCheckBox1.setChecked(False)
			self.updateUi()

	@pyqtSlot(QString)
	def on_OpenFileComboBox2_currentIndexChanged(self, text):
		ainame2 = text
		if ainame2 and ainame2 != self.aiFileName2:
			self.aiFileName2 = ainame2
			self.loadAi2 = True
			self.OpenFileComboBox2.setCurrentIndex(self.OpenFileComboBox2.count() - 1)
			self.ReplayComboBox.setCurrentIndex(-1)
			self.loadRepFile = False
			self.fileInfo = None
			self.repFileName = ""
			self.HumanCheckBox2.setChecked(False)
			self.updateUi()

	@pyqtSlot(QString)
	def on_ReplayComboBox_currentIndexChanged(self, text):
		fname = text
		if fname and fname!= self.repFileName:
			try:
				fileInfo = replayer.load(fname)
			except:
				QMessageBox.critical(self, QString.fromUtf8("文件加载错误"), QString.fromUtf8("加载中出现问题,加载失败。"), QMessageBox.Ok, QMessageBox.NoButton)
			else:
				self.loadRepFile = True
				self.fileInfo = fileInfo
				self.repFileName = fname
				self.ReplayComboBox.setCurrentIndex(self.ReplayComboBox.count() - 1)
				self.MapComboBox.setCurrentIndex(-1)
				self.OpenFileComboBox1.setCurrentIndex(-1)
				self.OpenFileComboBox2.setCurrentIndex(-1)
				self.HumanCheckBox1.setChecked(False)
				self.HumanCheckBox2.setChecked(False)
				self.loadMap = False
				self.loadAi1 = False
				self.loadAi2 = False
				self.mapInfo = None
				self.mapFileName = ""
				self.aiFileName1 = ""
				self.aiFileName2 = ""
				self.updateUi()

	@pyqtSlot(QString)
	def on_MapComboBox_currentIndexChanged(self, text):
		mapname = text
		if mapname and mapname != self.mapFileName:
			try:
				mapInfo = map_info.load(mapname)
			except:
				QMessageBox.critical(self, QString.fromUtf8("文件加载错误"), QString.fromUtf8("加载中出现问题,加载失败。"), QMessageBox.Ok, QMessageBox.NoButton)
			else:
				self.loadMap = True
				self.mapInfo = mapInfo
				self.mapFileName = mapname
				self.MapComboBox.setCurrentIndex(self.MapComboBox.count() - 1)
				self.ReplayComboBox.setCurrentIndex(-1)
				self.loadRepFile = False
				self.fileInfo = None
				self.repFileName = ""
				self.updateUi()

	@pyqtSlot()
	def on_ExitButton_clicked(self):
		self.HumanCheckBox1.setChecked(False)
		self.HumanCheckBox2.setChecked(False)
		self.isPaused = False
		self.started = False
		self.playing = False
		self.fileInfo = None
		self.mapInfo = None
		self.repFileName = ""
		self.mapFileName = ""
		self.aiFileName1 = ""
		self.aiFileName2 = ""
		self.timer = None
		self.loadRepFile = False
		self.loadMap = False
		self.loadAi1 = False
		self.loadAi2 = False
		self.on_StopPushButton_clicked()
		self.MapComboBox.setCurrentIndex(-1)
		self.ReplayComboBox.setCurrentIndex(-1)
		self.OpenFileComboBox1.setCurrentIndex(-1)
		self.OpenFileComboBox2.setCurrentIndex(-1)
		self.updateUi()
		self.willreturn.emit()

	@pyqtSlot()
	def on_LoadFilePushButton_clicked(self):
		fname = unicode(QFileDialog.getOpenFileName(self, QString.fromUtf8("加载回放文件"), REPLAY_FILE_DIR, "replay files(*.battle)"))
		if fname and fname!= self.repFileName:
			try:
				fileInfo = replayer.load(fname)
			except:
				if fname != "":
					QMessageBox.critical(self, QString.fromUtf8("文件加载错误"), QString.fromUtf8("加载中出现问题,加载失败。"), QMessageBox.Ok, QMessageBox.NoButton)
			else:
				self.loadRepFile = True
				self.ReplayComboBox.addItem(fname)
				self.ReplayComboBox.setCurrentIndex(self.ReplayComboBox.count() - 1)
				self.fileInfo = fileInfo
				self.repFileName = fname
				self.MapComboBox.setCurrentIndex(-1)
				self.OpenFileComboBox1.setCurrentIndex(-1)
				self.OpenFileComboBox2.setCurrentIndex(-1)
				self.HumanCheckBox1.setChecked(False)
				self.HumanCheckBox2.setChecked(False)
				self.loadMap = False
				self.loadAi1 = False
				self.loadAi2 = False
				self.mapInfo = None
				self.mapFileName = ""
				self.aiFileName1 = ""
				self.aiFileName2 = ""
				self.updateUi()

	@pyqtSlot()
	def on_LoadMapPushButton_clicked(self):
		mapname = unicode(QFileDialog.getOpenFileName(self, QString.fromUtf8("加载地图"), REPLAY_FILE_DIR, "map files(*.map)"))
		if mapname and mapname != self.mapFileName:
			try:
				mapInfo = map_info.load(mapname)
			except:
				if mapname != "":
					QMessageBox.critical(self, QString.fromUtf8("文件加载错误"), QString.fromUtf8("加载中出现问题,加载失败。"), QMessageBox.Ok, QMessageBox.NoButton)
			else:
				self.loadMap = True
				self.MapComboBox.addItem(mapname)
				self.MapComboBox.setCurrentIndex(self.MapComboBox.count() - 1)
				self.ReplayComboBox.setCurrentIndex(-1)
				self.loadRepFile = False
				self.fileInfo = None
				self.repFileName = ""
				self.mapInfo = mapInfo
				self.mapFileName = mapname
				self.updateUi()

	@pyqtSlot()
	def on_OpenFileButton1_clicked(self):
		ainame1 = unicode(QFileDialog.getOpenFileName(self, QString.fromUtf8("加载ai1"), REPLAY_FILE_DIR, "ai files(*.exe)"))
		if ainame1 and ainame1 != self.aiFileName1:
			self.aiFileName1 = ainame1
			self.loadAi1 = True
			self.OpenFileComboBox1.addItem(ainame1)
			self.OpenFileComboBox1.setCurrentIndex(self.OpenFileComboBox1.count() - 1)
			self.ReplayComboBox.setCurrentIndex(-1)
			self.loadRepFile = False
			self.fileInfo = None
			self.repFileName = ""
			self.HumanCheckBox1.setChecked(False)
			self.updateUi()

	@pyqtSlot()
	def on_OpenFileButton2_clicked(self):
		ainame2 = unicode(QFileDialog.getOpenFileName(self, QString.fromUtf8("加载ai1"), REPLAY_FILE_DIR, "ai files(*.exe)"))
		if ainame2 and ainame2 != self.aiFileName2:
			self.aiFileName2 = ainame2
			self.loadAi2 = True
			self.OpenFileComboBox2.addItem(ainame2)
			self.OpenFileComboBox2.setCurrentIndex(self.OpenFileComboBox2.count() - 1)
			self.ReplayComboBox.setCurrentIndex(-1)
			self.loadRepFile = False
			self.fileInfo = None
			self.repFileName = ""
			self.HumanCheckBox2.setChecked(False)
			self.updateUi()

	@pyqtSlot(int)
	def on_HumanCheckBox1_stateChanged(self):
		if self.HumanCheckBox1.isChecked():
			self.ReplayComboBox.setCurrentIndex(-1)
			self.loadRepFile = False
			self.fileInfo = None
			self.repFileName = ""
			self.OpenFileComboBox1.setCurrentIndex(-1)
			self.aiFileName1 = ""
			self.loadAi1 = False
		self.updateUi()

	@pyqtSlot(int)
	def on_HumanCheckBox2_stateChanged(self):
		if self.HumanCheckBox2.isChecked():
			self.ReplayComboBox.setCurrentIndex(-1)
			self.loadRepFile = False
			self.fileInfo = None
			self.repFileName = ""
			self.OpenFileComboBox2.setCurrentIndex(-1)
			self.aiFileName2 = ""
			self.loadAi2 = False
		self.updateUi()

	@pyqtSlot(bool)
	def on_Frog1RadioButton_clicked(self, check):
		if check:
			self.CenterWidget.frogIndex = 0
			self.CenterWidget.setFrog(0)

	@pyqtSlot(bool)
	def on_Frog2RadioButton_clicked(self, check):
		if check:
			self.CenterWidget.frogIndex = 1
			self.CenterWidget.setFrog(1)

	@pyqtSlot(bool)
	def on_GodVisionRadioButton_clicked(self, check):
		if check:
			self.CenterWidget.frogIndex = -1
			self.CenterWidget.resetFrog()

	@pyqtSlot()
	def on_OneStepButton_clicked(self):
		if self.started:
			if self.CenterWidget.HUMAN_REPLAY == 3:
					if self.CenterWidget.nowRound == self.fileInfo.max_round:
						return
			self.PlayPushButton.setChecked(False)
			if not self.isPaused:
				self.isPaused = True
			self.CreateWidget.team1.Initialize(self.fileInfo)
			self.CreateWidget.team2.Initialize(self.fileInfo)
			self.CenterWidget.Play(self.fileInfo)
			self.isPaused = True
			self.SmallMap.Initialize(self.fileInfo, self.CenterWidget.HUMAN_REPLAY)
			self.infoWidget1.setText(self.fileInfo)
			self.infoWidget2.updateInfo(self.fileInfo)
			self.RoundLcdNumber.display(self.fileInfo.round())

		else:
			self.updateUi()
			if self.loadMap and not self.mapInfo:
				try:
					mapInfo = map_info.load(self.mapFileName)
				except:
					if self.mapFileName != "":
						QMessageBox.critical(self, QString.fromUtf8("文件加载错误"), QString.fromUtf8("加载中出现问题,加载失败。"), QMessageBox.Ok, QMessageBox.NoButton)
					return
				else:
					self.mapInfo = mapInfo
			if self.loadRepFile and not self.fileInfo:
				print "in reload"
				try:
					fileInfo = replayer.load(self.repFileName)
					print self.repFileName
				except:
					if self.repFileName != "":
						QMessageBox.critical(self, QString.fromUtf8("文件加载错误"), QString.fromUtf8("加载中出现问题,加载失败。"), QMessageBox.Ok, QMessageBox.NoButton)
					return
				else:
					self.fileInfo = fileInfo
			if self.CenterWidget.HUMAN_REPLAY == 4:
				self.fileInfo = ai_battle.AIBattle(self.mapInfo, DEFAULT_PORT, DEFAULT_TIMEOUT, str(self.aiFileName1), str(self.aiFileName2))
			elif self.CenterWidget.HUMAN_REPLAY == 0:
				self.fileInfo = human_ai_battle.HumanAIBattle(self.mapInfo, DEFAULT_PORT, DEFAULT_TIMEOUT, None, str(self.aiFileName2))
			elif self.CenterWidget.HUMAN_REPLAY == 1:
				self.fileInfo = human_ai_battle.HumanAIBattle(self.mapInfo, DEFAULT_PORT, DEFAULT_TIMEOUT, None, str(self.aiFileName1))
			elif self.CenterWidget.HUMAN_REPLAY == 2:
				QMessageBox.information(self, QString.fromUtf8("抱歉"), QString.fromUtf8("人人对战即将推出，敬请期待"), QMessageBox.Ok)
				self.HumanCheckBox2.setChecked(False)
				self.HumanCheckBox1.setChecked(False)
				self.on_StopPushButton_clicked()
			self.started = True
			if self.CenterWidget.HUMAN_REPLAY == 3:
				self.totalround = self.fileInfo.max_round
				self.RoundSlider.setRange(0,self.totalround)
				self.fileInfo.goto(0)
				self.synRoundSlider()
			else:
				self.totalround = self.fileInfo.map_info().max_round
			self.CenterWidget.Initialize(self.fileInfo)
			self.CreateWidget.team1.Initialize(self.fileInfo)
			self.CreateWidget.team2.Initialize(self.fileInfo)
			self.SmallMap.Initialize(self.fileInfo, self.CenterWidget.HUMAN_REPLAY)
			self.CenterWidget.Play(self.fileInfo)
			self.isPaused = True
			self.infoWidget1.setText(self.fileInfo)
			self.infoWidget2.updateInfo(self.fileInfo)
			self.RoundLcdNumber.display(self.fileInfo.round())
			self.updateUi()

	@pyqtSlot(bool)
	def on_PlayPushButton_toggled(self, trigger):
		if self.started:
			if trigger:
				self.isPaused = False
				if self.CenterWidget.HUMAN_REPLAY == 3:
					if self.CenterWidget.nowRound == self.fileInfo.max_round:
						return
				self.CreateWidget.team1.Initialize(self.fileInfo)
				self.CreateWidget.team2.Initialize(self.fileInfo)
				self.CenterWidget.Play(self.fileInfo)
				self.SmallMap.Initialize(self.fileInfo, self.CenterWidget.HUMAN_REPLAY)
				self.infoWidget1.setText(self.fileInfo)
				self.infoWidget2.updateInfo(self.fileInfo)
				self.RoundLcdNumber.display(self.fileInfo.round())
				
			else:
				self.isPaused = True

		else:
			self.updateUi()
			if self.loadMap and not self.mapInfo:
				try:
					mapInfo = map_info.load(self.mapFileName)
				except:
					if self.mapFileName != "":
						QMessageBox.critical(self, QString.fromUtf8("文件加载错误"), QString.fromUtf8("加载中出现问题,加载失败。"), QMessageBox.Ok, QMessageBox.NoButton)
					return
				else:
					self.mapInfo = mapInfo
			if self.loadRepFile and not self.fileInfo:
				print "in reload"
				try:
					fileInfo = replayer.load(self.repFileName)
					print self.repFileName
				except:
					if self.repFileName != "":
						QMessageBox.critical(self, QString.fromUtf8("文件加载错误"), QString.fromUtf8("加载中出现问题,加载失败。"), QMessageBox.Ok, QMessageBox.NoButton)
					return
				else:
					self.fileInfo = fileInfo
			if self.CenterWidget.HUMAN_REPLAY == 4:
				self.fileInfo = ai_battle.AIBattle(self.mapInfo, DEFAULT_PORT, DEFAULT_TIMEOUT, str(self.aiFileName1), str(self.aiFileName2))
			elif self.CenterWidget.HUMAN_REPLAY == 0:
				self.fileInfo = human_ai_battle.HumanAIBattle(self.mapInfo, DEFAULT_PORT, DEFAULT_TIMEOUT, None, str(self.aiFileName2))
			elif self.CenterWidget.HUMAN_REPLAY == 1:
				self.fileInfo = human_ai_battle.HumanAIBattle(self.mapInfo, DEFAULT_PORT, DEFAULT_TIMEOUT, None, str(self.aiFileName1))
			elif self.CenterWidget.HUMAN_REPLAY == 2:
				QMessageBox.information(self, QString.fromUtf8("抱歉"), QString.fromUtf8("人人对战即将推出，敬请期待"), QMessageBox.Ok)
				self.HumanCheckBox2.setChecked(False)
				self.HumanCheckBox1.setChecked(False)
				self.on_StopPushButton_clicked()
			self.started = True
			if self.CenterWidget.HUMAN_REPLAY == 3:
				self.totalround = self.fileInfo.max_round
				self.RoundSlider.setRange(0,self.totalround)
				self.fileInfo.goto(0)
				self.synRoundSlider()
			else:
				self.totalround = self.fileInfo.map_info().max_round
			self.CenterWidget.Initialize(self.fileInfo)
			self.CreateWidget.team1.Initialize(self.fileInfo)
			self.CreateWidget.team2.Initialize(self.fileInfo)
			self.SmallMap.Initialize(self.fileInfo, self.CenterWidget.HUMAN_REPLAY)
			self.CenterWidget.Play(self.fileInfo)
			self.infoWidget1.setText(self.fileInfo)
			self.infoWidget2.updateInfo(self.fileInfo)
			self.RoundLcdNumber.display(self.fileInfo.round())
			self.updateUi()

	def on_animEnd(self):
		self.CenterWidget.TerminateAni()
		if self.CenterWidget.changed:
			self.CenterWidget.setMap()
			self.CenterWidget.setUnit()
			self.CenterWidget.mouseUnit.setVisible(False)
			if self.CenterWidget.frogIndex != -1:
				self.CenterWidget.setFrog(self.frogIndex)
			self.CenterWidget.changed = False
		if self.isPaused:
			return
		print self.fileInfo.state()
		print
		print
		if self.fileInfo.state() in [0, 1] or self.CenterWidget.nowRound == self.totalround:
			sts = None
			if self.fileInfo.state() == 0: 
				sts = "0队获胜"
			elif self.fileInfo.state() == 1:
				sts = "1队获胜"
			else:
				sts = "平局"
			QMessageBox.information(self, QString.fromUtf8("游戏结束"), QString.fromUtf8(sts+" "+str(self.fileInfo.score(0))+":"+str(self.fileInfo.score(1))), QMessageBox.Ok)
			self.on_StopPushButton_clicked()
			return
		self.synRoundSlider()
		self.CenterWidget.Play(self.fileInfo)
		self.SmallMap.Initialize(self.fileInfo, self.CenterWidget.HUMAN_REPLAY)
		self.CreateWidget.team1.Initialize(self.fileInfo)
		self.CreateWidget.team2.Initialize(self.fileInfo)
		self.infoWidget2.updateInfo(self.fileInfo)
		self.infoWidget1.setText(self.fileInfo)
		self.RoundLcdNumber.display(self.fileInfo.round())

	@pyqtSlot(int)    
	def on_RoundSlider_valueChanged(self, round_):
		if self.started:
			if round_ != self.CenterWidget.nowRound:
				self.CenterWidget.mouseUnit.setVisible(False)
				self.fileInfo.goto(round_)
				self.CenterWidget.nowRound = round_
				self.CenterWidget.Initialize(self.fileInfo)
				self.CreateWidget.team1.Initialize(self.fileInfo)
				self.CreateWidget.team2.Initialize(self.fileInfo)
				self.SmallMap.Initialize(self.fileInfo, self.CenterWidget.HUMAN_REPLAY)
				if not self.isPaused:
					self.CenterWidget.Play(self.fileInfo)
				self.RoundLcdNumber.display(round_)
				self.infoWidget1.setText(self.fileInfo)
				self.infoWidget2.updateInfo(self.fileInfo)

	@pyqtSlot()
	def on_StopPushButton_clicked(self):
		self.started = False
		self.isPaused = True
		if self.CenterWidget.HUMAN_REPLAY != 3 and self.CenterWidget.HUMAN_REPLAY != -1:
			choice = QMessageBox.question(self, QString.fromUtf8("储存"), QString.fromUtf8("您想储存回放文件吗？"), QMessageBox.Yes|QMessageBox.No)
			if choice == QMessageBox.Yes:
				saveFile = QFileDialog.getSaveFileName(self, QString.fromUtf8("储存回放文件"), REPLAY_FILE_DIR, "replay files(*.battle)")
				try:
					battle.Battle.save(self.fileInfo, saveFile)
				except:
					if saveFile != "":
						QMessageBox.critical(self, QString.fromUtf8("文件储存错误"), QString.fromUtf8("储存中出现问题,储存失败。"), QMessageBox.Ok, QMessageBox.NoButton)
			else:
				pass
		self.isPaused = False
		self.mapInfo = None
		self.CenterWidget.scale(1.0/self.CenterWidget.scale_num, 1.0/self.CenterWidget.scale_num)
		self.CenterWidget.scale_num = 1.0
		self.SmallMap.reset()
		self.CreateWidget.team1.reset()
		self.CreateWidget.team2.reset()
		self.infoWidget1.reset()
		self.infoWidget2.resetUnitInfo()
		self.PlayPushButton.setCheckable(False)
		self.PlayPushButton.setEnabled(False)
		self.RoundSlider.setRange(0, 0)
		self.RoundLcdNumber.display(0)
		self.fileInfo = None
		self.updateUi()
		self.CenterWidget.reset()
		

	@pyqtSlot()
	def on_HelpButton_clicked(self):
		self.helpdlg.setVisible(True)

	@pyqtSlot()
	def synRoundSlider(self):
		self.RoundSlider.blockSignals(True)
		self.RoundSlider.setValue(self.CenterWidget.nowRound + 1)
		self.RoundSlider.blockSignals(False)

if __name__ == "__main__":
	app = QApplication(sys.argv)
	form = AIReplayerWidget()
	form.show()
	app.exec_()
