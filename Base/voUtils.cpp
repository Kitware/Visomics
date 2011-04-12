
// Qt includes
#include <QtGlobal>

// Visomics includes
#include "voUtils.h"

// VTK includes
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>

//----------------------------------------------------------------------------
void voUtils::transposeTable(vtkTable* table)
{
  Q_ASSERT(table);

  vtkNew<vtkTable> transposeTable;

  for(int cid = 0; cid < table->GetNumberOfColumns(); ++cid)
    {
    vtkStringArray * column = vtkStringArray::SafeDownCast(table->GetColumn(cid));
    Q_ASSERT(column);
    for (int rid = 0; rid < column->GetNumberOfValues(); ++rid)
      {
      vtkStdString value = column->GetValue(rid);
      vtkSmartPointer<vtkStringArray> transposedColumn;
      if (cid == 0)
        {
        transposedColumn = vtkSmartPointer<vtkStringArray>::New();
        transposedColumn->SetNumberOfValues(table->GetNumberOfRows());
        transposeTable->AddColumn(transposedColumn);
        }
      else
        {
        transposedColumn = vtkStringArray::SafeDownCast(transposeTable->GetColumn(rid));
        }
      transposedColumn->SetValue(cid, value);
      }
    }

  table->DeepCopy(transposeTable.GetPointer());
}
