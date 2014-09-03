import platform

from PyQt4.QtCore import Qt
from PyQt4.QtGui import QWidget, QListWidgetItem, QVBoxLayout, QHBoxLayout, QLineEdit, QLabel, QAbstractItemView

from ImageFactory import ImageListWidget

class FileListWidgetItem(QListWidgetItem):

	def __init__(self, parent=None):
		super(FileListWidgetItem, self).__init__(parent)

class FileListWidget(ImageListWidget):

	def __init__(self, parent=None):
		super(FileListWidget, self).__init__(parent)
		self.setAcceptDrops(True)

		### Why this is never work in PyQt4 ?!
		# self.setVerticalScrollMode(QAbstractItemView.ScrollPerPixel)
		# self.verticalScrollBar().setSingleStep(2)

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

	def addToFactory(self, path, meta):
		# TODO: use a central "supportedFormats" stuff
		if path.endswith(".jpg") or path.endswith(".png"):
			self._imageFactory.add(path, meta)

class FilesWidget(QWidget):

	def __init__(self, app, parent=None):
		super(FilesWidget, self).__init__(parent)
		self.app = app
		self.setAcceptDrops(False)

		vlayout = QVBoxLayout()
		vlayout.setContentsMargins(0, 0, 0, 0)
		vlayout.setSpacing(0)

		hlayout = QHBoxLayout()
		hlayout.setContentsMargins(5, 5, 5, 5)
		hlayout.setSpacing(5)
		self.pathEdit = QLineEdit()
		self.pathEdit.returnPressed.connect(self.pathEditReturnPressed)
		hlayout.addWidget(QLabel("Path:"))
		hlayout.addWidget(self.pathEdit)
		vlayout.addLayout(hlayout)

		self.list = FileListWidget()
		self.list.setIconSize(192, 192)
		self.list.setItemSize(192 + 8, 192 + 32)
		vlayout.addWidget(self.list)

		self.setLayout(vlayout)

		self.path = ""
		self.sortBy = "name"
		self.sortReverse = False

	def refresh(self):
		self.app.request({
			"call": self.app.system.files.clearImageCheckers
		})
		self.app.request({
			"call": self.app.system.files.getFilesAndDirs,
			"callback": self.refreshCallback,
			"getDirs": False,
			"getBack": True,
			"paths": [self.path],
			"sortBy": self.sortBy,
			"sortReverse": self.sortReverse
		})

	def refreshCallback(self, data):
		self.list.stop()
		self.list.clear()
		self.list.scrollToTop() # TODO: if user hit F5, don't do this
		for k in ["dirs", "files"]:
			for path in data[k].keys():
				for d in data[k][path]:
					text = d["name"]
					item = FileListWidgetItem()
					if k == "dirs":
						p = path + d["name"] + "/"
					else:
						p = path + d["name"]
						if d["size"]:
							text += "\n[" + unicode(d["size"])+"]"
					item.setText(text)
					self.list.addItem(p, item)
		self.list.create()

	def changeDir(self, path):
		self.path = path
		self.refresh()
		if platform.system() == "Windows":
			p = path.replace("/", "\\")[:-1]
			p = p[:1].upper() + p[1:]
		else:
			p = path[:-1]
		self.pathEdit.setText(p)

	def pathEditReturnPressed(self):
		self.pathEdit.text()


