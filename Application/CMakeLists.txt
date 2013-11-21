project(VisomicsApp)

set(KIT_SRCS
  voAnalysisParameterDialog.cpp
  voAnalysisParameterDialog.h
  voAnalysisParameterEditorWidget.cpp
  voAnalysisParameterEditorWidget.h
  voDataBrowserWidget.cpp
  voDataBrowserWidget.h
  voDataPropertyWidget.cpp
  voDataPropertyWidget.h
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
  voMongoLoadDialog.cpp
  voMongoLoadDialog.h
  voMongoSaveDialog.cpp
  voMongoSaveDialog.h
  voOpenTreeLoadDialog.cpp
  voOpenTreeLoadDialog.h
  voNormalizationWidget.cpp
  voNormalizationWidget.h
  voStartupView.cpp
  voStartupView.h
  voViewTabWidget.cpp
  voViewTabWidget.h
  voViewStackedWidget.cpp
  voViewStackedWidget.h
  voRemoteAnalysisConnectionDialog.cpp
  )
if(USE_ARBOR_BRAND)
  set(KIT_SRCS  "${KIT_SRCS};voAboutDialog_Arbor.cpp;voAboutDialog_Arbor.h")
else()
  set(KIT_SRCS  "${KIT_SRCS};voAboutDialog.cpp;voAboutDialog.h")
endif()

set(KIT_MOC_SRCS
  voAboutDialog.h
  voAnalysisParameterDialog.h
  voAnalysisParameterEditorWidget.h
  voDataBrowserWidget.h
  voDataPropertyWidget.h
  voDelimitedTextImportDialog.h
  voDelimitedTextImportWidget.h
  voDelimitedTextPreview.h
  voDelimitedTextPreviewModel.h
  voMainWindow.h
  voMongoLoadDialog.h
  voMongoSaveDialog.h
  voOpenTreeLoadDialog.h
  voNormalizationWidget.h
  voStartupView.h
  voViewStackedWidget.h
  voViewTabWidget.h
  voRemoteAnalysisConnectionDialog.h
  )

set(KIT_UI_FORMS
  Resources/UI/voDelimitedTextImportDialog.ui
  Resources/UI/voDelimitedTextImportWidget.ui
  Resources/UI/voMongoLoadDialog.ui
  Resources/UI/voMongoSaveDialog.ui
  Resources/UI/voOpenTreeLoadDialog.ui
  Resources/UI/voRemoteAnalysisConnectionDialog.ui
  )

if(USE_ARBOR_BRAND)
  set(KIT_UI_FORMS  "${KIT_UI_FORMS};Resources/UI/voMainWindow_Arbor.ui")
else()
  set(KIT_UI_FORMS  "${KIT_UI_FORMS};Resources/UI/voMainWindow.ui")
endif()

set(KIT_resources
  Resources/VisomicsApp.qrc
  )

