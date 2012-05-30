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
#ifndef __voApplication_h
#define __voApplication_h

// Qt includes
#include <QApplication>
#include <QScopedPointer>

class ctkErrorLogModel;
class QMainWindow;
class voAnalysisDriver;
class voAnalysisFactory;
class voAnalysisViewFactory;
class voApplicationPrivate;
class voDataModel;
class voIOManager;
class voRegistry;
class voViewFactory;
class voViewManager;

class voApplication : public QApplication
{
  Q_OBJECT
public:
  typedef QApplication Superclass;
  voApplication(int & argc, char ** argv);
  virtual ~voApplication();

  /// Return a reference to the application singleton
  static voApplication* application();

  /// Initialize application
  /// If exitWhenDone is True, it's your responsability to exit the application
  void initialize(bool& exitWhenDone);

  /// Return true if the application has been initialized
  /// \note initialize() should be called only one time.
  bool initialized() const;

  /// Return true is this instance of the application is running from an installed directory
  bool isInstalled()const;

  /// Get application home directory
  QString homeDirectory()const;

  QString rHome()const;

  voDataModel* dataModel()const;

  /// Get errorLogModel
  ctkErrorLogModel* errorLogModel()const;

  voAnalysisDriver* analysisDriver()const;

  voIOManager* ioManager()const;

  voViewManager* viewManager()const;

  voAnalysisFactory* analysisFactory()const;

  voRegistry* normalizerRegistry()const;

  voViewFactory* viewFactory()const;

  QMainWindow* mainWindow()const;

protected:
  QScopedPointer<voApplicationPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voApplication);
  Q_DISABLE_COPY(voApplication);

};

#endif
