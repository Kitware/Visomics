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
#include <QLayout>
#include <QResizeEvent>

// Visomics includes
#include "voDataObject.h"
#include "voTreeHeatmapView.h"
#include "voUtils.h"
#include "vtkExtendedTable.h"


// VTK includes
#include <QVTKWidget.h>
#include <vtkContextMouseEvent.h>
#include <vtkContextScene.h>
#include <vtkContextTransform.h>
#include <vtkContextView.h>
#include <vtkDataSetAttributes.h>
#include <vtkDendrogramItem.h>
#include <vtkGL2PSExporter.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkTransform2D.h>
#include <vtkTree.h>
#include <vtkTreeHeatmapItem.h>

// --------------------------------------------------------------------------
class voTreeHeatmapViewPrivate
{
public:
  voTreeHeatmapViewPrivate();
  vtkSmartPointer<vtkContextView>      ContextView;
  vtkSmartPointer<vtkTreeHeatmapItem>  TreeItem;
  vtkSmartPointer<vtkContextTransform> TransformItem;
  QVTKWidget*                          Widget;
  bool                                 DataAlreadyCentered;
};

// --------------------------------------------------------------------------
// voTreeHeatmapViewPrivate methods

// --------------------------------------------------------------------------
voTreeHeatmapViewPrivate::voTreeHeatmapViewPrivate()
{
  this->Widget = 0;
  this->DataAlreadyCentered = false;
}

// --------------------------------------------------------------------------
// voTreeHeatmapView methods

// --------------------------------------------------------------------------
voTreeHeatmapView::voTreeHeatmapView(QWidget * newParent):
    Superclass(newParent), d_ptr(new voTreeHeatmapViewPrivate)
{
}

// --------------------------------------------------------------------------
voTreeHeatmapView::~voTreeHeatmapView()
{
}

// --------------------------------------------------------------------------
vtkTreeHeatmapItem * voTreeHeatmapView::getTreeHeatmapItem() const
{
  Q_D(const voTreeHeatmapView);
  return d->TreeItem;
}
// --------------------------------------------------------------------------
void voTreeHeatmapView::setupUi(QLayout *layout)
{
  Q_D(voTreeHeatmapView);

  d->ContextView = vtkSmartPointer<vtkContextView>::New();
  d->Widget = new QVTKWidget();
  d->ContextView->SetInteractor(d->Widget->GetInteractor());
  d->Widget->SetRenderWindow(d->ContextView->GetRenderWindow());
  d->ContextView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);

  d->TreeItem = vtkSmartPointer<vtkTreeHeatmapItem>::New();
  d->TransformItem = vtkSmartPointer<vtkContextTransform>::New();
  d->TransformItem->AddItem(d->TreeItem);
  d->TransformItem->SetInteractive(true);
  d->ContextView->GetScene()->AddItem(d->TransformItem);

  layout->addWidget(d->Widget);
}

