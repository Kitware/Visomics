
SET(KIT ${PROJECT_NAME}Analysis)

CREATE_TEST_SOURCELIST(Tests ${KIT}CppTests.cpp
  voAnalysisRunTest.cpp
  )

SET(TestsToRun ${Tests})
REMOVE(TestsToRun ${KIT}CppTests.cpp)

ADD_EXECUTABLE(${KIT}CppTests ${Tests})
TARGET_LINK_LIBRARIES(${KIT}CppTests ${PROJECT_NAME}Lib ${MongoDB_LIBRARIES})

MACRO(ANALYSIS_RUN_TEST ANALYSISNAME)
  SET(TESTNAME ${ANALYSISNAME}AnalysisTest)
  #MESSAGE(STATUS "Adding: ${TESTNAME}")
  ADD_TEST(NAME ${TESTNAME}
    COMMAND ${Visomics_LAUNCH_COMMAND} $<TARGET_FILE:${KIT}CppTests> voAnalysisRunTest ${ANALYSISNAME} ${ARGN})
  #SET_PROPERTY(TEST ${TESTNAME} PROPERTY LABELS ${PROJECT_NAME})
ENDMACRO()


# commonAnalysisTests collects tests that use the same input sample data
SET(commonAnalysisTests
voANOVAStatistics
voFoldChange
voHierarchicalClustering
voKEGGCompounds
voKEGGPathway
voKMeansClustering
voPCAStatistics
voPLSStatistics
voTTest
voXCorrel
)

FOREACH(analysisName ${commonAnalysisTests})
  ANALYSIS_RUN_TEST(${analysisName})
ENDFOREACH()



# other independent tests:
