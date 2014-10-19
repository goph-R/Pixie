# -*- coding: utf-8 -*-

import sys
import shutil
from cx_Freeze import setup, Executable

base = None
if sys.platform == 'win32':
    base = 'Win32GUI'

options = {
    'build_exe': {
        'includes': 'atexit'
    }
}

executables = [
    Executable('App.py', base=base, icon='images/favicon.ico')
]

setup(name='App',
      version='0.1',
      description='Pixie The Image Manager',
      options=options,
      executables=executables
      )

# copy resources
try:
    shutil.rmtree('build/exe.win-amd64-2.7/images')
except:
    pass
shutil.copytree('images', 'build/exe.win-amd64-2.7/images')
shutil.copy('style.qss', 'build/exe.win-amd64-2.7/style.qss')