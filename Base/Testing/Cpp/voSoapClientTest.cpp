
// Qt includes
#include <QApplication>
#include <QVariant>

// QtSoap includes
#include <QtSoapType>

// Visomics includes
#include "voSoapClient.h"
#include "voUtilsSoap.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int voSoapClientTest(int argc, char * argv [])
{
  QApplication app(argc, argv);

  voSoapClient client(80, "soap.genome.jp", "keggapi/request_v6.2.cgi", "SOAP/KEGG");

  const QtSoapMessage & result1 = client.submitSoapRequest("invalid_method");
  if (!result1.isFault())
    {
    std::cerr << "Line " << __LINE__ << " - Problem with voSoapClient::submitSoapRequest !"
              << " - Inoking 'invalid_method' should have failed. " << std::endl;
    voUtils::Soap::DumpAll(result1.returnValue(), std::cerr);
    return EXIT_FAILURE;
    }

  const QtSoapMessage & result2 = client.submitSoapRequest("list_databases");
  if (result2.isFault())
    {
    std::cerr << "Line " << __LINE__ << " - Problem with voSoapClient::submitSoapRequest !" << std::endl;
    voUtils::Soap::DumpAll(result2.faultString(), std::cerr);
    voUtils::Soap::DumpAll(result2.faultDetail(), std::cerr);
    return EXIT_FAILURE;
    }
  //voUtils::Soap::DumpAll(result2.returnValue());
  return EXIT_SUCCESS;
}
