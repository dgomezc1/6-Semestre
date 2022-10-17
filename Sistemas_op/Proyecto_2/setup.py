
from distutils.core import setup, Extension

modulo = Extension('spam', sources=['mem.cpp'])

setup(name='spam', version="1.0", author="DavidSamuel", description="Memoria procesos",
ext_modules=[modulo])

#from distutils.core import setup, Extension

#module = Extension('spam',sources = ['test1.cpp'])

#setup (name = 'spam',version = '1.0',description = 'This is a demo package',ext_modules = [module])