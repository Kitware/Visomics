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


// Qt includes
#include <QScopedPointer>
#include <QMainWindow>
#include <QMap>
#include <QList>

#include "voConfigure.h" // For Visomics_INSTALL_DATA_DIR, Visomics_VERSION, Visomics_BUILD_TESTING, USE_ARBOR_BRAND

class QDockWidget;
class QSplitter;
class QTreeWidget;
class QTableWidget;
class QUrl;

class voAnalysisPanel;
class voAnalysis;
class voDataModelItem;
class voView;

class voMainWindowPrivate;

class voMainWindow : public QMainWindow
{
  Q_OBJECT
public:
  typedef QMainWindow Superclass;
  voMainWindow(QWidget * newParent = 0);
  virtual ~voMainWindow();

public slots:
  void onFileOpenActionTriggered();
  void onCloseActionTriggered();
  void onViewErrorLogActionTriggered();
  void onFileSaveWorkflowActionTriggered();
  void onFileLoadWorkflowActionTriggered();
#ifdef Visomics_BUILD_TESTING
  void playTest(QString filename);
#endif
  void onFileOpenTreeOfLifeActionTriggered();
  void onFileMakeTreeHeatmapActionTriggered();

  void about();

  void loadSampleTable();
  void loadSampleTreeHeatmap();
  void loadAnalysisScripts();
  void onCustomAnalysisAdded(const QString& analysisName);
  void provideRemoteAnalysisUrl(QUrl *url);
  void onRemoteAnalysisSettingTriggered();

signals:
  void remoteAnalysisUrlUpdated(QUrl *url);

protected slots:
  void onAnalysisSelected(voAnalysis* analysis);
  void onActiveAnalysisChanged(voAnalysis* analysis);
  void onInputSelected(QList<voDataModelItem*> inputTargets);
  void onAboutToRunAnalysis(voAnalysis* analysis);

  void setViewActions(const QString& objectUuid, voView* newView);
  void makeTreeHeatmap();
  void makeTreeHeatmapDialogClosed();

protected:
  QScopedPointer<voMainWindowPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voMainWindow);
  Q_DISABLE_COPY(voMainWindow);
};
