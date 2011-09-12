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


#ifndef __voDynView_h
#define __voDynView_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voView.h"

class voDataObject;
class voDynViewPrivate;

class voDynView : public voView
{
  Q_OBJECT
  Q_PROPERTY(QString htmlFilePath READ htmlFilePath)
public:
  typedef voView Superclass;
  voDynView(QWidget* newParent = 0);
  virtual ~voDynView();

  QString htmlFilePath()const;

  virtual void initialize();

  virtual void setDataObject(voDataObject* dataObject);

  QString viewName()const;

public slots:
  void loadDataObject();

protected:
  virtual void setupUi(QLayout * layout);

  virtual QString stringify(const voDataObject& dataObject);

protected:
  QScopedPointer<voDynViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voDynView);
  Q_DISABLE_COPY(voDynView);
};

#endif
