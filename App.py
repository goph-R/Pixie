import sys
import traceback
import logging

from Utils import Utils

from PySide.QtCore import QCoreApplication, QObject, Signal
from PySide.QtGui import QApplication

from Config import Config
from System import System, SystemThread
from MainWindow import MainWindow

class App(QObject):

	requestSignal = Signal(dict)

	def __init__(self):
		super(App, self).__init__()
		self.qApp = None
		self.exceptHook = None
		self.mainWindow = None
		self.system = None

	def init(self):
		logging.basicConfig(filename=Config.LogPath, level=Config.LogLevel,
							format=Config.LogFormat, datefmt=Config.LogDateFormat)

		QCoreApplication.setOrganizationName(Config.OrganizationName)
		QCoreApplication.setOrganizationDomain(Config.OrganizationDomain)
		QCoreApplication.setApplicationName(Config.ApplicationName)

		self.qApp = QApplication(sys.argv)

		self.exceptHook = sys.excepthook
		sys.excepthook = self.handleError

		system = System()
		self.system = SystemThread(self, system)

		self.requestSignal.connect(system.request)
		system.responseSignal.connect(self.response)

		self.system.start()

		Utils.Init()

	def request(self, data):
		self.requestSignal.emit(data)

	def response(self, data):
		data['callback'](data['return'])

	def run(self):
		self.mainWindow = MainWindow(self)
		self.mainWindow.show()

		return self.qApp.exec_()

	def handleError(self, t, v, tb):
		text = t.__name__ + ': ' + str(v) + '\n'
		for textAndLine in traceback.extract_tb(tb):
			text += str(textAndLine[0]) + ' ' + str(textAndLine[1]) + ' ' \
				+ str(textAndLine[2]) + ' ' + str(textAndLine[3]) + '\n'
		logging.critical(text)

		self.exceptHook(t, v, tb)

if __name__ == '__main__':
	app = App()
	app.init()
	sys.exit(app.run())
