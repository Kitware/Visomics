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

#ifndef __voCustomAnalysisParameter_h
#define __voCustomAnalysisParameter_h

// Qt includes
#include <QObject>

// Visomics includes
#include "voCustomAnalysisParameterField.h"

class voCustomAnalysisParameter : public QObject
{
  Q_OBJECT
public:
  typedef QObject Superclass;
  voCustomAnalysisParameter(QObject* newParent = 0);
  virtual ~voCustomAnalysisParameter();

  QString name() const;
  QString type() const;
  voCustomAnalysisParameterField * field(int i) const;
  QList<voCustomAnalysisParameterField*> fields() const;

  void setName(const QString& name);
  void setType(const QString& type);
  void addField(voCustomAnalysisParameterField * field);

protected:
  QString parameterName;
  QString parameterType;
  QList<voCustomAnalysisParameterField*> fieldsList;

private:
  Q_DISABLE_COPY(voCustomAnalysisParameter);
};

#endif
