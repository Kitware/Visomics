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

#ifndef __voAnalysisParameterEditorWidget_h
#define __voAnalysisParameterEditorWidget_h

// Qt includes
#include <QWidget>

class QtProperty;
class voAnalysis;
class voAnalysisParameterEditorWidgetPrivate;

class voAnalysisParameterEditorWidget : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  voAnalysisParameterEditorWidget(QWidget* newParent = 0);
  virtual ~voAnalysisParameterEditorWidget();

public slots:
  void setAnalysis(voAnalysis* newAnalysis);

signals:
  void runAnalysisRequested(const QString& analysisName, const QHash<QString, QVariant>& parameters);
  void updateAnalysisRequested(voAnalysis* analysis, const QHash<QString, QVariant>& parameters);

protected slots:

  void reset();
  void updateAnalysis();
  void cloneAnalysis();
  void onLocalValueChanged(QtProperty *localProp, const QVariant &localValue);

protected:
  QScopedPointer<voAnalysisParameterEditorWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voAnalysisParameterEditorWidget);
  Q_DISABLE_COPY(voAnalysisParameterEditorWidget);
};

#endif
