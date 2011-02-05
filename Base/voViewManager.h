
#ifndef __voViewManager_h
#define __voViewManager_h

// Qt includes
#include <QObject>

class voViewManagerPrivate;
class voView;

class voViewManager : public QObject
{
  Q_OBJECT
public:
  voViewManager();
  virtual ~voViewManager();

  void deleteView(voView * view);

public slots:

  void createView(const QString& objectUuid);


signals:

  void viewCreated(const QString& objectUuid, voView * newView);


protected:
  QScopedPointer<voViewManagerPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voViewManager);
  Q_DISABLE_COPY(voViewManager);
};

#endif
