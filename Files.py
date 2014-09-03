import platform
import locale
locale.setlocale(locale.LC_ALL, "")

from PyQt4.QtCore import QObject, QDir, pyqtSignal, QDirIterator, QThreadPool, QRunnable

if platform.system() == "Windows":
	import win32api


class SubChecker(QObject):

	doneSignal = pyqtSignal(dict)
	finishedSignal = pyqtSignal()

	def __init__(self):
		super(SubChecker, self).__init__()

	# abstract
	def run(self, data):
		pass

class SubDirChecker(SubChecker):

	def run(self, data):
		for path in data["paths"]:
			it = QDirIterator(path, QDir.Dirs | QDir.NoDotAndDotDot)
			hasDirs = it.hasNext()
			self.doneSignal.emit({
				"callback": data["callback"],
				"return": {"path": path, "hasDirs": hasDirs}
			})
		self.finishedSignal.emit()


class SubImageChecker(SubChecker):

	def run(self, data):
		maxCount = data["count"]
		imagePaths = []
		for path in data["paths"]:
			it = QDirIterator(path, QDir.Files | QDir.NoDotAndDotDot)
			count = 0
			while it.hasNext():
				name = unicode(it.next())
				# TODO: use a central "supportedFormats" stuff
				if name.endswith('.jpg') or name.endswith('.png'):
					imagePaths.append(path + name)
					count += 1
					if count == maxCount:
						break
			self.doneSignal.emit({
				"callback": data["callback"],
				"return": {"path": path, "imagePaths": imagePaths}
			})
		self.finishedSignal.emit()


class SubRunnable(QRunnable):

	def __init__(self, worker, data):
		super(SubRunnable, self).__init__()
		self.worker = worker
		self.data = data
		self.forImage = False

	def run(self):
		self.worker.run(self.data)


class Files(QObject):

	def __init__(self, system, parent=None):
		super(Files, self).__init__(parent)
		self.sortBy = "name"
		self.system = system
		self.subDirChecker = None
		self.subImageChecker = None
		self.checkerPool = QThreadPool()
		self.checkerQueue = []
		self.quiting = False

	def quit(self, data):
		self.quiting = True
		self.checkerPool.waitForDone()

	def clearImageCheckers(self, data):
		for i in reversed(range(len(self.checkerQueue))):
			if self.checkerQueue[i].forImage:
				del self.checkerQueue[i]

	def sort(self, a, b):
		if not self.sortBy in a:
			return 1
		if type(a[self.sortBy]) == unicode:
			ret = locale.strcoll(a[self.sortBy].lower(), b[self.sortBy].lower())
		else:
			ret = 1 if a[self.sortBy] > b[self.sortBy] else -1
		return ret

	def getFiles(self, data):
		self.sortBy = data["sortBy"] if "sortBy" in data else "name"
		rev = data["sortReverse"] if "sortReverse" in data else False
		paths = data["paths"]
		getDirs = data["getDirs"] if "getDirs" in data else False
		getBack = getDirs & data["getBack"] if "getBack" in data else False
		ret = {}
		for path in paths:
			allFiles = QDir(path).entryInfoList(QDir.Dirs | QDir.Files | QDir.NoDotAndDotDot)
			files = []
			for fileInfo in allFiles:
				isDir = fileInfo.isDir()
				if (not isDir and getDirs) or (isDir and not getDirs):
					continue
				data = {
					"name": unicode(fileInfo.fileName()),
					"mtime": int(fileInfo.created().toTime_t()),
					"ctime": int(fileInfo.lastModified().toTime_t()),
					"size": fileInfo.size()
				}
				files.append(data)
			files.sort(cmp=self.sort, reverse=rev)
			if getBack:
				files.insert(0, {
					"name": "..",
					"mtime": 0,
					"ctime": 0,
					"size": 0
				})
			ret[path] = files
		return ret

	def getFilesAndDirs(self, data):
		data["getDirs"] = True
		dirs = self.getFiles(data)
		data["getDirs"] = False
		files = self.getFiles(data)
		return {"dirs": dirs, "files": files}

	def getRoots(self, data):
		self.sortBy = data["sortBy"] if "sortBy" in data else "name"
		rev = data["sortReverse"] if "sortReverse" in data else False
		if platform.system() == "Windows":
			drives = win32api.GetLogicalDriveStrings()
			roots = []
			for drive in drives.split(chr(0)):
				if not drive:
					continue
				try:
					info = win32api.GetVolumeInformation(drive)
				except:
					info = [""]
				data = {
					"name": drive[:-1].lower(),
					"label": info[0]
				}
				roots.append(data)
		else:
			ret = self.getFiles({
				"sortBy": self.sortBy,
				"sortReverse": rev,
				"getDirs": True,
				"paths": ["/"]
			})
			roots = []
			for r in ret["/"]:
				data = {"name": "/" + r["name"]}
				roots.append(data)
		roots.sort(cmp=self.sort, reverse=rev)
		return roots

	def createChecker(self, checker, data, forImage):
		checker.doneSignal.connect(self.checkDone)
		checker.finishedSignal.connect(self.queueNextCheck)
		runnable = SubRunnable(checker, data)
		runnable.forImage = forImage
		self.checkerQueue.insert(0, runnable)
		self.queueNextCheck()

	def checkSubDirs(self, data):
		checker = SubDirChecker()
		self.createChecker(checker, data, False)

	def checkSubImages(self, data):
		checker = SubImageChecker()
		self.createChecker(checker, data, True)

	def checkDone(self, data):
		self.system.response(data)

	def queueNextCheck(self):
		while self.checkerPool.activeThreadCount() < self.checkerPool.maxThreadCount() and len(self.checkerQueue):
			runnable = self.checkerQueue[-1]
			if self.checkerPool.tryStart(runnable):
				del self.checkerQueue[-1]