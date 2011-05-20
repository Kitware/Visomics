
// Qt includes
//#include <QHash>
//#include <QVariant>

// Visomics includes
#include "voNormalization.h"
#include "voUtils.h"

// VTK includes
#include <vtkArrayData.h>
#include <vtkNew.h>
#include <vtkRCalculatorFilter.h>
#include <vtkSmartPointer.h>
#include <vtkTable.h>

namespace Normalization
{

//------------------------------------------------------------------------------
bool applyQuantile(vtkTable * dataTable, const QHash<int, QVariant>& settings)
{
  Q_UNUSED(settings);
  if (!dataTable)
    {
    return false;
    }

  vtkSmartPointer<vtkArray> inputDataArray;
  bool result = voUtils::tableToArray(dataTable, inputDataArray);
  if (!result)
    {
    return false;
    }

  vtkNew<vtkArrayData> RInputArrayData;
  RInputArrayData->AddArray(inputDataArray);

  // Run R code
  vtkNew<vtkRCalculatorFilter> RCalc;
  RCalc->SetRoutput(0);
  RCalc->SetInputConnection(RInputArrayData->GetProducerPort());
  RCalc->PutArray("0", "inputData");
  RCalc->GetArray("outputData","outputData");
  RCalc->SetRscript(QString(
  "library(\"preprocessCore\")\n"
  "outputData <- normalize.quantiles(inputData)"
  ).toLatin1());
  RCalc->Update();

  vtkSmartPointer<vtkArrayData> outputArrayData = vtkArrayData::SafeDownCast(RCalc->GetOutput());
  vtkNew<vtkTable> outputTable;
  voUtils::arrayToTable(outputArrayData->GetArrayByName("outputData"), outputTable.GetPointer());

  dataTable->ShallowCopy(outputTable.GetPointer());
  dataTable->Modified();

  return true;
}

} // end of Normalization namespace
