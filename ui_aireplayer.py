# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'AIReplayer.ui'
#
# Created: Mon Sep 15 16:17:07 2014
#      by: PyQt4 UI code generator 4.10.4
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class Ui_AIReplayer(object):
    def setupUi(self, AIReplayer):
        AIReplayer.setObjectName(_fromUtf8("AIReplayer"))
        AIReplayer.resize(1347, 689)
        self.formLayoutWidget = QtGui.QWidget(AIReplayer)
        self.formLayoutWidget.setGeometry(QtCore.QRect(280, 10, 811, 621))
        self.formLayoutWidget.setObjectName(_fromUtf8("formLayoutWidget"))
        self.CenterLayout = QtGui.QFormLayout(self.formLayoutWidget)
        self.CenterLayout.setFieldGrowthPolicy(QtGui.QFormLayout.AllNonFixedFieldsGrow)
        self.CenterLayout.setMargin(0)
        self.CenterLayout.setObjectName(_fromUtf8("CenterLayout"))
        self.AILabel1 = QtGui.QLabel(AIReplayer)
        self.AILabel1.setGeometry(QtCore.QRect(10, 60, 54, 12))
        self.AILabel1.setObjectName(_fromUtf8("AILabel1"))
        self.ExitButton = QtGui.QPushButton(AIReplayer)
        self.ExitButton.setGeometry(QtCore.QRect(10, 10, 31, 31))
        self.ExitButton.setObjectName(_fromUtf8("ExitButton"))
        self.HelpButton = QtGui.QPushButton(AIReplayer)
        self.HelpButton.setGeometry(QtCore.QRect(60, 10, 31, 31))
        self.HelpButton.setObjectName(_fromUtf8("HelpButton"))
        self.AILineEdit1 = QtGui.QLineEdit(AIReplayer)
        self.AILineEdit1.setGeometry(QtCore.QRect(10, 80, 113, 20))
        self.AILineEdit1.setObjectName(_fromUtf8("AILineEdit1"))
        self.AILabel2 = QtGui.QLabel(AIReplayer)
        self.AILabel2.setGeometry(QtCore.QRect(1290, 60, 54, 12))
        self.AILabel2.setObjectName(_fromUtf8("AILabel2"))
        self.AILineEdit2 = QtGui.QLineEdit(AIReplayer)
        self.AILineEdit2.setGeometry(QtCore.QRect(1230, 80, 113, 20))
        self.AILineEdit2.setObjectName(_fromUtf8("AILineEdit2"))
        self.layoutWidget = QtGui.QWidget(AIReplayer)
        self.layoutWidget.setGeometry(QtCore.QRect(290, 640, 781, 44))
        self.layoutWidget.setObjectName(_fromUtf8("layoutWidget"))
        self.horizontalLayout = QtGui.QHBoxLayout(self.layoutWidget)
        self.horizontalLayout.setMargin(0)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.verticalLayout = QtGui.QVBoxLayout()
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.BattleFrogCheck1 = QtGui.QCheckBox(self.layoutWidget)
        self.BattleFrogCheck1.setObjectName(_fromUtf8("BattleFrogCheck1"))
        self.verticalLayout.addWidget(self.BattleFrogCheck1)
        self.BattleFrogCheck2 = QtGui.QCheckBox(self.layoutWidget)
        self.BattleFrogCheck2.setObjectName(_fromUtf8("BattleFrogCheck2"))
        self.verticalLayout.addWidget(self.BattleFrogCheck2)
        self.horizontalLayout.addLayout(self.verticalLayout)
        self.PlayPushButton = QtGui.QPushButton(self.layoutWidget)
        self.PlayPushButton.setObjectName(_fromUtf8("PlayPushButton"))
        self.horizontalLayout.addWidget(self.PlayPushButton)
        self.PausePushButton = QtGui.QPushButton(self.layoutWidget)
        self.PausePushButton.setObjectName(_fromUtf8("PausePushButton"))
        self.horizontalLayout.addWidget(self.PausePushButton)
        self.StopPushButton = QtGui.QPushButton(self.layoutWidget)
        self.StopPushButton.setObjectName(_fromUtf8("StopPushButton"))
        self.horizontalLayout.addWidget(self.StopPushButton)
        self.verticalLayout_2 = QtGui.QVBoxLayout()
        self.verticalLayout_2.setObjectName(_fromUtf8("verticalLayout_2"))
        self.SpeedSliderLabel = QtGui.QLabel(self.layoutWidget)
        self.SpeedSliderLabel.setObjectName(_fromUtf8("SpeedSliderLabel"))
        self.verticalLayout_2.addWidget(self.SpeedSliderLabel)
        self.SpeedSlider = phonon.Phonon.SeekSlider(self.layoutWidget)
        self.SpeedSlider.setMouseTracking(False)
        self.SpeedSlider.setObjectName(_fromUtf8("SpeedSlider"))
        self.verticalLayout_2.addWidget(self.SpeedSlider)
        self.horizontalLayout.addLayout(self.verticalLayout_2)
        self.verticalLayout_3 = QtGui.QVBoxLayout()
        self.verticalLayout_3.setObjectName(_fromUtf8("verticalLayout_3"))
        self.RoundNumberLabel = QtGui.QLabel(self.layoutWidget)
        self.RoundNumberLabel.setObjectName(_fromUtf8("RoundNumberLabel"))
        self.verticalLayout_3.addWidget(self.RoundNumberLabel)
        self.RoundNumberSlider = phonon.Phonon.SeekSlider(self.layoutWidget)
        self.RoundNumberSlider.setIconVisible(False)
        self.RoundNumberSlider.setTracking(True)
        self.RoundNumberSlider.setSingleStep(500)
        self.RoundNumberSlider.setObjectName(_fromUtf8("RoundNumberSlider"))
        self.verticalLayout_3.addWidget(self.RoundNumberSlider)
        self.horizontalLayout.addLayout(self.verticalLayout_3)
        self.RoundNumberLCD = QtGui.QLCDNumber(self.layoutWidget)
        self.RoundNumberLCD.setObjectName(_fromUtf8("RoundNumberLCD"))
        self.horizontalLayout.addWidget(self.RoundNumberLCD)
        self.dockWidget = QtGui.QDockWidget(AIReplayer)
        self.dockWidget.setGeometry(QtCore.QRect(10, 110, 261, 511))
        self.dockWidget.setObjectName(_fromUtf8("dockWidget"))
        self.dockWidgetContents = QtGui.QWidget()
        self.dockWidgetContents.setObjectName(_fromUtf8("dockWidgetContents"))
        self.dockWidget.setWidget(self.dockWidgetContents)
        self.dockWidget_2 = QtGui.QDockWidget(AIReplayer)
        self.dockWidget_2.setGeometry(QtCore.QRect(1100, 110, 241, 511))
        self.dockWidget_2.setObjectName(_fromUtf8("dockWidget_2"))
        self.dockWidgetContents_2 = QtGui.QWidget()
        self.dockWidgetContents_2.setObjectName(_fromUtf8("dockWidgetContents_2"))
        self.dockWidget_2.setWidget(self.dockWidgetContents_2)
        self.AILabel1.setBuddy(self.AILineEdit1)
        self.AILabel2.setBuddy(self.AILineEdit2)

        self.retranslateUi(AIReplayer)
        QtCore.QMetaObject.connectSlotsByName(AIReplayer)

    def retranslateUi(self, AIReplayer):
        AIReplayer.setWindowTitle(_translate("AIReplayer", "AIReplayer", None))
        self.AILabel1.setText(_translate("AIReplayer", "1号AI：", None))
        self.ExitButton.setText(_translate("AIReplayer", "后退", None))
        self.HelpButton.setText(_translate("AIReplayer", "帮助", None))
        self.AILabel2.setText(_translate("AIReplayer", "2号AI：", None))
        self.BattleFrogCheck1.setText(_translate("AIReplayer", "1号战争迷雾", None))
        self.BattleFrogCheck2.setText(_translate("AIReplayer", "2号战争迷雾", None))
        self.PlayPushButton.setText(_translate("AIReplayer", "播放", None))
        self.PausePushButton.setText(_translate("AIReplayer", "暂停", None))
        self.StopPushButton.setText(_translate("AIReplayer", "停止", None))
        self.SpeedSliderLabel.setText(_translate("AIReplayer", "播放速度", None))
        self.RoundNumberLabel.setText(_translate("AIReplayer", "回合数", None))

from PyQt4 import phonon
