# vtk is included above this scope

def execute(inputs):
  tree1 = inputs["tree1"]
  tree2 = inputs["tree2"]

  filter = vtk.vtkTreeDifferenceFilter()
  filter.SetInputDataObject(0, tree1)
  filter.SetInputDataObject(1, tree2)

  filter.SetIdArrayName("node name")
  filter.SetComparisonArrayIsVertexData(False)
  filter.SetComparisonArrayName("weight")
  filter.SetOutputArrayName("differences")

  filter.Update()

  comparisonTree = vtk.vtkTree()
  comparisonTree.ShallowCopy(filter.GetOutput())

  outputs = { "comparisonTree": comparisonTree }
  return outputs
