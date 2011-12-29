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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.4)

#-----------------------------------------------------------------------------
PROJECT(Visomics)
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Superbuild Option - Enabled by default
#
OPTION(Visomics_SUPERBUILD "Build Visomics and the projects it depends on via SuperBuild.cmake." ON)
MARK_AS_ADVANCED(Visomics_SUPERBUILD)

#-----------------------------------------------------------------------------
# Build options
#
SET(Visomics_KEGG_SERVER_HOSTNAME "visomics.kitware.com" CACHE STRING "KEGG Server hostname")
SET(Visomics_KEGG_SERVER_PORT "8090" CACHE STRING "KEGG Server port")

#-----------------------------------------------------------------------------
# Visomics version number.  An even minor number corresponds to releases.
#
SET(Visomics_MAJOR_VERSION 0)
SET(Visomics_MINOR_VERSION 5)
SET(Visomics_BUILD_VERSION 2)
SET(Visomics_VERSION
    "${Visomics_MAJOR_VERSION}.${Visomics_MINOR_VERSION}.${Visomics_BUILD_VERSION}")

#-----------------------------------------------------------------------------
# Install directories, used for install rules.
#
SET(Visomics_INSTALL_BIN_DIR "bin")
SET(Visomics_INSTALL_LIB_DIR "lib")
SET(Visomics_INSTALL_INCLUDE_DIR "include/visomics-${Visomics_MAJOR_VERSION}.${Visomics_MINOR_VERSION}")
SET(Visomics_INSTALL_DATA_DIR "data")
SET(Visomics_INSTALL_RSCRIPTS_DIR "share/R")
SET(Visomics_INSTALL_SHARE_DIR "share")
SET(Visomics_INSTALL_DOC_DIR "doc")

#-----------------------------------------------------------------------------
# Update CMake module path
# Note: FindXXX.cmake script specific to utility should be copied into Utilities/CMake
#
SET(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/Utilities/CMake"
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake"
  ${CMAKE_MODULE_PATH})

#-----------------------------------------------------------------------------
# CMake Function(s) and Macro(s)
#
INCLUDE(CMake/ctkFunctionCheckCompilerFlags.cmake)
INCLUDE(CMake/ctkFunctionGetGccVersion.cmake)

#-----------------------------------------------------------------------------
# Output directories.
#
SET(output_dir ${Visomics_BINARY_DIR}/bin)
FOREACH(type LIBRARY RUNTIME ARCHIVE)
  SET(CMAKE_${type}_OUTPUT_DIRECTORY ${output_dir} CACHE INTERNAL "Single output directory for building all libraries.")
ENDFOREACH()

#-----------------------------------------------------------------------------
# Qt
#
FIND_PACKAGE(Qt4 4.7 COMPONENTS QtCore QtGui QtNetwork QtScript QtWebKit REQUIRED)
INCLUDE(${QT_USE_FILE})

#-----------------------------------------------------------------------------
# Testing
#
OPTION(BUILD_TESTING "Test the project" ON)
IF(BUILD_TESTING)
  ENABLE_TESTING()
  INCLUDE(CTest)
  MARK_AS_ADVANCED(TCL_TCLSH DART_ROOT)

  set(Visomics_LAUNCHER_EXECUTABLE ${Visomics_BINARY_DIR}/bin/visomics)
  set(Visomics_LAUNCH_COMMAND ${Visomics_LAUNCHER_EXECUTABLE} --command)

  # Setup file for setting custom ctest vars
  CONFIGURE_FILE(
    CMake/CTestCustom.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake
    @ONLY
    )

ENDIF()

#-----------------------------------------------------------------------------
# Coverage
#
OPTION(WITH_COVERAGE "Enable/Disable coverage" OFF)

#-----------------------------------------------------------------------------
# Documentation
#

#-----------------------------------------------------------------------------
# Check compiler version
# 
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(REQUIRED_GNUCXX_VERSION "4.0.0")
  ctkFunctionGetGccVersion(${CMAKE_CXX_COMPILER} GCC_VERSION)
  IF(${GCC_VERSION} VERSION_LESS ${REQUIRED_GNUCXX_VERSION})
    MESSAGE(FATAL_ERROR "Gcc >= ${REQUIRED_GNUCXX_VERSION} is required ! Your current version is ${GCC_VERSION}.")
  ENDIF()
ENDIF()

#-----------------------------------------------------------------------------
# Additional CXX/C Flags
#
SET(ADDITIONAL_C_FLAGS "" CACHE STRING "Additional C Flags")
MARK_AS_ADVANCED(ADDITIONAL_C_FLAGS)
SET(ADDITIONAL_CXX_FLAGS "" CACHE STRING "Additional CXX Flags")
MARK_AS_ADVANCED(ADDITIONAL_CXX_FLAGS)

