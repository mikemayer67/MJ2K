#import os
#import importlib
#
#py_dir   = os.path.dirname(__file__)
#pkg      = os.path.split(py_dir)[-1]
#for mod in (f[:-3] for f in os.listdir(py_dir) if f.endswith('.py')):
#    if mod not in ('__init__', 'base'):
#        cls = mod.upper()
#        try:
#            locals()[cls] = getattr(importlib.import_module(f'{pkg}.{mod}'),cls)
#        except:
#            pass

