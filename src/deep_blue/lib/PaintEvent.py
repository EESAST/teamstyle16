#!/usr/bin/env python
# -*- coding: UTF-8 -*-
#绘制函数

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import qrc_source

UNIT_WIDTH = 30
UNIT_HEIGHT = 10
GRID_WIDTH = 30
GRID_HEIGHT = 30
SMALL_WIDTH = 2
SMALL_HEIGHT = 2
LINE_WIDTH = 1
CHOSEN_WIDTH = 50
CHOSEN_HEIGHT = 50
#for test
fil = open("test.txt","w")

FILE_UNIT = ["base","fort","mine","oilfield","submarine","destroyer","carrier","cargo","fighter","scout"]
FILE_MAP = ["water","land"]
FILE_SMALLMAP = ["watersmall", "landsmall"]

def GetPos(x, y, z, flag = True, size = 30):
	if flag:
		return QPointF(x * size, y * size + (2 - z) * size / 3)
	else:
		return QPoint(x * SMALL_WIDTH, y * SMALL_HEIGHT)

class AbstractUnit(QGraphicsObject):
	"""界面上元素的基类"""

	def __init__(self, x, y, z, parent):
		super(AbstractUnit, self).__init__(parent)
		self.setOpacity(0.8)
		self.corX = x
		self.corY = y
		self.corZ = z
	def boundingRect(self):
		return QRectF(0, 0, CHOSEN_WIDTH, CHOSEN_WIDTH)
	def setPos(self, x, y, z, size = 30, flag = True):
		self.corX = x
		self.corY = y
		self.corZ = z
		QGraphicsObject.setPos(self, GetPos(x, y, z, flag, size))
	def getPos(self):
		return (self.corX, self.corY, self.corZ)

	def getParent(self):
		return self.scene().views()[0]

class MapUnit(AbstractUnit):
	"""地形元素类"""
	def __init__(self, x, y, z, map_, parent = None):
		super(MapUnit, self).__init__(x, y, 2, parent)
		self.obj = map_
		self.pos = (x, y, z)

	def paint(self, painter, option, widget = None):
		painter.save()
		filename = ":" + FILE_MAP[self.obj] + ".png"
		image = QImage(filename).convertToFormat(QImage.Format_ARGB32)
		painter.drawImage(QPoint(0,0), image.scaled(GRID_WIDTH, GRID_HEIGHT, Qt.IgnoreAspectRatio))
		'''
		painter.setBrush(Qt.NoBrush)
		pen = QPen(QColor(255, 255, 255))
		pen.setWidth(1)
		painter.setPen(pen)
		painter.drawRect(0, 0, GRID_WIDTH, GRID_HEIGHT)
		'''
		painter.restore()

class MapMakerUnit(AbstractUnit):
	"""地形元素类"""
	def __init__(self, x, y, z, size, map_, parent = None):
		super(MapMakerUnit, self).__init__(x, y, 2, parent)
		self.obj = map_
		self.pos = (x, y, z)
		self.size = size

	def setitsSize(self, size):
		self.size = size

	def paint(self, painter, option, widget = None):
		painter.save()
		filename = ":" + FILE_MAP[self.obj] + ".png"
		image = QImage(filename).convertToFormat(QImage.Format_ARGB32)
		painter.drawImage(QPoint(0,0), image.scaled(self.size, self.size, Qt.IgnoreAspectRatio))
		painter.restore()

class SmallMapUnit(AbstractUnit):
	"""小地图类"""
	def __init__(self, x, y, z, map_, parent = None):
		super(SmallMapUnit, self).__init__(x, y, 2, parent)
		self.obj = map_
		self.pos = (x, y, z)
		self.setZValue(0.8)

	def paint(self, painter, option, widget = None):
		painter.save()
		filename = ":" + FILE_SMALLMAP[self.obj] + ".png"
		image = QImage(filename).convertToFormat(QImage.Format_ARGB32)
		painter.drawImage(QPoint(0,0), image.scaled(SMALL_WIDTH, SMALL_HEIGHT, Qt.IgnoreAspectRatio))
		painter.restore()	

class PointUnit(AbstractUnit):
	"""小地图单位点类"""
	def __init__(self, pos, i, parent = None):
		super(PointUnit, self).__init__(pos.x, pos.y, 2, parent)
		self.team = i
		self.setZValue(0.9)

	def paint(self, painter, option, widget = None):
		painter.save()
		painter.setPen(Qt.NoPen)
		if self.team == 0:
			brush = QBrush(Qt.red,Qt.SolidPattern)
		else:
			brush = QBrush(Qt.blue,Qt.SolidPattern)
		painter.setBrush(brush)
		painter.drawEllipse(0, 0, 5, 5)
		painter.restore()

