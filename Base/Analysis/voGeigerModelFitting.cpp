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
#include "voTableDataObject.h"
#include "voUtils.h"
#include "vtkExtendedTable.h"

// VTK includes
#include <vtkArrayData.h>
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
void voGeigerModelFitting::setInputInformation()
{
  this->addInputType("input1", "vtkTree");
  this->addInputType("input2", "vtkTable");
}

// --------------------------------------------------------------------------
void voGeigerModelFitting::setOutputInformation()
{
  this->addOutputType("resultTable", "vtkTable" ,
                      "voTableView", "fitted modeling parameters");

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
  Q_D(voGeigerModelFitting);


  // Import tree and assiciated traits table
  vtkTree* tree =  vtkTree::SafeDownCast(this->input("input1")->dataAsVTKDataObject());
  if (!tree)
    {
    qCritical() << "Input 1 (tree) is Null";
    return false;
    }

  vtkTable* table =  vtkTable::SafeDownCast(this->input("input2")->dataAsVTKDataObject());
  if (!table)
    {
    qCritical() << "Input 2 (table) is Null";
    return false;
    }

  // Parameters
  QString modelType = this->enumParameter("modelType");
  QString selectedDataName = this->stringParameter("selectedDataName");

  //check whether the data name exsits in the table
  bool FOUND = false;
  for (unsigned int c = 1; c < table->GetNumberOfColumns(); c++)
    {
    const char * v = table->GetColumnName(c);
    if (selectedDataName.compare(QString(v)) == 0)
      {
      FOUND = true;
      }
    }

  if (not FOUND)
    {
    qWarning() << QObject::tr("Invalid data columne name,could not find a matching data name");
    return false;
    }

  // Run R
  vtkNew <vtkRCalculatorFilter> RCalc;
  RCalc->RemoveAllPutVariables();
  RCalc->RemoveAllGetVariables();
  RCalc->SetRoutput(0);
  RCalc->AddInputData(0,table);
  RCalc->AddInputData(0,tree);
  RCalc->PutTree("tree");
  RCalc->PutTable("tableData");
  RCalc->GetTable("resultTable");


  QString R_ComposeOutputTable = "";
  
  if (modelType == QString("OU"))
    {
    R_ComposeOutputTable = "resultTable=list(parameter=\"value\",sigsq=result$sigsq,alpha=result$alpha,\" \"=\" \",lnL=result$lnL,AIC=result$aic,AICc=result$aicc)";
    }
  else if (modelType== "BM")
    {
    R_ComposeOutputTable = "resultTable=list(parameter=\"value\",sigsq=result$sigsq,\" \"=\" \",lnL=result$lnL,AIC=result$aic,AICc=result$aicc)";
    }
  else if (modelType == "EB")
    {
    R_ComposeOutputTable = "resultTable=list(parameter=\"value\",sigsq=result$sigsq,a=result$a,\" \"=\" \",lnL=result$lnL,AIC=result$aic,AICc=result$aicc)";
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
  ).arg(selectedDataName,modelType,R_ComposeOutputTable).toLatin1());


  RCalc->Update();

  vtkTable * outTable= vtkTable::SafeDownCast(RCalc->GetOutput());

  this->setOutput("resultTable", new voTableDataObject("resultTable",outTable));

  return true;
}
