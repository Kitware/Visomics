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
#ifndef __voAnalysisDriver_h
#define __voAnalysisDriver_h

// Qt includes
#include <QScopedPointer>
#include <QObject>
#include <QHash>

class voAnalysis;
class voDataModelItem;
class voDataObject;
class voAnalysisDriverPrivate;

class voAnalysisDriver : public QObject
{
  Q_OBJECT
public:
  typedef QObject Superclass;
  voAnalysisDriver(QObject* newParent = 0);
  virtual ~voAnalysisDriver();

  void runAnalysis(const QString& analysisName, voDataModelItem* inputTarget, bool acceptDefaultParameter = false);

signals:
  void aboutToRunAnalysis(voAnalysis*);
  void analysisAddedToObjectModel(voAnalysis*);

public slots:
  void runAnalysisForAllInputs(const QString& analysisName, bool acceptDefaultParameter = false);

  void runAnalysisForCurrentInput(
    const QString& analysisName, const QHash<QString, QVariant>& parameters);

  void updateAnalysis(
    voAnalysis * analysis, const QHash<QString, QVariant>& parameters);

protected slots:

  void onAnalysisOutputSet(const QString& outputName, voDataObject* dataObject, voAnalysis* analysis);

protected:
  void runAnalysis(voAnalysis * analysis, voDataModelItem* inputTarget);

  static void addAnalysisToObjectModel(voAnalysis * analysis, voDataModelItem* insertLocation);

protected:
  QScopedPointer<voAnalysisDriverPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voAnalysisDriver);
  Q_DISABLE_COPY(voAnalysisDriver);
};

#endif
