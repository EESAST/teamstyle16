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

FILE_UNIT = [["base","fort","mine","oilfield","submarine","destroyer","carrier","cargo","fighter","scout"],["base2","fort2","mine2","oilfield2","submarine2","destroyer2","carrier2","cargo2","fighter2","scout2"],["base2","fort1","mine","oilfield","submarine2","destroyer2","carrier2","cargo2","fighter2","scout2"]]
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
		self.size = 1
	'''
	def boundingRect(self):
		if hasattr(self, obj):
			if hasattr(self.obj, team):
				return QRectF(0, 0, 30 * self.size, 10 * self.size)
			else:
				return QRectF(0, 0, 30, 30)
		else:
			if isinstance()
			return QRectF(0, 0, 30, 30)
	'''
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

	def boundingRect(self):
		return QRectF(0, 0, 30, 30)

	def paint(self, painter, option, widget = None):
		painter.save()
		painter.setPen(Qt.NoPen)
		if self.obj:
			brush = QBrush(QColor(61, 145, 64))
		else:
			brush = QBrush(QColor(65, 105, 225))
		painter.setBrush(brush)
		painter.drawRect(0, 0, GRID_WIDTH, GRID_HEIGHT)
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

	def boundingRect(self):
		return QRectF(0, 0, self.size, self.size)

	def paint(self, painter, option, widget = None):
		painter.save()
		painter.setPen(Qt.NoPen)
		if self.obj:
			brush = QBrush(QColor(61, 145, 64))
		else:
			brush = QBrush(QColor(65, 105, 225))
		painter.setBrush(brush)
		painter.drawRect(0, 0, self.size, self.size)
		painter.restore()

class SmallMapUnit(AbstractUnit):
	"""小地图类"""
	def __init__(self, x, y, z, map_, parent = None):
		super(SmallMapUnit, self).__init__(x, y, 2, parent)
		self.obj = map_
		self.pos = (x, y, z)
		self.setZValue(0.5)

	def boundingRect(self):
		return QRectF(0, 0, SMALL_WIDTH, SMALL_HEIGHT)

	def paint(self, painter, option, widget = None):
		painter.save()
		painter.setPen(Qt.NoPen)
		if self.obj:
			brush = QBrush(QColor(61, 145, 64))
		else:
			brush = QBrush(QColor(65, 105, 225))
		painter.setBrush(brush)
		painter.drawRect(0, 0, SMALL_WIDTH, SMALL_HEIGHT)
		painter.restore()	

class PointUnit(AbstractUnit):
	"""小地图单位点类"""
	def __init__(self, pos, i, base, parent = None):
		super(PointUnit, self).__init__(pos.x, pos.y, 2, parent)
		self.team = i
		self.setZValue(0.6)
		self.base = base

	def boundingRect(self):
		if self.base:
			return QRectF(0, 0, 3, 3)
		else:
			return QRectF(0, 0, 6, 6)

	def paint(self, painter, option, widget = None):
		painter.save()
		painter.setPen(Qt.NoPen)
		if self.team == 0:
			brush = QBrush(Qt.red,Qt.SolidPattern)
		elif self.team == 1:
			brush = QBrush(Qt.blue,Qt.SolidPattern)
		else:
			brush = QBrush(Qt.green,Qt.SolidPattern)
		painter.setBrush(brush)
		if self.base:
			painter.drawEllipse(0, 0, 3, 3)
		else:
			painter.drawEllipse(0, 0, 6, 6)
		painter.restore()

class ChosenIndUnit(AbstractUnit):
	"""小地图选定框类"""
	def __init__(self, x, y, z, size, parent = None):
		super(ChosenIndUnit, self).__init__(x, y, z, parent)
		self.setZValue(0.7)
		self.chosen = size

	def boundingRect(self):
		return QRectF(0, 0, CHOSEN_WIDTH, CHOSEN_HEIGHT)

	def paint(self, painter, option, widget = None):
		painter.save()
		pen = QPen()
		pen.setWidth(1)
		pen.setCapStyle(Qt.RoundCap)
		pen.setJoinStyle(Qt.RoundJoin)
		pen.setColor(QColor(Qt.blue).darker())
		painter.setPen(pen)
		painter.drawRect(QRect(0, 0, self.chosen, self.chosen))
		painter.restore()

