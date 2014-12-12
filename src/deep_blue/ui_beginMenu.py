# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'BeginMenu.ui'
#
# Created: Fri Sep 26 20:11:09 2014
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
        BeginMenu.resize(766, 654)
        self.OnlinePlayButton = QtGui.QPushButton(BeginMenu)
        self.OnlinePlayButton.setGeometry(QtCore.QRect(510, 340, 250, 73))
        self.OnlinePlayButton.setText(_fromUtf8(""))
        self.OnlinePlayButton.setObjectName(_fromUtf8("OnlinePlayButton"))
        self.AboutUsButton = QtGui.QPushButton(BeginMenu)
        self.AboutUsButton.setGeometry(QtCore.QRect(510, 500, 250, 73))
        self.AboutUsButton.setText(_fromUtf8(""))
        self.AboutUsButton.setObjectName(_fromUtf8("AboutUsButton"))
        self.ExitGameButton = QtGui.QPushButton(BeginMenu)
        self.ExitGameButton.setGeometry(QtCore.QRect(510, 580, 250, 73))
        self.ExitGameButton.setText(_fromUtf8(""))
        self.ExitGameButton.setObjectName(_fromUtf8("ExitGameButton"))
        self.SinglePlayerButton = QtGui.QPushButton(BeginMenu)
        self.SinglePlayerButton.setGeometry(QtCore.QRect(510, 260, 250, 73))
        self.SinglePlayerButton.setText(_fromUtf8(""))
        self.SinglePlayerButton.setObjectName(_fromUtf8("SinglePlayerButton"))
        self.MapEditorButton = QtGui.QPushButton(BeginMenu)
        self.MapEditorButton.setGeometry(QtCore.QRect(510, 420, 250, 73))
        self.MapEditorButton.setStyleSheet(_fromUtf8(""))
        self.MapEditorButton.setText(_fromUtf8(""))
        self.MapEditorButton.setObjectName(_fromUtf8("MapEditorButton"))

        self.retranslateUi(BeginMenu)
        QtCore.QMetaObject.connectSlotsByName(BeginMenu)

    def retranslateUi(self, BeginMenu):
        BeginMenu.setWindowTitle(_translate("BeginMenu", "Form", None))