// --------------------------------------------------------------------------
void voTreeHeatmapView::setDataObjectListInternal(const QList<voDataObject*> dataObjects)
{
  Q_D(voTreeHeatmapView);

  if (dataObjects.size() == 2)
    {
    vtkTree * Tree = vtkTree::SafeDownCast(dataObjects[0]->dataAsVTKDataObject());
    if (!Tree)
      {
      qCritical() << "voTreeHeatmapView - Failed to setDataObject - vtkTree data is expected !";
      return;
      }

    vtkExtendedTable * Table = vtkExtendedTable::SafeDownCast(dataObjects[1]->dataAsVTKDataObject());
    if (!Table)
      {
      qCritical() << "voTreeHeatmapView - Failed to setDataObject - vtkExtendedTable data is expected !";
      return;
      }
    //If there is a real change in the tree (# of vertices), update the tree, otherwise, just leave it there,
    //so that expanding/collapsing interactions results could be kept on the view.
    if (d->TreeItem->GetTree()->GetNumberOfVertices() != Tree->GetNumberOfVertices())
      {
      // making a copy of the Tree so that the TreeHeatmap & Dendrogram no longer
      // share a single input data source
      vtkSmartPointer<vtkTree> treeCopy = vtkSmartPointer<vtkTree>::New();
      treeCopy->DeepCopy(Tree);
      d->TreeItem->SetTree(treeCopy);
      }
    if (d->TreeItem->GetTable() != Table->GetInputData())
      {
      d->TreeItem->SetTable(Table->GetInputData());
      }
    }
  else
    {
    // one of our dataObjects must have been deleted...
    // first check if we have ANY data left
    if (dataObjects.size() == 0)
      {
      return;
      }

    vtkTree * Tree = vtkTree::SafeDownCast(dataObjects[0]->dataAsVTKDataObject());
    if (Tree)
      {
      // do we have a tree?
      if (d->TreeItem->GetTree() != Tree)
        {
        // is it different from the one we're already drawing?
        d->TreeItem->SetTree(Tree);
        }
      d->TreeItem->SetTable(NULL);
      }
    else
      {
      // or do we have a table?
      vtkExtendedTable * Table = vtkExtendedTable::SafeDownCast(dataObjects[0]->dataAsVTKDataObject());
      if (!Table)
        {
        qCritical() << "voTreeHeatmapView - Failed to setDataObjectListInternal - Neither tree or table were found !";
        return;
        }
      if (d->TreeItem->GetTable() != Table->GetInputData())
        {
        // and is it different from the one we're already drawing?
        d->TreeItem->SetTable(Table->GetInputData());
        }
      d->TreeItem->SetTree(NULL);
      }
    }

  this->colorTreeForDifference();
  d->ContextView->GetRenderWindow()->SetMultiSamples(0);
}

// --------------------------------------------------------------------------
void voTreeHeatmapView::setDataObjectInternal(const voDataObject& dataObject)
{//only render the tree
  Q_D(voTreeHeatmapView);

  vtkTree * tree = vtkTree::SafeDownCast(dataObject.dataAsVTKDataObject());
  if (!tree)
    {
    qCritical() << "voTreeHeatmapView - Failed to setDataObject - vtkTree data is expected !";
    return;
    }

  if (d->TreeItem->GetTree() != tree)
    {
    d->TreeItem->SetTree(tree);
    }
  this->colorTreeForDifference();
  d->ContextView->GetRenderWindow()->SetMultiSamples(0);
}

// --------------------------------------------------------------------------
void voTreeHeatmapView::colorTreeForDifference()
{
  Q_D(voTreeHeatmapView);
  vtkDataArray *differenceArray =
    d->TreeItem->GetTree()->GetVertexData()->GetArray("differences");
  if (differenceArray)
    {
    d->TreeItem->GetDendrogram()->SetColorArray("differences");
    d->TreeItem->GetDendrogram()->SetLineWidth(2.0);
    }
}

// --------------------------------------------------------------------------
void voTreeHeatmapView::centerData()
{
  Q_D(voTreeHeatmapView);

  // this function gets called every time the view is selected, but we
  // only want to perform this data centering on the initial render.
  if (d->DataAlreadyCentered)
    {
    return;
    }
  d->DataAlreadyCentered = true;

  // make sure the underlying data has been rendered before we start querying
  // it about its bounds & center.
  d->ContextView->Render();

  // get the size of our data.  because this is measured in scene coordinates,
  // we convert it to pixel coordinates.  in practice this should be a no-op,
  // as the transform involved should be an identity as this point.
  double itemSize[3];
  d->TreeItem->GetSize(itemSize);
  itemSize[2] = 0.0;
  d->TransformItem->GetTransform()->MultiplyPoint(itemSize, itemSize);

  // get the size of the scene (measured in pixels).
  double sceneWidth = d->ContextView->GetScene()->GetSceneWidth();
  double sceneHeight = d->ContextView->GetScene()->GetSceneHeight();

  // scale the data so that the full extent of the item fills 90% of the scene.
  this->scaleItem(itemSize[0], itemSize[1],
                  sceneWidth * 0.9, sceneHeight * 0.9);

  // get the center of the item and convert it to pixel coordinates.
  // because of the previous scale operation, this probably isn't a no-op.
  double itemCenter[2];
  d->TreeItem->GetCenter(itemCenter);
  d->TransformItem->GetTransform()->MultiplyPoint(itemCenter, itemCenter);

  // get the center of the scene
  double sceneCenter[2];
  sceneCenter[0] = sceneWidth / 2.0;
  sceneCenter[1] = sceneHeight / 2.0;

  // construct a mouse event dragging from the center of the item to the
  // center of the scene.  this centers the data.
  vtkContextMouseEvent mouseEvent;
  mouseEvent.SetButton(vtkContextMouseEvent::LEFT_BUTTON);
  mouseEvent.SetInteractor(d->Widget->GetInteractor());

  vtkVector2i lastPos;
  lastPos.Set(itemCenter[0], itemCenter[1]);
  mouseEvent.SetLastScreenPos(lastPos);

  vtkVector2i pos;
  pos.Set(sceneCenter[0], sceneCenter[1]);
  mouseEvent.SetScreenPos(pos);

  d->TransformItem->MouseMoveEvent(mouseEvent);
}

