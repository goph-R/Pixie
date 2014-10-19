import logging
import tempfile

class Config(object):

    # general informations (QSettings uses it)
    OrganizationName = 'DynArt'
    OrganizationDomain = 'dynart.info'
    ApplicationName = 'Pixie'

    # logging config
    LogPath = tempfile.gettempdir() + '/pixie.log'
    LogLevel = logging.DEBUG
    LogFormat = '%(asctime)s %(levelname)s: %(message)s'
    LogDateFormat = '%Y-%m-%d %H:%M:%S'


