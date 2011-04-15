
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
set(QT_QMAKE_EXECUTABLE   "$ENV{HOME}/Dashboards/Support/qtsdk-2010.05/qt/bin/qmake")
set(R_COMMAND             "$ENV{HOME}/Dashboards/Support/R-2.11.1/bin/R")
set(CTEST_SITE            "karakoram.kitware") # for example: mymachine.kitware, mymachine.bwh.harvard.edu, ...
set(CTEST_DASHBOARD_ROOT  "$ENV{HOME}/Dashboards/")
set(CTEST_CMAKE_COMMAND   "$ENV{HOME}/Dashboards/Support/cmake-2.8.4-Linux-i386/bin/cmake")
# Open a shell and type in "cmake --help" to obtain the proper spelling of the generator
set(CTEST_CMAKE_GENERATOR "Unix Makefiles")

#-----------------------------------------------------------------------------
# Dashboard options
#-----------------------------------------------------------------------------
set(WITH_KWSTYLE FALSE)
set(WITH_MEMCHECK FALSE)
set(WITH_COVERAGE FALSE)
set(WITH_DOCUMENTATION FALSE)
#set(DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY ) # for example: $ENV{HOME}/Projects/Doxygen
set(WITH_PACKAGES FALSE)
set(CTEST_BUILD_CONFIGURATION "Release") # Release or Debug
set(CTEST_TEST_TIMEOUT 100)
set(CTEST_BUILD_FLAGS "") # Use multiple CPU cores to build. For example "-j4" on unix

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
if(NOT DEFINED SCRIPT_MODE)
  set(SCRIPT_MODE "nightly") # "experimental", "continuous", "nightly"
endif()

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
if(NOT DEFINED GIT_TAG)
  set(GIT_TAG "master") # master, next, ...
endif()

#-----------------------------------------------------------------------------
# Git repository
#-----------------------------------------------------------------------------
set(GIT_REPOSITORY git://kwsource.kitwarein.com/bioinformatics/visomics.git)
# OR
#set(GIT_REPOSITORY git://kwsource.kitwarein.com/~<YOURUSERNAME>/bioinformatics/<YOURUSERNAME>s-visomics.git)

#-----------------------------------------------------------------------------
# Build Name
#-----------------------------------------------------------------------------
# Note: If you have multiple dashboard scripts checking out different GIT TAG and/or REPOSITORY
#       Make sure you append a suffix to the source directory to differentate them.
set(BUILD_OPTIONS_STRING "Qt4.7-R-2.11.1-${GIT_TAG}")

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
set(CTEST_BUILD_NAME "${MY_OPERATING_SYSTEM}-${MY_COMPILER}-QT${MY_QT_VERSION}-${BUILD_OPTIONS_STRING}-${CTEST_BUILD_CONFIGURATION}")

#
# Display build info
#
message("site name: ${CTEST_SITE}")
message("build name: ${CTEST_BUILD_NAME}")
message("script mode: ${SCRIPT_MODE}")
message("coverage: ${WITH_COVERAGE}, memcheck: ${WITH_MEMCHECK}")

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