// --------------------------------------------------------------------------
void voTreeHeatmapView::scaleItem(double oldWidth, double oldHeight,
                                  double newWidth, double newHeight)
{
  Q_D(voTreeHeatmapView);

  double sx =  newWidth / oldWidth;
  double sy = newHeight / oldHeight;

  // regardless if zooming out or zooming in, we should always use
  // the scale factor closest to 1.0, as this will not resize the
  // scene in such a way as to push objects of interest out of view.
  double dx = abs(sx - 1.0);
  double dy = abs(sy - 1.0);
  if (dy > dx)
    {
    d->TransformItem->Scale(sx, sx);
    }
  else
    {
    d->TransformItem->Scale(sy, sy);
    }
}

// --------------------------------------------------------------------------
void voTreeHeatmapView::resizeEvent(QResizeEvent *event)
{
  // if this resize is occurring as part of the initial render, then
  // we do nothing here.  centerData() will be called slightly later
  // from voViewStackedWidget.
  if (event->oldSize().width() == -1)
    {
    return;
    }

  // otherwise, zoom in or out to compensate for the change in size of the
  // render window.
  else
    {
    this->scaleItem(static_cast<double>(event->oldSize().width()),
                    static_cast<double>(event->oldSize().height()),
                    static_cast<double>(event->size().width()),
                    static_cast<double>(event->size().height()));
    }
}

// --------------------------------------------------------------------------
QList<QAction*> voTreeHeatmapView::actions()
{
  QList<QAction*> actionList;

  QAction * saveScreenshotAction =
      new QAction(QIcon(":/Icons/saveScreenshot.png"), "Save screenshot", this);
  saveScreenshotAction->setToolTip("Save the current view to disk.");
  connect(saveScreenshotAction, SIGNAL(triggered()),
          this, SLOT(onSaveScreenshotActionTriggered()));
  actionList << saveScreenshotAction;

  return actionList;
}

// --------------------------------------------------------------------------
void voTreeHeatmapView::onSaveScreenshotActionTriggered()
{
  QString defaultFileName =
    QDesktopServices::storageLocation(QDesktopServices::DesktopLocation)
    + "/" + voUtils::cleanString(this->objectName()) + ".svg";

  QString fileName = QFileDialog::getSaveFileName(
        0, tr("Save screenshot..."), defaultFileName, "Image (*.svg *.png)");
  if(fileName.isEmpty())
    {
    return;
    }
  this->saveScreenshot(fileName);
}

// --------------------------------------------------------------------------
void voTreeHeatmapView::saveScreenshot(const QString& fileName)
{
  Q_D(voTreeHeatmapView);
  if (fileName.endsWith(".png"))
    {
    this->Superclass::saveScreenshot(fileName);
    return;
    }

  vtkNew<vtkGL2PSExporter> exporter;
  exporter->SetRenderWindow(d->ContextView->GetRenderWindow());
  exporter->SetFileFormatToSVG();
  exporter->UsePainterSettings();
  exporter->CompressOff();
  exporter->SetTitle(this->objectName().toStdString().c_str());
  exporter->DrawBackgroundOn();

  QFileInfo fileInfo(fileName);
  QString fileNameWithoutExt = fileInfo.dir().absolutePath() +
    QString("/") + fileInfo.baseName();

  exporter->SetFilePrefix(fileNameWithoutExt.toStdString().c_str());
  exporter->Write();
}
