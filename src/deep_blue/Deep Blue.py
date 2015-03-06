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
from UILoginWidget import *
from UIWebPlay import *

class MainWindow(QGraphicsView):
	def __init__(self, sizex, sizey, parent = None):
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
		self.backWindow.setX((sizex - self.backWindow.geometry().width())/2)
		self.backWindow.setY((sizey - self.backWindow.geometry().height())/2)
		self.backWindow.setZValue(0)
		self.scene.addItem(self.backWindow)

		#开始界面
		self.beginWindow =  QGraphicsProxyWidget()
		self.beginWidget =  BeginMenu()
		self.beginWindow.setWidget(self.beginWidget)
		self.beginWindow.setX((sizex + self.backWindow.geometry().width())/2 - self.beginWindow.geometry().width())
		self.beginWindow.setY((sizey + self.backWindow.geometry().height())/2 - self.beginWindow.geometry().height())
		self.beginWindow.setZValue(0.5)
		self.scene.addItem(self.beginWindow)

		#对战界面
		self.replayerWindow = QGraphicsProxyWidget()
		self.replayerWidget = AIReplayerWidget()
		self.replayerWindow.setWidget(self.replayerWidget)
		self.replayerWindow.setX((sizex - self.replayerWindow.geometry().width())/2)
		self.replayerWindow.setY((sizey - self.replayerWindow.geometry().height())/2)
		self.replayerWindow.setZValue(0.5)
		self.scene.addItem(self.replayerWindow)

		#地图编辑器
		self.mapMakerWindow =  QGraphicsProxyWidget()
		self.mapMakerWidget =  MapMaker()
		self.mapMakerWindow.setWidget(self.mapMakerWidget)
		self.mapMakerWindow.setX((sizex - self.mapMakerWindow.geometry().width())/2)
		self.mapMakerWindow.setY((sizey - self.mapMakerWindow.geometry().height())/2)
		self.mapMakerWindow.setZValue(0.5)
		self.scene.addItem(self.mapMakerWindow)

		#关于我们
		self.teamWindow =  QGraphicsProxyWidget()
		self.teamWidget =  TeamWidget()
		self.teamWindow.setWidget(self.teamWidget)
		self.teamWindow.setX((sizex - self.teamWindow.geometry().width())/2)
		self.teamWindow.setY((sizey - self.teamWindow.geometry().height())/2)
		self.teamWindow.resize(sizex, sizey)
		self.teamWindow.setZValue(0.5)
		self.scene.addItem(self.teamWindow)

		#网站链接
		self.webWindow = QGraphicsProxyWidget()
		self.webWidget = WebWidget()
		self.webWindow.setWidget(self.webWidget)
		self.webWindow.setX((sizex - self.webWindow.geometry().width())/2)
		self.webWindow.setY((sizey - self.webWindow.geometry().height())/2)
		self.webWindow.setZValue(0.5)
		self.scene.addItem(self.webWindow)

		#网络对战
		self.webPlayWindow = QGraphicsProxyWidget()
		self.webPlay = WebPlay()
		self.webPlayWindow.setWidget(self.webPlay)
		self.webPlayWindow.setX((sizex - self.webPlayWindow.geometry().width())/2)
		self.webPlayWindow.setY((sizey - self.webPlayWindow.geometry().height())/2)
		self.webPlayWindow.setZValue(0.5)
		self.scene.addItem(self.webPlayWindow)

		#登录界面
		self.loginWindow = QGraphicsProxyWidget()
		self.loginDialog = LoginDialog()
		self.loginWindow.setWidget(self.loginDialog)
		self.loginWindow.setX((sizex - self.loginWindow.geometry().width())/2)
		self.loginWindow.setY((sizey - self.loginWindow.geometry().height())/2)
		self.loginWindow.setZValue(0.5)
		self.scene.addItem(self.loginWindow)

		#关闭所有多余widget
		self.replayerWindow.widget().close()
		self.mapMakerWindow.widget().close()
		self.teamWindow.widget().close()
		self.webWindow.widget().close()
		self.loginWindow.widget().close()
		self.webPlayWindow.widget().close()

		#背景涂黑
		self.setBackgroundBrush(QBrush(QColor(0,0,0)))

		#状态机(目前还不全)
		self.stateMachine =  QStateMachine(self)
		self.MainState =  QState(self.stateMachine)
		self.TeamState =  QState(self.stateMachine)
		self.ReplayState =  QState(self.stateMachine)
		self.WebState =  QState(self.stateMachine)
		self.MapState =  QState(self.stateMachine)
		self.LoginState = QState(self.stateMachine)
		self.WebPlayState = QState(self.stateMachine)
		self.QuitState = QFinalState(self.stateMachine)

		#states和windows映射的dict
		self.stateDict = {self.MainState:self.beginWindow, self.TeamState:self.teamWindow,
						  self.ReplayState:self.replayerWindow, self.MapState:self.mapMakerWindow,
						  self.WebState:self.webWindow, self.LoginState:self.loginWindow,
						  self.WebPlayState:self.webPlayWindow}

		#存下上一个state
		self.preState = None

		#建立转换与动画
		self.trans_MainToLogin = self.MainState.addTransition(self.beginWidget.OnlinePlayButton,
															SIGNAL("clicked()"),
															self.LoginState)
		self.trans_LoginToWebPlay = self.LoginState.addTransition(self.loginDialog,
															SIGNAL("LoginSuccess()"),
															self.WebPlayState)
		self.trans_WebPlayToMain = self.WebPlayState.addTransition(self.webPlay.ReturnButton,
															SIGNAL("clicked()"),
															self.MainState)
		self.trans_LoginToMain = self.LoginState.addTransition(self.loginDialog.ReturnPushButton,
															SIGNAL("clicked()"),
															self.MainState)
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
								self.trans_MainToMap, self.trans_TeamToWeb, self.trans_WebToTeam,
								self.trans_MainToLogin, self.trans_LoginToMain, self.trans_LoginToWebPlay,
								self.trans_WebPlayToMain]

		for transition in self.transitionList:
			self.connect(transition, SIGNAL("triggered()"), self.showWindow)

		self.setAttribute(Qt.WA_DeleteOnClose)
		self.connect(self.stateMachine, SIGNAL("finished()"), self, SLOT("close()"))
		self.connect(self.loginDialog, SIGNAL("LoginSuccess()"), self.loginsuccess)
		self.stateMachine.setInitialState(self.MainState)
		self.stateMachine.start()

	def loginsuccess(self):
		print "in here"
		self.webPlay.update_info(self.loginDialog.datas)
		self.webPlay.Initialize_online()

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
	cursor = QCursor(QPixmap(":cursor.png").scaled(30,30), 0, 0)
	app.setOverrideCursor(cursor)
	splash = QSplashScreen(QPixmap(":splash.png"), Qt.WindowStaysOnTopHint)
	splash.show()
	screen = QDesktopWidget().screenGeometry()
	form = MainWindow(screen.width(), screen.height())
	time.sleep(0.5)
	delta_x = float(screen.width())/1366.0
	delta_y = float(screen.height())/768.0
	print delta_x, delta_y
	if delta_x > delta_y:
		scale_number = delta_y
	else:
		scale_number = delta_x
	form.scale(scale_number, scale_number)
	form.showFullScreen()
	print screen.width(), screen.height()
	splash.finish(form)
	del splash
	sys.exit(app.exec_())