class ChosenIndUnit(AbstractUnit):
	"""小地图选定框类"""
	def __init__(self, x, y, z, parent = None):
		super(ChosenIndUnit, self).__init__(x, y, z, parent)
		self.setZValue(0.7)

	def paint(self, painter, option, widget = None):
		painter.save()
		pen = QPen()
		pen.setWidth(1)
		pen.setCapStyle(Qt.RoundCap)
		pen.setJoinStyle(Qt.RoundJoin)
		pen.setColor(QColor(Qt.blue).darker())
		painter.setPen(pen)
		painter.setCompositionMode(QPainter.CompositionMode_Multiply)
		painter.drawRect(QRect(0, 0, CHOSEN_WIDTH, CHOSEN_HEIGHT))
		painter.restore()

class SoldierUnit(AbstractUnit):
	"""单位基类"""
	def __init__(self, unit, parent = None):
		super(SoldierUnit, self).__init__(unit.position.x, unit.position.y, unit.position.z,parent)
		self.obj = unit
		self.nowMove = False
		self.setZValue(0.5)

	def paint(self, painter, option, widget = None):
		painter.save()
		filename = ":" + FILE_UNIT[self.obj.kind] + ".png"
		image = QImage(filename).convertToFormat(QImage.Format_ARGB32)
		painter.drawImage(QPoint(0,0), image.scaled(UNIT_WIDTH * self.obj.size[0], UNIT_HEIGHT * self.obj.size[1], Qt.IgnoreAspectRatio))
		painter.restore()

class SoldierMakerUnit(AbstractUnit):
	"""单位基类"""
	def __init__(self, unit, x, y, size, parent = None):
		super(SoldierMakerUnit, self).__init__(unit.position.x, unit.position.y, unit.position.z,parent)
		self.obj = unit
		self.setZValue(0.5)
		self.size = size

	def paint(self, painter, option, widget = None):
		painter.save()
		filename = ":" + FILE_UNIT[self.obj.kind] + ".png"
		image = QImage(filename).convertToFormat(QImage.Format_ARGB32)
		painter.drawImage(QPoint(0,0), image.scaled(self.size, self.size/3, Qt.IgnoreAspectRatio))
		painter.restore()


class FrogUnit(AbstractUnit):
	"""战争迷雾"""
	def __init__(self, x, y, z, parent = None):
		super(FrogUnit, self).__init__(x, y, z, parent)
		self.setZValue(0.6)
		self.setOpacity(1)

	def paint(self, painter, option, widget = None):
		painter.save()
		brush = QBrush(Qt.SolidPattern)
		brush.setColor(QColor(qGray))
		painter.setPen(Qt.NoPen)
		painter.setBrush(brush)
		painter.drawRect(0,0,UNIT_WIDTH,UNIT_HEIGHT)
		painter.restore()

class MouseIndUnit(AbstractUnit):
	"""光标"""
	def __init__(self, x, y, z, parent = None):
		super(MouseIndUnit, self).__init__(x, y, 2, parent)
		self.timer = QTimer()
		self.connect(self.timer, SIGNAL("timeout()"), self.timeOut)
		self.setZValue(0.7)

	def setVis(self, vis):
		if vis:
			self.timer.start(600)
		else:
			self.timer.stop()

	def timeOut(self):
		self.setVisible(not self.isVisible())

	def paint(self, painter, option, widget = None):
		painter.save()
		RLINE = 0.3 #rate of line
		pen = QPen()
		pen.setWidth(1)
		pen.setCapStyle(Qt.RoundCap)
		pen.setJoinStyle(Qt.RoundJoin)
		pen.setColor(QColor(Qt.blue).lighter())
		painter.setPen(pen)
		painter.drawLine(QPointF(0, 0),
						 QPointF(0, RLINE*(UNIT_HEIGHT)))
		painter.drawLine(QPointF(0, 0),
						 QPointF(RLINE*(UNIT_WIDTH), 0))
		painter.drawLine(QPointF(UNIT_WIDTH, 0),
						 QPointF(UNIT_WIDTH, RLINE*(UNIT_HEIGHT)))
		painter.drawLine(QPointF(UNIT_WIDTH, 0),
						 QPointF((1-RLINE)*(UNIT_WIDTH), 0))
		painter.drawLine(QPointF(0, (UNIT_HEIGHT)),
						 QPointF(0, (1-RLINE)*(UNIT_HEIGHT)))
		painter.drawLine(QPointF(0, (UNIT_HEIGHT)),
						 QPointF(RLINE*(UNIT_WIDTH), (UNIT_HEIGHT)))
		painter.drawLine(QPointF(UNIT_WIDTH, (UNIT_HEIGHT)),
						 QPointF(UNIT_WIDTH, (1-RLINE)*(UNIT_HEIGHT)))
		painter.drawLine(QPointF(UNIT_WIDTH, (UNIT_HEIGHT)),
						 QPointF((1-RLINE)*(UNIT_WIDTH ), (UNIT_HEIGHT)))
		painter.restore()

