import os

from PyQt4.QtGui import QImageReader


class Utils:

	SupportedFormats = []

	@staticmethod
	def Init():
		for fmt in QImageReader.supportedImageFormats():
			Utils.SupportedFormats.append(unicode(fmt))

	@staticmethod
	def IsSupported(path):
		name, ext = os.path.splitext(path.lower())
		ext = ext[1:]
		return ext in Utils.SupportedFormats

	@staticmethod
	def GetAppDir():
		return os.path.dirname(__file__).replace('\\', '/') + '/'

	@staticmethod
	def GetImagesDir():
		return Utils.GetAppDir() + 'images/'

