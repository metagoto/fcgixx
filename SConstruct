import os

env = Environment(ENV = {'PATH' : os.environ['PATH']})

LIBNAME = 'fcgixx'

DESTDIR = 'lib'


SOURCES = [
  'src/fastcgi.cpp'
]

CXXFLAGS = [
  '-Wall',
  '-O3',
  '-fomit-frame-pointer',
  '-fdata-sections',
  '-ffunction-sections'
]

env.Append(CXXFLAGS=CXXFLAGS)

lib = env.StaticLibrary(LIBNAME, SOURCES)

env.Install(DESTDIR, lib)
