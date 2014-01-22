# import vtk wrapped version that will raise exceptions for error events
import vtkwithexceptions as vtk

import imp
import tempfile
from visomics.vtk.common import LoadInput, SerializeOutput, parse_json

from celery import Celery
from celery import task, current_task
from celery.result import AsyncResult

celery = Celery()
celery.config_from_object('celeryconfig')

@celery.task
def run(input):
  task_description =  parse_json(input);

  # load inputs into a dictionary
  inputs = {}
  for name, type, format, data in task_description['inputs']:
    inputs[name] = LoadInput(type, format, data)

  # load incoming Python script code into a custom module
  module_code = task_description['script']
  custom = imp.new_module("custom")
  exec module_code in custom.__dict__

  # call the custom Python code and get its output
  outputs = custom.execute(inputs)

  # wrap the output up into a dictionary & return
  output_list = []
  for name, object in outputs.iteritems():
    type = object.GetClassName()
    data = SerializeOutput(object)
    d = {"name": name, "type": type, "data": data }
    output_list.append(d)

  output_json = {"output": output_list}
  return output_json
