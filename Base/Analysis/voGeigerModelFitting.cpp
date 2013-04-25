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

// QtPropertyBrowser includes
#include <QtVariantPropertyManager>

// Visomics includes
#include "voGeigerModelFitting.h"
#include "voInputFileDataObject.h"
#include "voTableDataObject.h"
#include "voUtils.h"
#include "vtkExtendedTable.h"

// VTK includes
#include <vtkArrayData.h>
#include <vtkCompositeDataIterator.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkNew.h>
#include <vtkRCalculatorFilter.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkTree.h>

// --------------------------------------------------------------------------
// voGeigerModelFittingPrivate methods

// --------------------------------------------------------------------------
class voGeigerModelFittingPrivate
{
public:
  vtkSmartPointer<vtkRCalculatorFilter> RCalc;
};

// --------------------------------------------------------------------------
// voGeigerModelFitting methods

// --------------------------------------------------------------------------
voGeigerModelFitting::voGeigerModelFitting():
    Superclass(), d_ptr(new voGeigerModelFittingPrivate)
{
  Q_D(voGeigerModelFitting);

  d->RCalc = vtkSmartPointer<vtkRCalculatorFilter>::New();
}


// --------------------------------------------------------------------------
voGeigerModelFitting::~voGeigerModelFitting()
{
}


// --------------------------------------------------------------------------
void voGeigerModelFitting::setOutputInformation()
{
  this->addOutputType("resultTable", "vtkTable" ,
                      "", "",
                      "voTableView", "fitted modeling parameters");
  this->addOutputType("resultTree", "vtkTree",
                      "", "",
                      "voTreeHeatmapView", "fitted tree");
}

// --------------------------------------------------------------------------
void voGeigerModelFitting::setParameterInformation()
{
  QList<QtProperty*> GeigerModel_parameters;

  QStringList modeling_methods;

  modeling_methods << "OU" << "BM" << "EB";
  GeigerModel_parameters << this->addEnumParameter("modelType", tr("Model Type"), modeling_methods, "OU");

  GeigerModel_parameters<< this->addStringParameter("selectedDataName", QObject::tr("Data Column Name"), "awesomeness");

  this->addParameterGroup("GeigerModel parameters", GeigerModel_parameters);
}

// --------------------------------------------------------------------------
QString voGeigerModelFitting::parameterDescription()const
{
  return QString("<dl>"
                 "<dt><b>Model Type</b>:</dt>"
                 "<dd>Geiger Tree Model Types.<br>"
                 "<u>Choose from the three existing models</u>,OU(Ornstein-Uhlenbeck), BM(Brownian Motion), and EB(Early Burst) </dd>"
                 "<dt><b>Data Column Name</b>:</dt>"
                 "<dd>Choose a data column to be used for the modeling fitting.<br>"
                 "<u>Choose from the table data attached with the tree. </dd>"
                 "</dl>");
}

// --------------------------------------------------------------------------
bool voGeigerModelFitting::execute()
{
  // Import tree and assiciated traits table
  vtkTree* tree =  vtkTree::SafeDownCast(this->input(0)->dataAsVTKDataObject());
  if (!tree)
    {
    qCritical() << "Input 1 (tree) is Null";
    return false;
    }

  vtkExtendedTable* extendedTable =
    vtkExtendedTable::SafeDownCast(this->input(1)->dataAsVTKDataObject());
  if (!extendedTable)
    {
    qCritical() << "Input 2 (extended table) is Null";
    return false;
    }

  vtkTable* table = extendedTable->GetInputData();
  if (!table)
    {
    qCritical() << "Input 2 (table) is Null";
    return false;
    }

  // Parameters
  QString modelType = this->enumParameter("modelType");
  QString selectedDataName = this->stringParameter("selectedDataName");

  //check whether the data name exists in the table
  bool FOUND = false;
  for (int c = 1; c < table->GetNumberOfColumns(); c++)
    {
    const char * v = table->GetColumnName(c);
    if (selectedDataName.compare(QString(v)) == 0)
      {
      FOUND = true;
      }
    }

  if (!FOUND)
    {
    qCritical() << QObject::tr("Invalid data column name,could not find a matching data name");
    return false;
    }

  // Run R
  vtkNew <vtkRCalculatorFilter> RCalc;
  RCalc->RemoveAllPutVariables();
  RCalc->RemoveAllGetVariables();
  RCalc->SetRoutput(0);

  vtkNew<vtkMultiBlockDataSet> composite;
  composite->SetNumberOfBlocks(2);
  composite->SetBlock(0, table);
  composite->SetBlock(1, tree);
  RCalc->AddInputData(0, composite.GetPointer());

  RCalc->PutTree("tree");
  RCalc->PutTable("tableData");
  RCalc->GetTable("resultTable");
  RCalc->GetTree("resultTree");


  QString R_ComposeOutputTable = "";
  QString R_ComposeOutputTree = "";

  if (modelType == QString("OU"))
    {
    R_ComposeOutputTable = "resultTable=list(parameter=\"value\",z0=result$z0,sigsq=result$sigsq,alpha=result$alpha,\" \"=\" \",lnL=result$lnL,AIC=result$aic,AICc=result$aicc)";
    R_ComposeOutputTree = "resultTree<-transform(tree, \"OU\", o$opt$alpha)";
    }
  else if (modelType== "BM")
    {
    R_ComposeOutputTable = "resultTable=list(parameter=\"value\",z0=result$z0,sigsq=result$sigsq,\" \"=\" \",lnL=result$lnL,AIC=result$aic,AICc=result$aicc)";
    R_ComposeOutputTree = "resultTree<-tree";
    //R_ComposeOutputTree = "resultTree<-transform(tree, \"BM\", o$opt$alpha)";
    }
  else if (modelType == "EB")
    {
    R_ComposeOutputTable = "resultTable=list(parameter=\"value\",z0=result$z0,sigsq=result$sigsq,a=result$a,\" \"=\" \",lnL=result$lnL,AIC=result$aic,AICc=result$aicc)";
    R_ComposeOutputTree = "resultTree<-transform(tree, \"EB\", o$opt$a)";
    }
  else
   {
    return false;
   }

  RCalc->SetRscript(QString(
      "library(geiger)\n"
      "data<-as.numeric(tableData$\"%1\")\n"
      "names(data)<-tableData[[1]] \n" // the first element of the list(tableData) is the name
      "o<-fitContinuous(tree, data, model=\"%2\",SE=0)\n"
      "result=o$opt\n" //o$opt is a list of output parameters
      "%3\n"
      "%4\n"
  ).arg(selectedDataName,modelType,R_ComposeOutputTable, R_ComposeOutputTree).toLatin1());

  RCalc->Update();

  vtkMultiBlockDataSet * outData = vtkMultiBlockDataSet::SafeDownCast(RCalc->GetOutput());
  if(!outData)
    {
    qCritical() << QObject::tr("Fatal error in %1 R script").arg(this->objectName());
    qCritical() << QObject::tr("Please make sure your geiger version is at least 1.4-4");
    return false;
    }

  vtkTable *outTable = vtkTable::SafeDownCast(outData->GetBlock(0));
  if (!outTable)
    {
    qCritical() << QObject::tr("Error generating output table.");
    return false;
    }

  vtkTree *outTree = vtkTree::SafeDownCast(outData->GetBlock(1));
  if (!outTree)
    {
    qCritical() << QObject::tr("Error generated output tree.");
    return false;
    }

  this->setOutput("resultTable", new voTableDataObject("resultTable",outTable));
  this->setOutput("resultTree",
    new voInputFileDataObject("resultTree", outTree));

  return true;
}
