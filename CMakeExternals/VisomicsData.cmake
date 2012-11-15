
#
# VisomicsData
#

if(BUILD_TESTING)

  # Sanity checks
  if(DEFINED VisomicsData_DIR AND NOT EXISTS ${VisomicsData_DIR})
    message(FATAL_ERROR "VisomicsData_DIR variable is defined but corresponds to non-existing directory")
  endif()

  SET(proj VisomicsData)
  set(proj_DEPENDENCIES)

  SET(VisomicsData_DEPENDS ${proj})

  IF(NOT DEFINED VisomicsData_DIR)
    MESSAGE(STATUS "Adding external project: ${proj}")
    ExternalProject_Add(${proj}
      GIT_REPOSITORY ${git_protocol}://visomics.org/visomicsdata.git
      GIT_TAG "origin/master"
      SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
      BINARY_DIR ${proj}-build
      PREFIX ${proj}${ep_suffix}
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
      DEPENDS
        ${proj_DEPENDENCIES}
      )
    SET(VisomicsData_DIR ${CMAKE_BINARY_DIR}/${proj})
  ELSE()
    ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
  ENDIF()
ENDIF()
