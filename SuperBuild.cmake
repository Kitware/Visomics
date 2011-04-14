
#-----------------------------------------------------------------------------
# GNU R
#-----------------------------------------------------------------------------
# Note :GNU R is should be provided at configuration time. For example:
#       cmake <OTHER_OPTIONS> -DR_COMMAND:FILEPATH=/path/to/R-2.11.1/bin/R ../Visomics

find_package(R REQUIRED)

#-----------------------------------------------------------------------------
# Git protocole option
#-----------------------------------------------------------------------------

option(Visomics_USE_GIT_PROTOCOL "If behind a firewall turn this off to use http instead." ON)

set(git_protocol "git")
if(NOT Visomics_USE_GIT_PROTOCOL)
  set(git_protocol "http")
endif()

#-----------------------------------------------------------------------------
# Enable and setup External project global properties
#-----------------------------------------------------------------------------

INCLUDE(ExternalProject)
INCLUDE(ctkMacroEmptyExternalProject)

set(ep_base        "${CMAKE_BINARY_DIR}")
#set(ep_install_dir "${ep_base}/Install")
set(ep_suffix      "-cmake")

SET(ep_common_args
  #-DCMAKE_INSTALL_PREFIX:PATH=${ep_install_dir}
  -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
  -DBUILD_TESTING:BOOL=OFF
  )

SET(ep_common_c_flags "${CMAKE_C_FLAGS_INIT} ${ADDITIONAL_C_FLAGS}")
SET(ep_common_cxx_flags "${CMAKE_CXX_FLAGS_INIT} ${ADDITIONAL_CXX_FLAGS}")
  
# Compute -G arg for configuring external projects with the same CMake generator:
if(CMAKE_EXTRA_GENERATOR)
  set(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
else()
  set(gen "${CMAKE_GENERATOR}")
endif()

#------------------------------------------------------------------------------
# Include ExternalProject Target
#------------------------------------------------------------------------------

include(CMakeExternals/VTK.cmake)
include(CMakeExternals/QtPropertyBrowser.cmake)
#include(CMakeExternals/CTK.cmake)

#-----------------------------------------------------------------------------
# Set superbuild boolean args
#-----------------------------------------------------------------------------

SET(project_cmake_boolean_args
  BUILD_DOCUMENTATION
  BUILD_TESTING
  BUILD_SHARED_LIBS
  WITH_COVERAGE
  WITH_MEMCHECK
  )
  
SET(project_superbuild_boolean_args)
FOREACH(project_cmake_arg ${project_cmake_boolean_args})
  LIST(APPEND project_superbuild_boolean_args -D${project_cmake_arg}:BOOL=${${project_cmake_arg}})
ENDFOREACH()

# MESSAGE("CMake args:")
# FOREACH(arg ${project_superbuild_boolean_args})
#   MESSAGE("  ${arg}")
# ENDFOREACH()

#-----------------------------------------------------------------------------
# Configure and build the project
#------------------------------------------------------------------------------

set(proj Visomics)
ExternalProject_Add(${proj}
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  BINARY_DIR Visomics-build
  CMAKE_GENERATOR ${gen}
  PREFIX ${proj}${ep_suffix}
  CMAKE_ARGS
    ${project_superbuild_boolean_args}
    -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
    -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
    -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DGIT_EXECUTABLE:FILEPATH=${GIT_EXECUTABLE}
    -DVisomics_SUPERBUILD:BOOL=OFF
    -DVisomics_C_FLAGS:STRING=${Visomics_C_FLAGS}
    -DVisomics_CXX_FLAGS:STRING=${Visomics_CXX_FLAGS}
    -DCTEST_CONFIGURATION_TYPE:STRING=${CTEST_CONFIGURATION_TYPE}
    # QtPropertyBrowser
    -DQtPropertyBrowser_DIR:PATH=${QtPropertyBrowser_DIR}
    # VTK
    -DVTK_DIR:PATH=${VTK_DIR}
    #-DVTK_DEBUG_LEAKS:BOOL=${Slicer_USE_VTK_DEBUG_LEAKS}
    # Qt
    -DQT_QMAKE_EXECUTABLE:PATH=${QT_QMAKE_EXECUTABLE}
    # CTK
    #-DCTK_DIR:PATH=${CTK_DIR}
  DOWNLOAD_COMMAND ""
  INSTALL_COMMAND ""
  DEPENDS 
    # Mandatory dependencies
    ${QtPropertyBrowser_DEPENDS}
    #${CTK_DEPENDS}
    ${VTK_DEPENDS}
  )
