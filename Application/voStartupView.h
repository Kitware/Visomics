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


#ifndef __voStartupView_h
#define __voStartupView_h

// Qt includes
#include <QScopedPointer>
#include <QWidget>

class voStartupViewPrivate;

class voStartupView : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  voStartupView(QWidget * newParent = 0);
  virtual ~voStartupView();

protected:
  virtual void resizeEvent(QResizeEvent* event);

protected:
  QScopedPointer<voStartupViewPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voStartupView);
  Q_DISABLE_COPY(voStartupView);
};

#endif
