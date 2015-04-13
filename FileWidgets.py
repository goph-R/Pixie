import platform

from PySide.QtCore import Qt, QRect, QSize, QPoint, Signal
from PySide.QtGui import QWidget, QListWidgetItem, QVBoxLayout, QHBoxLayout, QLineEdit, QLabel, QImage, \
	QIcon, QPixmap, QStyledItemDelegate, QAbstractItemView, QSizeGrip, QPainter

from ImageFactory import ImageListWidget
from Utils import Utils


class FileListWidgetItemDelegate(QStyledItemDelegate):

	def __init__(self, fileList, parent=None):
		super(FileListWidgetItemDelegate, self).__init__(parent)
		self.list = fileList

	def paint(self, painter, option, index):
		super(FileListWidgetItemDelegate, self).paint(painter, option, index)
		path = unicode(index.data(Qt.UserRole).toString())
		item = self.list._itemsByPath[path]


class FileListWidgetItem(QListWidgetItem):

	def __init__(self, parent=None):
		super(FileListWidgetItem, self).__init__(parent)

class FileListWidget(ImageListWidget):

	def __init__(self, parent=None):
		super(FileListWidget, self).__init__(parent)
		self.setAcceptDrops(True)
		### Why this doesn't working in PySide.8/Python2.7 ?
		self.setVerticalScrollMode(QAbstractItemView.ScrollPerItem)
		#self.verticalScrollBar().setSingleStep(2)

		# set icons
		imagesDir = Utils.GetImagesDir()
		self.icons = {
			'folder': {'image': QImage(imagesDir + 'file-icon-folder.png'), 'icon': None },
			'file': {'image': QImage(imagesDir + 'file-icon-file.png'), 'icon': None },
			'loading': {'image': QImage(imagesDir + 'file-icon-loading.png'), 'icon': None }
		}

		# TODO: from settings
		self.setIconSize(192, 192)
		self.setItemSize(192 + 8, 192 + 64)

	def dragEnterEvent(self, event):
		super(FileListWidget, self).dragEnterEvent(event)
		if event.keyboardModifiers() & Qt.ControlModifier == Qt.ControlModifier:
			event.setDropAction(Qt.CopyAction)
		else:
			event.setDropAction(Qt.MoveAction)

	def dragMoveEvent(self, event):
		super(FileListWidget, self).dragMoveEvent(event)
		if event.keyboardModifiers() & Qt.ControlModifier == Qt.ControlModifier:
			event.setDropAction(Qt.CopyAction)
		else:
			event.setDropAction(Qt.MoveAction)

	def dropEvent(self, event):
		event.setDropAction(Qt.IgnoreAction)
		event.ignore()
		super(FileListWidget, self).dropEvent(event)

	def addToFactory(self, path, width, height, meta):
		if Utils.IsSupported(path):
			self._imageFactory.add(path, width, height, meta)
		elif path in self._itemsByPath and not meta['isDir']:
			for item in self._itemsByPath[path]:
				item.setIcon(self.icons['file']['icon'])

	def setIconSize(self, w, h):
		super(FileListWidget, self).setIconSize(w, h)
		self.scaleIcons()

	def scaleIcons(self):
		size = self.iconSize()
		for icon in self.icons.values():
			if not icon['image'].isNull():
				image = icon['image'].scaled(size.width(), size.height(), Qt.KeepAspectRatio, Qt.SmoothTransformation)
				icon['icon'] = QIcon(QPixmap.fromImage(image))

class FilesWidget(QWidget):

	def __init__(self, app, parent=None):
		super(FilesWidget, self).__init__(parent)
		self.app = app
		self.setAcceptDrops(False)
		self.data = {}
		vlayout = QVBoxLayout()
		vlayout.setContentsMargins(0, 0, 0, 0)
		vlayout.setSpacing(0)

		hlayout = QHBoxLayout()
		hlayout.setContentsMargins(5, 5, 5, 5)
		hlayout.setSpacing(5)
		self.pathEdit = QLineEdit()
		self.pathEdit.returnPressed.connect(self.pathEditReturnPressed)
		hlayout.addWidget(QLabel('Path:'))
		hlayout.addWidget(self.pathEdit)
		vlayout.addLayout(hlayout)

		self.list = FileListWidget()
		self.list.itemDoubleClicked.connect(self.fileDoubleClicked)
		vlayout.addWidget(self.list)

		self.setLayout(vlayout)

		self.paths = []
		self.sortBy = 'name'
		self.sortReverse = False

		self.imageWidgets = []

	def refresh(self):
		self.app.request({
			'call': self.app.system.files.clearImageCheckers
		})
		self.app.request({
			'call': self.app.system.files.getFilesAndDirs,
			'callback': self.refreshCallback,
			'getDirs': False,
			'getBack': len(self.paths) > 3, # TODO: this trick will not work on linux
			'paths': self.paths,
			'sortBy': self.sortBy,
			'sortReverse': self.sortReverse
		})

	def refreshCallback(self, data):
		self.list.stop()
		self.list.clear()
		self.list.scrollToTop() # TODO: if user hits F5, don't do this

		# store data
		self.data = data

		# create folder items
		for path in data['dirs'].keys():
			for d in data['dirs'][path]:
				item = FileListWidgetItem()
				item.setText(d['name'])
				p = path + d['name'] + '/'
				item.setData(Qt.UserRole, p)
				if d['name'] != '..':
					item.setIcon(self.list.icons['folder']['icon'])
				self.list.addItem(p, item, meta={'isDir': True})

		# create file items
		for path in data['files'].keys():
			for d in data['files'][path]:
				item = FileListWidgetItem()
				item.setText(d['name'])
				item.setIcon(self.list.icons['loading']['icon'])
				p = path + d['name']
				item.setData(Qt.UserRole, p)
				self.list.addItem(p, item, meta={'isDir': False})

		# create thumbnails
		self.list.create()

	def changeDir(self, paths):
		self.paths = paths
		self.refresh()
		pathText = ''
		sep = ''
		for path in self.paths:
			if platform.system() == 'Windows':
				p = path.replace('/', '\\')[:-1]
				p = p[:1].upper() + p[1:]
			else:
				p = path[:-1]
			pathText += sep + p
			sep = '|'
		self.pathEdit.setText(pathText)

	def pathEditReturnPressed(self):
		self.pathEdit.text()

	def fileDoubleClicked(self):
		items = self.list.selectedItems()
		if not items:
			return
		print unicode(items[0].data(Qt.UserRole))
