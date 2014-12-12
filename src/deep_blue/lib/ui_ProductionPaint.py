# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ProductionPaint.ui'
#
# Created: Tue Nov 18 21:17:15 2014
#      by: PyQt4 UI code generator 4.11.1
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

class Ui_QProductionPaint(object):
    def setupUi(self, QProductionPaint):
        QProductionPaint.setObjectName(_fromUtf8("QProductionPaint"))
        QProductionPaint.resize(240,220)
        self.ProductionTabList = QtGui.QTabWidget(QProductionPaint)
        self.ProductionTabList.setGeometry(QtCore.QRect(0, 0, 240, 180))
        self.ProductionTabList.setToolTip(_fromUtf8(""))
        self.ProductionTabList.setStatusTip(_fromUtf8(""))
        self.ProductionTabList.setWhatsThis(_fromUtf8(""))
        self.ProductionTabList.setAccessibleName(_fromUtf8(""))
        self.ProductionTabList.setObjectName(_fromUtf8("ProductionTabList"))
        self.tab = QtGui.QWidget()
        self.tab.setObjectName(_fromUtf8("tab"))
        self.ProductionList = QtGui.QFrame(self.tab)
        self.ProductionList.setGeometry(QtCore.QRect(10, 10, 220, 140))
        self.ProductionList.setFrameShape(QtGui.QFrame.StyledPanel)
        self.ProductionList.setFrameShadow(QtGui.QFrame.Raised)
        self.ProductionList.setObjectName(_fromUtf8("ProductionList"))
        self.object1_1 = QtGui.QGraphicsView(self.ProductionList)
        self.object1_1.setGeometry(QtCore.QRect(10, 20, 60, 45))
        self.object1_1.setObjectName(_fromUtf8("object1_1"))
        self.object1_2 = QtGui.QGraphicsView(self.ProductionList)
        self.object1_2.setGeometry(QtCore.QRect(80, 20, 60, 45))
        self.object1_2.setObjectName(_fromUtf8("object1_2"))
        self.object1_3 = QtGui.QGraphicsView(self.ProductionList)
        self.object1_3.setGeometry(QtCore.QRect(150, 20, 60, 45))
        self.object1_3.setObjectName(_fromUtf8("object1_3"))
        self.object1_4 = QtGui.QGraphicsView(self.ProductionList)
        self.object1_4.setGeometry(QtCore.QRect(10, 75, 60, 45))
        self.object1_4.setObjectName(_fromUtf8("object1_4"))
        self.object1_5 = QtGui.QGraphicsView(self.ProductionList)
        self.object1_5.setGeometry(QtCore.QRect(80, 75, 60, 45))
        self.object1_5.setObjectName(_fromUtf8("object1_5"))
        self.object1_6 = QtGui.QGraphicsView(self.ProductionList)
        self.object1_6.setGeometry(QtCore.QRect(150, 75, 60, 45))
        self.object1_6.setObjectName(_fromUtf8("object1_6"))
        self.ProductionTabList.addTab(self.tab, _fromUtf8(""))
        self.tab_2 = QtGui.QWidget()
        self.tab_2.setObjectName(_fromUtf8("tab_2"))
        self.ProductionList_2 = QtGui.QFrame(self.tab_2)
        self.ProductionList_2.setGeometry(QtCore.QRect(10, 10, 220, 140))
        self.ProductionList_2.setFrameShape(QtGui.QFrame.StyledPanel)
        self.ProductionList_2.setFrameShadow(QtGui.QFrame.Raised)
        self.ProductionList_2.setObjectName(_fromUtf8("ProductionList_2"))
        self.object2_1 = QtGui.QGraphicsView(self.ProductionList_2)
        self.object2_1.setGeometry(QtCore.QRect(10, 20, 60, 45))
        self.object2_1.setObjectName(_fromUtf8("object2_1"))
        self.object2_2 = QtGui.QGraphicsView(self.ProductionList_2)
        self.object2_2.setGeometry(QtCore.QRect(80, 20, 60, 45))
        self.object2_2.setObjectName(_fromUtf8("object2_2"))
        self.object2_3 = QtGui.QGraphicsView(self.ProductionList_2)
        self.object2_3.setGeometry(QtCore.QRect(150, 20, 60, 45))
        self.object2_3.setObjectName(_fromUtf8("object2_3"))
        self.object2_4 = QtGui.QGraphicsView(self.ProductionList_2)
        self.object2_4.setGeometry(QtCore.QRect(10, 75, 60, 45))
        self.object2_4.setObjectName(_fromUtf8("object2_4"))
        self.object2_5 = QtGui.QGraphicsView(self.ProductionList_2)
        self.object2_5.setGeometry(QtCore.QRect(80, 75, 60, 45))
        self.object2_5.setObjectName(_fromUtf8("object2_5"))
        self.object2_6 = QtGui.QGraphicsView(self.ProductionList_2)
        self.object2_6.setGeometry(QtCore.QRect(150, 75, 60, 45))
        self.object2_6.setObjectName(_fromUtf8("object2_6"))
        self.ProductionTabList.addTab(self.tab_2, _fromUtf8(""))

        self.retranslateUi(QProductionPaint)
        self.ProductionTabList.setCurrentIndex(0)
        QtCore.QMetaObject.connectSlotsByName(QProductionPaint)

    def retranslateUi(self, QProductionPaint):
        QProductionPaint.setWindowTitle(_translate("QProductionPaint", "Form", None))
        self.ProductionTabList.setTabText(self.ProductionTabList.indexOf(self.tab), _translate("QProductionPaint", "Team 1", None))
        self.ProductionTabList.setTabText(self.ProductionTabList.indexOf(self.tab_2), _translate("QProductionPaint", "Team 2", None))

