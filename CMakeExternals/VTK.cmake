#
# VTK
#

# Sanity checks
IF(DEFINED VTK_DIR AND NOT EXISTS ${VTK_DIR})
  MESSAGE(FATAL_ERROR "VTK_DIR variable is defined but corresponds to non-existing directory")
ENDIF()

SET(proj VTK)
SET(proj_DEPENDENCIES)

SET(VTK_MODULE_ARGS)
FOREACH(VTK_LIBRARY ${VTK_LIBRARIES})
  LIST(APPEND VTK_MODULE_ARGS -DModule_${VTK_LIBRARY}:BOOL=ON )
ENDFOREACH()

IF(NOT DEFINED VTK_DIR)
  MESSAGE(STATUS "Adding external project: ${proj}")
  ExternalProject_Add(${proj}
    GIT_REPOSITORY ${git_protocol}://vtk.org/VTK.git
    GIT_TAG "origin/master"
    INSTALL_COMMAND ""
    SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
    BINARY_DIR ${proj}-build
    PREFIX ${proj}${ep_suffix}
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DBUILD_EXAMPLES:BOOL=OFF
      -DBUILD_SHARED_LIBS:BOOL=OFF
      -DBUILD_TESTING:BOOL=OFF
      -DVTK_LEGACY_REMOVE:BOOL=ON
      -DVTK_USE_TK:BOOL=OFF
      -DVTK_WRAP_JAVA:BOOL=OFF
      -DVTK_WRAP_PYTHON:BOOL=OFF
      -DVTK_WRAP_TCL:BOOL=OFF
      # Modules
      -DVTK_Group_StandAlone:BOOL=OFF
      ${VTK_MODULE_ARGS}
      # Module options - vtkFiltersStatisticsGnuR
      -DR_COMMAND:PATH=${R_COMMAND}
      -DR_HOME:PATH=${R_HOME}
      -DR_INCLUDE_DIR:PATH=${R_INCLUDE_DIR}
      -DR_LIBRARY_BASE:FILEPATH=${R_LIBRARY_BASE}
      -DR_LIBRARY_BLAS:FILEPATH=${R_LIBRARY_BLAS}
      -DR_LIBRARY_LAPACK:FILEPATH=${R_LIBRARY_LAPACK}
      -DR_LIBRARY_READLINE:FILEPATH=${R_LIBRARY_READLINE}
      # Module options - vtkViewsQt
      -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
    DEPENDS
      ${proj_DEPENDENCIES}
    )
  SET(VTK_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

ELSE()
  # The project is provided using VTK_DIR, nevertheless since other project may depend on VTK,
  # let's add an 'empty' one
  ctkMacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")
ENDIF()
