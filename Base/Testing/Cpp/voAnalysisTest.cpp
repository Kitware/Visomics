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
#include <QApplication>
#include <QStringList>

// QtPropertyBrowser includes
#include <QtVariantPropertyManager>

// Visomics includes
#include "voAnalysis.h"
#include "voDataObject.h"

// VTK includes
#include <vtkArrayData.h>
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkTable.h>

// STD includes
#include <cstdlib>
#include <iostream>

namespace
{
class voCustomAnalysis : public voAnalysis
{
public:
  voCustomAnalysis():voAnalysis(){}
  virtual ~voCustomAnalysis(){}

  virtual bool execute()
    {
    if (this->numberOfInput() == 0)
      {
      //std::cerr << "There are no inputs." << std::endl;
      return false;
      }
    if (!this->input())
      {
      //std::cerr << "There is no Data associated with the input." << std::endl;
      return false;
      }
    vtkTable* table =  vtkTable::SafeDownCast(this->input()->dataAsVTKDataObject());
    if (!table)
      {
      std::cerr << "Input is Null" << std::endl;
      return false;
      }

    Q_ASSERT(table->GetNumberOfColumns() == 1);

    vtkIntArray * inputArray = vtkIntArray::SafeDownCast(table->GetColumn(0));
    Q_ASSERT(inputArray);

    vtkNew<vtkTable> outputTable;
    vtkNew<vtkIntArray> outputArray;
    outputArray->SetNumberOfValues(1);
    outputArray->SetValue(0, inputArray->GetValue(0) + inputArray->GetValue(1));
    outputTable->AddColumn(outputArray.GetPointer());
    this->setOutput("output", new voDataObject("output", outputTable.GetPointer()));

    return true;
    }

  virtual void setInputInformation()
    {
    this->addInputType("input", "vtkTable");
    }

  virtual void setOutputInformation()
    {
    this->addOutputType("output", "vtkTable", "voTreeGraphView", "This is a graph view",
                        "voTableView", "This is a raw view");
    }

  virtual void setParameterInformation()
    {
    QList<QtProperty*> parameters;

    parameters << this->addIntegerParameter("centers", QObject::tr("Number of clusters"), 2, 10, 4);
    parameters << this->addStringParameter("host", QObject::tr("Host name"), "paraviewweb.kitware.com:88");
    this->addBooleanParameter("transpose", QObject::tr("Transpose"), false);
    this->addDoubleParameter("ratio", QObject::tr("Ratio"), 0.0, 5.0, 0.5);
    QStringList algorithms;
    algorithms << "Hartigan-Wong" << "Lloyd" << "Forgy" << "MacQueen";
    parameters << this->addEnumParameter("algorithm", "Algorithm", algorithms);
    }
};

} // end of anonymous namespace

