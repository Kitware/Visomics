
####################################################################################
# OS      :
# Hardware:
# GPU     :
####################################################################################
# WARNING - The specific version and processor type of this machine should be reported
# in the header above. Indeed, this file will be send to the dashboard as a NOTE file.
#
# On linux, you could run:
#     'uname -o' and 'cat /etc/*-release' to obtain the OS name.
#     'uname -mpi' to obtain hardware details.
#     'glxinfo | grep OpenGL' to obtain GPU details.
####################################################################################

cmake_minimum_required(VERSION 2.8.2)

#
# For additional information, see https://www.kitware.com/Omics/index.php/Main_Page#Dashboard_setup
#

#-----------------------------------------------------------------------------
# Dashboard properties
#-----------------------------------------------------------------------------
set(MY_OPERATING_SYSTEM   "Linux") # Windows, Linux, Darwin...
set(MY_COMPILER           "g++4.4.3")
set(MY_QT_VERSION         "4.7")
set(MY_R_VERSION          "2.11.1")
set(QT_QMAKE_EXECUTABLE   "$ENV{HOME}/Dashboards/Support/qtsdk-2010.05/qt/bin/qmake")
set(R_COMMAND             "$ENV{HOME}/Dashboards/Support/R-2.11.1/bin/R")
set(CTEST_SITE            "karakoram.kitware") # for example: mymachine.kitware, mymachine.bwh.harvard.edu, ...
set(CTEST_DASHBOARD_ROOT  "$ENV{HOME}/Dashboards/")
set(CTEST_CMAKE_COMMAND   "$ENV{HOME}/Dashboards/Support/cmake-2.8.4-Linux-i386/bin/cmake")
# Open a shell and type in "cmake --help" to obtain the proper spelling of the generator
set(CTEST_CMAKE_GENERATOR "Unix Makefiles")

#-----------------------------------------------------------------------------
# Attempt to extract dashboard option passed from command line
#-----------------------------------------------------------------------------
# Note: The syntax to pass option from the command line while invoking ctest is
#       the following: ctest -S /path/to/script.cmake,OPTNAME1##OPTVALUE1^^OPTNAME2##OPTVALUE2
#
# Example:
#       ctest -S /path/to/script.cmake,SCRIPT_MODE##continuous^^GIT_TAG##next
#
if(NOT CTEST_SCRIPT_ARG STREQUAL "")
  string(REPLACE "^^" "\\;" CTEST_SCRIPT_ARG_AS_LIST "${CTEST_SCRIPT_ARG}")
  set(CTEST_SCRIPT_ARG_AS_LIST ${CTEST_SCRIPT_ARG_AS_LIST})
  foreach(argn_argv ${CTEST_SCRIPT_ARG_AS_LIST})
    string(REPLACE "##" "\\;" argn_argv_list ${argn_argv})
    set(argn_argv_list ${argn_argv_list})
    list(GET argn_argv_list 0 argn)
    list(GET argn_argv_list 1 argv)
    set(${argn} ${argv})
  endforeach()
endif()

# Macro allowing to set a variable to its default value only if not already defined
macro(setIfNotDefined var defaultvalue)
  if(NOT DEFINED ${var})
    set(${var} "${defaultvalue}")
  endif()
endmacro()

#-----------------------------------------------------------------------------
# Dashboard options
#-----------------------------------------------------------------------------
setIfNotDefined(WITH_KWSTYLE FALSE)
setIfNotDefined(WITH_MEMCHECK FALSE)
setIfNotDefined(WITH_COVERAGE FALSE)
setIfNotDefined(WITH_DOCUMENTATION FALSE)
set(DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY ) # for example: $ENV{HOME}/Projects/Doxygen
setIfNotDefined(WITH_PACKAGES FALSE)
setIfNotDefined(CTEST_BUILD_CONFIGURATION "Release") # Release or Debug
setIfNotDefined(CTEST_TEST_TIMEOUT 100)
setIfNotDefined(CTEST_BUILD_FLAGS "") # Use multiple CPU cores to build. For example "-j4" on unix

# experimental:
#     - run_ctest() macro will be called *ONE* time
#     - binary directory will *NOT* be cleaned
# continuous:
#     - run_ctest() macro will be called EVERY 5 minutes ...
#     - binary directory will *NOT* be cleaned
#     - configure/build will be executed *ONLY* if the repository has been updated
# nightly:
#     - run_ctest() macro will be called *ONE* time
#     - binary directory *WILL BE* cleaned
setIfNotDefined(SCRIPT_MODE "nightly") # "experimental", "continuous", "nightly"

# You could invoke the script with the following syntax:
#  ctest -S karakoram_Visomics_nightly.cmake -C <CTEST_BUILD_CONFIGURATION> -V
#
# Note that '-C <CTEST_BUILD_CONFIGURATION>' is mandatory on windows

