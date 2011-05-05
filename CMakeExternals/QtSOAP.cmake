  
# Sanity checks
IF(DEFINED QtSOAP_DIR AND NOT EXISTS ${QtSOAP_DIR})
  MESSAGE(FATAL_ERROR "QtSOAP_DIR variable is defined but corresponds to non-existing directory")
ENDIF()

SET(proj QtSOAP)
SET(proj_DEPENDENCIES)

SET(QtSOAP_DEPENDS ${proj})

IF(NOT DEFINED QtSOAP_DIR)
  MESSAGE(STATUS "Adding external project: ${proj}")
  ExternalProject_Add(${proj}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
    BINARY_DIR ${proj}-build
    PREFIX ${proj}${ep_suffix}
    GIT_REPOSITORY "${git_protocol}://github.com/commontk/QtSOAP.git"
    GIT_TAG "origin/master"
    CMAKE_GENERATOR ${gen}
    INSTALL_COMMAND ""
    CMAKE_ARGS
      ${ep_common_args}
      -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
    DEPENDS
      ${proj_DEPENDENCIES}
    )
  SET(QtSOAP_DIR "${ep_build_dir}/${proj}")

ELSE()
  ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
ENDIF()

