

// VTK includes
#include <vtkNew.h>
#include <vtkRInterface.h>
#include <vtksys/SystemTools.hxx>

// STD includes
#include <cstdlib>
#include <iostream>

int voCheckR_HOMETest(int /*argc*/, char * /*argv*/ [])
{
  vtkNew<vtkRInterface> rInterface;
  const char* path = vtksys::SystemTools::GetEnv("R_HOME");
  std::cout << "R_HOME:" << path << std::endl;
  return EXIT_SUCCESS;
}
