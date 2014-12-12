#!/usr/bin/env python
# -*- coding: UTF-8 -*-
#地图编辑器

from lib.PaintEvent import *
from platform import *
from ui_mapmaker import *
from mapreplayer import *

REPLAY_FILE_DIR = "."

class MapMaker(QWidget, Ui_Mapmaker):
    goback = pyqtSignal()
    def __init__(self, parent = None):
        super(MapMaker, self).__init__(parent)
        self.setupUi(self)

        palette = QPalette()
        self.setFixedSize(self.size())
        palette.setBrush(QPalette.Window, QBrush(Qt.NoBrush))
        self.setPalette(palette)

        self.num = [0 for x in range(10)]
        self.num[1] = (60, 60)
        self.population = 60
        self.round = 500

        vali = QIntValidator(0, 10)
        self.SubmarineLineEdit.setValidator(vali)
        self.DestroyerLineEdit.setValidator(vali)
        self.CarrierLineEdit.setValidator(vali)
        self.CargoLineEdit.setValidator(vali)
        self.FighterLineEdit.setValidator(vali)
        self.ScoutLineEdit.setValidator(vali)
        self.PopulationLineEdit.setValidator(QIntValidator(30, 200))
        self.RoundLineEdit.setValidator(QIntValidator(1, 1000))

        self.scene = QGraphicsScene()
        self.CenterWidget = MapMakerReplayer(self.scene)
        self.CenterLayout.addWidget(self.CenterWidget)

        self.create = False

    def get_population(self):
        return self.num[4] * 2 + self.num[5] * 3 + self.num[6] * 4 + self.num[7] + self.num[8] * 3 + self.num[9]

    @pyqtSlot()
    def on_SubmarineLineEdit_editingFinished(self):
        text = self.SubmarineLineEdit.text()
        if text != "":
            temp = self.num[4]
            self.num[4] = int(unicode(text))
            if self.get_population() > self.population:
                self.num[4] = temp
                self.SubmarineLineEdit.setText("%d" %self.num[4])
                QMessageBox.critical(self, QString.fromUtf8("设置失败"), QString.fromUtf8("超出人口限制。"), QMessageBox.Ok, QMessageBox.NoButton)

    @pyqtSlot()
    def on_DestroyerLineEdit_editingFinished(self):
        text = self.DestroyerLineEdit.text()
        if text != "":
            temp = self.num[5]
            self.num[5] = int(unicode(text))
            if self.get_population() > self.population:
                self.num[5] = temp
                self.DestroyerLineEdit.setText("%d" %self.num[5])
                QMessageBox.critical(self, QString.fromUtf8("设置失败"), QString.fromUtf8("超出人口限制。"), QMessageBox.Ok, QMessageBox.NoButton)

    @pyqtSlot()
    def on_CarrierLineEdit_editingFinished(self):
        text = self.CarrierLineEdit.text()
        if text != "":
            temp = self.num[6]
            self.num[6] = int(unicode(text))
            if self.get_population() > self.population:
                self.num[6] = temp
                self.CarrierLineEdit.setText("%d" %self.num[6])
                QMessageBox.critical(self, QString.fromUtf8("设置失败"), QString.fromUtf8("超出人口限制。"), QMessageBox.Ok, QMessageBox.NoButton)

    @pyqtSlot()
    def on_CargoLineEdit_editingFinished(self):
        text = self.CargoLineEdit.text()
        if text != "":
            temp = self.num[7]
            self.num[7] = int(unicode(text))
            if self.get_population() > self.population:
                self.num[7] = temp
                self.CargoLineEdit.setText("%d" %self.num[7])
                QMessageBox.critical(self, QString.fromUtf8("设置失败"), QString.fromUtf8("超出人口限制。"), QMessageBox.Ok, QMessageBox.NoButton)

    @pyqtSlot()
    def on_FighterLineEdit_editingFinished(self):
        text = self.FighterLineEdit.text()
        if text != "":
            temp = self.num[8]
            self.num[8] = int(unicode(text))
            if self.get_population() > self.population:
                self.num[8] = temp
                self.FighterLineEdit.setText("%d" %self.num[8])
                QMessageBox.critical(self, QString.fromUtf8("设置失败"), QString.fromUtf8("超出人口限制。"), QMessageBox.Ok, QMessageBox.NoButton)

    @pyqtSlot()
    def on_ScoutLineEdit_editingFinished(self):
        text = self.ScoutLineEdit.text()
        if text != "":
            temp = self.num[9]
            self.num[9] = int(unicode(text))
            if self.get_population() > self.population:
                self.num[9] = temp
                self.ScoutLineEdit.setText("%d" %self.num[9])
                QMessageBox.critical(self, QString.fromUtf8("设置失败"), QString.fromUtf8("超出人口限制。"), QMessageBox.Ok, QMessageBox.NoButton)

    @pyqtSlot()
    def on_PopulationLineEdit_editingFinished(self):
        text = self.PopulationLineEdit.text()
        if text != "":
            temp = self.population
            self.population = int(unicode(text))
            if self.get_population() > self.population:
                self.population = temp
                self.PopulationLineEdit.setText("%d" %self.population)
                QMessageBox.critical(self, QString.fromUtf8("设置失败"), QString.fromUtf8("人口限制小于已有人口数。"), QMessageBox.Ok, QMessageBox.NoButton)

    @pyqtSlot()
    def on_RoundLineEdit_editingFinished(self):
        text = self.RoundLineEdit.text()
        if text != "":
            self.round = unicode(text)

    @pyqtSlot(int)
    def on_CenterCombo_currentIndexChanged(self, index):
        self.num[0] = index

    @pyqtSlot(int)
    def on_SizeCombo_currentIndexChanged(self, index):
        self.num[1] = (60 + index * 10, 60 + index * 10)

    @pyqtSlot(int)
    def on_LandCombo_currentIndexChanged(self, index):
        self.num[2] = index

    @pyqtSlot(int)
    def on_SourceCombo_currentIndexChanged(self, index):
        self.num[3] = index

    @pyqtSlot()
    def on_SaveButton_clicked(self):
        if self.create:
            saveFile = QFileDialog.getSaveFileName(self, QString.fromUtf8("储存地图"), REPLAY_FILE_DIR, "map files(*.map)")
            try:
                self.CenterWidget.save(unicode(saveFile))
            except:
                QMessageBox.critical(self, QString.fromUtf8("文件储存错误"), QString.fromUtf8("储存中出现问题,加载失败。"), QMessageBox.Ok, QMessageBox.NoButton)
        else:
            QMessageBox.critical(self, QString.fromUtf8("储存失败"), QString.fromUtf8("您尚未生成地图。"), QMessageBox.Ok, QMessageBox.NoButton)

    @pyqtSlot()
    def on_RandomButton_clicked(self):
        self.CenterWidget.reset()
        self.CenterWidget.createMap(self.num, self.population, self.round)
        self.create = True

    @pyqtSlot()
    def on_ExitButton_clicked(self):
        self.CenterWidget.reset()
        self.goback.emit()

    @pyqtSlot(int)    
    def on_SizeSlider_valueChanged(self, size):
        self.CenterWidget.size = size
        self.CenterWidget.show()