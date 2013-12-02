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

#ifndef __voTreeDropTipWithoutData_h
#define __voTreeDropTipWithoutData_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voAnalysis.h"

class voTreeDropTipWithoutDataPrivate;
class vtkTree;
class vtkSelection;
class vtkQStringList;
class vtkIdTypeArray;

class voTreeDropTipWithoutData : public voAnalysis
{
  Q_OBJECT
public:
  typedef voAnalysis Superclass;
  voTreeDropTipWithoutData();
  virtual ~voTreeDropTipWithoutData();

protected:
  virtual void setOutputInformation();
  virtual void setParameterInformation();
  virtual QString parameterDescription()const;

  virtual int execute();

  bool removeInternalBranch(vtkTree * tree, vtkIdTypeArray * selArray, vtkIdType parentId);
  bool getTipSelection(vtkTree * tree, vtkSelection * sel, QStringList tipNameList);
  bool getSelectionByTipNames(vtkTree * tree, vtkSelection * sel);
  bool getSelectionByPrunedTree(vtkTree *tree, vtkSelection * sel);

protected:
  QScopedPointer<voTreeDropTipWithoutDataPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voTreeDropTipWithoutData);
  Q_DISABLE_COPY(voTreeDropTipWithoutData);
};

#endif
