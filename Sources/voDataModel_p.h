
#ifndef __voDataModel_p_h
#define __voDataModel_p_h

// Qt includes
#include <QObject>
#include <QModelIndex>

class QItemSelectionModel;
class voAnalysis;
class voDataModel;
class voDataModelItem;

class voDataModelPrivate : public QObject
{
  Q_OBJECT
  Q_DECLARE_PUBLIC(voDataModel);
protected:
  voDataModel* const q_ptr;
public:
  voDataModelPrivate(voDataModel& object);
  virtual ~voDataModelPrivate();

public slots:

  void onCurrentRowChanged(const QModelIndex & current, const QModelIndex & previous);

public:

  QItemSelectionModel*          SelectionModel;
  QList<voDataModelItem*>       SelectedInputDataObjects;
  QHash<QString, unsigned int>  NameCountMap;
  voAnalysis*                   ActiveAnalysis;
};

#endif
