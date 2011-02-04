
SET(KIT_SRCS
  voAnalysis.cpp
  voAnalysis.h
  voAnalysisDriver.cpp
  voAnalysisDriver.h
  voAnalysisFactory.cpp
  voAnalysisFactory.h
  voApplication.cpp
  voApplication.h
  voCorrelationGraphView.cpp
  voCorrelationGraphView.h
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
  voPCAProjectionPlot.cpp
  voPCAProjectionPlot.h
  voPCABarPlot.cpp
  voPCABarPlot.h
  voPCAStatistics.cpp
  voPCAStatistics.h
  voQObjectFactory.h
  voTableDataObject.cpp
  voTableDataObject.h
  voTableView.cpp
  voTableView.h
  voView.cpp
  voView.h
  voViewFactory.cpp
  voViewFactory.h
  voViewManager.cpp
  voViewManager.h
  voViewTabWidget.cpp
  voViewTabWidget.h
  voXCorrel.cpp
  voXCorrel.h
  voKMeansClustering.cpp
  voKMeansClustering.h
  voTreeGraphView.h
  voTreeGraphView.cpp
  voHierarchicalClustering.h
  voHierarchicalClustering.cpp
  )
  
SET(KIT_MOC_SRCS
  voAnalysis.h
  voAnalysisDriver.h
  voApplication.h
  voDataBrowserWidget.h
  voDataModel.h
  voDataModel_p.h
  voDataObject.h
  voCorrelationGraphView.h
  voInputFileDataObject.h
  voMainWindow.h
  voPCAProjectionPlot.h
  voPCABarPlot.h
  voPCAStatistics.h
  voTableDataObject.h
  voTableView.h
  voView.h
  voViewManager.h
  voXCorrel.h
  voViewTabWidget.h
  voKMeansClustering.h
  voHierarchicalClustering.h
  voTreeGraphView.h
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

