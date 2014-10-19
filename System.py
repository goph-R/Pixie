from PyQt4.QtCore import QObject, QThread, pyqtSignal

from Files import Files

class SystemThread(QThread):

	def __init__(self, app, system):
		super(SystemThread, self).__init__()
		self.system = system
		self.app = app
		self.files = Files(system)
		self.files.moveToThread(self)

	def run(self):
		return self.exec_()

class System(QObject):

	responseSignal = pyqtSignal(dict)

	def __init__(self, parent=None):
		super(System, self).__init__(parent)

	def request(self, data):
		data['return'] = data['call'](data)
		if 'callback' in data and not 'noResponse' in data:
			self.response(data)

	def response(self, data):
		self.responseSignal.emit(data)

