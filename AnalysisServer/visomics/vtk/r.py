# import vtk wrapped version that will raise exceptions for error events
import vtkwithexceptions as vtk
import base64
import os
import tempfile

from celery import Celery
from celery import task, current_task
from celery.result import AsyncResult

from visomics.vtk.common import parse_json

celery = Celery()
celery.config_from_object('celeryconfig')

@celery.task
def run(input):
    task_description =  parse_json(input);

    return execute(task_description['inputs'], task_description['outputs'],
                   task_description['script'])

def execute(inputs, outputs, script):

    # prepend some R code to the beginning of the script
    # this allows us to capture useful error output information
    script = "con <- file(\"/tmp/R_errors.txt\", \"w\")\nsink(con, type=\"message\")\n" + script

    # Setup the pipeline
    rcalc = vtk.vtkRCalculatorFilter()
    rcalc.SetRscript(script)

    readers = []
    input_tables = []
    input_trees = []
    for name, type, format, value in inputs:

        if type == 'Table':
            input_tables.append(name)
            input = vtk.vtkTableReader()
        elif type == 'Tree':
            input_trees.append(name)
            input = vtk.vtkTreeReader()

        data_value = base64.b64decode(value)
        input.SetBinaryInputString(data_value, len(data_value))
        input.SetReadFromInputString(1)

        readers.append(input)

    names = vtk.vtkStringArray()
    names.SetNumberOfComponents(1)
    names.SetNumberOfTuples(len(input_tables))
    index = 0
    for name in input_tables:
        names.SetValue(index, name)
        index += 1
    rcalc.PutTables(names)

    names = vtk.vtkStringArray()
    names.SetNumberOfComponents(1)
    names.SetNumberOfTuples(len(input_trees))
    index = 0
    for name in input_trees:
        names.SetValue(index, name)
        index += 1
    rcalc.PutTrees(names)

    # Multi inputs
    if len(inputs) > 1:
        input = vtk.vtkMultiBlockDataGroupFilter()

        for reader in readers:
            input.AddInputConnection(reader.GetOutputPort())

    rcalc.AddInputConnection(input.GetOutputPort())

    output_tables = []
    output_trees = []
    for o in outputs:
        if o['type'] == 'Table':
            output_tables.append(o['name'])
        elif o['type'] == 'Tree':
            output_trees.append(o['name'])

    names = vtk.vtkStringArray()
    names.SetNumberOfComponents(1)
    names.SetNumberOfTuples(len(output_tables))
    index = 0
    for name in output_tables:
        names.SetValue(index, name)
        index += 1
    rcalc.GetTables(names)

    names = vtk.vtkStringArray()
    names.SetNumberOfComponents(1)
    names.SetNumberOfTuples(len(output_trees))
    index = 0
    for name in output_trees:
        names.SetValue(index, name)
        index += 1
    rcalc.GetTrees(names)

    try:
        rcalc.Update()
        output = rcalc.GetOutput()
    except vtk.ErrorEventException:
        f = file("/tmp/R_errors.txt", "r")
        error_msg = f.read()
        raise Exception(error_msg)

    print str(output)
    output_dataobjects = []

    if len(outputs) > 1:
        iter = output.NewIterator();
        iter.InitTraversal()
        while not iter.IsDoneWithTraversal():
            dataobject = iter.GetCurrentDataObject()
            output_dataobjects.append(dataobject)
            iter.GoToNextItem()
        iter.FastDelete()
    else:
        output_dataobjects.append(output)

    output_json  =  {'output': []}

    index = 0

    for dataobject in output_dataobjects:

        output = outputs[index]
        index += 1

        if output['type'] == 'Table':
            writer = vtk.vtkTableWriter()
        elif output['type'] == 'Tree':
            writer = vtk.vtkTreeWriter()

        tmp = tempfile.mktemp()

        writer.SetFileName(tmp)
        writer.SetFileTypeToBinary()
        writer.SetInputData(dataobject)
        writer.Update()

        with open(tmp, 'r') as fp:
            data = fp.read()

        os.remove(tmp)

        data = base64.b64encode(data)

        output_json['output'].append({'name': output['name'], 'type': output['type'], 'data': data})

    return output_json
