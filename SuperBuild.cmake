###########################################################################
#
#  Program: Visomics
#
#  Copyright (c) Kitware Inc.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0.txt
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
###########################################################################

#-----------------------------------------------------------------------------
# Git protocol option
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
include(CMakeExternals/VisomicsData.cmake)
if(BUILD_TESTING)
  include(CMakeExternals/QtTesting.cmake)
endif()
#include(CMakeExternals/CTK.cmake)

#-----------------------------------------------------------------------------
# Set superbuild boolean args
#-----------------------------------------------------------------------------

SET(project_cmake_boolean_args
  #BUILD_DOCUMENTATION # Not used
  BUILD_TESTING
  BUILD_SHARED_LIBS
  WITH_COVERAGE
  #WITH_MEMCHECK # Not used
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
# Define list of additional options used to configure Visomics
#------------------------------------------------------------------------------
set(project_superbuild_extra_args)

if(DEFINED CTEST_CONFIGURATION_TYPE)
  list(APPEND project_superbuild_extra_args -DCTEST_CONFIGURATION_TYPE:STRING=${CTEST_CONFIGURATION_TYPE})
endif()

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
    ${project_superbuild_extra_args}
    ${project_superbuild_boolean_args}
    -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
    -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
    -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    #-DGIT_EXECUTABLE:FILEPATH=${GIT_EXECUTABLE} # Not used
    -DVisomics_SUPERBUILD:BOOL=OFF
    -DVisomics_C_FLAGS:STRING=${Visomics_C_FLAGS}
    -DVisomics_CXX_FLAGS:STRING=${Visomics_CXX_FLAGS}
    -DVisomics_KEGG_SERVER_HOSTNAME=${Visomics_KEGG_SERVER_HOSTNAME}
    -DVisomics_KEGG_SERVER_PORT=${Visomics_KEGG_SERVER_PORT}
    # QtPropertyBrowser
    -DQtPropertyBrowser_DIR:PATH=${QtPropertyBrowser_DIR}
    # QtTesting
    -DQtTesting_DIR:PATH=${QtTesting_DIR}
    # VisomicsData
    -DVisomicsData_DIR:PATH=${VisomicsData_DIR}
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
    ${VisomicsData_DEPENDS}
    #${CTK_DEPENDS}
    ${VTK_DEPENDS}
    ${QtTesting_DEPENDS}
  )
