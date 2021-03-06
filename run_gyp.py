#!/usr/bin/env python

import os
import sys
script_dir = os.path.dirname(os.path.abspath(__file__))
root = os.path.normpath(script_dir);
build = os.path.join(os.path.normpath(script_dir), 'build');
sys.path.insert(0, os.path.join(os.path.join(build, 'gyp'), 'pylib'))

import glob
import shlex
import gyp


# Directory within which we want all generated files (including Makefiles)
# to be written.

def run_gyp(args):
  rc = gyp.main(args)
  if rc != 0:
    print 'Error running GYP'
    sys.exit(rc)

if __name__ == '__main__':
  def doRun(opt, platform):
    name = opt['name']
    args = []

    # GYP bug.
    # On msvs it will crash if it gets an absolute path.
    # On Mac/make it will crash if it doesn't get an absolute path.
    common_fn = ""
    if sys.platform == 'win32':
        args.append(os.path.join(root, name))
        if opt['commons']:
          common_fn  = os.path.join(root, 'commons.gypi')
    else:
        args.append(os.path.join(root, name))
        if opt['commons']:
          common_fn  = os.path.join(os.path.abspath(root), 'commons.gypi')
    
    print common_fn
    if len(common_fn) > 0 and os.path.exists(common_fn):
      args.extend(['-I', common_fn])

    args.append('--depth=./')
  
    additional_include = os.getenv("INCLUDE")
    additional_lib = os.getenv("LIB")
    if  additional_include :
      args.append('-Dadditional_include=' + additional_include)
    else :
      args.append('-Dadditional_include=""')
  
    if additional_lib :
      args.append('-Dadditional_lib=' + additional_lib)
    else :
      args.append('-Dadditional_lib=""')
    
    # There's a bug with windows which doesn't allow this feature.
    if sys.platform != 'win32':
      # Tell gyp to write the Makefiles into output_dir
      args.extend(['--generator-output', build])
      # Tell make to write its output into the same dir
      args.extend(['-Goutput_dir=' + build])
      # Create Makefiles, not XCode projects
    if sys.platform != 'darwin' and sys.platform != 'win32':
      args.extend('-f make'.split())
    elif sys.platform == 'darwin':
      args.extend('-f xcode'.split())

    args.append('-Dtarget_arch=' + platform)
      
    args.append('-Dcurrent_dir=' + os.getcwd().replace('\\', '/'))
    gyp_args = list(args)
    print gyp_args
    print '\n'
    run_gyp(gyp_args)

  if len(sys.argv) >= 2:
    platform = sys.argv[1]
  else:
    platform = 'x86'

  for item in [{'name': 'celero.gyp', 'commons': False}, {'name': 'perf-test.gyp', 'commons': True}, {'name': 'test.gyp', 'commons': True}]:
    doRun(item, platform);
