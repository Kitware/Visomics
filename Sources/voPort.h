#ifndef __voPort_h
#define __voPort_h

// Qt includes
#include <QString>

class voAnalysis;
class vtkDataObject;

class voPort
{
public:
  voPort() :
    Analysis(0) { }

  voPort(voAnalysis* an) :
    Analysis(an), Name("output") { }

  voPort(voAnalysis* an, QString nm) :
    Analysis(an), Name(nm) { }

  voAnalysis* analysis()const;
  void setAnalysis(voAnalysis* an);

  QString name()const;
  void setName(const QString& nm);

  vtkDataObject* data();

protected:
  voAnalysis* Analysis;
  QString     Name;
};

#endif
