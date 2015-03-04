# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'LoginDialog.ui'
#
# Created: Wed Mar 04 23:02:41 2015
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

class Ui_LoginDialog(object):
    def setupUi(self, LoginDialog):
        LoginDialog.setObjectName(_fromUtf8("LoginDialog"))
        LoginDialog.resize(200, 106)
        self.LoginPushButton = QtGui.QPushButton(LoginDialog)
        self.LoginPushButton.setGeometry(QtCore.QRect(20, 70, 75, 23))
        self.LoginPushButton.setObjectName(_fromUtf8("LoginPushButton"))
        self.ReturnPushButton = QtGui.QPushButton(LoginDialog)
        self.ReturnPushButton.setGeometry(QtCore.QRect(100, 70, 75, 23))
        self.ReturnPushButton.setAccessibleName(_fromUtf8(""))
        self.ReturnPushButton.setObjectName(_fromUtf8("ReturnPushButton"))
        self.widget = QtGui.QWidget(LoginDialog)
        self.widget.setGeometry(QtCore.QRect(10, 10, 177, 48))
        self.widget.setObjectName(_fromUtf8("widget"))
        self.gridLayout = QtGui.QGridLayout(self.widget)
        self.gridLayout.setMargin(0)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.label = QtGui.QLabel(self.widget)
        self.label.setObjectName(_fromUtf8("label"))
        self.gridLayout.addWidget(self.label, 0, 0, 1, 1)
        self.UserNameLineEdit = QtGui.QLineEdit(self.widget)
        self.UserNameLineEdit.setObjectName(_fromUtf8("UserNameLineEdit"))
        self.gridLayout.addWidget(self.UserNameLineEdit, 0, 1, 1, 1)
        self.label_2 = QtGui.QLabel(self.widget)
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.gridLayout.addWidget(self.label_2, 1, 0, 1, 1)
        self.PasswordLineEdit = QtGui.QLineEdit(self.widget)
        self.PasswordLineEdit.setEchoMode(QtGui.QLineEdit.Password)
        self.PasswordLineEdit.setObjectName(_fromUtf8("PasswordLineEdit"))
        self.gridLayout.addWidget(self.PasswordLineEdit, 1, 1, 1, 1)

        self.retranslateUi(LoginDialog)
        QtCore.QMetaObject.connectSlotsByName(LoginDialog)

    def retranslateUi(self, LoginDialog):
        LoginDialog.setWindowTitle(_translate("LoginDialog", "Login", None))
        self.LoginPushButton.setText(_translate("LoginDialog", "登录", None))
        self.ReturnPushButton.setText(_translate("LoginDialog", "返回", None))
        self.label.setText(_translate("LoginDialog", "用户名：  ", None))
        self.label_2.setText(_translate("LoginDialog", "密  码：", None))

