
#ifndef __voAnalysis_h
#define __voAnalysis_h

#include "voPort.h"

#include <vtkSmartPointer.h>

#include <QString>
#include <QStringList>
#include <QMap>

class voView;
class vtkAlgorithm;
class vtkDataObject;

class voAnalysis : public QObject
{
public:
  voAnalysis();
  virtual ~voAnalysis();

  virtual voPort input(const QString& i)const;

  virtual voPort input()const
    { return this->input("input"); }

  virtual void setInput(const QString& i, voPort port);

  virtual void setInput(voPort port)
    { this->setInput("input", port); }

  virtual void update();

  virtual void updateViews();

  virtual vtkDataObject* output(const QString& i)const;

  virtual vtkDataObject* output() const
    { return this->output("output"); }

  virtual QStringList views() const
    { return this->Views.keys(); }

  virtual voView* view(const QString& str)
    { return this->Views[str]; }

protected:
  virtual void addInput(const QString& name);

  /// Call \a updateInternal on all associated inputs
  /// \sa updateInternal
  virtual void updateInputs();

  /// Set \a Algorithm inputs, update \a Algorithm and retrieve the associated outputs
  /// The map \a Outputs will contains the output data object
  virtual void updateInternal();

  QMap<QString, voPort> Inputs;
  QMap<QString, vtkSmartPointer<vtkDataObject> > Outputs;
  QMap<QString, voView*> Views;

  QMap<QString, int> InputNameToIndex;
  QMap<QString, int> OutputNameToIndex;

  vtkAlgorithm* Algorithm;
};

#endif
