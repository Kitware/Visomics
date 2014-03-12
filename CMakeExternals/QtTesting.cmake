# Sanity checks
if(DEFINED QtTesting_DIR AND NOT EXISTS ${QtTesting_DIR})
  message(FATAL_ERROR "QtTesting_DIR variable is defined but corresponds to non-existing directory (${QtTesting_DIR})")
endif()

set(proj QtTesting)
set(proj_DEPENDENCIES)

set(QtTesting_DEPENDS ${proj})

if(NOT DEFINED QtTesting_DIR)
  message(STATUS "Adding external project: ${proj}")
  ExternalProject_Add(${proj}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
    BINARY_DIR ${proj}-build
    PREFIX ${proj}${ep_suffix}
    GIT_REPOSITORY "${git_protocol}://paraview.org/QtTesting.git"
    GIT_TAG "origin/master"
    CMAKE_GENERATOR ${gen}
    INSTALL_COMMAND ""
    CMAKE_ARGS
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DBUILD_TESTING:BOOL=OFF # Not used
      -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
    DEPENDS
      ${proj_DEPENDENCIES}
    )
  set(QtTesting_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

else()
  ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
endif()
