import os

from PyQt4.QtGui import QImageReader


class Utils:

	AppDir = ''
	SupportedFormats = []

	@staticmethod
	def Init():
		Utils.AppDir = os.path.dirname(__file__).replace('\\', '/') + '/'
		if Utils.AppDir.endswith("library.zip/"):
			Utils.AppDir = Utils.AppDir[:-12]
		for fmt in QImageReader.supportedImageFormats():
			Utils.SupportedFormats.append(unicode(fmt))

	@staticmethod
	def IsSupported(path):
		name, ext = os.path.splitext(path.lower())
		ext = ext[1:]
		return ext in Utils.SupportedFormats

	@staticmethod
	def GetImagesDir():
		return Utils.AppDir + 'images/'

