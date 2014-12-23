#!/usr/bin/env python
# -*- coding: UTF-8 -*-
#部分代码来自上一届队式

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
from UIBeginMenu import *
from mapmaker import *
from UIReplayer import *
from UITeamWidget import *
from UIWebWidget import *
from mainAnimation import *
from UIBackWindow import *

class MainWindow(QGraphicsView):
	def __init__(self, parent = None):
		super(MainWindow, self).__init__(parent)
		#不显示滚动条
		self.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
		self.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
		self.scene =  QGraphicsScene(self)
		self.scene.setSceneRect(self.scene.itemsBoundingRect())
		self.setScene(self.scene)
		self.setAttribute(55, True)

		#背景界面
		self.backWindow = QGraphicsProxyWidget()
		self.backWidget = BackWidget()
		self.backWindow.setWidget(self.backWidget)
		self.backWindow.setX(0)
		self.backWindow.setY(0)
		self.backWindow.setZValue(0)
		self.scene.addItem(self.backWindow)

		#开始界面
		self.beginWindow =  QGraphicsProxyWidget()
		self.beginWidget =  BeginMenu()
		self.beginWindow.setWidget(self.beginWidget)
		self.beginWindow.setX(605)
		self.beginWindow.setY(114)
		self.beginWindow.setZValue(0.5)
		self.scene.addItem(self.beginWindow)

		#对战界面
		self.replayerWindow = QGraphicsProxyWidget()
		self.replayerWidget = AIReplayerWidget()
		self.replayerWindow.setWidget(self.replayerWidget)
		self.replayerWindow.setX(0)
		self.replayerWindow.setY(0)
		self.replayerWindow.setZValue(0.5)
		self.scene.addItem(self.replayerWindow)

		#地图编辑器
		self.mapMakerWindow =  QGraphicsProxyWidget()
		self.mapMakerWidget =  MapMaker()
		self.mapMakerWindow.setWidget(self.mapMakerWidget)
		self.mapMakerWindow.setX(0)
		self.mapMakerWindow.setY(0)
		self.mapMakerWindow.setZValue(0.5)
		self.scene.addItem(self.mapMakerWindow)

		#关于我们
		self.teamWindow =  QGraphicsProxyWidget()
		self.teamWidget =  TeamWidget()
		self.teamWindow.setWidget(self.teamWidget)
		self.teamWindow.setX(380)
		self.teamWindow.setY(220)
		self.teamWindow.setZValue(0.5)
		self.scene.addItem(self.teamWindow)

		#网站链接
		self.webWindow = QGraphicsProxyWidget()
		self.webWidget = WebWidget()
		self.webWindow.setWidget(self.webWidget)
		self.webWidget.resize(1367, 768)
		self.webWindow.setX(140)
		self.webWindow.setY(0)
		self.webWindow.setZValue(0.5)
		self.scene.addItem(self.webWindow)

		#关闭所有多余widget
		self.replayerWindow.widget().close()
		self.mapMakerWindow.widget().close()
		self.teamWindow.widget().close()
		self.webWindow.widget().close()

		#背景涂黑
		#self.setBackgroundBrush(QBrush(QColor(0,0,0)))

		#状态机(目前还不全)
		self.stateMachine =  QStateMachine(self)
		self.MainState =  QState(self.stateMachine)
		self.TeamState =  QState(self.stateMachine)
		self.ReplayState =  QState(self.stateMachine)
		self.WebState =  QState(self.stateMachine)
		self.MapState =  QState(self.stateMachine)
		self.QuitState = QFinalState(self.stateMachine)

		#states和windows映射的dict
		self.stateDict = {self.MainState:self.beginWindow, self.TeamState:self.teamWindow,
						  self.ReplayState:self.replayerWindow, self.MapState:self.mapMakerWindow,
						  self.WebState:self.webWindow}

		#存下上一个state
		self.preState = None

		#建立转换与动画
		self.trans_MainToQuit = self.MainState.addTransition(self.beginWidget.ExitGameButton,
															SIGNAL("clicked()"),
															self.QuitState)

		self.trans_MainToReplayer = self.MainState.addTransition(self.beginWidget.SinglePlayerButton,
															   SIGNAL("clicked()"), self.ReplayState)
		self.ani_MainToReplayer = MenuAnimation(self.beginWindow, self.replayerWindow)
		self.trans_MainToReplayer.addAnimation(self.ani_MainToReplayer)

		self.trans_ReplayerToMain = self.ReplayState.addTransition(self.replayerWidget, SIGNAL("willreturn()"),
											 self.MainState)
		self.ani_ReplayerToMain = MenuAnimation(self.replayerWindow, self.beginWindow)
		self.trans_ReplayerToMain.addAnimation(self.ani_ReplayerToMain)

		self.trans_MainToTeam = self.MainState.addTransition(self.beginWidget.AboutUsButton,SIGNAL("clicked()"),
															  self.TeamState)
		self.trans_TeamToMain = self.TeamState.addTransition(self.teamWidget.ExitButton,SIGNAL("clicked()"),
									 self.MainState)

		self.trans_MapToMain = self.MapState.addTransition(self.mapMakerWidget,SIGNAL("goback()"),
															self.MainState)
		self.ani_MapToMain = MenuAnimation(self.mapMakerWindow, self.beginWindow)
		self.trans_MapToMain.addAnimation(self.ani_MapToMain)

		self.trans_MainToMap = self.MainState.addTransition(self.beginWidget.MapEditorButton,SIGNAL("clicked()"),
 															   self.MapState)
		self.ani_MainToMap = MenuAnimation(self.beginWindow, self.mapMakerWindow)
		self.trans_MainToMap.addAnimation(self.ani_MainToMap)

		self.trans_TeamToWeb = self.TeamState.addTransition(self.teamWidget.websiteButton,SIGNAL("clicked()"),self.WebState)
		self.ani_TeamToWeb = MenuAnimation(self.teamWindow, self.webWindow)
		self.trans_TeamToWeb.addAnimation(self.ani_TeamToWeb)
		
		self.trans_WebToTeam = self.WebState.addTransition(self.webWidget.returnButton, SIGNAL("clicked()"), self.TeamState)
		self.ani_WebToTeam = MenuAnimation(self.webWindow, self.teamWindow)
		self.trans_WebToTeam.addAnimation(self.ani_WebToTeam)

		for state in self.stateDict.keys():
			self.connect(state, SIGNAL("entered()"), self.closeWindow)

		self.transitionList = [self.trans_MainToQuit, self.trans_MainToReplayer, self.trans_ReplayerToMain,
								self.trans_MainToTeam, self.trans_TeamToMain, self.trans_MapToMain,
								self.trans_MainToMap, self.trans_TeamToWeb, self.trans_WebToTeam]

		for transition in self.transitionList:
			self.connect(transition, SIGNAL("triggered()"), self.showWindow)

		self.setAttribute(Qt.WA_DeleteOnClose)
		self.connect(self.stateMachine, SIGNAL("finished()"), self, SLOT("close()"))
		self.stateMachine.setInitialState(self.MainState)
		self.stateMachine.start()

	def closeWindow(self):
		sender = self.sender()
		if isinstance(sender, QState):
			if sender in self.stateDict:
				if isinstance(self.preState, QState):
					self.stateDict[self.preState].widget().close()
			self.preState = sender

	def showWindow(self):
		sender = self.sender()
		if isinstance(sender, QAbstractTransition):
			target = sender.targetState()
			if target in self.stateDict:
				self.stateDict[target].widget().show()

	def resizeEvent(self, event):
		QGraphicsView.resizeEvent(self,event)
		self.scene.setSceneRect(self.scene.itemsBoundingRect())

if __name__ == "__main__":
	import time
	app = QApplication(sys.argv)
	palette = app.palette()
	palette.setBrush(QPalette.Active, QPalette.ButtonText, QColor(150,255,255))
	palette.setBrush(QPalette.Disabled, QPalette.ButtonText, QColor(0,0,0))
	QApplication.addLibraryPath(".")
	splash = QSplashScreen(QPixmap(":splash.png"), Qt.WindowStaysOnTopHint)
	splash.show()
	form = MainWindow()
	time.sleep(1)
	form.showFullScreen()
	splash.finish(form)
	del splash
	sys.exit(app.exec_())