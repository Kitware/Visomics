
SET(KIT ${PROJECT_NAME})

CREATE_TEST_SOURCELIST(Tests ${KIT}CppTests.cpp
  voDataBrowserWidgetTest.cpp
  voDelimitedTextImportDialogTest.cpp
  voDelimitedTextImportWidgetTest.cpp
  )

SET(TestsToRun ${Tests})
REMOVE(TestsToRun ${KIT}CppTests.cpp)

ADD_EXECUTABLE(${KIT}CppTests ${Tests})
TARGET_LINK_LIBRARIES(${KIT}CppTests ${PROJECT_NAME}Lib ${MongoDB_LIBRARIES})

MACRO(SIMPLE_TEST TESTNAME)
  ADD_TEST(NAME ${TESTNAME}
    COMMAND ${Visomics_LAUNCH_COMMAND} $<TARGET_FILE:${KIT}CppTests> ${TESTNAME} ${ARGN})
  #SET_PROPERTY(TEST ${TESTNAME} PROPERTY LABELS ${PROJECT_NAME})
ENDMACRO()

SIMPLE_TEST(voDataBrowserWidgetTest ${VisomicsData_DIR}/Data/sample/visomics-sample_liver.csv)
#SIMPLE_TEST(voDelimitedTextImportDialogTest ${VisomicsData_DIR}/Data/sample/visomics-sample_liver.csv)
SIMPLE_TEST(voDelimitedTextImportWidgetTest ${VisomicsData_DIR}/Data/sample/visomics-sample_liver.csv)
