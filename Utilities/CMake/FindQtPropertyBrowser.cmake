###########################################################################
#
#  Library:   CTK
#
#  Copyright (c) Kitware Inc.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.commontk.org/LICENSE
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
###########################################################################

# - Find a QtPropertyBrowser installation or build tree.
# The following variables are set if QtPropertyBrowser is found.  If QtPropertyBrowser is not
# found, QtPropertyBrowser_FOUND is set to false.
#  QtPropertyBrowser_FOUND         - Set to true when QtPropertyBrowser is found.
#  QtPropertyBrowser_USE_FILE      - CMake file to use QtPropertyBrowser.
#  QtPropertyBrowser_INCLUDE_DIRS  - Include directories for QtPropertyBrowser
#  QtPropertyBrowser_LIBRARY_DIRS  - Link directories for QtPropertyBrowser libraries
#
# The following cache entries must be set by the user to locate QtPropertyBrowser:
#  QtPropertyBrowser_DIR  - The directory containing QtPropertyBrowserConfig.cmake.
#                           This is either the root of the build tree,
#                           or the lib/QtPropertyBrowser directory.  This is the
#                           only cache entry.
#

# Assume not found.
SET(QtPropertyBrowser_FOUND 0)

# Construct consitent error messages for use below.
SET(QtPropertyBrowser_DIR_MESSAGE "QtPropertyBrowser not found.  Set the QtPropertyBrowser_DIR cmake cache entry to the directory containing QtPropertyBrowserConfig.cmake.  This is either the root of the build tree, or PREFIX/lib/QtPropertyBrowser for an installation.")

# Use the Config mode of the find_package() command to find QtPropertyBrowserConfig.
# If this succeeds (possibly because QtPropertyBrowser_DIR is already set), the
# command will have already loaded QtPropertyBrowserConfig.cmake and set QtPropertyBrowser_FOUND.
IF(NOT QtPropertyBrowser_FOUND)
  FIND_PACKAGE(QtPropertyBrowser QUIET NO_MODULE)
ENDIF()

#-----------------------------------------------------------------------------
IF(NOT QtPropertyBrowser_FOUND)
  # QtPropertyBrowser not found, explain to the user how to specify its location.
  IF(QtPropertyBrowser_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR ${QtPropertyBrowser_DIR_MESSAGE})
  ELSE(QtPropertyBrowser_FIND_REQUIRED)
    IF(NOT QtPropertyBrowser_FIND_QUIETLY)
      MESSAGE(STATUS ${QtPropertyBrowser_DIR_MESSAGE})
    ENDIF(NOT QtPropertyBrowser_FIND_QUIETLY)
  ENDIF(QtPropertyBrowser_FIND_REQUIRED)
ENDIF()

