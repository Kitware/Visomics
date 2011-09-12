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
#include <QAction>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QSharedDataPointer>

// Visomics includes
#include "voUtils.h"
#include "voView.h"

// VTK includes
#include <vtkDataObject.h>

//----------------------------------------------------------------------------
class voViewPrivate
{
public:
  voDataObject * DataObject;
};

// --------------------------------------------------------------------------
// voViewPrivate methods

// --------------------------------------------------------------------------
// voView methods

// --------------------------------------------------------------------------
voView::voView(QWidget* newParent) : Superclass(newParent), d_ptr(new voViewPrivate)
{
}

// --------------------------------------------------------------------------
voView::~voView()
{
}

// --------------------------------------------------------------------------
void voView::initialize()
{
  QVBoxLayout * verticalLayout = new QVBoxLayout(this);
  verticalLayout->setSpacing(0);
  verticalLayout->setContentsMargins(0, 0, 0, 0);
  this->setupUi(verticalLayout);
  QLabel * hintsLabel = new QLabel(this->hints());
  hintsLabel->setWordWrap(true);
  hintsLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
  verticalLayout->addWidget(hintsLabel);
}

// --------------------------------------------------------------------------
QString voView::hints()const
{
  return QString();
}

// --------------------------------------------------------------------------
voDataObject* voView::dataObject()const
{
  Q_D(const voView);
  return d->DataObject;
}

// --------------------------------------------------------------------------
void voView::setDataObject(voDataObject* dataObject)
{
  Q_D(voView);
  if (!dataObject)
    {
    qCritical() << qPrintable(this->objectName())
                << "- Failed to setDataObject - dataObject is NULL";
    return;
    }
  d->DataObject = dataObject;
  this->setDataObjectInternal(*dataObject);
}

// --------------------------------------------------------------------------
QList<QAction*> voView::actions()
{
  QList<QAction*> actionList;

  QAction * saveScreenshotAction =
      new QAction(QIcon(":/Icons/saveScreenshot.png"), "Save screenshot", this);
  saveScreenshotAction->setToolTip("Save the current view as PNG image.");
  connect(saveScreenshotAction, SIGNAL(triggered()), this, SLOT(onSaveScreenshotActionTriggered()));
  actionList << saveScreenshotAction;

  return actionList;
}

// --------------------------------------------------------------------------
void voView::onSaveScreenshotActionTriggered()
{
  QString defaultFileName = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation)
      + "/" + voUtils::cleanString(this->objectName()) + ".png";

  QString fileName = QFileDialog::getSaveFileName(
        0, tr("Save screenshot..."), defaultFileName, "Image (*.png)");
  if(fileName.isEmpty())
    {
    return;
    }
  this->saveScreenshot(fileName);
}

// --------------------------------------------------------------------------
void voView::saveScreenshot(const QString& fileName)
{
  this->saveScreenshot(fileName, this->mainWidget()->size());
}

// --------------------------------------------------------------------------
void voView::saveScreenshot(const QString& fileName, const QSize& size)
{
  QSize savedSize = this->mainWidget()->size();
  if (size != savedSize)
    {
    this->mainWidget()->resize(size);
    }

  QString tmpFileName = fileName;
  if (!tmpFileName.endsWith(".jpg", Qt::CaseInsensitive)
      && !tmpFileName.endsWith(".png", Qt::CaseInsensitive))
    {
    tmpFileName.append( ".png" );
    }

  QPixmap::grabWidget(this->mainWidget()).save(tmpFileName);

  if (size != savedSize)
    {
    this->mainWidget()->resize(savedSize);
    }
}

// --------------------------------------------------------------------------
QWidget* voView::mainWidget()
{
  QWidget* mainWidget = this->layout()->itemAt(0)->widget();
  Q_ASSERT(mainWidget);
  return mainWidget;
}
