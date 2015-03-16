#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
from ui_loginDialog import *
import hashlib, requests, MySQLdb

class LoginDialog(QWidget, Ui_LoginDialog):
	LoginSuccess = pyqtSignal()
	UsersInfo = pyqtSignal()
	def __init__(self, parent = None):
		super(LoginDialog, self).__init__(parent)
		self.setupUi(self)
		self.setAutoFillBackground(False)
		#palette = QPalette()
		#palette.setBrush(QPalette.Window, QBrush(Qt.NoBrush))
		#self.setPalette(palette)
		self.name = None
		self.pwd = None
		self.pwd_MD5 = None
		self.datas = None

	@pyqtSlot()
	def on_LoginPushButton_clicked(self):
		self.name = unicode(self.UserNameLineEdit.text())
		self.pwd = unicode(self.PasswordLineEdit.text())
		print self.pwd
		self.pwd_MD5 = hashlib.md5(self.pwd).hexdigest()
		print self.pwd_MD5
		db = MySQLdb.connect("teamstyle16.eesast.com","duishi16","******","info",charset='utf8')
		cursor = db.cursor()
		sql="select user_id,user_nickname,user_password_hash,user_team,user_email,test_battle from users where user_nickname='"+self.name+"' or user_email='"+self.name+"'"
		try:
			cursor.execute(sql)
			print "success"
			datas = cursor.fetchall()
			if datas[0][2] == self.pwd_MD5:
				self.datas = datas[0]
				self.emit(SIGNAL("LoginSuccess()"))
				self.UserNameLineEdit.setText("")
				self.PasswordLineEdit.setText("")
				self.name = None
				self.pwd = None
				self.pwd_MD5 = None
				self.datas = None
			else:
				QMessageBox.critical(self, QString.fromUtf8("登录失败"), QString.fromUtf8("密码错误"), QMessageBox.Ok, QMessageBox.NoButton)
			db.commit()
		except:
			print "fail"
			QMessageBox.critical(self, QString.fromUtf8("登录失败"), QString.fromUtf8("不存在的用户名"), QMessageBox.Ok, QMessageBox.NoButton)
			db.rollback()
		db.close()

	@pyqtSlot()
	def on_ReturnPushButton_clicked(self):
		self.UserNameLineEdit.setText("")
		self.PasswordLineEdit.setText("")
		self.name = None
		self.pwd = None
		self.pwd_MD5 = None
		self.datas = None