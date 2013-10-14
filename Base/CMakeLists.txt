
PROJECT(VisomicsBase)

SET(KIT_SRCS
  Analysis/voFoldChange.cpp
  Analysis/voFoldChange.h
  Analysis/voANOVAStatistics.cpp
  Analysis/voANOVAStatistics.h
  Analysis/voHierarchicalClustering.h
  Analysis/voHierarchicalClustering.cpp
  Analysis/voKEGGCompounds.h
  Analysis/voKEGGCompounds.cpp
  Analysis/voKEGGPathway.h
  Analysis/voKEGGPathway.cpp
  Analysis/voKMeansClustering.cpp
  Analysis/voKMeansClustering.h
  Analysis/voOneZoom.cpp
  Analysis/voOneZoom.h
  Analysis/voPCAStatistics.cpp
  Analysis/voPCAStatistics.h
  Analysis/voTTest.cpp
  Analysis/voTTest.h
  Analysis/voPLSStatistics.cpp
  Analysis/voPLSStatistics.h
  Analysis/voXCorrel.cpp
  Analysis/voXCorrel.h
  Analysis/voGeigerModelFitting.cpp
  Analysis/voGeigerModelFitting.h
  Analysis/voCustomAnalysis.cpp
  Analysis/voCustomAnalysis.h
  Analysis/voCustomAnalysisData.cpp
  Analysis/voCustomAnalysisData.h
  Analysis/voCustomAnalysisInformation.cpp
  Analysis/voCustomAnalysisInformation.h
  Analysis/voCustomAnalysisParameter.cpp
  Analysis/voCustomAnalysisParameter.h
  Analysis/voCustomAnalysisParameterField.cpp
  Analysis/voCustomAnalysisParameterField.h
  Analysis/voTreeDropTip.cpp
  Analysis/voTreeDropTip.h
  Analysis/voCompareTrees.cpp
  Analysis/voCompareTrees.h
  Analysis/voTreeDropTipWithoutData.cpp
  Analysis/voTreeDropTipWithoutData.h
  Analysis/voRemoteCustomAnalysis.cpp
  Analysis/voRemoteCustomAnalysis.h
  Normalization/voNormalization.h
  Normalization/voLog2.cpp
  Normalization/voQuantile.cpp

  Views/voCorrelationGraphView.cpp
  Views/voCorrelationGraphView.h
  Views/voExtendedTableView.cpp
  Views/voExtendedTableView.h
  Views/voHeatMapView.cpp
  Views/voHeatMapView.h
  Views/voHierarchicalClusteringDynView.cpp
  Views/voHierarchicalClusteringDynView.h
  Views/voHorizontalBarView.h
  Views/voHorizontalBarView.cpp
  Views/voInteractorStyleRubberBand2D.h
  Views/voInteractorStyleRubberBand2D.cpp
  Views/voKEGGImageView.cpp
  Views/voKEGGImageView.h
  Views/voKEGGPathwayView.cpp
  Views/voKEGGPathwayView.h
  Views/voKEGGTableView.cpp
  Views/voKEGGTableView.h
  Views/voKMeansClusteringDynView.cpp
  Views/voKMeansClusteringDynView.h
  Views/voOneZoomDynView.cpp
  Views/voOneZoomDynView.h
  Views/voPCABarView.cpp
  Views/voPCABarView.h
  Views/voPCAProjectionDynView.cpp
  Views/voPCAProjectionDynView.h
  Views/voPCAProjectionView.cpp
  Views/voPCAProjectionView.h
  Views/voTableView.cpp
  Views/voTableView.h
  Views/voTreeGraphView.h
  Views/voTreeGraphView.cpp
  Views/voTreeHeatmapView.h
  Views/voTreeHeatmapView.cpp
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
  voDynView.cpp
  voDynView.h
  voJavascriptBridge.cpp
  voJavascriptBridge.h
  voInputFileDataObject.cpp
  voInputFileDataObject.h
  voIOManager.cpp
  voIOManager.h
  voKEGGUtils.cpp
  voKEGGUtils.h
  voQObjectFactory.h
  voOutputDataObject.cpp
  voOutputDataObject.h
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
  Analysis/voANOVAStatistics.h
  Analysis/voHierarchicalClustering.h
  Analysis/voKEGGCompounds.h
  Analysis/voKEGGPathway.h
  Analysis/voKMeansClustering.h
  Analysis/voOneZoom.h
  Analysis/voPCAStatistics.h
  Analysis/voTTest.h
  Analysis/voPLSStatistics.h
  Analysis/voXCorrel.h
  Analysis/voGeigerModelFitting.h
  Analysis/voCompareTrees.h
  Analysis/voCustomAnalysis.h
  Analysis/voCustomAnalysisData.h
  Analysis/voCustomAnalysisInformation.h
  Analysis/voCustomAnalysisParameter.h
  Analysis/voCustomAnalysisParameterField.h
  Analysis/voTreeDropTip.h
  Analysis/voTreeDropTipWithoutData.h
  Analysis/voRemoteCustomAnalysis.h


  Views/voCorrelationGraphView.h
  Views/voExtendedTableView.h
  Views/voHeatMapView.h
  Views/voHierarchicalClusteringDynView.h
  Views/voHorizontalBarView.h
  Views/voKEGGImageView.h
  Views/voKEGGPathwayView.h
  Views/voKEGGTableView.h
  Views/voKMeansClusteringDynView.h
  Views/voOneZoomDynView.h
  Views/voPCABarView.h
  Views/voPCAProjectionDynView.h
  Views/voPCAProjectionView.h
  Views/voTableView.h
  Views/voTreeGraphView.h
  Views/voTreeHeatmapView.h
  Views/voVolcanoView.h


  voAnalysis.h
  voAnalysisDriver.h
  voApplication.h
  voDataModel.h
  voDataModel_p.h
  voDataObject.h
  voDynView.h
  voJavascriptBridge.h
  voInputFileDataObject.h
  voOutputDataObject.h
  voTableDataObject.h
  voView.h
  voViewManager.h
  )

SET(KIT_UI_FORMS
  )

IF(USE_ARBOR_BRAND)
  SET(KIT_resources Resources/ArborBase.qrc)
ELSE()
  SET(KIT_resources Resources/VisomicsBase.qrc)
ENDIF()

include(CheckIncludeFile)
CHECK_INCLUDE_FILE(unistd.h HAVE_UNISTD_H)
if(HAVE_UNISTD_H)
  add_definitions(-DHAVE_UNISTD_H)
endif()

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

SET(${PROJECT_NAME}_LINK_LIBRARIES
  ${VTK_LIBRARIES}
  ${EXTRA_LIBRARIES}
  ${QT_LIBRARIES}
  CTKLib
  QtPropertyBrowser
  )
TARGET_LINK_LIBRARIES(
  ${libname}
  ${${PROJECT_NAME}_LINK_LIBRARIES}
  )

# Install rules
INSTALL(DIRECTORY Views/Resources/dynamicviews
  DESTINATION ${Visomics_INSTALL_SHARE_DIR} COMPONENT Runtime)

IF(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
  ADD_SUBDIRECTORY(Analysis/Testing)
  ADD_SUBDIRECTORY(Normalization/Testing)
ENDIF()
