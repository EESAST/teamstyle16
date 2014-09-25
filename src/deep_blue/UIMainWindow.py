#!/usr/bin/env python
# -*- coding: UTF-8 -*-
#部分代码来自上一届队式

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
from UIBeginMenu import *
from UIMapmaker import *
from UIReplayer import *
from UITeamWidget import *
from UIWebWidget import *
from mainAnimation import *

class MainWindow(QGraphicsView):
	def __init__(self, parent = None):
		super(MainWindow, self).__init__(parent)
		#不显示滚动条
		self.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
		self.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
		self.scene =  QGraphicsScene(self)
		self.scene.setSceneRect(self.scene.itemsBoundingRect())
		self.setScene(self.scene)
		self.setAttribute(55)

		#开始界面
		self.beginWindow =  QGraphicsProxyWidget()
		self.beginWidget =  BeginMenu()
		self.beginWindow.setWidget(self.beginWidget)
		self.beginWindow.setX(460)
		self.beginWindow.setY(280)
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
		self.webWidget.resize(1024, 768)
		self.webWindow.setX(0)
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
		self.ani_MainToReplayer = MenuToWindowAnimation(self.beginWindow, self.replayerWindow)
		self.trans_MainToReplayer.addAnimation(self.ani_MainToReplayer)

		self.trans_ReplayerToMain = self.ReplayState.addTransition(self.replayerWidget.ExitButton, SIGNAL("clicked()"),
											 self.MainState)
		self.ani_ReplayerToMain = WindowToMenuAnimation(self.replayerWindow, self.beginWindow)
		self.trans_ReplayerToMain.addAnimation(self.ani_ReplayerToMain)

		self.trans_MainToTeam = self.MainState.addTransition(self.beginWidget.AboutUsButton,SIGNAL("clicked()"),
															  self.TeamState)
		self.trans_TeamToMain = self.TeamState.addTransition(self.teamWidget.ExitButton,SIGNAL("clicked()"),
									 self.MainState)

		self.trans_MapToMain = self.MapState.addTransition(self.mapMakerWidget.ExitButton,SIGNAL("clicked()"),
															self.MainState)
		self.ani_MapToMain = WindowToMenuAnimation(self.mapMakerWindow, self.beginWindow)
		self.trans_MapToMain.addAnimation(self.ani_MapToMain)

		self.trans_MainToMap = self.MainState.addTransition(self.beginWidget.MapEditorButton,SIGNAL("clicked()"),
 															   self.MapState)
		self.ani_MainToMap = MenuToWindowAnimation(self.beginWindow, self.mapMakerWindow)
		self.trans_MainToMap.addAnimation(self.ani_MainToMap)

		self.trans_TeamToWeb = self.MainState.addTransition(self.teamWidget.websiteButton,SIGNAL("clicked()"),self.WebState)
		self.ani_TeamToWeb = MenuToWindowAnimation(self.teamWindow, self.webWindow)
		self.trans_TeamToWeb.addAnimation(self.ani_TeamToWeb)
		
		self.trans_WebToTeam = self.WebState.addTransition(self.webWidget.returnButton, SIGNAL("clicked()"), self.TeamState)
		self.ani_WebToTeam = WindowToMenuAnimation(self.webWindow, self.teamWindow)
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
		print sender, "hi"
		if isinstance(sender, QState):
			if sender in self.stateDict:
				if isinstance(self.preState, QState):
					self.stateDict[self.preState].widget().close()
					print "close"
			self.preState = sender

	def showWindow(self):
		sender = self.sender()
		if isinstance(sender, QAbstractTransition):
			target = sender.targetState()
			if target in self.stateDict:
				self.stateDict[target].widget().show()


if __name__ == "__main__":
	app = QApplication(sys.argv)
	form = MainWindow()
	form.showFullScreen()
	sys.exit(app.exec_())