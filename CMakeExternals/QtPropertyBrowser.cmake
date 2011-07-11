
# Sanity checks
IF(DEFINED QtPropertyBrowser_DIR AND NOT EXISTS ${QtPropertyBrowser_DIR})
  MESSAGE(FATAL_ERROR "QtPropertyBrowser_DIR variable is defined but corresponds to non-existing directory")
ENDIF()

SET(proj QtPropertyBrowser)
SET(proj_DEPENDENCIES)

SET(QtPropertyBrowser_DEPENDS ${proj})

IF(NOT DEFINED QtPropertyBrowser_DIR)
  MESSAGE(STATUS "Adding external project: ${proj}")
  ExternalProject_Add(${proj}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
    BINARY_DIR ${proj}-build
    PREFIX ${proj}${ep_suffix}
    GIT_REPOSITORY "${git_protocol}://github.com/commontk/QtPropertyBrowser.git"
    GIT_TAG "origin/master"
    CMAKE_GENERATOR ${gen}
    INSTALL_COMMAND ""
    CMAKE_ARGS
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      #-DBUILD_TESTING:BOOL=OFF # Not used
      -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
    DEPENDS
      ${proj_DEPENDENCIES}
    )
  SET(QtPropertyBrowser_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
  
ELSE()
  ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
ENDIF()