//-----------------------------------------------------------------------------
int voAnalysisTest(int argc, char * argv [])
{
  QApplication app(argc, argv);

  voCustomAnalysis analysis;
  if (analysis.uuid().isEmpty())
    {
    std::cerr << "Line " << __LINE__ << " - Problem with uuid() !" << std::endl;
    return EXIT_FAILURE;
    }

  //-----------------------------------------------------------------------------
  // Input
  //-----------------------------------------------------------------------------

  if (analysis.numberOfInput() != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with numberOfInput() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.inputNames().size() != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with inputNames() !" << std::endl;
    return EXIT_FAILURE;
    }

  analysis.addInputType("", "vtkTable");

  if (analysis.numberOfInput() != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with numberOfInput() !" << std::endl;
    return EXIT_FAILURE;
    }

  analysis.addInputType("input", "");

  if (analysis.numberOfInput() != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with numberOfInput() !" << std::endl;
    return EXIT_FAILURE;
    }

  analysis.addInputType("input", "vtkTable");

  if (analysis.numberOfInput() != 1)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with numberOfInput() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.inputNames().size() != 1)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with inputNames() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.inputNames().at(0) != "input")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with inputNames() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (!analysis.hasInput("input"))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with hasInput() !" << std::endl;
    return EXIT_FAILURE;
    }

  analysis.addInput("input", 0);
  if (analysis.input("input") != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with addInput() / input() !" << std::endl;
    return EXIT_FAILURE;
    }

  vtkNew<vtkTable> inputTable;
  voDataObject * inputDataObject = new voDataObject("input", inputTable.GetPointer());
  analysis.addInput("input", inputDataObject);
  if (analysis.input("input") != inputDataObject)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with addInput() / input() !" << std::endl;
    return EXIT_FAILURE;
    }

  analysis.removeAllInputs();

  if (analysis.numberOfInput() != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with numberOfInput() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.inputNames().size() != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with inputNames() !" << std::endl;
    return EXIT_FAILURE;
    }

  //-----------------------------------------------------------------------------
  // Output
  //-----------------------------------------------------------------------------
  if (analysis.numberOfOutput() != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with numberOfOutput() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.outputNames().size() != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with outputNames() !" << std::endl;
    return EXIT_FAILURE;
    }

  analysis.addOutputType("", "vtkTable", "voTableView", "");

  if (analysis.numberOfOutput() != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with numberOfOutput() !" << std::endl;
    return EXIT_FAILURE;
    }

  analysis.addOutputType("output", "", "voTableView", "");

  if (analysis.numberOfOutput() != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with numberOfOutput() !" << std::endl;
    return EXIT_FAILURE;
    }

  analysis.addOutputType("output", "vtkTable", "", "");

  if (analysis.numberOfOutput() != 1)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with numberOfOutput() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.viewTypesForOutput("output").size() != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with viewTypesForOutput() !" << std::endl;
    return EXIT_FAILURE;
    }

  analysis.removeAllOutputs();

  if (analysis.numberOfOutput() != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with numberOfOutput() !" << std::endl;
    return EXIT_FAILURE;
    }

  analysis.addOutputType("output", "vtkTable", "voTreeGraphView", "");

  if (analysis.numberOfOutput() != 1)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with numberOfOutput() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.outputNames().size() != 1)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with outputNames() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.outputNames().at(0) != "output")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with outputNames() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (!analysis.hasOutput("output"))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with hasOutput() !" << std::endl;
    return EXIT_FAILURE;
    }

  analysis.setOutput("output", 0);
  if (analysis.output("output") != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with setOutput / output() !" << std::endl;
    return EXIT_FAILURE;
    }

  vtkNew<vtkTable> outputTable;
  voDataObject * outputDataObject = new voDataObject("output", outputTable.GetPointer());
  analysis.setOutput("output", outputDataObject);
  if (analysis.output("output") != outputDataObject)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with setOutput / output() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.hasOutputWithViewType("", "voTreeGraphView"))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with hasOutputWithRawViewType() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.hasOutputWithViewType("output", ""))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with hasOutputWithViewType() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (!analysis.hasOutputWithViewType("output", "voTreeGraphView"))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with hasOutputWithViewType() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.viewTypesForOutput("output").size() != 1)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with viewTypesForOutput() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.viewTypesForOutput("output").at(0) != "voTreeGraphView")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with viewTypesForOutput() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.viewPrettyName("", "voTreeGraphView") != "")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with viewPrettyName() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.viewPrettyName("output", "") != "")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with viewPrettyName() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.viewPrettyName("output", "voTreeGraphView") != "")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with viewPrettyName() !" << std::endl;
    return EXIT_FAILURE;
    }

  analysis.removeAllOutputs();

  if (analysis.numberOfOutput() != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with numberOfOutput() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.outputNames().size() != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with outputNames() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.hasOutputWithViewType("output", "voTreeGraphView"))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with hasOutputWithViewType() !" << std::endl;
    return EXIT_FAILURE;
    }

  analysis.addOutputType("output", "vtkTable", "voTreeGraphView", "This is a graph view");

  if (analysis.numberOfOutput() != 1)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with numberOfOutput() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.outputNames().size() != 1)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with outputNames() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.outputNames().at(0) != "output")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with outputNames() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.viewPrettyName("output", "voTreeGraphView") != "This is a graph view")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with viewPrettyName() !" << std::endl;
    return EXIT_FAILURE;
    }

  analysis.addOutputType("output2", "vtkTable", "voTreeGraphView", "This is a graph view",
                         "voTableView", "");

  if (analysis.numberOfOutput() != 2)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with numberOfOutput() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.outputNames().size() != 2)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with outputNames() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.rawViewPrettyName("", "voTableView") != "")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with rawViewPrettyName() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.rawViewPrettyName("output2", "") != "")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with rawViewPrettyName() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.rawViewPrettyName("output2", "voTableView") != "")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with rawViewPrettyName() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (!analysis.hasOutputWithRawViewType("output2", "voTableView"))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with hasOutputWithRawViewType() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.rawViewTypeForOutput("output2") != "voTableView")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with rawViewTypeForOutput() !" << std::endl;
    return EXIT_FAILURE;
    }

  analysis.removeAllOutputs();

  if (analysis.numberOfOutput() != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with numberOfOutput() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.outputNames().size() != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with outputNames() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.hasOutputWithRawViewType("output", "voTableView"))
    {
    std::cerr << "Line " << __LINE__ << " - Problem with hasOutputWithViewType() !" << std::endl;
    return EXIT_FAILURE;
    }

  analysis.addOutputType("output3", "vtkTable", "voTreeGraphView", "This is a graph view",
                         "voTableView", "This is a raw view");

  if (analysis.rawViewPrettyName("output3", "voTableView") != "This is a raw view")
    {
    std::cerr << "Line " << __LINE__ << " - Problem with rawViewPrettyName() !" << std::endl;
    return EXIT_FAILURE;
    }

  analysis.removeAllOutputs();

  //-----------------------------------------------------------------------------
  // Parameters
  //-----------------------------------------------------------------------------

  if (!analysis.propertyManager())
    {
    std::cerr << "Line " << __LINE__ << " - Problem with propertyManager() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.acceptDefaultParameterValues())
    {
    std::cerr << "Line " << __LINE__ << " - Problem with acceptDefaultParameterValues() !" << std::endl;
    return EXIT_FAILURE;
    }

  analysis.setAcceptDefaultParameterValues(true);

  if (!analysis.acceptDefaultParameterValues())
    {
    std::cerr << "Line " << __LINE__ << " - Problem with acceptDefaultParameterValues() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.parameterCount() != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with parameterCount() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.topLevelParameterGroups().size() != 0)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with topLevelParameterGroups() !" << std::endl;
    return EXIT_FAILURE;
    }

  analysis.initializeParameterInformation();

  if (analysis.parameterCount() != 5)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with parameterCount() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.topLevelParameterGroups().size() != 5)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with topLevelParameterGroups() !" << std::endl;
    return EXIT_FAILURE;
    }

  //-----------------------------------------------------------------------------
  // Execute
  //-----------------------------------------------------------------------------

  analysis.removeAllInputs();
  analysis.removeAllOutputs();

  if (analysis.abortExecution())
    {
    std::cerr << "Line " << __LINE__ << " - Problem with abortExecution() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.run())
    {
    // Since input / output information are not set, analysis shouldn't run !
    std::cerr << "Line " << __LINE__ << " - Problem with run() !" << std::endl;
    return EXIT_FAILURE;
    }

  analysis.initializeInputInformation();
  analysis.initializeOutputInformation();

  if (analysis.numberOfInput() != 1)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with numberOfInput() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.numberOfOutput() != 1)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with numberOfOutput() !" << std::endl;
    return EXIT_FAILURE;
    }

  if (analysis.run())
    {
    // Since no data has been associated with the input, analysis is expected to fail.
    std::cerr << "Line " << __LINE__ << " - Problem with run() !" << std::endl;
    return EXIT_FAILURE;
    }

  vtkNew<vtkArrayData> arrayData;
  analysis.addInput("input", new voDataObject("input", arrayData.GetPointer()));

  if (analysis.run())
    {
    // Since wrong type of data has been associated with the input, analysis is expected to fail.
    std::cerr << "Line " << __LINE__ << " - Problem with run() !" << std::endl;
    return EXIT_FAILURE;
    }

  vtkNew<vtkTable> table;
  vtkIntArray * intColumn = vtkIntArray::New();
  intColumn->SetNumberOfValues(2);
  intColumn->SetValue(0, 1);
  intColumn->SetValue(1, 2);
  table->AddColumn(intColumn);
  analysis.addInput("input", new voDataObject("input", table.GetPointer()));

  if (!analysis.run())
    {
    std::cerr << "Line " << __LINE__ << " - Problem with run() !" << std::endl;
    return EXIT_FAILURE;
    }

  vtkTable * outputTable2 = vtkTable::SafeDownCast(analysis.output("output")->dataAsVTKDataObject());
  if (!outputTable2 || outputTable2->GetNumberOfColumns() != 1)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with output() !" << std::endl;
    return EXIT_FAILURE;
    }

  vtkIntArray * outputArray = vtkIntArray::SafeDownCast(outputTable2->GetColumn(0));
  if (!outputArray || outputArray->GetValue(0) != 3)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with output() !" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
