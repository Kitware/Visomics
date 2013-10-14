# Module to add observers that will raise exceptions for ErrorEvents
import types
import sys

class ErrorEventException(Exception):
    def __init__(self, obj, message):
        super(ErrorEventException, self).__init__()

def raise_exception_error_handler(obj, event_type, msg):
    raise ErrorEventException(ojb, msg)

# needed to get the third parameter!
raise_exception_error_handler.CallDataType = "string0"

class vtkModuleExceptionRaisingWrapper(types.ModuleType):
    def __getattribute__(self, cls_name):
        cls = super(types.ModuleType, self).__getattribute__(cls_name)
        if cls_name.startswith('vtk'):
            cls = object.__getattribute__(self, cls_name)

            if not type(cls) == types.ClassType:
                return cls

            class vtkClassExceptionRaisingWrapper(cls):
                def __init__(self):
                    try:
                        cls.__init__(self)
                    except AttributeError:
                        pass

                    self.AddObserver('ErrorEvent', raise_exception_error_handler)

            return  vtkClassExceptionRaisingWrapper

        return cls

vtk = vtkModuleExceptionRaisingWrapper('vtk')

sys.modules['vtk'] = vtk

# need to reload so we get the wrapped version
reload(vtk)

from vtk import *