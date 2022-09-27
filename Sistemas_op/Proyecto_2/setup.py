from distutils.core import setup, Extension

modulo = Extension("Operaciones", sources=["mem.cpp"])

setup(name="Operaciones", version="1.0", author="DavidSamuel", description="Memoria procesos",
ext_modules=[modulo])