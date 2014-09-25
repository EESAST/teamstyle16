# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'mainwindow.ui'
#
# Created: Sun Sep 14 21:25:45 2014
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

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName(_fromUtf8("MainWindow"))
        MainWindow.resize(732, 544)
        self.graphicsView = QtGui.QGraphicsView(MainWindow)
        self.graphicsView.setGeometry(QtCore.QRect(50, 30, 621, 111))
        self.graphicsView.setObjectName(_fromUtf8("graphicsView"))
        self.layoutWidget = QtGui.QWidget(MainWindow)
        self.layoutWidget.setGeometry(QtCore.QRect(310, 160, 77, 325))
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

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(_translate("MainWindow", "Deep Blue", None))
        self.SinglePlayerButton.setText(_translate("MainWindow", "单人游戏", None))
        self.OnlinePlayButton.setText(_translate("MainWindow", "网络对战", None))
        self.MapEditorButton.setText(_translate("MainWindow", "地图编辑", None))
        self.AboutUsButton.setText(_translate("MainWindow", "关于我们", None))
        self.ExitGameButton.setText(_translate("MainWindow", "退出游戏", None))

