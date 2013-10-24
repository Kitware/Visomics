# Module to add observers that will raise exceptions for ErrorEvents
import types
import sys
import inspect
from functools import partial

class ErrorEventException(StandardError):
    def __init__(self, obj, message):
        print "message: "+ message
        super(ErrorEventException, self).__init__(message)

def raise_exception_error_handler(obj, event_type, message):
    obj._error_event = ErrorEventException(obj, message)

# needed to get the third parameter!
raise_exception_error_handler.CallDataType = "string0"

class vtkModuleExceptionRaisingWrapper(types.ModuleType):
    def __getattribute__(self, cls_name):
        cls = super(vtkModuleExceptionRaisingWrapper, self).__getattribute__(cls_name)

        if cls_name.startswith('vtk'):
            cls = object.__getattribute__(self, cls_name)
            if not type(cls).__name__ == 'vtkclass':
                return cls
            class vtkClassExceptionRaisingWrapper(cls):
                def __init__(self):
                    print 'init'
                    try:
                        cls.__init__(self)
                    except AttributeError:
                        pass
                    self._error_event = None
                    self._wrap_methods()
                    self.AddObserver('ErrorEvent', raise_exception_error_handler)

                def _wrap_methods(self):
                    def _wrapper(original, *pargs):
                        if self._error_event:
                            raise self._error_event
                        else:
                            return original(*pargs)

                    for builtin in inspect.getmembers(self, inspect.isbuiltin):
                        func = partial(_wrapper, builtin[1])
                        setattr(self, builtin[0], func)

            return  vtkClassExceptionRaisingWrapper
        return cls

vtk = vtkModuleExceptionRaisingWrapper('vtk')

sys.modules['vtk'] = vtk

# need to reload so we get the wrapped version
reload(vtk)

from vtk import *
