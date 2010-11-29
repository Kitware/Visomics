CMAKE_MINIMUM_REQUIRED(VERSION 2.8.2)

#-----------------------------------------------------------------------------
# See http://cmake.org/cmake/help/cmake-2-8-docs.html#section_Policies for details
#

SET(project_policies
  CMP0001 # NEW: CMAKE_BACKWARDS_COMPATIBILITY should no longer be used.
  CMP0002 # NEW: Logical target names must be globally unique.
  CMP0003 # NEW: Libraries linked via full path no longer produce linker search paths.
  CMP0004 # NEW: Libraries linked may NOT have leading or trailing whitespace.
  CMP0005 # NEW: Preprocessor definition values are now escaped automatically.
  CMP0006 # NEW: Installing MACOSX_BUNDLE targets requires a BUNDLE DESTINATION.
  CMP0007 # NEW: List command no longer ignores empty elements.
  CMP0008 # NEW: Libraries linked by full-path must have a valid library file name.
  CMP0009 # NEW: FILE GLOB_RECURSE calls should not follow symlinks by default.
  CMP0010 # NEW: Bad variable reference syntax is an error.
  CMP0011 # NEW: Included scripts do automatic cmake_policy PUSH and POP.
  CMP0012 # NEW: if() recognizes numbers and boolean constants.
  CMP0013 # NEW: Duplicate binary directories are not allowed.
  CMP0014 # NEW: Input directories must have CMakeLists.txt
  )
FOREACH(policy ${project_policies})
  IF(POLICY ${policy})
    CMAKE_POLICY(SET ${policy} NEW)
  ENDIF()
ENDFOREACH()

#-----------------------------------------------------------------------------
PROJECT(Visomics)
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Visomics version number.  An even minor number corresponds to releases.
#
SET(Visomics_MAJOR_VERSION 0)
SET(Visomics_MINOR_VERSION 1)
SET(Visomics_BUILD_VERSION 0)
SET(Visomics_VERSION
    "${Visomics_MAJOR_VERSION}.${Visomics_MINOR_VERSION}.${Visomics_BUILD_VERSION}")
    
#-----------------------------------------------------------------------------
# Install directories, used for install rules.
#
SET(Visomics_INSTALL_BIN_DIR "bin")
SET(Visomics_INSTALL_LIB_DIR "bin")
SET(Visomics_INSTALL_INCLUDE_DIR "include/visomics-${Visomics_MAJOR_VERSION}.${Visomics_MINOR_VERSION}")
SET(Visomics_INSTALL_DOC_DIR "doc")

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
FIND_PACKAGE(VTK 5.7 REQUIRED)
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
ENDIF()

#-----------------------------------------------------------------------------
ADD_SUBDIRECTORY(Sources)
