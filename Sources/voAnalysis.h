
#ifndef __voAnalysis_h
#define __voAnalysis_h

// Qt includes
#include <QSharedPointer>
#include <QScopedPointer>
#include <QLatin1String>
#include <QObject>
#include <QStringList>

class voAnalysisPrivate;
class voDataObject;
class vtkDataObject;

class voAnalysis : public QObject
{
  Q_OBJECT
public:
  voAnalysis();
  virtual ~voAnalysis();


  void addInputType(const QString& inputName, const QString& inputType);

  QString inputType(const QString& inputName) const;

  int numberOfInput();

  QStringList inputNames()const;

  bool hasInput(const QString& inputName) const;

  void setInput(const QString& inputName, voDataObject * dataObject);

  voDataObject * input(const QString& inputName = QLatin1String("input")) const;

  void removeAllInputs();



  void addOutputType(const QString& outputName, const QString& outputType,
                     const QString& viewType, const QString& viewPrettyName,
                     const QString& rawViewType = QString());

  QString outputType(const QString& outputName) const;

  QString viewPrettyName(const QString& outputName, const QString& viewType);

  int numberOfOutput();

  QStringList outputNames() const;

  bool hasOutput(const QString& outputName) const;

  void setOutput(const QString& outputName, voDataObject * dataObject);

  voDataObject * output(const QString& outputName) const;

  bool hasOutputWithViewType(const QString& outputName, const QString& viewType) const;

  QStringList viewTypesForOutput(const QString& outputName)const;

//  QStringList viewTypes()const;

  bool hasOutputWithRawViewType(const QString& outputName, const QString& rawViewType) const;

  QString rawViewTypeForOutput(const QString& outputName)const;

  void removeAllOutputs();

  bool run();

  void initializeInputInformation();
  void initializeOutputInformation();

protected:

  virtual bool execute();

  virtual void setInputInformation(){}
  virtual void setOutputInformation(){}

protected:
  QScopedPointer<voAnalysisPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(voAnalysis);
  Q_DISABLE_COPY(voAnalysis);
};

#endif
