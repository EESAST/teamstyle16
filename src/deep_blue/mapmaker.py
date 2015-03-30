#!/usr/bin/env python
# -*- coding: UTF-8 -*-
#地图编辑器

from lib.PaintEvent import *
from deep_blue import *
from ui_mapmaker import *
from mapreplayer import *
from UIHelpdlg2 import *

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
        self.helpdlg = HelpDialog(self)
        self.helpdlg.setVisible(False)

        self.num = [0 for x in range(11)]
        self.num[1] = (30, 30)
        self.population = 60
        self.round = 200

        vali = QIntValidator(0, 10)
        self.SubmarineLineEdit.setValidator(vali)
        self.DestroyerLineEdit.setValidator(vali)
        self.CarrierLineEdit.setValidator(vali)
        self.CargoLineEdit.setValidator(vali)
        self.FighterLineEdit.setValidator(vali)
        self.ScoutLineEdit.setValidator(vali)
        self.PopulationLineEdit.setValidator(QIntValidator(30, 200))
        self.RoundLineEdit.setValidator(QIntValidator(1, 1000))
        self.WeatherLineEdit.setValidator(QIntValidator(-1, 1))

        self.scene = QGraphicsScene()
        self.CenterWidget = MapMakerReplayer(self.scene)
        self.CenterLayout.addWidget(self.CenterWidget)

        self.RandomButton.setEnabled(True)

        self.create = False

        self.base = True

    @pyqtSlot()
    def on_HelpButton_clicked(self):
        self.helpdlg.setVisible(True)

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
    def on_WeatherLineEdit_editingFinished(self):
        text = self.WeatherLineEdit.text()
        if text != "":
            self.num[10] = int(unicode(text))

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
            self.round = int(unicode(text))

    @pyqtSlot(int)
    def on_CenterCombo_currentIndexChanged(self, index):
        self.num[0] = index

    @pyqtSlot(int)
    def on_SizeCombo_currentIndexChanged(self, index):
        self.num[1] = (30 + index * 10, 30 + index * 10)

    @pyqtSlot(int)
    def on_LandCombo_currentIndexChanged(self, index):
        self.num[2] = index

    @pyqtSlot(int)
    def on_SourceCombo_currentIndexChanged(self, index):
        self.num[3] = index

    @pyqtSlot()
    def on_SaveButton_clicked(self):
        if self.create:
            if self.CenterWidget.have_base[0] and self.CenterWidget.have_base[1]:
                saveFile = QFileDialog.getSaveFileName(self, QString.fromUtf8("储存地图"), REPLAY_FILE_DIR, "map files(*.map)")
                try:
                    self.CenterWidget.save(unicode(saveFile))
                except:
                    if saveFile != "":
                        QMessageBox.critical(self, QString.fromUtf8("文件储存错误"), QString.fromUtf8("储存中出现问题,加载失败。"), QMessageBox.Ok, QMessageBox.NoButton)
            else:
                QMessageBox.critical(self, QString.fromUtf8("储存失败"), QString.fromUtf8("必须有基地"), QMessageBox.Ok, QMessageBox.NoButton)
                return
        else:
            QMessageBox.critical(self, QString.fromUtf8("储存失败"), QString.fromUtf8("您尚未生成地图。"), QMessageBox.Ok, QMessageBox.NoButton)
        self.CenterWidget.reset()
        self.RandomButton.setEnabled(True)
        self.create = False

    @pyqtSlot()
    def on_RandomButton_clicked(self):
        self.CenterWidget.reset()
        self.CenterWidget.createMap(self.num, self.population, self.round)
        self.create = True
        self.CenterWidget.have_base = [True, True]

    @pyqtSlot()
    def on_ExitButton_clicked(self):
        self.on_ClosePushButton_clicked()
        self.num = [0 for x in range(11)]
        self.num[1] = (30, 30)
        self.population = 60
        self.round = 200
        self.CargoLineEdit.setText("0")
        self.CarrierLineEdit.setText("0")
        self.ScoutLineEdit.setText("0")
        self.FighterLineEdit.setText("0")
        self.DestroyerLineEdit.setText("0")
        self.SubmarineLineEdit.setText("0")
        self.WeatherLineEdit.setText("0")
        self.RoundLineEdit.setText("200")
        self.PopulationLineEdit.setText("60")
        self.SizeCombo.setCurrentIndex(0)
        self.CenterCombo.setCurrentIndex(0)
        self.LandCombo.setCurrentIndex(0)
        self.SourceCombo.setCurrentIndex(0)
        self.goback.emit()

    @pyqtSlot()
    def on_OpenPushButton_clicked(self):
        if self.create:
            choice = QMessageBox.question(self, QString.fromUtf8("储存"), QString.fromUtf8("您想储存地图文件吗？"), QMessageBox.Yes|QMessageBox.No)
            if choice == QMessageBox.Yes:
                saveFile = QFileDialog.getSaveFileName(self, QString.fromUtf8("储存地图"), REPLAY_FILE_DIR, "map files(*.map)")
                try:
                    self.CenterWidget.save(unicode(saveFile))
                except:
                    if saveFile != "":
                        QMessageBox.critical(self, QString.fromUtf8("文件储存错误"), QString.fromUtf8("储存中出现问题,加载失败。"), QMessageBox.Ok, QMessageBox.NoButton)
            else:
                pass
        self.CenterWidget.reset()
        mapname = unicode(QFileDialog.getOpenFileName(self, QString.fromUtf8("加载地图"), REPLAY_FILE_DIR, "map files(*.map)"))
        if mapname:
            try:
                mapInfo = map_info.load(mapname)
            except:
                if mapname != "":
                    QMessageBox.critical(self, QString.fromUtf8("文件加载错误"), QString.fromUtf8("加载中出现问题,加载失败。"), QMessageBox.Ok, QMessageBox.NoButton)
            else:
                self.CenterWidget.map = mapInfo
                self.population = mapInfo.max_population
                self.round = mapInfo.max_round
                all_elements = mapInfo.elements
                for element_ in all_elements.values():
                    if element_.kind == 2 or element_.kind == 3:
                        continue
                    if element_.team:
                        continue
                    self.num[element_.kind] += 1
                self.CenterWidget.now_population[0] = self.get_population()
                for i in range(4, 10):
                    self.num[i] = 0
                for element_ in all_elements.values():
                    if element_.kind == 2 or element_.kind == 3:
                        continue
                    if element_.team != 1:
                        continue
                    self.num[element_.kind] += 1
                self.CenterWidget.now_population[1] = self.get_population()
                self.create = True
                self.CenterWidget.x_max = mapInfo.x_max
                self.CenterWidget.y_max = mapInfo.y_max
                self.CenterWidget.have_base = [True, True]
        self.CenterWidget.show()
        self.RandomButton.setEnabled(False)
        self.create = True

    @pyqtSlot()
    def on_ClosePushButton_clicked(self):
        if self.create and self.CenterWidget.have_base[0] and self.CenterWidget.have_base[1]:
            choice = QMessageBox.question(self, QString.fromUtf8("储存"), QString.fromUtf8("您想储存地图文件吗？"), QMessageBox.Yes|QMessageBox.No)
            if choice == QMessageBox.Yes:
                saveFile = QFileDialog.getSaveFileName(self, QString.fromUtf8("储存地图"), REPLAY_FILE_DIR, "map files(*.map)")
                try:
                    self.CenterWidget.save(unicode(saveFile))
                except:
                    if saveFile != "":
                        QMessageBox.critical(self, QString.fromUtf8("文件储存错误"), QString.fromUtf8("储存中出现问题,加载失败。"), QMessageBox.Ok, QMessageBox.NoButton)
            else:
                pass
        self.CenterWidget.reset()
        self.RandomButton.setEnabled(True)

    @pyqtSlot(bool)
    def on_ChangeRadioButton_clicked(self, check):
        if check:
            self.CenterWidget.RightState = 0

    @pyqtSlot(bool)
    def on_FighterButton_clicked(self, check):
        if check:
            self.CenterWidget.RightState = 9

    @pyqtSlot(bool)
    def on_ScoutButton_clicked(self, check):
        if check:
            self.CenterWidget.RightState = 10

    @pyqtSlot(bool)
    def on_CargoButton_clicked(self, check):
        if check:
            self.CenterWidget.RightState = 8

    @pyqtSlot(bool)
    def on_CarrierButton_clicked(self, check):
        if check:
            self.CenterWidget.RightState = 7

    @pyqtSlot(bool)
    def on_DestroyerButton_clicked(self, check):
        if check:
            self.CenterWidget.RightState = 6

    @pyqtSlot(bool)
    def on_SubmarineButton_clicked(self, check):
        if check:
            self.CenterWidget.RightState = 5

    @pyqtSlot(bool)
    def on_OilfieldButton_clicked(self, check):
        if check:
            self.CenterWidget.RightState = 4

    @pyqtSlot(bool)
    def on_MineButton_clicked(self, check):
        if check:
            self.CenterWidget.RightState = 3

    @pyqtSlot(bool)
    def on_FortButton_clicked(self, check):
        if check:
            self.CenterWidget.RightState = 2

    @pyqtSlot(bool)
    def on_BaseButton_clicked(self, check):
        if check:
            self.CenterWidget.RightState = 1

    @pyqtSlot(bool)
    def on_DeleteButton_clicked(self, check):
        if check:
            self.CenterWidget.RightState = -1

    @pyqtSlot(bool)
    def on_Team0Button_clicked(self, check):
        if check:
            self.CenterWidget.change_team = 0

    @pyqtSlot(bool)
    def on_Team1Button_clicked(self, check):
        if check:
            self.CenterWidget.change_team = 1

    @pyqtSlot()
    def on_NewButton_clicked(self):
        if self.create:
            choice = QMessageBox.question(self, QString.fromUtf8("储存"), QString.fromUtf8("您想储存地图文件吗？"), QMessageBox.Yes|QMessageBox.No)
            if choice == QMessageBox.Yes:
                saveFile = QFileDialog.getSaveFileName(self, QString.fromUtf8("储存地图"), REPLAY_FILE_DIR, "map files(*.map)")
                try:
                    self.CenterWidget.save(unicode(saveFile))
                except:
                    if saveFile != "":
                        QMessageBox.critical(self, QString.fromUtf8("文件储存错误"), QString.fromUtf8("储存中出现问题,加载失败。"), QMessageBox.Ok, QMessageBox.NoButton)
            else:
                pass
        self.CenterWidget.reset()
        self.CenterWidget.have_base = [False, False]
        self.RandomButton.setEnabled(False)
        self.CenterWidget.map = map_info.MapInfo(self.num[1][0], self.num[1][1], self.population, self.round, 1, 1.0, self.num[10])
        self.CenterWidget.x_max = self.num[1][0]
        self.CenterWidget.y_max = self.num[1][1]
        self.CenterWidget.show()
        self.create = True
