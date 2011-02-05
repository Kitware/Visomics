
SET(KIT_SRCS
  Analysis/voHierarchicalClustering.h
  Analysis/voHierarchicalClustering.cpp
  Analysis/voKMeansClustering.cpp
  Analysis/voKMeansClustering.h
  Analysis/voPCAStatistics.cpp
  Analysis/voPCAStatistics.h
  Analysis/voXCorrel.cpp
  Analysis/voXCorrel.h
  
  Views/voCorrelationGraphView.cpp
  Views/voCorrelationGraphView.h
  Views/voPCABarPlot.cpp
  Views/voPCABarPlot.h
  Views/voPCAProjectionPlot.cpp
  Views/voPCAProjectionPlot.h
  Views/voTableView.cpp
  Views/voTableView.h
  Views/voTreeGraphView.h
  Views/voTreeGraphView.cpp
  
  voAnalysis.cpp
  voAnalysis.h
  voAnalysisDriver.cpp
  voAnalysisDriver.h
  voAnalysisFactory.cpp
  voAnalysisFactory.h
  voAnalysisParameterDialog.cpp
  voAnalysisParameterDialog.h
  voAnalysisParameterEditorWidget.cpp
  voAnalysisParameterEditorWidget.h
  voApplication.cpp
  voApplication.h
  voCSVReader.cpp
  voCSVReader.h
  voDataBrowserWidget.cpp
  voDataBrowserWidget.h
  voDataModel.cpp
  voDataModel.h
  voDataModel_p.h
  voDataModelItem.cpp
  voDataModelItem.h
  voDataObject.cpp
  voDataObject.h
  voInputFileDataObject.cpp
  voInputFileDataObject.h
  voIOManager.cpp
  voIOManager.h
  voMainWindow.cpp
  voMainWindow.h
  voQObjectFactory.h
  voTableDataObject.cpp
  voTableDataObject.h
  voView.cpp
  voView.h
  voViewFactory.cpp
  voViewFactory.h
  voViewManager.cpp
  voViewManager.h
  voViewTabWidget.cpp
  voViewTabWidget.h
  )
  
SET(KIT_MOC_SRCS
  Analysis/voHierarchicalClustering.h
  Analysis/voKMeansClustering.h
  Analysis/voPCAStatistics.h
  Analysis/voXCorrel.h
  
  Views/voCorrelationGraphView.h
  Views/voPCABarPlot.h
  Views/voPCAProjectionPlot.h
  Views/voTableView.h
  Views/voTreeGraphView.h
  
  voAnalysis.h
  voAnalysisDriver.h
  voAnalysisParameterDialog.h
  voAnalysisParameterEditorWidget.h
  voApplication.h
  voDataBrowserWidget.h
  voDataModel.h
  voDataModel_p.h
  voDataObject.h
  voInputFileDataObject.h
  voMainWindow.h
  voTableDataObject.h
  voView.h
  voViewManager.h
  voViewTabWidget.h
  )
  
SET(KIT_UI_FORMS
  Resources/UI/voMainWindow.ui
  )
  
SET(KIT_resources
  Resources/Visomics.qrc
  )
  
INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Analysis
  ${CMAKE_CURRENT_BINARY_DIR}/Analysis
  ${CMAKE_CURRENT_SOURCE_DIR}/Views
  ${CMAKE_CURRENT_BINARY_DIR}/Views
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

SET(VTK_LIBRARIES QVTK vtkCharts vtkInfovis)

SET(${PROJECT_NAME}_LINK_LIBRARIES
  ${VTK_LIBRARIES}
  ${QT_LIBRARIES}
  QtPropertyBrowser
  )
TARGET_LINK_LIBRARIES(
  ${libname}
  ${${PROJECT_NAME}_LINK_LIBRARIES}
  )

SET(Visomics_LIBRARY_DIRS
  ${VTK_DIR}/bin
  ${R_LIBRARY_DIR}
  )

CONFIGURE_FILE(
  ${Visomics_SOURCE_DIR}/CMake/visomics-forward.c.in
  ${CMAKE_CURRENT_BINARY_DIR}/visomics-forward.c
  @ONLY IMMEDIATE)

# Add forward executable
ADD_EXECUTABLE(${PROJECT_NAME}
  ${CMAKE_CURRENT_BINARY_DIR}/visomics-forward.c)
ADD_DEPENDENCIES(${PROJECT_NAME} ${PROJECT_NAME}-real)

# Add regular application
ADD_EXECUTABLE(${PROJECT_NAME}-real Main.cpp)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}-real ${libname})

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