class MouseFocusUnit(AbstractUnit):
	def __init__(self, x, y, z, parent = None):
		super(MouseFocusUnit, self).__init__(x, y, 2, parent)
		self.setZValue(0.9)

	def paint(self, painter, option, widget = None):
		painter.save()
		pen = QPen()
		pen.setWidth(1)
		pen.setCapStyle(Qt.RoundCap)
		pen.setJoinStyle(Qt.RoundJoin)
		pen.setColor(QColor(Qt.blue).darker())
		painter.setPen(pen)
		painter.setCompositionMode(QPainter.CompositionMode_Multiply)
		painter.drawRect(QRect(0, 0, UNIT_WIDTH, UNIT_HEIGHT))
		painter.restore()

class DieIndUnit(AbstractUnit):
	def __init__(self, x = 0, y = 0, z = 2, parent = None):
		super(DieIndUnit, self).__init__(x, y, 2, parent)

	def paint(self, painter, option, widget = None):
		painter.save()
		brush = QBrush(Qt.SolidPattern)
		brush.setColor(QColor(255,0,0,140))
		painter.setBrush(brush)
		painter.drawRect(QRect(0, 0, UNIT_WIDTH, UNIT_HEIGHT))	
		painter.restore()

class AttackEffectUnit(AbstractUnit):
	def __init__(self, x, y, z, parent = None):
		super(AttackEffectUnit, self).__init__(x, y, 2, parent)
		self.setZValue(0.8)

	def paint(self, painter, option, widget = None):
		painter.save()
		brush = QBrush(QColor(Qt.blue),Qt.SolidPattern)
		painter.setBrush(brush)
		painter.drawEllipse(0, 0, 10, 10)
		painter.restore()

class EffectIndUnit(QGraphicsTextItem):
	def __init__(self, text, parent = None):
		super(EffectIndUnit, self).__init__(text, parent)
		self.text = text
		font = self.font()
		self.setDefaultTextColor(QColor(Qt.red))
		if text[0] == "-" :
			font.setPointSize(font.pointSize() * 2)
			font.setBold(True)
			self.setDefaultTextColor(QColor(Qt.red))
		elif text[0] == "+" :
			font.setPointSize(font.pointSize() * 2)
			font.setBold(True)
			self.setDefaultTextColor(QColor(Qt.green))
		self.setFont(font)
		
		
	def setText(self, text):
		if text[0] == "+" or text[0] == '-':
			font.setPointSize(font.pointSize() * 2)
			font.setBold(True)
		QGraphicsTextItem.setText(self, text)

	def setColor(self, color):
		self.setDefaultTextColor(color)

class FixEffectUnit(AbstractUnit):
	def __init__(self, x, y, z, parent = None):
		super(RepairIndUnit, self).__init__(x, y, 2, perent)
		self.setZValue(0.8)

	def paint(self, painter, option, widget = None):
		painter.save()
		image = QImage(":repair.png")
		painter.drawImage(QPoint(0,0), image.scaled(UNIT_WIDTH, UNIT_HEIGHT, Qt.IgnoreAspectRatio))

class ColEffectUnit(AbstractUnit):
	def __init__(self, x, y, z, parent = None):
		super(RepairIndUnit, self).__init__(x, y, 2, perent)
		self.setZValue(0.8)

	def paint(self, painter, option, widget = None):
		painter.save()
		image = QImage(":collect.png")
		painter.drawImage(QPoint(0,0), image.scaled(UNIT_WIDTH, UNIT_HEIGHT, Qt.IgnoreAspectRatio))
'''
if __name__ == "__main__":
	import sys,qrc_source
	app = QApplication(sys.argv)
	view = QGraphicsView()
	scene = QGraphicsScene()
	view.setScene(scene)
	items = [FrogUnit(0,0,0), MapUnit(1,0,0,0), MapUnit(2,0,0,1), MouseIndUnit(0,0,0),MouseFocusUnit(0,0,0),ChosenIndUnit(0,0,0),SmallMapUnit(0,0,0,0),SmallMapUnit(0,0,0,1),EffectIndUnit("+2")]#,# AttackIndUnit(0,0)]
	for i in range(len(items)-1):
		scene.addItem(items[i])
		items[i].setPos(i, 0, 0)
	items[-1].setPos(2,0)
	scene.addItem(items[-1])
	view.show()
	app.exec_()
'''