include_directories(
  ${Visomics_SOURCE_DIR}/Base
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

QT4_WRAP_UI(KIT_UI_CPP ${KIT_UI_FORMS})
QT4_WRAP_CPP(KIT_MOC_CPP ${KIT_MOC_SRCS} )
QT4_ADD_RESOURCES(KIT_QRC_SRCS ${KIT_resources})

add_library(VisomicsAppLib STATIC
  ${KIT_SRCS}
  ${KIT_UI_CPP}
  ${KIT_MOC_CPP}
  ${KIT_QRC_SRCS}
  )
target_link_libraries(VisomicsAppLib VisomicsBaseLib)

# Set Visomics_FORWARD_PATH_BUILD and Visomics_FORWARD_PATH_INSTALL
set(tmp "\".\" CONFIG_DIR_POST")
if(WIN32)
  set(tmp "${tmp}, \"${QT_BINARY_DIR}\"")
else()
  set(tmp "${tmp}, \"${QT_LIBRARY_DIR}\"")
endif()
set(Visomics_FORWARD_PATH_BUILD ${tmp})
set(Visomics_FORWARD_PATH_INSTALL "../lib")
if(APPLE)
  set(Visomics_FORWARD_PATH_INSTALL "../Resources")
endif()

configure_file(
  ${Visomics_SOURCE_DIR}/CMake/visomics-forward.cxx.in
  ${CMAKE_CURRENT_BINARY_DIR}/visomics-forward.cxx
  @ONLY IMMEDIATE)

# Add forward executable
set(FORWARD_SRCS ${CMAKE_CURRENT_BINARY_DIR}/visomics-forward.cxx)
if(Q_WS_WIN)
  list(APPEND FORWARD_SRCS Resources/VisomicsApp.rc)
endif()
if(APPLE)
  # Use icons to represent the Mac bundle
  if(USE_ARBOR_BRAND)
    set(Visomics_Icons
      ${VisomicsBase_SOURCE_DIR}/Resources/Icons/ArborIcon.icns)
    set(Visomics_Icons_Filename "ArborIcon.icns")
  else()
    set(Visomics_Icons
      ${VisomicsBase_SOURCE_DIR}/Resources/Icons/VisomicsIcon.icns)
    set(Visomics_Icons_Filename "VisomicsIcon.icns")
  endif()
  list(APPEND FORWARD_SRCS ${Visomics_Icons})
  set(MACOSX_BUNDLE_ICON_FILE ${Visomics_Icons_Filename})
  set_SOURCE_FILES_PROPERTIES(${Visomics_Icons} PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources)
endif()
add_executable(VisomicsApp MACOSX_BUNDLE ${FORWARD_SRCS})
target_link_libraries(VisomicsApp vtksys)
add_dependencies(VisomicsApp VisomicsApp-real)
set_target_properties(VisomicsApp PROPERTIES OUTPUT_NAME ${APP_NAME})

# Add regular application
add_executable(VisomicsApp-real Main.cpp)
target_link_libraries(VisomicsApp-real VisomicsAppLib ${MongoDB_LIBRARIES})
set_target_properties(VisomicsApp-real PROPERTIES OUTPUT_NAME ${APP_NAME}-real)

if(APPLE)
  # Name of bundle
  set(APP "${APP_NAME}.app")

  # Make sure the "regular application" gets included in the bundle
  set_property(TARGET VisomicsApp-real PROPERTY RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${APP}/Contents/Resources)

  # Install Visomics
  install(TARGETS VisomicsApp
    DESTINATION "."
    COMPONENT Runtime
    )

  set(plugin_path "${APP}/Contents/plugins/imageformats")
  install(FILES ${Visomics_SOURCE_DIR}/CMake/qt.conf
          DESTINATION "${APP}/Contents/Resources"
          COMPONENT Runtime
    )

else(APPLE)
  # Install rules
  install(TARGETS VisomicsApp
    DESTINATION ${Visomics_INSTALL_BIN_DIR}
    COMPONENT Runtime
    )
  install(TARGETS VisomicsApp-real
    DESTINATION ${Visomics_INSTALL_LIB_DIR}
    COMPONENT Runtime
    )

  # executables to fixup
  set(APP "${Visomics_INSTALL_BIN_DIR}/${APP_NAME}${CMAKE_EXECUTABLE_SUFFIX}"
          "${Visomics_INSTALL_LIB_DIR}/${APP_NAME}-real${CMAKE_EXECUTABLE_SUFFIX}"
  )
  set(plugin_path "plugins/imageformats")
endif(APPLE)

# Make sure Qt plugins are properly installed
get_filename_component(qgif_plugin ${QT_QGIF_PLUGIN_RELEASE} NAME)
get_filename_component(qico_plugin ${QT_QICO_PLUGIN_RELEASE} NAME)
get_filename_component(qjpeg_plugin ${QT_QJPEG_PLUGIN_RELEASE} NAME)
get_filename_component(qmng_plugin ${QT_QMNG_PLUGIN_RELEASE} NAME)
get_filename_component(qsvg_plugin ${QT_QSVG_PLUGIN_RELEASE} NAME)
get_filename_component(qtiff_plugin ${QT_QTIFF_PLUGIN_RELEASE} NAME)
install(FILES ${QT_QGIF_PLUGIN_RELEASE}
              ${QT_QICO_PLUGIN_RELEASE}
              ${QT_QJPEG_PLUGIN_RELEASE}
              ${QT_QMNG_PLUGIN_RELEASE}
              ${QT_QSVG_PLUGIN_RELEASE}
              ${QT_QTIFF_PLUGIN_RELEASE}
        DESTINATION ${plugin_path}
        COMPONENT Runtime
)

# Setup the script that's run at install time
configure_file(
  "${Visomics_SOURCE_DIR}/CMake/CompleteBundles.cmake.in"
  "${Visomics_BINARY_DIR}/CompleteBundles.cmake"
  @ONLY
)
install(SCRIPT "${Visomics_BINARY_DIR}/CompleteBundles.cmake"
        COMPONENT Runtime)

if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
