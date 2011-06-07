
// QtSoap includes
#include <QtSoapType>

// Visomics includes
#include "voUtilsSoap.h"

//----------------------------------------------------------------------------
void voUtils::Soap::DumpAll(const QtSoapType& type, std::ostream &os, int indent)
{
  if(indent == 0)
    {
    QString s;
    s = "Dumping:\n" + type.typeName() + ": " + type.name().name();
    if (type.count() == 0)
      {
      s += " - Value: \"" + type.value().toString() + "\"";
      }
    os << qPrintable(s) << std::endl;
    indent = 4;
    }
  for (int i = 0; i < type.count(); ++i)
    {
    QString s;
    s = QString(indent, ' ') + type[i].typeName() + ": " + type[i].name().name();
    if (type[i].count() == 0)
      {
      s += " - Value: \"" + type[i].value().toString() + "\"";
      }
    os << qPrintable(s) << std::endl;
    if(type[i].count() > 0)
      {
      voUtils::Soap::DumpAll(type[i], os, indent + 4);
      }
    }
}

//----------------------------------------------------------------------------
void voUtils::Soap::DumpType(const QtSoapType& sstruct)
{
  std::cout << "Dumping: " << qPrintable(sstruct.typeName()) << " " << qPrintable(sstruct.name().name())  << std::endl;
  for (int i = 0; i < sstruct.count(); ++i)
    {
    std::cout << qPrintable(sstruct[i].typeName()) << "  " << qPrintable(sstruct[i].name().name()) << std::endl;
    }
}
