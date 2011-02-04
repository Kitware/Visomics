CMAKE_MINIMUM_REQUIRED(VERSION 2.8.2)

#-----------------------------------------------------------------------------
PROJECT(Visomics)
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Superbuild Option - Enabled by default
#
OPTION(Visomics_SUPERBUILD "Build Visomics and the projects it depends on via SuperBuild.cmake." ON)
MARK_AS_ADVANCED(Visomics_SUPERBUILD)

#-----------------------------------------------------------------------------
# Visomics version number.  An even minor number corresponds to releases.
#
SET(Visomics_MAJOR_VERSION 0)
SET(Visomics_MINOR_VERSION 2)
SET(Visomics_BUILD_VERSION 0)
SET(Visomics_VERSION
    "${Visomics_MAJOR_VERSION}.${Visomics_MINOR_VERSION}.${Visomics_BUILD_VERSION}")

#-----------------------------------------------------------------------------
# Install directories, used for install rules.
#
SET(Visomics_INSTALL_BIN_DIR "bin")
SET(Visomics_INSTALL_LIB_DIR "lib")
SET(Visomics_INSTALL_INCLUDE_DIR "include/visomics-${Visomics_MAJOR_VERSION}.${Visomics_MINOR_VERSION}")
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

#-----------------------------------------------------------------------------
# Output directories.
#
SET(output_dir ${Visomics_BINARY_DIR}/bin)
FOREACH(type LIBRARY RUNTIME ARCHIVE)
  SET(CMAKE_${type}_OUTPUT_DIRECTORY ${output_dir} CACHE INTERNAL "Single output directory for building all libraries.")
ENDFOREACH()

#-----------------------------------------------------------------------------
# VTK
#
FIND_PACKAGE(VTK REQUIRED)
SET(current_vtk_version ${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}.${VTK_BUILD_VERSION})
SET(required_vtk_version 5.7)
SET(preferred_vtk_version 5.8)
IF(${current_vtk_version} VERSION_LESS ${required_vtk_version})
  MESSAGE(FATAL_ERROR "You are trying to build Visomics using VTK ${current_vtk_version}. Please, update to VTK ${preferred_vtk_version} or higher !")
ENDIF()
IF(${current_vtk_version} VERSION_LESS ${preferred_vtk_version})
  MESSAGE(WARNING "You are building Visomics using VTK ${current_vtk_version}. We recommend to use version  ${preferred_vtk_version} or higher")
ENDIF()
INCLUDE(${VTK_USE_FILE})

#-----------------------------------------------------------------------------
# Qt
#
SET(QT_QMAKE_EXECUTABLE ${VTK_QT_QMAKE_EXECUTABLE})
FIND_PACKAGE(Qt4 4.7 REQUIRED)
INCLUDE(${QT_USE_FILE})

#-----------------------------------------------------------------------------
# Testing
#
OPTION(BUILD_TESTING "Test the project" ON)
IF(BUILD_TESTING)
  ENABLE_TESTING()
  INCLUDE(CTest)
  MARK_AS_ADVANCED(TCL_TCLSH DART_ROOT)
  SET(CPP_TEST_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

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
# CTK C/CXX Flags
#
SET(CTK_C_FLAGS "${CMAKE_C_FLAGS_INIT} ${COVERAGE_C_FLAGS} ${ADDITIONAL_C_FLAGS}")
SET(CTK_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} ${VISIBILITY_CXX_FLAGS} ${COVERAGE_CXX_FLAGS} ${ADDITIONAL_CXX_FLAGS}")

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(cflags "-Wall -Wextra -Wpointer-arith -Winvalid-pch -Wcast-align -Wwrite-strings -D_FORTIFY_SOURCE=2")

  ctkFunctionCheckCompilerFlags("-fdiagnostics-show-option" cflags)
  ctkFunctionCheckCompilerFlags("-fstack-protector-all" cflags)

  SET(CTK_C_FLAGS "${cflags} ${CTK_C_FLAGS}")
  SET(CTK_CXX_FLAGS "${cflags} -Woverloaded-virtual -Wold-style-cast -Wstrict-null-sentinel -Wsign-promo ${CTK_CXX_FLAGS}")
ENDIF()

#-----------------------------------------------------------------------------
# Superbuild script
#
IF(Visomics_SUPERBUILD)
  INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
  RETURN()
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
ADD_SUBDIRECTORY(Sources)

#-----------------------------------------------------------------------------
# Setup CPack

INCLUDE(CMake/VisomicsCPack.cmake)
