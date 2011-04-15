
#
# Included from a dashboard script, this cmake file will drive the configure and build
# steps of the project
#

# Helper function used to generate the CMake script that will perform the initial clone
# of a git repository. (Copied from ExternalProject CMake module)
function(_write_gitclone_script script_filename source_dir git_EXECUTABLE git_repository git_tag src_name work_dir)
  file(WRITE ${script_filename}
"if(\"${git_tag}\" STREQUAL \"\")
  message(FATAL_ERROR \"Tag for git checkout should not be empty.\")
endif()

execute_process(
  COMMAND \${CMAKE_COMMAND} -E remove_directory \"${source_dir}\"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR \"Failed to remove directory: '${source_dir}'\")
endif()

execute_process(
  COMMAND \"${git_EXECUTABLE}\" clone \"${git_repository}\" \"${src_name}\"
  WORKING_DIRECTORY \"${work_dir}\"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR \"Failed to clone repository: '${git_repository}'\")
endif()

execute_process(
  COMMAND \"${git_EXECUTABLE}\" checkout ${git_tag}
  WORKING_DIRECTORY \"${work_dir}/${src_name}\"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR \"Failed to checkout tag: '${git_tag}'\")
endif()

execute_process(
  COMMAND \"${git_EXECUTABLE}\" submodule init
  WORKING_DIRECTORY \"${work_dir}/${src_name}\"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR \"Failed to init submodules in: '${work_dir}/${src_name}'\")
endif()

execute_process(
  COMMAND \"${git_EXECUTABLE}\" submodule update --recursive
  WORKING_DIRECTORY \"${work_dir}/${src_name}\"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR \"Failed to update submodules in: '${work_dir}/${src_name}'\")
endif()

"
)
endfunction(_write_gitclone_script)

# Helper function used to generate the CMake script that will update a git clone given a tag.
# (Inspired from ExternalProject CMake module)
function(_update_gitclone_script script_filename source_dir git_EXECUTABLE git_repository git_tag src_name work_dir)
  file(WRITE ${script_filename}
"if(\"${git_tag}\" STREQUAL \"\")
  message(FATAL_ERROR \"Tag for git checkout should not be empty.\")
endif()

execute_process(
  COMMAND \"${git_EXECUTABLE}\" fetch
  WORKING_DIRECTORY \"${work_dir}/${src_name}\"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR \"Failed to fetch repository: '${git_repository}'\")
endif()

execute_process(
  COMMAND \"${git_EXECUTABLE}\" checkout ${git_tag}
  WORKING_DIRECTORY \"${work_dir}/${src_name}\"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR \"Failed to checkout tag: '${git_tag}'\")
endif()

execute_process(
  COMMAND \"${git_EXECUTABLE}\" submodule update --recursive
  WORKING_DIRECTORY \"${work_dir}/${src_name}\"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR \"Failed to update submodules in: '${work_dir}/${src_name}'\")
endif()

"
)
endfunction(_update_gitclone_script)

# The following variable are expected to be define in the top-level script:
set(expected_variables
  ADDITIONAL_CMAKECACHE_OPTION
  CTEST_NOTES_FILES
  CTEST_SITE
  CTEST_DASHBOARD_ROOT
  CTEST_CMAKE_COMMAND
  CTEST_CMAKE_GENERATOR
  WITH_MEMCHECK
  WITH_COVERAGE
  WITH_DOCUMENTATION
  WITH_PACKAGES
  CTEST_BUILD_CONFIGURATION
  CTEST_TEST_TIMEOUT
  CTEST_BUILD_FLAGS
  TEST_TO_EXCLUDE_REGEX
  CTEST_PROJECT_NAME
  CTEST_SOURCE_DIRECTORY
  CTEST_BINARY_DIRECTORY
  CTEST_BUILD_NAME
  SCRIPT_MODE
  CTEST_COVERAGE_COMMAND
  CTEST_MEMORYCHECK_COMMAND
  CTEST_SVN_COMMAND
  CTEST_GIT_COMMAND
  GIT_REPOSITORY
  GIT_TAG
  QT_QMAKE_EXECUTABLE
  R_COMMAND
  )
if(WITH_DOCUMENTATION)
  list(APPEND expected_variables DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY)
endif()
if(NOT DEFINED CTEST_PARALLEL_LEVEL)
  set(CTEST_PARALLEL_LEVEL 8)
endif()

foreach(var ${expected_variables})
  if(NOT DEFINED ${var})
    message(FATAL_ERROR "Variable ${var} should be defined in top-level script !")
  endif()
endforeach()

# Make sure command 'ctest_upload' is available if WITH_PACKAGES is True
if (WITH_PACKAGES)
  if(NOT COMMAND ctest_upload)
    message(FATAL_ERROR "Failed to enable option WITH_PACKAGES ! CMake ${CMAKE_VERSION} doesn't support 'ctest_upload' command.")
  endif()
endif()

# Should binary directory be cleaned?
set(empty_binary_directory FALSE)

# Attempt to build and test also if 'ctest_update' returned an error
set(force_build FALSE)

# Set model and track options
set(model "")
set(track_suffix "")
if (SCRIPT_MODE STREQUAL "experimental")
  set(empty_binary_directory FALSE)
  set(force_build TRUE)
  set(model Experimental)
  set(track_suffix "")
elseif (SCRIPT_MODE STREQUAL "continuous")
  set(empty_binary_directory TRUE)
  set(force_build FALSE)
  set(model Continuous)
  set(track_suffix "-${GIT_TAG}")
elseif (SCRIPT_MODE STREQUAL "nightly")
  set(empty_binary_directory TRUE)
  set(force_build TRUE)
  set(model Nightly)
  set(track_suffix "-${GIT_TAG}")
else()
  message(FATAL_ERROR "Unknown script mode: '${SCRIPT_MODE}'. Script mode should be either 'experimental', 'continuous' or 'nightly'")
endif()

# Track associated with 'master' should default to either 'Continuous', 'Nightly' or 'Experimental'
if (track_suffix STREQUAL "-master")
  set(track_suffix "")
endif()
set(track ${model}${track_suffix})

# For more details, see http://www.kitware.com/blog/home/post/11
set(CTEST_USE_LAUNCHERS 1)

if(empty_binary_directory)
  message("Directory ${CTEST_BINARY_DIRECTORY} cleaned !")
  ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})
endif()

# Compute 'work_dir' and 'src_name' variable used by both _write_gitclone_script and _update_gitclone_script
get_filename_component(src_dir ${CTEST_SOURCE_DIRECTORY} REALPATH)
get_filename_component(work_dir ${src_dir} PATH)
get_filename_component(src_name ${src_dir} NAME)

if(NOT EXISTS "${CTEST_SOURCE_DIRECTORY}")
  _write_gitclone_script(
    ${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}-gitclone.cmake # script_filename
    ${CTEST_SOURCE_DIRECTORY} # source_dir
    ${CTEST_GIT_COMMAND} # git_EXECUTABLE
    ${GIT_REPOSITORY} # git_repository
    "origin/${GIT_TAG}" # git_tag
    ${src_name} # src_name
    ${work_dir} # work_dir
  )
  # Note: The following command should be specified as a string.
  set(CTEST_CHECKOUT_COMMAND "${CMAKE_COMMAND} -P ${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}-gitclone.cmake")
endif()

_update_gitclone_script(
  ${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}-gitupdate.cmake # script_filename
  ${CTEST_SOURCE_DIRECTORY} # source_dir
  ${CTEST_GIT_COMMAND} # git_EXECUTABLE
  ${GIT_REPOSITORY} # git_repository
  "origin/${GIT_TAG}" # git_tag
  ${src_name} # src_name
  ${work_dir} # work_dir
  )

#set(CTEST_UPDATE_COMMAND "${CTEST_GIT_COMMAND}")

# Note: The following command should be specified as a list.
set(CTEST_GIT_UPDATE_CUSTOM ${CMAKE_COMMAND} -P ${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}-gitupdate.cmake)

#
# run_ctest macro
#
MACRO(run_ctest)
  ctest_start(${model} TRACK ${track})
  ctest_update(SOURCE "${CTEST_SOURCE_DIRECTORY}" RETURN_VALUE FILES_UPDATED)

  # force a build if this is the first run and the build dir is empty
  if(NOT EXISTS "${CTEST_BINARY_DIRECTORY}/CMakeCache.txt")
    message("First time build - Initialize CMakeCache.txt")
    set(force_build 1)

    #-----------------------------------------------------------------------------
    # Write initial cache.
    #-----------------------------------------------------------------------------
    file(WRITE "${CTEST_BINARY_DIRECTORY}/CMakeCache.txt" "
CTEST_USE_LAUNCHERS:BOOL=${CTEST_USE_LAUNCHERS}
GIT_EXECUTABLE:FILEPATH=${CTEST_GIT_COMMAND}
Subversion_SVN_EXECUTABLE:FILEPATH=${CTEST_SVN_COMMAND}
WITH_COVERAGE:BOOL=${WITH_COVERAGE}
DOCUMENTATION_TARGET_IN_ALL:BOOL=${WITH_DOCUMENTATION}
DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY:PATH=${DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY}
${ADDITIONAL_CMAKECACHE_OPTION}
")
  endif()
  
  if (FILES_UPDATED GREATER 0 OR force_build)

    set(force_build 0)
    
    #-----------------------------------------------------------------------------
    # The following variable can be used while testing the driver scripts
    #-----------------------------------------------------------------------------
    set(run_ctest_with_update TRUE)
    set(run_ctest_with_configure TRUE)
    set(run_ctest_with_build TRUE)
    set(run_ctest_with_test TRUE)
    set(run_ctest_with_coverage TRUE)
    set(run_ctest_with_memcheck TRUE)
    set(run_ctest_with_packages TRUE)
    set(run_ctest_with_notes TRUE)
    
    #-----------------------------------------------------------------------------
    # Update
    #-----------------------------------------------------------------------------
    if(run_ctest_with_update)
      ctest_submit(PARTS Update)
    endif()
    
    #-----------------------------------------------------------------------------
    # Configure
    #-----------------------------------------------------------------------------
    if (run_ctest_with_configure)
      message("----------- [ Configure ${CTEST_PROJECT_NAME} ] -----------")
      
      #set(label ${CTEST_PROJECT_NAME})
      
      set_property(GLOBAL PROPERTY SubProject ${label})
      set_property(GLOBAL PROPERTY Label ${label})
       
      ctest_configure(BUILD "${CTEST_BINARY_DIRECTORY}")
      ctest_read_custom_files("${CTEST_BINARY_DIRECTORY}")
      ctest_submit(PARTS Configure)
    endif()

    #-----------------------------------------------------------------------------
    # Build top level
    #-----------------------------------------------------------------------------
    if (run_ctest_with_build)
      message("----------- [ Build ${CTEST_PROJECT_NAME} ] -----------")
      ctest_build(BUILD "${CTEST_BINARY_DIRECTORY}" APPEND)
      ctest_submit(PARTS Build)
    endif()
    
    #-----------------------------------------------------------------------------
    # Inner build directory
    #-----------------------------------------------------------------------------
    set(project_build_dir "${CTEST_BINARY_DIRECTORY}/${CTEST_PROJECT_NAME}-build")
    
    #-----------------------------------------------------------------------------
    # Test
    #-----------------------------------------------------------------------------
    if (run_ctest_with_test)
      message("----------- [ Test ${CTEST_PROJECT_NAME} ] -----------")
      ctest_test(
        BUILD "${project_build_dir}" 
        #INCLUDE_LABEL ${label}
        PARALLEL_LEVEL ${CTEST_PARALLEL_LEVEL}
        EXCLUDE ${TEST_TO_EXCLUDE_REGEX})
      # runs only tests that have a LABELS property matching "${label}"
      ctest_submit(PARTS Test)
    endif()
    
    #-----------------------------------------------------------------------------
    # Global coverage ... 
    #-----------------------------------------------------------------------------
    if (run_ctest_with_coverage)
      # HACK Unfortunately ctest_coverage ignores the BUILD argument, try to force it...
      file(READ ${project_build_dir}/CMakeFiles/TargetDirectories.txt project_build_coverage_dirs)
      file(APPEND "${CTEST_BINARY_DIRECTORY}/CMakeFiles/TargetDirectories.txt" "${project_build_coverage_dirs}")
      
      if (WITH_COVERAGE AND CTEST_COVERAGE_COMMAND)
        message("----------- [ Global coverage ] -----------")
        ctest_coverage(BUILD "${project_build_dir}")
        ctest_submit(PARTS Coverage)
      endif ()
    endif()
    
    #-----------------------------------------------------------------------------
    # Global dynamic analysis ...
    #-----------------------------------------------------------------------------
    if (WITH_MEMCHECK AND CTEST_MEMORYCHECK_COMMAND AND run_ctest_with_memcheck)
        message("----------- [ Global memcheck ] -----------")
        ctest_memcheck(BUILD "${project_build_dir}")
        ctest_submit(PARTS MemCheck)
    endif ()
    
    #-----------------------------------------------------------------------------
    # Create packages / installers ...
    #-----------------------------------------------------------------------------
    if (WITH_PACKAGES AND run_ctest_with_packages)
      message("----------- [ WITH_PACKAGES ] -----------")
      #include("${CTEST_SOURCE_DIRECTORY}/CMake/SlicerFunctionCTestPackage.cmake")
      #set(packages)
      #message("Packaging ...")
      #SlicerFunctionCTestPackage(
      #  BINARY_DIR ${project_build_dir} 
      #  CONFIG ${CTEST_BUILD_CONFIGURATION}
      #  RETURN_VAR packages)
      #message("Uploading ...")
      #foreach(p ${packages})
      #  ctest_upload(FILES ${p})
      #  ctest_submit(PARTS Upload)
      #endforeach()
    endif()
    
    #-----------------------------------------------------------------------------
    # Note should be at the end
    #-----------------------------------------------------------------------------
    if (run_ctest_with_notes)
      ctest_submit(PARTS Notes)
    endif()
  
  endif()
endmacro()

if(SCRIPT_MODE STREQUAL "continuous")
  while(${CTEST_ELAPSED_TIME} LESS 68400)
    set(START_TIME ${CTEST_ELAPSED_TIME})
    run_ctest()
    # Loop no faster than once every 5 minutes
    message("Wait for 5 minutes ...")
    ctest_sleep(${START_TIME} 300 ${CTEST_ELAPSED_TIME})
  endwhile()
else()
  run_ctest()
endif()
