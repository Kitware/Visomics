
PROJECT(CTK)

# Source files
SET(KIT_SRCS
  # Core
  ctkCallback.cpp
  ctkCallback.h
  ctkErrorLogModel.cpp
  ctkErrorLogModel.h
  ctkErrorLogQtMessageHandler.cpp
  ctkErrorLogQtMessageHandler.h
  ctkErrorLogStreamMessageHandler.cpp
  ctkErrorLogStreamMessageHandler.h
  ctkPimpl.h
  # Widgets
  ctkErrorLogWidget.cpp
  ctkErrorLogWidget.h
  # Visualization/VTK/Core
  ctkVTKErrorLogMessageHandler.cpp
  ctkVTKErrorLogMessageHandler.h
  )

# Headers that should run through moc
SET(KIT_MOC_SRCS
  # Core
  ctkCallback.h
  ctkErrorLogModel.h
  # Widgets
  ctkErrorLogWidget.h
  )

# UI files
SET(KIT_UI_FORMS
  # Widgets
  Resources/UI/ctkErrorLogWidget.ui
  )

# Resources
SET(KIT_resources
#  Resources/ctkWidgets.qrc
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

SET(${PROJECT_NAME}_LINK_LIBRARIES
  ${QT_LIBRARIES}
  ${VTK_LIBRARIES}
  )

TARGET_LINK_LIBRARIES(
  ${libname}
  ${${PROJECT_NAME}_LINK_LIBRARIES}
  )
