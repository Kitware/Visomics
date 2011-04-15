
PROJECT(VisomicsBase)

SET(KIT_SRCS
  Analysis/voHierarchicalClustering.h
  Analysis/voHierarchicalClustering.cpp
  Analysis/voKEGG.h
  Analysis/voKEGG.cpp
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
  Views/voCorrelationHeatMapView.cpp
  Views/voCorrelationHeatMapView.h
  Views/voChartXY.h
  Views/voChartXY.cpp

  
  voAnalysis.cpp
  voAnalysis.h
  voAnalysisDriver.cpp
  voAnalysisDriver.h
  voAnalysisFactory.cpp
  voAnalysisFactory.h
  voApplication.cpp
  voApplication.h
  voCSVReader.cpp
  voCSVReader.h
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
  voQObjectFactory.h
  voTableDataObject.cpp
  voTableDataObject.h
  voView.cpp
  voView.h
  voViewFactory.cpp
  voViewFactory.h
  voViewManager.cpp
  voViewManager.h
  )
  
SET(KIT_MOC_SRCS
  Analysis/voHierarchicalClustering.h
  Analysis/voKEGG.h
  Analysis/voKMeansClustering.h
  Analysis/voPCAStatistics.h
  Analysis/voXCorrel.h
  
  Views/voCorrelationGraphView.h
  Views/voPCABarPlot.h
  Views/voPCAProjectionPlot.h
  Views/voTableView.h
  Views/voTreeGraphView.h
  Views/voCorrelationHeatMapView.h
  
  voAnalysis.h
  voAnalysisDriver.h
  voApplication.h
  voDataModel.h
  voDataModel_p.h
  voDataObject.h
  voInputFileDataObject.h
  voTableDataObject.h
  voView.h
  voViewManager.h
  )
  
SET(KIT_UI_FORMS
  )
  
SET(KIT_resources
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

SET(EXTRA_LIBRARIES)
IF(Q_WS_X11)
  # If the faster 'gold' linker is used, to avoid complaints about undefined symbol
  # 'XGetWindowAttributes', 'XCreateWindow', ..., let's link against X11 libraries.
  FIND_PACKAGE(X11)
  LIST(APPEND EXTRA_LIBRARIES ${X11_LIBRARIES})
ENDIF()

IF(UNIX)
  # If the faster 'gold' linker is used, to avoid complaints about undefined symbol
  # '_gfortran_concat_string', '_gfortran_pow_i4_i4', ..., let's link against gfortran libraries.
  # These errors happen while linking against VTK static built with R support
  SET(CMAKE_FIND_LIBRARY_SUFFIXES_SAVED ${CMAKE_FIND_LIBRARY_SUFFIXES}) # Backup
  LIST(APPEND CMAKE_FIND_LIBRARY_SUFFIXES ".so.3")
  FIND_LIBRARY(GFortran_LIBRARY gfortran)
  SET(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES_SAVED}) # Restore
  LIST(APPEND EXTRA_LIBRARIES ${GFortran_LIBRARY})
ENDIF()

SET(VTK_LIBRARIES QVTK vtkCharts vtkInfovis)

SET(${PROJECT_NAME}_LINK_LIBRARIES
  ${VTK_LIBRARIES}
  ${EXTRA_LIBRARIES}
  ${QT_LIBRARIES}
  QtPropertyBrowser
  )
TARGET_LINK_LIBRARIES(
  ${libname}
  ${${PROJECT_NAME}_LINK_LIBRARIES}
  )
  
IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
ENDIF()

