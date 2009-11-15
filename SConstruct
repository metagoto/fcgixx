import os


OPTIONS = {
  '--boost-dir': {
    'help': 'boost installation directory',
    'default': '/usr/local'
  },
  '--extra-dir': {
    'help': 'comma separated list of directories'
  },
}


def SetOptions(options):
  for (name, option) in options.iteritems():
    AddOption(name,
      dest   = option.get('dest', name),
      type   = option.get('type', 'string'),
      nargs  = option.get('nargs', 1),
      default= option.get('default', None),
      help   = option.get('help', ''),
      action = 'store'
    )

def AddBuildPath(env, path, prepend=False):
  if path is None:
    return
  if prepend:
    env.Prepend(CPPPATH=[os.path.join(path,'include')])
    env.Prepend(LIBPATH=[os.path.join(path,'lib')])
  else:
    env.Append(CPPPATH=[os.path.join(path,'include')])
    env.Append(LIBPATH=[os.path.join(path,'lib')])


env = Environment(ENV = {'PATH' : os.environ['PATH']})

SetOptions(OPTIONS)

AddBuildPath(env, GetOption('--boost-dir'))

if env.GetOption('--extra-dir') is not None:
  [AddBuildPath(env, x) for x in GetOption('--extra-dir').split(',')]


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
