# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'webWidget.ui'
#
# Created: Wed Sep 24 18:43:45 2014
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

class Ui_WebWidget(object):
    def setupUi(self, WebWidget):
        WebWidget.setObjectName(_fromUtf8("WebWidget"))
        WebWidget.resize(1100, 742)
        self.returnButton = QtGui.QPushButton(WebWidget)
        self.returnButton.setGeometry(QtCore.QRect(510, 720, 75, 23))
        self.returnButton.setObjectName(_fromUtf8("returnButton"))
        self.webView = QtWebKit.QWebView(WebWidget)
        self.webView.setGeometry(QtCore.QRect(10, 10, 1081, 701))
        self.webView.setUrl(QtCore.QUrl(_fromUtf8("http://teamstyle16.eesast.com/")))
        self.webView.setObjectName(_fromUtf8("webView"))

        self.retranslateUi(WebWidget)
        QtCore.QMetaObject.connectSlotsByName(WebWidget)

    def retranslateUi(self, WebWidget):
        WebWidget.setWindowTitle(_translate("WebWidget", "Form", None))
        self.returnButton.setText(_translate("WebWidget", "返回上级", None))

from PyQt4 import QtWebKit
