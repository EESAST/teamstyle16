# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'BeginMenu.ui'
#
# Created: Tue Sep 23 19:11:58 2014
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

class Ui_BeginMenu(object):
    def setupUi(self, BeginMenu):
        BeginMenu.setObjectName(_fromUtf8("BeginMenu"))
        BeginMenu.resize(400, 371)
        self.layoutWidget = QtGui.QWidget(BeginMenu)
        self.layoutWidget.setGeometry(QtCore.QRect(160, 20, 77, 325))
        self.layoutWidget.setObjectName(_fromUtf8("layoutWidget"))
        self.verticalLayout = QtGui.QVBoxLayout(self.layoutWidget)
        self.verticalLayout.setMargin(0)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.SinglePlayerButton = QtGui.QPushButton(self.layoutWidget)
        self.SinglePlayerButton.setObjectName(_fromUtf8("SinglePlayerButton"))
        self.verticalLayout.addWidget(self.SinglePlayerButton)
        spacerItem = QtGui.QSpacerItem(20, 40, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.verticalLayout.addItem(spacerItem)
        self.OnlinePlayButton = QtGui.QPushButton(self.layoutWidget)
        self.OnlinePlayButton.setObjectName(_fromUtf8("OnlinePlayButton"))
        self.verticalLayout.addWidget(self.OnlinePlayButton)
        spacerItem1 = QtGui.QSpacerItem(20, 40, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.verticalLayout.addItem(spacerItem1)
        self.MapEditorButton = QtGui.QPushButton(self.layoutWidget)
        self.MapEditorButton.setStyleSheet(_fromUtf8(""))
        self.MapEditorButton.setObjectName(_fromUtf8("MapEditorButton"))
        self.verticalLayout.addWidget(self.MapEditorButton)
        spacerItem2 = QtGui.QSpacerItem(20, 40, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.verticalLayout.addItem(spacerItem2)
        self.AboutUsButton = QtGui.QPushButton(self.layoutWidget)
        self.AboutUsButton.setObjectName(_fromUtf8("AboutUsButton"))
        self.verticalLayout.addWidget(self.AboutUsButton)
        spacerItem3 = QtGui.QSpacerItem(20, 40, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.verticalLayout.addItem(spacerItem3)
        self.ExitGameButton = QtGui.QPushButton(self.layoutWidget)
        self.ExitGameButton.setObjectName(_fromUtf8("ExitGameButton"))
        self.verticalLayout.addWidget(self.ExitGameButton)

        self.retranslateUi(BeginMenu)
        QtCore.QMetaObject.connectSlotsByName(BeginMenu)

    def retranslateUi(self, BeginMenu):
        BeginMenu.setWindowTitle(_translate("BeginMenu", "Form", None))
        self.SinglePlayerButton.setText(_translate("BeginMenu", "单人游戏", None))
        self.OnlinePlayButton.setText(_translate("BeginMenu", "网络对战", None))
        self.MapEditorButton.setText(_translate("BeginMenu", "地图编辑", None))
        self.AboutUsButton.setText(_translate("BeginMenu", "关于我们", None))
        self.ExitGameButton.setText(_translate("BeginMenu", "退出游戏", None))

