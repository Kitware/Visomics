
PROJECT(VisomicsApp)

SET(KIT_SRCS  
  voAnalysisParameterDialog.cpp
  voAnalysisParameterDialog.h
  voAnalysisParameterEditorWidget.cpp
  voAnalysisParameterEditorWidget.h
  voDataBrowserWidget.cpp
  voDataBrowserWidget.h
  voDelimitedTextImportDialog.cpp
  voDelimitedTextImportDialog.h
  voDelimitedTextImportWidget.cpp
  voDelimitedTextImportWidget.h
  voDelimitedTextPreview.cpp
  voDelimitedTextPreview.h
  voDelimitedTextPreviewModel.cpp
  voDelimitedTextPreviewModel.h
  voMainWindow.cpp
  voMainWindow.h
  voNormalizationWidget.cpp
  voNormalizationWidget.h
  voViewTabWidget.cpp
  voViewTabWidget.h
  voViewStackedWidget.cpp
  voViewStackedWidget.h
  )
  
SET(KIT_MOC_SRCS
  voAnalysisParameterDialog.h
  voAnalysisParameterEditorWidget.h
  voDataBrowserWidget.h
  voDelimitedTextImportDialog.h
  voDelimitedTextImportWidget.h
  voDelimitedTextPreview.h
  voDelimitedTextPreviewModel.h
  voMainWindow.h
  voNormalizationWidget.h
  voViewStackedWidget.h
  voViewTabWidget.h
  )
  
SET(KIT_UI_FORMS
  Resources/UI/voMainWindow.ui
  Resources/UI/voDelimitedTextImportDialog.ui
  Resources/UI/voDelimitedTextImportWidget.ui
  )
  
SET(KIT_resources
  Resources/VisomicsApp.qrc
  )
  
INCLUDE_DIRECTORIES(
  ${Visomics_SOURCE_DIR}/Base
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )
  
QT4_WRAP_UI(KIT_UI_CPP ${KIT_UI_FORMS})
QT4_WRAP_CPP(KIT_MOC_CPP ${KIT_MOC_SRCS} )
QT4_ADD_RESOURCES(KIT_QRC_SRCS ${KIT_resources})

SET(libname ${PROJECT_NAME}Lib)
ADD_LIBRARY(${libname} STATIC
  ${KIT_SRCS}
  ${KIT_UI_CPP}
  ${KIT_MOC_CPP}
  ${KIT_QRC_SRCS}
  )

SET(${PROJECT_NAME}_LINK_LIBRARIES
  VisomicsBaseLib
  )
TARGET_LINK_LIBRARIES(
  ${libname}
  ${${PROJECT_NAME}_LINK_LIBRARIES}
  )


# Set Visomics_FORWARD_PATH_BUILD
set(tmp "\".\" CONFIG_DIR_POST")
if(WIN32)
  set(tmp "${tmp}, \"${QT_BINARY_DIR}\"")
else()
  set(tmp "${tmp}, \"${QT_LIBRARY_DIR}\"")
endif()
set(Visomics_FORWARD_PATH_BUILD ${tmp})

CONFIGURE_FILE(
  ${Visomics_SOURCE_DIR}/CMake/visomics-forward.c.in
  ${CMAKE_CURRENT_BINARY_DIR}/visomics-forward.c
  @ONLY IMMEDIATE)

# Add forward executable
ADD_EXECUTABLE(${PROJECT_NAME}
  ${CMAKE_CURRENT_BINARY_DIR}/visomics-forward.c)
ADD_DEPENDENCIES(${PROJECT_NAME} ${PROJECT_NAME}-real)
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES OUTPUT_NAME visomics)

# Add regular application
ADD_EXECUTABLE(${PROJECT_NAME}-real Main.cpp)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}-real ${libname})
SET_TARGET_PROPERTIES(${PROJECT_NAME}-real PROPERTIES OUTPUT_NAME visomics-real)

# Install rules
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${Visomics_INSTALL_BIN_DIR} COMPONENT Runtime)
INSTALL(TARGETS ${PROJECT_NAME}-real DESTINATION ${Visomics_INSTALL_LIB_DIR} COMPONENT Runtime)

#ADD_EXECUTABLE(${PROJECT_NAME}
#  Main.cpp
#  )
#TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${libname})

#INSTALL(TARGETS ${PROJECT_NAME}
#  RUNTIME DESTINATION ${Visomics_INSTALL_LIB_DIR} COMPONENT Runtime
#  LIBRARY DESTINATION ${Visomics_INSTALL_LIB_DIR} COMPONENT Runtime
#  ARCHIVE DESTINATION ${Visomics_INSTALL_LIB_DIR} COMPONENT Development)
  
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF()

