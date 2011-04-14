#
# VTK
#

# Sanity checks
IF(DEFINED VTK_DIR AND NOT EXISTS ${VTK_DIR})
  MESSAGE(FATAL_ERROR "VTK_DIR variable is defined but corresponds to non-existing directory")
ENDIF()

SET(proj VTK)
SET(proj_DEPENDENCIES)

SET(VTK_DEPENDS ${proj})

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
      ${ep_common_args}
      -DVTK_USE_GNU_R:BOOL=ON
      -DR_COMMAND:PATH=${R_COMMAND}
      -DR_INCLUDE_DIR:PATH=${R_INCLUDE_DIR} # Note: VTK/FindR should be fixed upstream to auto discover this value based on R_COMMAND
      -DVTK_USE_N_WAY_ARRAYS:BOOL=ON # Required for using R interface
      -DVTK_USE_INFOVIS:BOOL=ON # Required for using R interface
      -DBUILD_EXAMPLES:BOOL=OFF
      -DVTK_WRAP_TCL:BOOL=OFF
      -DVTK_USE_TK:BOOL=OFF
      -DVTK_WRAP_PYTHON:BOOL=OFF
      -DVTK_WRAP_JAVA:BOOL=OFF
      -DBUILD_SHARED_LIBS:BOOL=OFF
      -DVTK_LEGACY_REMOVE:BOOL=ON
      -DVTK_USE_GUISUPPORT:BOOL=ON
      -DVTK_USE_QVTK_QTOPENGL:BOOL=ON
      -DVTK_USE_QT:BOOL=ON
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

