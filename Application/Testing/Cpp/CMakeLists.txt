
SET(KIT ${PROJECT_NAME})

CREATE_TEST_SOURCELIST(Tests ${KIT}CppTests.cpp
  voDataBrowserWidgetTest.cpp
  voDelimitedTextImportDialogTest.cpp
  voDelimitedTextImportWidgetTest.cpp
  )
  
SET(TestsToRun ${Tests})
REMOVE(TestsToRun ${KIT}CppTests.cpp)

ADD_EXECUTABLE(${KIT}CppTests ${Tests})
TARGET_LINK_LIBRARIES(${KIT}CppTests ${PROJECT_NAME}Lib)

SET(KIT_TESTS ${CPP_TEST_PATH}/${KIT}CppTests)

MACRO(SIMPLE_TEST TESTNAME)
  ADD_TEST(${TESTNAME} ${KIT_TESTS} ${TESTNAME} ${ARGV})
  #SET_PROPERTY(TEST ${TESTNAME} PROPERTY LABELS ${PROJECT_NAME})
ENDMACRO()

SIMPLE_TEST(voDataBrowserWidgetTest ${VisomicsData_DIR}/Data/UNC/All_conc_kitware_transposed.csv)
SIMPLE_TEST(voDataBrowserWidgetTest ${VisomicsData_DIR}/Data/UNC/All_conc_kitware_transposed.csv)
SIMPLE_TEST(voDelimitedTextImportWidgetTest ${VisomicsData_DIR}/Data/UNC/All_conc_kitware_transposed.csv)


