/*=========================================================================

  Program: Visomics

  Copyright (c) Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QDebug>

// Visomics includes
#include "voPCAStatistics.h"
#include "voTableDataObject.h"
#include "voUtils.h"
#include "vtkExtendedTable.h"

// VTK includes
#include <vtkArrayData.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkRCalculatorFilter.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>

// --------------------------------------------------------------------------
// voPCAStatisticsPrivate methods

// --------------------------------------------------------------------------
class voPCAStatisticsPrivate
{
public:
  vtkSmartPointer<vtkRCalculatorFilter> RCalc;
};

// --------------------------------------------------------------------------
// voPCAStatistics methods

// --------------------------------------------------------------------------
voPCAStatistics::voPCAStatistics():
    Superclass(), d_ptr(new voPCAStatisticsPrivate)
{
  Q_D(voPCAStatistics);

  d->RCalc = vtkSmartPointer<vtkRCalculatorFilter>::New();
}

// --------------------------------------------------------------------------
voPCAStatistics::~voPCAStatistics()
{
}

// --------------------------------------------------------------------------
void voPCAStatistics::setInputInformation()
{
  this->addInputType("input", "vtkExtendedTable");
}

// --------------------------------------------------------------------------
void voPCAStatistics::setOutputInformation()
{
  this->addOutputType("x", "vtkTable",
                      "voPCAProjectionView", "Projection (Plot)",
                      "voTableView", "Projection (Table)");

  this->addOutputType("x-dynview", "vtkTable",
                      "voPCAProjectionDynView", "Projection (Interactive Plot)",
                      "", "");

  this->addOutputType("rot", "vtkTable",
                      "",  "",
                      "voTableView", "Rotation (Table)");

  this->addOutputType("sdev", "vtkTable",
                      "", "",
                      "voTableView", "Std. Deviation (Table)");

  this->addOutputType("loading", "vtkTable" ,
                      "voPCABarView", "Percent Loading (Plot)",
                      "voTableView", "Percent Loading (Table)");

  this->addOutputType("sumloading", "vtkTable" ,
                      "voPCABarView", "Cumulative Percent Loading (Plot)",
                      "voTableView", "Cumulative Percent Loading (Table)");
}

// --------------------------------------------------------------------------
bool voPCAStatistics::execute()
{
  Q_D(voPCAStatistics);

  // Import data table locally
  vtkExtendedTable* extendedTable =  vtkExtendedTable::SafeDownCast(this->input()->dataAsVTKDataObject());
  if (!extendedTable)
    {
    qCritical() << "Input is Null";
    return false;
    }

  //vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::Take(extendedTable->GetDataWithRowHeader());
  vtkSmartPointer<vtkTable> inputDataTable = extendedTable->GetData();

  // Build ArrayData for input to R
  vtkNew<vtkArrayData> RInputArrayData;
    {
    vtkSmartPointer<vtkArray> RInputArray;
    voUtils::tableToArray(inputDataTable.GetPointer(), RInputArray);
    RInputArrayData->AddArray(RInputArray.GetPointer());
    }

  d->RCalc->SetRoutput(0);
  d->RCalc->SetInputData(RInputArrayData.GetPointer());
  d->RCalc->PutArray("0", "PCAData");
  d->RCalc->GetArray("pcaRot", "pcaRot");
  d->RCalc->GetArray("pcaSdev", "pcaSdev");
  d->RCalc->GetArray("sumperload", "sumperload");
  d->RCalc->GetArray("perload", "perload");
  d->RCalc->GetArray("stddev", "stddev");
  d->RCalc->GetArray("projection", "projection");
  d->RCalc->SetRscript("pc1<-prcomp(t(PCAData), scale.=F, center=T, retx=T)\n"
                     "pcaRot<-pc1$rot\n"
                     "pcaSdev<-pc1$sdev\n"
                     "data<-summary(pc1)\n"
                     "numcol=ncol(data$importance)\n"
                     "OutputData<-unlist(data[6],use.names=FALSE)\n"
                     "stddev=OutputData[((1:numcol)*3)-2]\n"
                     "perload=OutputData[((1:numcol)*3)-1]\n"
                     "sumperload=OutputData[((1:numcol)*3)] \n"
                     "projection<-pc1$x");
  d->RCalc->Update();

  // Get R output
  vtkSmartPointer<vtkArrayData> outputArrayData = vtkArrayData::SafeDownCast(d->RCalc->GetOutput());
  if(!outputArrayData || !outputArrayData->GetArrayByName("stddev"))
    {
    qCritical() << QObject::tr("Fatal error in %1 R script").arg(this->objectName());
    return false;
    }

  // Get analyte names with row labels
  vtkNew<vtkStringArray> rowNames;
  voUtils::addCounterLabels(extendedTable->GetRowMetaDataOfInterestAsString(),
                            rowNames.GetPointer(), false);

  // Get experiment names with column labels
  vtkNew<vtkStringArray> columnNames;
  voUtils::addCounterLabels(extendedTable->GetColumnMetaDataOfInterestAsString(),
                            columnNames.GetPointer(), true);

  // ------------------------------------------------
  // Extract projection table
  vtkNew<vtkTable> projectionTable;
    {
    voUtils::arrayToTable(outputArrayData->GetArrayByName("projection"), projectionTable.GetPointer());
    voUtils::transposeTable(projectionTable.GetPointer());

    // Add column labels (experiment names)
    voUtils::setTableColumnNames(projectionTable.GetPointer(), columnNames.GetPointer());

    // Add row labels (components)
    vtkNew<vtkStringArray> headerArr;
    for (vtkIdType r = 0;r < projectionTable->GetNumberOfRows(); ++r)
      {
      headerArr->InsertNextValue(QString("PC%1").arg(r + 1).toLatin1());
      }
    voUtils::insertColumnIntoTable(projectionTable.GetPointer(), 0, headerArr.GetPointer());
    }
  this->setOutput("x", new voTableDataObject("x", projectionTable.GetPointer()));
  this->setOutput("x-dynview", new voTableDataObject("x-dynview", projectionTable.GetPointer()));

  // ------------------------------------------------
  // Extract rotation table (each column is an eigenvector)
  vtkNew<vtkTable> rotationTable;
    {
    voUtils::arrayToTable(outputArrayData->GetArrayByName("pcaRot"), rotationTable.GetPointer());

    // Add column labels (components)
    for(vtkIdType c = 0; c < rotationTable->GetNumberOfColumns(); ++c)
      {
      QByteArray colName = QString("PC%1").arg(c + 1).toLatin1();
      rotationTable->GetColumn(c)->SetName(colName);
      }

    // Add row labels (analyte names)
    voUtils::insertColumnIntoTable(rotationTable.GetPointer(), 0, rowNames.GetPointer());
    }
  this->setOutput("rot", new voTableDataObject("rot", rotationTable.GetPointer()));

  // ------------------------------------------------
  // Extract standard deviation table
  vtkNew<vtkTable> stdDevTable;
    {
    voUtils::arrayToTable(outputArrayData->GetArrayByName("stddev"), stdDevTable.GetPointer());
    voUtils::transposeTable(stdDevTable.GetPointer());

    // Add column labels (components)
    for(vtkIdType c = 0; c < stdDevTable->GetNumberOfColumns(); ++c)
      {
      QByteArray colName = QString("PC%1").arg(c + 1).toLatin1();
      stdDevTable->GetColumn(c)->SetName(colName);
      }

    // Add row label
    vtkNew<vtkStringArray> headerArr;
    headerArr->InsertNextValue("Std Dev");
    voUtils::insertColumnIntoTable(stdDevTable.GetPointer(), 0, headerArr.GetPointer());
    }
  this->setOutput("sdev", new voTableDataObject("sdev", stdDevTable.GetPointer()));

  // ------------------------------------------------
  // Extract percent loading (proportion of variance) table
  vtkNew<vtkTable> pctLoadingTable;
    {
    voUtils::arrayToTable(outputArrayData->GetArrayByName("perload"), pctLoadingTable.GetPointer());

    // Add loading vector number column (will be transposed to be first row)
    vtkNew<vtkDoubleArray> cardinalArray;
    for(vtkIdType r = 0; r < pctLoadingTable->GetNumberOfRows(); ++r)
      {
      cardinalArray->InsertNextValue(static_cast<double>(r));
      }
    voUtils::insertColumnIntoTable(pctLoadingTable.GetPointer(), 0, cardinalArray.GetPointer());

    voUtils::transposeTable(pctLoadingTable.GetPointer());

    // Add column labels (components)
    for(vtkIdType c = 0; c < pctLoadingTable->GetNumberOfColumns(); ++c)
      {
      QByteArray colName = QString("PC%1").arg(c + 1).toLatin1();
      pctLoadingTable->GetColumn(c)->SetName(colName);
      }

    // Add row labels
    vtkNew<vtkStringArray> headerArr;
    headerArr->InsertNextValue("Loading Vector");
    headerArr->InsertNextValue("Percent Loading");
    voUtils::insertColumnIntoTable(pctLoadingTable.GetPointer(), 0, headerArr.GetPointer());
    }
  this->setOutput("loading", new voTableDataObject("loading", pctLoadingTable.GetPointer()));

  // ------------------------------------------------
  // Extract percent loading (proportion of variance) table
  vtkNew<vtkTable> sumLoadingTable;
    {
    voUtils::arrayToTable(outputArrayData->GetArrayByName("sumperload"), sumLoadingTable.GetPointer());

    // Add loading vector number column (will be transposed to be first row)
    vtkNew<vtkDoubleArray> cardinalArray;
    for(vtkIdType r = 0; r < sumLoadingTable->GetNumberOfRows(); ++r)
      {
      cardinalArray->InsertNextValue(static_cast<double>(r));
      }
    voUtils::insertColumnIntoTable(sumLoadingTable.GetPointer(), 0, cardinalArray.GetPointer());

    voUtils::transposeTable(sumLoadingTable.GetPointer());

    // Add column labels (components)
    for(vtkIdType c = 0; c < sumLoadingTable->GetNumberOfColumns(); ++c)
      {
      QByteArray colName = QString("PC%1").arg(c + 1).toLatin1();
      sumLoadingTable->GetColumn(c)->SetName(colName);
      }

    // Add row labels
    vtkNew<vtkStringArray> headerArr;
    headerArr->InsertNextValue("Loading Vector");
    headerArr->InsertNextValue("Cumulative Percent Loading");
    voUtils::insertColumnIntoTable(sumLoadingTable.GetPointer(), 0, headerArr.GetPointer());
    }
  this->setOutput("sumloading", new voTableDataObject("sumloading", sumLoadingTable.GetPointer()));

  return true;
}
