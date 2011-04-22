
PROJECT(VisomicsBase)

SET(KIT_SRCS
  Analysis/voFoldChange.cpp
  Analysis/voFoldChange.h
  Analysis/voHierarchicalClustering.h
  Analysis/voHierarchicalClustering.cpp
  Analysis/voKEGG.h
  Analysis/voKEGG.cpp
  Analysis/voKMeansClustering.cpp
  Analysis/voKMeansClustering.h
  Analysis/voPCAStatistics.cpp
  Analysis/voPCAStatistics.h
  Analysis/voTTest.cpp
  Analysis/voTTest.h
  Analysis/voPLSStatistics.cpp
  Analysis/voPLSStatistics.h
  Analysis/voXCorrel.cpp
  Analysis/voXCorrel.h

  Normalization/voNormalization.h
  Normalization/voLog2.cpp
  Normalization/voQuantile.cpp
  
  Views/voCorrelationGraphView.cpp
  Views/voCorrelationGraphView.h
  Views/voCorrelationHeatMapView.cpp
  Views/voCorrelationHeatMapView.h
  Views/voExtendedTableView.cpp
  Views/voExtendedTableView.h
  Views/voHorizontalBarView.h
  Views/voHorizontalBarView.cpp
  Views/voInteractorStyleRubberBand2D.h
  Views/voInteractorStyleRubberBand2D.cpp
  Views/voPCABarView.cpp
  Views/voPCABarView.h
  Views/voPCAProjectionView.cpp
  Views/voPCAProjectionView.h
  Views/voTableView.cpp
  Views/voTableView.h
  Views/voTreeGraphView.h
  Views/voTreeGraphView.cpp
  Views/voVolcanoView.cpp
  Views/voVolcanoView.h
  
  voAnalysis.cpp
  voAnalysis.h
  voAnalysisDriver.cpp
  voAnalysisDriver.h
  voAnalysisFactory.cpp
  voAnalysisFactory.h
  voApplication.cpp
  voApplication.h
  voDataModel.cpp
  voDataModel.h
  voDataModel_p.h
  voDataModelItem.cpp
  voDataModelItem.h
  voDataObject.cpp
  voDataObject.h
  voDelimitedTextImportSettings.cpp
  voDelimitedTextImportSettings.h
  voInputFileDataObject.cpp
  voInputFileDataObject.h
  voIOManager.cpp
  voIOManager.h
  voQObjectFactory.h
  voRegistry.cpp
  voRegistry.h
  voTableDataObject.cpp
  voTableDataObject.h
  voUtils.cpp
  voUtils.h
  voView.cpp
  voView.h
  voViewFactory.cpp
  voViewFactory.h
  voViewManager.cpp
  voViewManager.h

  vtkExtendedTable.cpp
  vtkExtendedTable.h
  )
  
SET(KIT_MOC_SRCS
  Analysis/voFoldChange.h
  Analysis/voHierarchicalClustering.h
  Analysis/voKEGG.h
  Analysis/voKMeansClustering.h
  Analysis/voPCAStatistics.h
  Analysis/voTTest.h
  Analysis/voPLSStatistics.h
  Analysis/voXCorrel.h
  
  Views/voCorrelationGraphView.h
  Views/voCorrelationHeatMapView.h
  Views/voExtendedTableView.h
  Views/voHorizontalBarView.h
  Views/voPCABarView.h
  Views/voPCAProjectionView.h
  Views/voTableView.h
  Views/voTreeGraphView.h
  Views/voVolcanoView.h
  
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
  ${CMAKE_CURRENT_SOURCE_DIR}/Normalization
  ${CMAKE_CURRENT_BINARY_DIR}/Normalization
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

IF(UNIX AND NOT APPLE)
  # If the faster 'gold' linker is used, to avoid complaints about undefined symbol
  # '_gfortran_concat_string', '_gfortran_pow_i4_i4', ..., let's link against gfortran libraries.
  # These errors happen while linking against VTK static built with R support
  SET(CMAKE_FIND_LIBRARY_SUFFIXES_SAVED ${CMAKE_FIND_LIBRARY_SUFFIXES}) # Backup
  LIST(APPEND CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".so.1" ".so.2" ".so.3")
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
  ADD_SUBDIRECTORY(Analysis/Testing)
  ADD_SUBDIRECTORY(Normalization/Testing)
ENDIF()

