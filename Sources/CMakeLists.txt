
SET(KIT_SRCS
  Main.cpp
  voAnalysis.cpp
  voAnalysis.h
  voAnalysisPanel.cxx
  voAnalysisPanel.h
  voCorrelationGraphView.cpp
  voCorrelationGraphView.h
  voCSVReader.cpp
  voCSVReader.h
  voDataBrowserWidget.cpp
  voDataBrowserWidget.h
  voMainWindow.cpp
  voMainWindow.h
  voPCA.cpp
  voPCA.h
  voPCAProjectionPlot.cpp
  voPCAProjectionPlot.h
  voPCAStatistics.cpp
  voPCAStatistics.h
  voPort.cpp
  voPort.h
  voTableView.cpp
  voTableView.h
  voView.cpp
  voView.h
  )
  
SET(KIT_MOC_SRCS
  voAnalysisPanel.h
  voDataBrowserWidget.h
  voMainWindow.h
  )
  
SET(KIT_UI_FORMS
  voAnalysisPanel.ui
  voMainWindow.ui
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

ADD_EXECUTABLE(${PROJECT_NAME}
  ${KIT_SRCS}
  ${KIT_UI_CPP}
  ${KIT_MOC_CPP}
  ${KIT_QRC_SRCS}
  )

SET(VTK_LIBRARIES QVTK vtkCharts vtkInfovis)

TARGET_LINK_LIBRARIES(
  ${PROJECT_NAME}
  ${VTK_LIBRARIES}
  ${QT_LIBRARIES}
  )

