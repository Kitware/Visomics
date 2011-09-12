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

#ifndef __voAnalysisFactory_h
#define __voAnalysisFactory_h

// Qt includes
#include <QScopedPointer>
#include <QString>

class voAnalysisFactoryPrivate;
class voAnalysis;

class voAnalysisFactory
{

public:
  voAnalysisFactory();
  virtual ~voAnalysisFactory();

  /// Given the name of a voAnalysis subclass, return a new instance of the analysis.
  virtual voAnalysis* createAnalysis(const QString& className);

  /// Return list of registered analysis names
  QStringList registeredAnalysisNames() const;

  /// If any, return the \a analysisName associated with \a analysisPrettyName
  QString analysisNameFromPrettyName(const QString& analysisPrettyName) const;

  /// Return list of registered analysis pretty names
  QStringList registeredAnalysisPrettyNames() const;

protected:

  /// Register an analysis
  template<typename AnalysisClassType>
  void registerAnalysis(const QString& analysisPrettyName);

protected:
  QScopedPointer<voAnalysisFactoryPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voAnalysisFactory);
  Q_DISABLE_COPY(voAnalysisFactory);
};

#endif
