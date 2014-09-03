from PyQt4.QtCore import Qt, QUrl
from PyQt4.QtGui import QTreeWidget, QTreeWidgetItem, QDockWidget, QIcon

class DirDockWidget(QDockWidget):

	def __init__(self, app, parent=None):
		super(DirDockWidget, self).__init__(parent)
		self.app = app
		self.setObjectName("DirDockWidget")
		self.setWindowTitle("Folders")
		self.tree = DirTreeWidget(app)
		self.setWidget(self.tree)

class DirTreeWidgetItem(QTreeWidgetItem):

	def __init__(self, parent=None):
		super(DirTreeWidgetItem, self).__init__(parent)
		self.path = ""
		self.isLoading = False

class DirTreeWidget(QTreeWidget):

	def __init__(self, app, parent=None):
		super(DirTreeWidget, self).__init__(parent)
		self.app = app
		self.itemsByPath = {}

		self.itemExpanded.connect(self.expandItem)
		self.sortBy = "name"
		self.sortReverse = False
		self.header().close()
		self.dirIcon = QIcon("images/dir-icon-folder.png")
		self.driveIcon = QIcon("images/dir-icon-drive.png")

		self.setAcceptDrops(True)
		self.setDragEnabled(True)
		self.setDropIndicatorShown(False)
		self.dropAction = Qt.MoveAction
		self.setAlternatingRowColors(True)
		#self.setDragDropMode(QAbstractItemView::InternalMove)

		self.getRoots()

	def addItem(self, path, item):
		item.path = path
		self.itemsByPath[path] = item
		item = DirTreeWidgetItem(item)
		item.setText(0, "Loading..")
		item.setFlags(item.flags() & ~Qt.ItemIsSelectable)
		item.isLoading = True

	def getRoots(self):
		self.app.request({
			"call": self.app.system.files.getRoots,
			"callback": self.getRootsCallback
		})

	def getRootsCallback(self, data):
		for d in data:
			path = unicode(d["name"] + "/")
			name = d["name"].upper()
			if "label" in d:
				name += " ["+d["label"]+"]"
			item = DirTreeWidgetItem()
			item.setText(0, name)
			item.setIcon(0, self.driveIcon)
			item.setFlags(item.flags() & ~Qt.ItemIsDragEnabled)
			self.addTopLevelItem(item)
			self.addItem(path, item) # , True)

	def expandItem(self, item):
		if item.childCount() == 1:
			c = item.child(0)
			if c.isLoading:
				self.refresh([item.path])

	def refresh(self, paths):
		self.app.request({
			"call": self.app.system.files.getFiles,
			"callback": self.refreshCallback,
			"getDirs": True,
			"paths": paths,
			"sortBy": self.sortBy,
			"sortReverse": self.sortReverse
		})

	def refreshCallback(self, data):
		for path in data.keys():
			# remove current children
			parent = self.itemsByPath[path]
			children = parent.takeChildren()
			for child in children:
				if child.path in self.itemsByPath:
					del self.itemsByPath[child.path]
			del children[:]
			# add new children
			checkPaths = []
			for d in data[path]:
				item = DirTreeWidgetItem(parent)
				p = path + d["name"] + "/"
				checkPaths.append(p)
				item.setText(0, d["name"])
				item.setIcon(0, self.dirIcon)
				self.addItem(p, item)
			# check subdirs
			self.app.request({
				"call": self.app.system.files.checkSubDirs,
				"callback": self.subdirCheckResult,
				"noResponse": True,
				"paths": checkPaths
			})

	def subdirCheckResult(self, data):
		has = data["hasDirs"]
		if has:
			return
		path = data["path"]
		if not path in self.itemsByPath:
			return
		item = self.itemsByPath[path]
		if not item.childCount():
			return
		if item.child(0).isLoading:
			item.takeChild(0)

	def dragEnterEvent(self, event):
		super(DirTreeWidget, self).dragEnterEvent(event)
		mimeData = event.mimeData()
		urls = []
		for item in self.selectedItems():
			urls.append(QUrl.fromLocalFile(item.path))
		mimeData.setUrls(urls)
		if event.keyboardModifiers() & Qt.ControlModifier == Qt.ControlModifier:
			event.setDropAction(Qt.CopyAction)
		else:
			event.setDropAction(Qt.MoveAction)

	def dragMoveEvent(self, event):
		super(DirTreeWidget, self).dragMoveEvent(event)
		if event.keyboardModifiers() & Qt.ControlModifier == Qt.ControlModifier:
			event.setDropAction(Qt.CopyAction)
		else:
			event.setDropAction(Qt.MoveAction)



