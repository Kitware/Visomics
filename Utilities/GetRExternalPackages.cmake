###########################################################################
#
#  Program: Visomics
#
#  Copyright (c) Kitware Inc.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0.txt
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
###########################################################################

if(NOT R_cmd)
  message(FATAL_ERROR "error: The variable 'R_COMMAND' is not set.")
endif()

execute_process(
  COMMAND ${R_cmd} ${R_cmd_args}
  RESULT_VARIABLE return_val
  )

# Print and delete output of R script
if(EXISTS ${R_output_name})
  file(READ ${R_output_name} R_output)
  message(${R_output})
  execute_process(COMMAND ${CMAKE_COMMAND} -E remove ${R_output_name})
endif()

# If R script fails
if(NOT "${return_val}" STREQUAL "0")
  message( FATAL_ERROR
  "error: Could not install all external R packages.\n"
  "Try running \"make RExternalPackages\" as root (sudo)."
  )
endif()
