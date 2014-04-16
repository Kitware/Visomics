function (add_GUI_test testName expectedOutput)
  add_test(
    NAME ${testName}
    COMMAND VisomicsApp ${CMAKE_CURRENT_SOURCE_DIR}/Test${testName}.xml
  )
  set_tests_properties(${testName} PROPERTIES PASS_REGULAR_EXPRESSION
    "${expectedOutput}")
endfunction()

# custom analyses
add_GUI_test(ANOVA "0.447")
add_GUI_test(FOLD "109.4")
add_GUI_test(GeigerFitContinuous "Success =  true")
add_GUI_test(KMEANS "Success =  true")
add_GUI_test(OneZoom "Success =  true")
add_GUI_test(PCA "0.056")
add_GUI_test(PLS "0.517")
add_GUI_test(XCorrel "0.160")

# built-in analyses
add_GUI_test(TreeDropTip
  "output tree has 74 fewer vertices than the input tree.")
add_GUI_test(TreeDropTipInverted
  "output tree has 101 fewer vertices than the input tree.")
add_GUI_test(TreeDropTipWithoutData
  "output tree has 3 fewer vertices than the input tree.")
add_GUI_test(TreeDropTipWithoutDataInverted
  "output tree has 178 fewer vertices than the input tree.")
add_GUI_test(ReadWriteWorkflow "Tree Drop Tip With Data 2")
