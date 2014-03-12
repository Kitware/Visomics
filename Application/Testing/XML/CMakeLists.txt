function (add_GUI_test testName expectedOutput)
  if (NOT expectedOutput)
    set(expectedOutput "Success =  true")
  endif()
  add_test(
    NAME ${testName}
    COMMAND VisomicsApp ${CMAKE_CURRENT_SOURCE_DIR}/Test${testName}.xml
  )
  set_tests_properties(${testName} PROPERTIES PASS_REGULAR_EXPRESSION
    "${expectedOutput}")
endfunction()

add_GUI_test(GeigerFitContinuous "")
add_GUI_test(OneZoom "")
add_GUI_test(TreeDropTip
  "output tree has 74 fewer vertices than the input tree.")
add_GUI_test(TreeDropTipInverted
  "output tree has 101 fewer vertices than the input tree.")
add_GUI_test(TreeDropTipWithoutData
  "output tree has 3 fewer vertices than the input tree.")
add_GUI_test(TreeDropTipWithoutDataInverted
  "output tree has 178 fewer vertices than the input tree.")
