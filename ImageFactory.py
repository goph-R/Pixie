import os
import copy
import time

from PyQt4.QtCore import Qt, QObject, QThread, QThreadPool, QRunnable, pyqtSignal
from PyQt4.QtGui import QImage

class ImageFactoryWorker(QObject):

	doneSignal = pyqtSignal(dict)

	def __init__(self, imageData, parent=None):
		super(ImageFactoryWorker, self).__init__(parent)
		self._imageData = copy.deepcopy(imageData)

	def run(self):
		if not self._imageData['cached']:
			self._imageData['image'] = None
			if os.path.exists(self._imageData['path']):
				image = QImage(self._imageData['path'])
				if not image.isNull():
					ratio = Qt.KeepAspectRatio if self._imageData['keepRatio'] else Qt.IgnoreAspectRatio
					image = image.scaled(self._imageData['width'], self._imageData['height'], ratio, Qt.SmoothTransformation)
					if not image.isNull():
						self._imageData['image'] = image
		self.doneSignal.emit(self._imageData)


class ImageFactoryRunnable(QRunnable):

	def __init__(self, worker):
		super(ImageFactoryRunnable, self).__init__()
		self._worker = worker

	def run(self):
		self._worker.run()


class ImageFactory(QObject):

	doneSignal = pyqtSignal(dict)

	@staticmethod
	def DefaultCompare(a, b):
		return a['priority'] - b['priority']

	def __init__(self, parent=None):
		super(ImageFactory, self).__init__(parent)
		self._imagesData = []
		self._pool = QThreadPool()
		self._threadCount = QThread.idealThreadCount()
		self._nextPriority = 0
		self._width = 128
		self._height = 128
		self._keepRatio = True
		self._compare = ImageFactory.DefaultCompare
		self._runnableCount = 0
		self._cache = {}
		self._useCache = False

	def setUseCache(self, uc):
		self._useCache = uc

	def setThreadCount(self, tc):
		self._threadCount = tc

	def setSize(self, w, h):
		self._width = w
		self._height = h

	def setKeepRatio(self, kr):
		self._keepRatio = kr

	def setCompare(self, func):
		self._compare = func

	def add(self, path, meta={}):
		imageData = {
			'path': path,
			'meta': meta,
			'priority': self._nextPriority,
			'width': self._width,
			'height': self._height,
			'keepRatio': self._keepRatio,
			'cached': self._useCache and path in self._cache
		}
		self._imagesData.append(imageData)
		self._nextPriority += 1

	def start(self):
		self.sortQueue()
		self._queueNext()

	def sortQueue(self):
		self._imagesData = sorted(self._imagesData, cmp=self._compare, reverse=True)

	def _queueNext(self):
		while self._pool.activeThreadCount() < self._threadCount and self._imagesData:
			imageData = self._imagesData[-1]
			worker = ImageFactoryWorker(imageData)
			worker.doneSignal.connect(self.done)
			runnable = ImageFactoryRunnable(worker)
			if self._pool.tryStart(runnable):
				del self._imagesData[-1]

	def done(self, imageData):
		self._queueNext()
		if self._useCache:
			path = imageData['path']
			if imageData['cached']:
				imageData['image'] = self._cache[path]['image']
			else:
				self._cache[path] = imageData
				self._cache[path]['cacheTime'] = time.time()
		self.doneSignal.emit(imageData)

	def clearCache(self):
		self._cache.clear()

	def clear(self):
		del self._imagesData[:]
		self._pool.waitForDone()

	def quit(self):
		self.clear()

##############################################################################################################

from PyQt4.QtGui import QListWidget, QIcon, QPixmap, QListView
from PyQt4.QtCore import QSize, QTimer

class ImageListWidget(QListWidget):

	imageDoneSignal = pyqtSignal(dict)

	def __init__(self, parent=None):
		super(ImageListWidget, self).__init__(parent)

		self._noImage = QImage()
		self._noImageScaled = self._noImage

		self._imageFactory = ImageFactory()
		self._imageFactory.doneSignal.connect(self.imageDone)
		self._imageFactory.setCompare(self.compare)
		self.setIconSize(192, 192)

		self._itemsByPath = {}
		self._itemWidth = 216
		self._itemHeight = 232

		self.horizontalScrollBar().valueChanged.connect(self.viewChanged)
		self.verticalScrollBar().valueChanged.connect(self.viewChanged)

		self._viewChangedTimer = QTimer()
		self._viewChangedTimer.setSingleShot(True)
		self._viewChangedTimer.timeout.connect(self.viewChangedCallback)

		self.setViewMode(QListView.IconMode)
		self.setResizeMode(QListView.Adjust)


	def resizeEvent(self, event):
		super(ImageListWidget, self).resizeEvent(event)
		self.viewChanged()

	def setNoImage(self, image):
		self._noImage = image
		self._scaleNoImage()

	def _scaleNoImage(self):
		if self._noImage.isNull():
			return
		size = self.iconSize()
		self._noImageScaled = self._noImage.scaled(size.width(), size.height(), Qt.KeepAspectRatio, Qt.SmoothTransformation)

	def viewChanged(self):
		if self._viewChangedTimer.isActive():
			self._viewChangedTimer.stop()
		self._viewChangedTimer.start(100)

	def viewChangedCallback(self):
		self._imageFactory.sortQueue()

	def setIconSize(self, w, h):
		super(ImageListWidget, self).setIconSize(QSize(w, h))
		self._imageFactory.clearCache()
		self._imageFactory.setSize(w, h)
		self._scaleNoImage()

	def setItemSize(self, w, h):
		self._itemWidth = w
		self._itemHeight = h

	def addItem(self, path, item, meta={}):
		super(ImageListWidget, self).addItem(item)
		self.addToFactory(path, meta)
		if not path in self._itemsByPath:
			self._itemsByPath[path] = [item]
		else:
			self._itemsByPath[path].append(item)
		item.setSizeHint(QSize(self._itemWidth, self._itemHeight))

	def addToFactory(self, path, meta):
		self._imageFactory.add(path, meta)

	def _itemIsVisible(self, item):
		return self.rect().intersects(self.visualItemRect(item))

	def compare(self, a, b):
		aVisible = False
		bVisible = False
		if a['path'] in self._itemsByPath:
			for item in self._itemsByPath[a['path']]:
				aVisible |= self._itemIsVisible(item)
		if b['path'] in self._itemsByPath:
			for item in self._itemsByPath[b['path']]:
				bVisible |= self._itemIsVisible(item)
		if aVisible and not bVisible:
			return -1
		if bVisible and not aVisible:
			return 1
		return a['priority'] - b['priority']

	def create(self):
		self._imageFactory.start()

	def stop(self):
		self._imageFactory.clear()

	def imageDone(self, imageData):
		path = imageData['path']
		if not path in self._itemsByPath:
			return
		image = imageData['image'] if imageData['image'] else self._noImageScaled
		for item in self._itemsByPath[path]:
			icon = QIcon(QPixmap.fromImage(image))
			item.setIcon(icon)
		self.imageDoneSignal.emit(imageData)

	def clear(self):
		self._itemsByPath.clear()
		super(ImageListWidget, self).clear()

	def quit(self):
		self._imageFactory.quit()