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

#ifndef __voViewFactory_h
#define __voViewFactory_h

// Qt includes
#include <QScopedPointer>
#include <QString>

class voViewFactoryPrivate;
class voView;

class voViewFactory
{

public:
  voViewFactory();
  virtual ~voViewFactory();

  /// Given the name of a voView subclass, return a new instance of the view.
  virtual voView* createView(const QString& className);

  /// Return list of registered view names
  QStringList registeredViewNames() const;

protected:

  /// Register an a view
  template<typename ClassType>
  void registerView(const QString& viewName = QString());

protected:
  QScopedPointer<voViewFactoryPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voViewFactory);
  Q_DISABLE_COPY(voViewFactory);
};

#endif