#-----------------------------------------------------------------------------
# Set coverage Flags
#
IF(WITH_COVERAGE)
  IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    SET(coverage_flags "-g -fprofile-arcs -ftest-coverage -O0 -DNDEBUG")
    SET(COVERAGE_CXX_FLAGS ${coverage_flags})
    SET(COVERAGE_C_FLAGS ${coverage_flags})
  ENDIF()
ENDIF()

#-----------------------------------------------------------------------------
# Visomics C/CXX Flags
#
SET(Visomics_C_FLAGS "${CMAKE_C_FLAGS_INIT} ${COVERAGE_C_FLAGS} ${ADDITIONAL_C_FLAGS}")
SET(Visomics_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} ${VISIBILITY_CXX_FLAGS} ${COVERAGE_CXX_FLAGS} ${ADDITIONAL_CXX_FLAGS}")

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(cflags "-Wall -Wextra -Wpointer-arith -Winvalid-pch -Wcast-align -Wwrite-strings -D_FORTIFY_SOURCE=2")

  ctkFunctionCheckCompilerFlags("-fdiagnostics-show-option" cflags)
  ctkFunctionCheckCompilerFlags("-fstack-protector-all" cflags)

  SET(Visomics_C_FLAGS "${cflags} ${Visomics_C_FLAGS}")
  SET(Visomics_CXX_FLAGS "${cflags} -Woverloaded-virtual -Wold-style-cast -Wstrict-null-sentinel -Wsign-promo ${Visomics_CXX_FLAGS}")
ENDIF()

#-----------------------------------------------------------------------------
# Superbuild script
#
IF(Visomics_SUPERBUILD)
  INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
  RETURN()
ENDIF()

#-----------------------------------------------------------------------------
# VTK
#
FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})
GET_FILENAME_COMPONENT(VTK_QT_QMAKE_EXECUTABLE_REALPATH ${VTK_QT_QMAKE_EXECUTABLE} REALPATH)
GET_FILENAME_COMPONENT(QT_QMAKE_EXECUTABLE_REALPATH ${QT_QMAKE_EXECUTABLE} REALPATH)
IF(NOT ${VTK_QT_QMAKE_EXECUTABLE_REALPATH} STREQUAL ${QT_QMAKE_EXECUTABLE_REALPATH})
  MESSAGE(FATAL_ERROR "Qt version mismatch problem:
(1) QT_QMAKE_EXECUTABLE:${QT_QMAKE_EXECUTABLE_REALPATH}
(2) VTK_QT_QMAKE_EXECUTABLE:${VTK_QT_QMAKE_EXECUTABLE_REALPATH}
Try to rebuild VTK using (1)
")
ENDIF()

#-----------------------------------------------------------------------------
# QtPropertyBrowser

FIND_PACKAGE(QtPropertyBrowser REQUIRED)
INCLUDE(${QtPropertyBrowser_USE_FILE})

#-----------------------------------------------------------------------------
# Set C/CXX Flags
#
SET(CMAKE_CXX_FLAGS ${Visomics_CXX_FLAGS} CACHE STRING "CMake C Flags" FORCE)
SET(CMAKE_C_FLAGS ${Visomics_C_FLAGS} CACHE STRING "CMake CXX Flags" FORCE)

#-----------------------------------------------------------------------------
# Configure headers
#
SET(Visomics_BUILD_TESTING ${BUILD_TESTING}) # Used in voConfigure.h.in
FOREACH(header voConfigure.h voTestConfigure.h)
  CONFIGURE_FILE(
    ${header}.in
    ${CMAKE_CURRENT_BINARY_DIR}/${header}
    )
ENDFOREACH()

#-----------------------------------------------------------------------------
# Include directories
#
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

#-----------------------------------------------------------------------------
ADD_SUBDIRECTORY(ThirdParty/CTK)
INCLUDE_DIRECTORIES(ThirdParty/CTK)

#-----------------------------------------------------------------------------
ADD_SUBDIRECTORY(Base)
ADD_SUBDIRECTORY(Application)

#-----------------------------------------------------------------------------
# R scripts
#
set(r_scripts
  Utilities/GetRExternalPackages.R
  )
INSTALL(FILES ${r_scripts} DESTINATION ${Visomics_INSTALL_RSCRIPTS_DIR} COMPONENT Runtime)

#-----------------------------------------------------------------------------
# Install sample dataset(s)
#
IF(BUILD_TESTING)
  SET(datasets
    ${VisomicsData_DIR}/Data/UNC/All_conc_kitware_transposed.csv
    #${VisomicsData_DIR}/Data/UniversityOfWashington/PE_Microarray_dataset.csv
    )
  INSTALL(FILES ${datasets} DESTINATION ${Visomics_INSTALL_DATA_DIR} COMPONENT Runtime)
ENDIF()

#-----------------------------------------------------------------------------
# Setup CPack

INCLUDE(CMake/VisomicsCPack.cmake)