class SoldierUnit(AbstractUnit):
	"""单位基类"""
	def __init__(self, unit, parent = None):
		super(SoldierUnit, self).__init__(unit.position.x, unit.position.y, unit.position.z, parent)
		self.obj = unit
		if self.obj.kind in [0, 1]:
			self.setZValue(0.5)
		else:
			self.setZValue(0.6)
		if self.obj.kind in [2, 3]:
			filename = ":" + FILE_UNIT[2][self.obj.kind] + ".png"
		else:
			filename = ":" + FILE_UNIT[self.obj.team][self.obj.kind] + ".png"
		self.image = QImage(filename)

	def boundingRect(self):
		return QRectF(0, 0, UNIT_WIDTH * self.obj.size[0], UNIT_HEIGHT * self.obj.size[1])

	def paint(self, painter, option, widget = None):
		painter.save()
		painter.drawImage(QPoint(0,0), self.image.scaled(UNIT_WIDTH * self.obj.size[0], UNIT_HEIGHT * self.obj.size[1], Qt.IgnoreAspectRatio))
		painter.restore()

class SoldierMakerUnit(AbstractUnit):
	"""单位基类"""
	def __init__(self, unit, size, parent = None):
		super(SoldierMakerUnit, self).__init__(unit.position.x if unit.kind else unit.position.x - 1, unit.position.y if unit.kind else unit.position.y - 1, unit.position.z, parent)
		self.obj = unit
		self.setZValue(0.6)
		self.size = size
		if self.obj.kind in [2, 3]:
			filename = ":" + FILE_UNIT[2][self.obj.kind] + ".png"
		else:
			filename = ":" + FILE_UNIT[self.obj.team][self.obj.kind] + ".png"
		self.image = QImage(filename)

	def boundingRect(self):
		return QRectF(0, 0, self.size * self.obj.size[0], self.size * self.obj.size[1]/3)

	def paint(self, painter, option, widget = None):
		painter.save()
		painter.drawImage(QPoint(0,0), self.image.scaled(self.size*self.obj.size[0], self.size*self.obj.size[1]/3, Qt.IgnoreAspectRatio))
		painter.restore()


class FrogUnit(AbstractUnit):
	"""战争迷雾"""
	def __init__(self, x, y, z, parent = None):
		super(FrogUnit, self).__init__(x, y, z, parent)
		self.setZValue(0.9)
		self.setOpacity(1.0)

	def boundingRect(self):
		return QRectF(0, 0, 30, 10)

	def paint(self, painter, option, widget = None):
		painter.save()
		brush = QBrush(QColor(192, 192, 192))
		painter.setPen(Qt.NoPen)
		painter.setBrush(brush)
		painter.drawRect(0,0,UNIT_WIDTH,UNIT_HEIGHT)
		painter.restore()

class MouseIndUnit(AbstractUnit):
	"""光标"""
	def __init__(self, x, y, z, size, parent = None):
		super(MouseIndUnit, self).__init__(x, y, 2, parent)
		self.setZValue(0.7)
		self.size = size

	def boundingRect(self):
		return QRectF(0, 0, self.size * UNIT_WIDTH, self.size * UNIT_HEIGHT)

	def paint(self, painter, option, widget = None):
		painter.save()
		RLINE = 0.3 #rate of line
		pen = QPen()
		pen.setWidth(2)
		pen.setCapStyle(Qt.RoundCap)
		pen.setJoinStyle(Qt.RoundJoin)
		pen.setColor(QColor(Qt.blue).darker())
		painter.setPen(pen)
		painter.drawLine(QPointF(0, 0),
						 QPointF(0, RLINE*(UNIT_HEIGHT)*self.size))
		painter.drawLine(QPointF(0, 0),
						 QPointF(RLINE*(UNIT_WIDTH)*self.size, 0))
		painter.drawLine(QPointF(UNIT_WIDTH*self.size, 0),
						 QPointF(UNIT_WIDTH*self.size, RLINE*(UNIT_HEIGHT)*self.size))
		painter.drawLine(QPointF(UNIT_WIDTH*self.size, 0),
						 QPointF((1-RLINE)*(UNIT_WIDTH)*self.size, 0))
		painter.drawLine(QPointF(0, (UNIT_HEIGHT)*self.size),
						 QPointF(0, (1-RLINE)*(UNIT_HEIGHT)*self.size))
		painter.drawLine(QPointF(0, (UNIT_HEIGHT)*self.size),
						 QPointF(RLINE*(UNIT_WIDTH)*self.size, (UNIT_HEIGHT)*self.size))
		painter.drawLine(QPointF(UNIT_WIDTH*self.size, (UNIT_HEIGHT)*self.size),
						 QPointF(UNIT_WIDTH*self.size, (1-RLINE)*(UNIT_HEIGHT)*self.size))
		painter.drawLine(QPointF(UNIT_WIDTH*self.size, (UNIT_HEIGHT)*self.size),
						 QPointF((1-RLINE)*(UNIT_WIDTH )*self.size, (UNIT_HEIGHT)*self.size))
		painter.restore()

