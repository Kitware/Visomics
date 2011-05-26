
// STD includes
#include <iostream>

class QtSoapType;

namespace voUtils
{

namespace Soap
{

void DumpAll(const QtSoapType& type, std::ostream &os = std::cout, int indent = 0);

void DumpType(const QtSoapType& sstruct);

} // end of Soap namespace

} // end of voUtils namespace
