SET(target RExternalPackages)

SET(R_cmd ${R_COMMAND})
SET(R_cmd_batch CMD BATCH)
SET(R_options --no-restore --no-save --slave --no-timing)
SET(R_script_name "${CMAKE_BINARY_DIR}/GetRExternalPackages.R")
SET(R_output_name "${CMAKE_BINARY_DIR}/GetExternalPackages.Rout")

SET(R_cmd_args ${R_cmd_batch} ${R_options} -- ${R_script_name} ${R_output_name}) # Concatenate all command arguments for easy passing

configure_file(
  ${CMAKE_SOURCE_DIR}/Utilities/GetRExternalPackages.R.in
  ${CMAKE_BINARY_DIR}/GetRExternalPackages.R
  @ONLY
)

add_custom_target(${target} ALL
  COMMAND ${CMAKE_COMMAND}
   -DR_cmd=${R_cmd}
   "-DR_cmd_args=${R_cmd_args}"
   -DR_output_name=${R_output_name}
   -P ${CMAKE_SOURCE_DIR}/Utilities/GetRExternalPackages.cmake
  DEPENDS VisomicsData
  COMMENT "Installing external R packages"
  VERBATIM
  )
