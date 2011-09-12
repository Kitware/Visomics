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


#ifndef __voKEGGTableView_h
#define __voKEGGTableView_h

// Qt includes
#include <QTableWidget>

// Visomics includes
#include "voView.h"

// VTK includes
#include <vtkSmartPointer.h>

class voKEGGTableViewPrivate;

class voKEGGTableView : public voView
{
  Q_OBJECT;
public:
  typedef voView Superclass;
  voKEGGTableView(QWidget* newParent = 0);
  virtual ~voKEGGTableView();

  virtual QString hints()const;

  virtual void setDataObject(voDataObject* dataObject);

protected:
  void setupUi(QLayout * layout);

public slots:
  void onItemDoubleClicked(QTableWidgetItem* item);

protected:
  QScopedPointer<voKEGGTableViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voKEGGTableView);
  Q_DISABLE_COPY(voKEGGTableView);
};

#endif