class DieIndUnit(AbstractUnit):
	def __init__(self, x, y, z, parent = None):
		super(DieIndUnit, self).__init__(x, y, z, parent)

	def boundingRect(self):
		return QRectF(0, 0, UNIT_WIDTH, UNIT_HEIGHT)

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

	def boundingRect(self):
		return QRectF(0, 0, 10, 10)

	def paint(self, painter, option, widget = None):
		painter.save()
		brush = QBrush(QColor(Qt.red),Qt.SolidPattern)
		painter.setBrush(brush)
		painter.drawEllipse(0, 0, 5, 5)
		painter.restore()

class EffectIndUnit(QGraphicsTextItem):
	def __init__(self, text, parent = None):
		super(EffectIndUnit, self).__init__(text, parent)
		self.text = text
		font = self.font()
		self.setZValue(0.5)
		self.setScale(0.8)
		self.setDefaultTextColor(QColor(Qt.red))
		if text[0] == "-" :
			font.setPointSize(font.pointSize())
			self.setDefaultTextColor(QColor(Qt.red))
		elif text[0] == "+" :
			font.setPointSize(font.pointSize())
			self.setDefaultTextColor(QColor(Qt.green))
		else:
			font.setPointSize(font.pointSize())
			self.setDefaultTextColor(QColor(Qt.green))
		self.setFont(font)
		
	def boundingRect(self):
		return QRectF(0, 0, 100, 50)
		
	def setText(self, text):
		if text[0] == "+" or text[0] == '-':
			font.setPointSize(font.pointSize())
		QGraphicsTextItem.setText(self, text)

	def setColor(self, color):
		self.setDefaultTextColor(color)

class FixEffectUnit(AbstractUnit):
	def __init__(self, x, y, z, parent = None):
		super(FixEffectUnit, self).__init__(x, y, z, parent)
		self.setZValue(0.8)
		self.image = QImage(":fix.png").scaled(UNIT_WIDTH, UNIT_HEIGHT * 2, Qt.KeepAspectRatio)

	def boundingRect(self):
		return QRectF(0, 0, 30, 30)

	def paint(self, painter, option, widget = None):
		painter.save()
		painter.drawImage(QPoint(self.corX,self.corY), self.image)
		painter.restore()

class ColEffectUnit(AbstractUnit):
	def __init__(self, x, y, z, parent = None):
		super(ColEffectUnit, self).__init__(x, y, z, parent)
		self.setZValue(0.8)
		self.image = QImage(":collect.png").scaled(UNIT_WIDTH / 2, UNIT_HEIGHT, Qt.KeepAspectRatio)

	def boundingRect(self):
		return QRectF(0, 0, UNIT_WIDTH, UNIT_HEIGHT)

	def paint(self, painter, option, widget = None):
		painter.save()
		painter.drawImage(QPoint(self.corX,self.corY), self.image)
		painter.restore()

class SupEffectUnit(AbstractUnit):
	def __init__(self, x, y, z, x1, y1, z1, parent = None):
		super(SupEffectUnit, self).__init__(x, y, z, parent)
		self.setZValue(0.8)
		self.pos = (x, y, z, x1, y1, z1)
		self.image = QImage(":supply.png").scaled(UNIT_WIDTH / 2, UNIT_HEIGHT, Qt.KeepAspectRatio)

	def boundingRect(self):
		return QRectF(0, 0, 100, 100)

	def paint(self, painter, option, widget = None):
		painter.save()
		pen = QPen()
		pen.setWidth(2)
		pen.setCapStyle(Qt.RoundCap)
		pen.setJoinStyle(Qt.RoundJoin)
		pen.setColor(QColor(Qt.green))
		painter.setPen(pen)
		painter.drawLine(QPointF(self.pos[0]*GRID_WIDTH + 15, self.pos[1]*GRID_HEIGHT + (2-self.pos[2])*UNIT_HEIGHT + 5),
						 QPointF(self.pos[3]*GRID_WIDTH + 15, self.pos[4]*GRID_HEIGHT + (2-self.pos[5])*UNIT_HEIGHT + 5))
		#painter.drawImage(QPoint(self.corX,self.corY), self.image)
		painter.restore()

class BoomEffectUnit(AbstractUnit):
	def __init__(self, x, y, z, parent = None):
		super(BoomEffectUnit, self).__init__(x, y, z, parent)
		self.setZValue(0.8)
		self.image = QImage(":boom.png").scaled(30, 10, Qt.KeepAspectRatio)

	def boundingRect(self):
		return QRectF(0, 0, UNIT_WIDTH, UNIT_HEIGHT)

	def paint(self, painter, option, widget = None):
		painter.save()
		painter.drawImage(QPoint(0,0), self.image)
		painter.restore()

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