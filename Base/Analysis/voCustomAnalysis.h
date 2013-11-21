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

#ifndef __voCustomAnalysis_h
#define __voCustomAnalysis_h

// Qt includes
#include <QScopedPointer>

// Visomics includes
#include "voAnalysis.h"

class voCustomAnalysisInformation;
class voCustomAnalysisPrivate;

class voCustomAnalysis : public voAnalysis
{
  Q_OBJECT
public:
  typedef voAnalysis Superclass;
  voCustomAnalysis(QObject* newParent = 0);
  virtual ~voCustomAnalysis();
  void loadInformation(voCustomAnalysisInformation *info);

protected:
  virtual void setOutputInformation();
  virtual void setParameterInformation();
  virtual QString parameterDescription()const;

  virtual bool execute();

  voCustomAnalysisInformation * information() const;

  QScopedPointer<voCustomAnalysisPrivate> d_ptr;
  QString parameterDescriptions;

private:
  Q_DECLARE_PRIVATE(voCustomAnalysis);
  Q_DISABLE_COPY(voCustomAnalysis);
};

#endif
