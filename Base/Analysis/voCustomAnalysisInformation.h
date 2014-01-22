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

#ifndef __voCustomAnalysisInformation_h
#define __voCustomAnalysisInformation_h

// Qt includes
#include <QObject>

// Visomics includes
#include "voCustomAnalysisData.h"
#include "voCustomAnalysisParameter.h"

class voCustomAnalysisInformation : public QObject
{
  Q_OBJECT
public:
  typedef QObject Superclass;
  voCustomAnalysisInformation(QObject* newParent = 0);
  virtual ~voCustomAnalysisInformation();

  QString name() const;
  QString script() const;
  QString scriptType() const;
  voCustomAnalysisData * input(int i) const;
  QList<voCustomAnalysisData *> inputs() const;
  voCustomAnalysisData * output(int i) const;
  QList<voCustomAnalysisData *> outputs() const;
  voCustomAnalysisParameter * parameter(int i) const;
  QList<voCustomAnalysisParameter *> parameters() const;

  void setName(const QString& name);
  void setScript(const QString& script);
  void setScriptType(const QString& scriptType);
  void addInput(voCustomAnalysisData * input);
  void addOutput(voCustomAnalysisData * input);
  void addParameter(voCustomAnalysisParameter * parameter);

protected:
  QString analysisName;
  QString analysisScript;
  QString analysisScriptType;
  QList<voCustomAnalysisData *> inputsList;
  QList<voCustomAnalysisData *> outputsList;
  QList<voCustomAnalysisParameter *> parametersList;

private:
  Q_DISABLE_COPY(voCustomAnalysisInformation);
};

#endif
