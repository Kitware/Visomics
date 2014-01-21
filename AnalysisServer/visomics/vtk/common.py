#!/usr/bin/env python

import base64
import os
import tempfile
import vtk

def SerializeVTKTable(table):
  tmp = tempfile.mktemp()
  writer = vtk.vtkTableWriter()
  writer.SetFileName(tmp)
  writer.SetFileTypeToBinary()
  writer.SetInputData(table)
  writer.Update()
  with open(tmp, 'r') as fp:
    data = fp.read()
  os.remove(tmp)
  data = base64.b64encode(data)
  return data

def DeserializeVTKTable(tableString):
  data_tableString = base64.b64decode(tableString)
  reader = vtk.vtkTableReader()
  reader.ReadFromInputStringOn()
  reader.SetBinaryInputString(data_tableString, len(data_tableString))
  reader.Update()
  return reader.GetOutput()

def SerializeVTKTree(tree):
  tmp = tempfile.mktemp()
  writer = vtk.vtkTreeWriter()
  writer.SetFileName(tmp)
  writer.SetFileTypeToBinary()
  writer.SetInputData(tree)
  writer.Update()
  with open(tmp, 'r') as fp:
    data = fp.read()
  os.remove(tmp)
  data = base64.b64encode(data)
  return data

def DeserializeVTKTree(treeString):
  data_treeString = base64.b64decode(treeString)
  reader = vtk.vtkTreeReader()
  reader.ReadFromInputStringOn()
  reader.SetBinaryInputString(data_treeString, len(data_treeString))
  reader.Update()
  return reader.GetOutput()

def NewickToVTKTree(newick):
  reader = vtk.vtkNewickTreeReader()
  reader.SetReadFromInputString(1)
  reader.SetInputString(newick, len(newick))
  reader.Update()
  return reader.GetOutput()

def VTKTreeToNewick(tree):
  writer = vtk.vtkNewickTreeWriter()
  writer.SetWriteToOutputString(1)
  writer.SetInputData(tree)
  writer.Update()
  return writer.GetOutputString()

def CSVToVTKTable(csv):
  reader = vtk.vtkDelimitedTextReader()
  reader.SetReadFromInputString(1)
  reader.SetInputString(csv, len(csv))
  reader.SetHaveHeaders(1)
  reader.DetectNumericColumnsOn()
  reader.Update()
  table = reader.GetOutput()
  return table

def VTKTableToCSV(table):
  writer = vtk.vtkDelimitedTextWriter()
  writer.WriteToOutputStringOn()
  writer.SetInputData(table)
  writer.Update()
  return writer.RegisterAndGetOutputString()

def LoadInput(type, format, data):
  if type.lower() == "table":
    if format.lower() == "csv":
      return util.CSVToVTKTable(data)
    elif format.lower() == "treestore":
      pass
    elif format.lower() == "vtk":
      return util.DeserializeVTKTable(data)
  elif type.lower() == "tree":
    if format.lower() == "newick":
      return util.NewickToVTKTree(data)
    elif format.lower() == "treestore":
      pass
    elif format.lower() == "vtk":
      return util.DeserializeVTKTree(data)

def SerializeOutput(object):
  if object.GetClassName() == "vtkTable":
    return SerializeVTKTable(object)
  elif object.GetClassName() == "vtkTree":
    return SerializeVTKTree(object)