#-----------------------------------------------------------------------------
# Additional CMakeCache options
#-----------------------------------------------------------------------------
# For example:
#   ADDITIONAL_{C/CXX}_FLAGS: /MP -> Multi process build with MSVC
#
set(ADDITIONAL_CMAKECACHE_OPTION "
QT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
R_COMMAND:FILEPATH=${R_COMMAND}
")

#-----------------------------------------------------------------------------
# List of test that should be explicitly disabled on this machine
#-----------------------------------------------------------------------------
set(TEST_TO_EXCLUDE_REGEX "")

#-----------------------------------------------------------------------------
# Set any extra environment variables here
#-----------------------------------------------------------------------------
if(UNIX)
  set(ENV{DISPLAY} ":0")
endif()

#-----------------------------------------------------------------------------
# Required executables
#-----------------------------------------------------------------------------
find_program(CTEST_SVN_COMMAND NAMES svn)
find_program(CTEST_GIT_COMMAND NAMES git)
find_program(CTEST_COVERAGE_COMMAND NAMES gcov)
find_program(CTEST_MEMORYCHECK_COMMAND NAMES valgrind)

#-----------------------------------------------------------------------------
# Git tag - Should be pecified without the prefix 'origin/'
#-----------------------------------------------------------------------------
setIfNotDefined(GIT_TAG "master") # master, next, ...

#-----------------------------------------------------------------------------
# Git repository
#-----------------------------------------------------------------------------
setIfNotDefined(GIT_REPOSITORY git://kwsource.kitwarein.com/bioinformatics/visomics.git)
# OR
#set(GIT_REPOSITORY git://kwsource.kitwarein.com/~<YOURUSERNAME>/bioinformatics/<YOURUSERNAME>s-visomics.git)

#-----------------------------------------------------------------------------
# Build Name
#-----------------------------------------------------------------------------
# Note: If you have multiple dashboard scripts checking out different GIT TAG and/or REPOSITORY
#       Make sure you append a suffix to the source directory to differentate them.
setIfNotDefined(BUILD_OPTIONS_STRING "-${GIT_TAG}")

#-----------------------------------------------------------------------------
# Build directory
#-----------------------------------------------------------------------------
# Note: If you have multiple dashboard scripts checking out different GIT TAG and/or REPOSITORY
#       Make sure you append a suffix to the source directory to differentate them.
set(CTEST_BINARY_DIRECTORY "${CTEST_DASHBOARD_ROOT}/Visomics-build-${CTEST_BUILD_CONFIGURATION}-${SCRIPT_MODE}-${GIT_TAG}")

#-----------------------------------------------------------------------------
# Source directory
#-----------------------------------------------------------------------------
# Note: If you have multiple dashboard scripts checking out different GIT TAG and/or REPOSITORY
#       Make sure you append a suffix to the source directory to differentate them.
#
set(CTEST_SOURCE_DIRECTORY "${CTEST_DASHBOARD_ROOT}/Visomics-${GIT_TAG}")

##########################################
# WARNING: DO NOT EDIT BEYOND THIS POINT #
##########################################

set(CTEST_NOTES_FILES "${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}")

#
# Project specific properties
#
set(CTEST_PROJECT_NAME "Visomics")
set(CTEST_BUILD_NAME "${MY_OPERATING_SYSTEM}-${MY_COMPILER}-Qt${MY_QT_VERSION}-R${MY_R_VERSION}${BUILD_OPTIONS_STRING}-${CTEST_BUILD_CONFIGURATION}")

#
# Display build info
#
message("CTEST_SITE ................: ${CTEST_SITE}")
message("CTEST_BUILD_NAME ..........: ${CTEST_BUILD_NAME}")
message("SCRIPT_MODE ...............: ${SCRIPT_MODE}")
message("GIT_TAG ...................: ${GIT_TAG}")
message("CTEST_BUILD_CONFIGURATION .: ${CTEST_BUILD_CONFIGURATION}")
message("WITH_KWSTYLE ..............: ${WITH_KWSTYLE}")
message("WITH_COVERAGE: ............: ${WITH_COVERAGE}")
message("WITH_MEMCHECK .............: ${WITH_MEMCHECK}")
message("WITH_PACKAGES .............: ${WITH_PACKAGES}")
message("WITH_DOCUMENTATION ........: ${WITH_DOCUMENTATION}")
message("DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY: ${DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY}")

#
# Convenient macro allowing to download a file
#
MACRO(downloadFile url dest)
  FILE(DOWNLOAD ${url} ${dest} STATUS status)
  LIST(GET status 0 error_code)
  LIST(GET status 1 error_msg)
  IF(error_code)
    MESSAGE(FATAL_ERROR "error: Failed to download ${url} - ${error_msg}")
  ENDIF()
ENDMACRO()

#
# Download and include dashboard driver script
#
set(url http://kwsource.kitwarein.com/bioinformatics/visomics/blobs/raw/master/CMake/VisomicsDashboardDriverScript.cmake)
set(dest ${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}.driver)
downloadFile(${url} ${dest})
INCLUDE(${dest})
