
// Qt includes
#include <QFileInfo>
#include <QDebug>

// Visomics includes
#include "voApplication.h"
#include "voCSVReader.h"
#include "voDataModel.h"
#include "voDataModelItem.h"
#include "voDataObject.h"
#include "voInputFileDataObject.h"
#include "voIOManager.h"

// --------------------------------------------------------------------------
void voIOManager::openCSVFiles(const QStringList& fileNames)
{
  QList<voDataObject*> loadedDataObjects;

  foreach(const QString& file, fileNames)
    {
    voCSVReader reader;
    reader.setFileName(file);
    reader.update();
    voInputFileDataObject * dataObject = new voInputFileDataObject();
    dataObject->setData(reader.output());
    dataObject->setName(QFileInfo(file).baseName());
    dataObject->setFileName(file);
    loadedDataObjects << dataObject;
    }

  voDataModel * model = voApplication::application()->dataModel();

  QList<voDataModelItem*> itemsAdded;
  foreach(voDataObject* dataObject, loadedDataObjects)
    {
    voDataModelItem * newItem = model->addDataObject(dataObject);
    newItem->setRawViewType("voTableView");
    itemsAdded << newItem;
    }

  // Select the first item added
  if (itemsAdded.count())
    {
    model->setSelected(itemsAdded.value(0));
    }
}
