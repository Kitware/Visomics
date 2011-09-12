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


#ifndef __voTableView_h
#define __voTableView_h

// Qt includes
#include <QStandardItemModel>

// Visomics includes
#include "voView.h"

// VTK includes
#include <vtkSmartPointer.h>

class voTableViewPrivate;
class QTableView;
class vtkTable;

class voTableView : public voView
{
  Q_OBJECT;
public:
  typedef voView Superclass;
  voTableView(QWidget* newParent = 0);
  virtual ~voTableView();

  virtual QList<QAction*> actions();

protected slots:
  void onExportToCsvActionTriggered();

protected:
  void setupUi(QLayout * layout);

  virtual void setDataObjectInternal(const voDataObject& dataObject);

protected:
  QScopedPointer<voTableViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voTableView);
  Q_DISABLE_COPY(voTableView);
};

#endif
