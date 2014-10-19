import os
from PyQt4.QtCore import Qt, QSettings, QSize
from PyQt4.QtGui import QMainWindow, QIcon, QMenu, QMenuBar, QImage

from DirWidgets import DirDockWidget
from FileWidgets import FilesWidget
from Utils import Utils


class MainWindow(QMainWindow):

	def __init__(self, app, parent=None):
		super(MainWindow, self).__init__(parent)

		self.createStyle()

		self.app = app
		self.homePath = 'c:/users/gopher'
		self.readSettings()
		self.setWindowTitle('Pixie - The Image Manager')
		self.setWindowIcon(QIcon('images/favicon.ico'))

		self.dirDock = DirDockWidget(app)
		self.addDockWidget(Qt.LeftDockWidgetArea, self.dirDock)

		menuBar = QMenuBar(self)
		menu = QMenu('&Windows', menuBar)
		menu.addAction(self.dirDock.toggleViewAction())
		menuBar.addMenu(menu)
		self.setMenuBar(menuBar)

		self.filesWidget = FilesWidget(app)
		self.setCentralWidget(self.filesWidget)

		self.dirDock.tree.itemSelectionChanged.connect(self.dirChanged)

	def createStyle(self):
		f = open('style.qss')
		style = f.read()
		f.close()
		style = style.replace('%imagesdir%', Utils.GetImagesDir())
		self.setStyleSheet(style)

	def dirChanged(self):
		items = self.dirDock.tree.selectedItems()
		if items:
			paths = []
			for item in items:
				paths.append(item.path)
			self.filesWidget.changeDir(paths)

	def closeEvent(self, event):
		self.writeSettings()
		self.filesWidget.list.clear()
		self.app.request({
			'call': self.app.system.files.quit,
			'callback': self.closeCallback()
		})
		event.ignore()
		super(MainWindow, self).closeEvent(event)

	def closeCallback(self):
		self.app.qApp.quit()

	def readSettings(self):
		settings = QSettings()
		self.restoreGeometry(settings.value('MainWindow/geometry').toByteArray());
		self.restoreState(settings.value('MainWindow/windowState').toByteArray());

	def writeSettings(self):
		settings = QSettings()
		settings.setValue('MainWindow/geometry', self.saveGeometry())
		settings.setValue('MainWindow/windowState', self.saveState())

	def sizeHint(self):
		return QSize(640, 480)


