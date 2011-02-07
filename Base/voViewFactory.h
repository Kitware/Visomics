
#ifndef __voViewFactory_h
#define __voViewFactory_h

// Qt includes
#include <QScopedPointer>
#include <QString>

class voViewFactoryPrivate;
class voView;

class voViewFactory
{

public:
  voViewFactory();
  virtual ~voViewFactory();

  /// Given the name of a voView subclass, return a new instance of the view.
  virtual voView* createView(const QString& className);

  /// Return list of registered view names
  QStringList registeredViewNames() const;

protected:

  /// Register an a view
  template<typename ClassType>
  void registerView(const QString& viewName = QString());

protected:
  QScopedPointer<voViewFactoryPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voViewFactory);
  Q_DISABLE_COPY(voViewFactory);
};

#endif
