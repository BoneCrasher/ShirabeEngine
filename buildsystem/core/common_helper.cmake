#----------------------------------------------------------------------------------
# File-System-Helpers to simplify determining include paths of file inclusions.
#----------------------------------------------------------------------------------

#----------------------------------------------------------------------------------
# append: 
#     --> Take a variable name <__to> and append all parameters provided
#         in the subsequent variadic argument array.
#----------------------------------------------------------------------------------
macro(append __to)
    # BEGIN
    set(extra_macro_args ${ARGN}) # Need to store ARGN in a variable to make it usable.
    set(
        ${__to}    # Single dereferencing to get the effective variable handle.
        ${${__to}} # Double dereferencing to get the effective variable current value.
        ${extra_macro_args}
        )
	# END
endmacro(append)
#----------------------------------------------------------------------------------

#----------------------------------------------------------------------------------
# append_parentscope: 
#     --> Take a variable name <__to> and append all parameters provided
#         in the subsequent variadic argument array. The value of <__to> will 
#         be set in the caller's parent scope.
#----------------------------------------------------------------------------------
macro(append_parentscope __to)
    # BEGIN
    set(extra_macro_args ${ARGN}) # Need to store ARGN in a variable to make it usable.
    set(
        ${__to}	   # Single dereferencing to get the effective variable handle.
        ${${__to}} # Double dereferencing to get the effective variable current value.
        ${extra_macro_args}
        PARENT_SCOPE
        )
endmacro(append_parentscope)
#----------------------------------------------------------------------------------


#----------------------------------------------------------------------------------
# LogStatus: 
#     --> Take a list of messages named LOGS_MESSAGES and print them as STATUS
#         output.
#----------------------------------------------------------------------------------
function(
        LogStatus
        )
    # BEGIN
    # Define and register options, one and multi value argument keywords.
    set(options)
    set(oneValueArgs)
    set(multiValueArgs MESSAGES)

    # Read them...
    cmake_parse_arguments(
        LOGS
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
        )

    if(SHIRABE_CONFIG__DEBUG_LOG)
        foreach(MSG ${LOGS_MESSAGES})
            message(STATUS ${MSG})
        endforeach(MSG)
    endif()
    # END
endfunction(LogStatus)
#----------------------------------------------------------------------------------

#----------------------------------------------------------------------------------
# LogError: 
#     --> Take a list of messages named LOGE_MESSAGES and print them as SEND_ERROR
#         output.
#----------------------------------------------------------------------------------
function(
        LogError
        )
    # BEGIN
    # Define and register options, one and multi value argument keywords.
    set(options)
    set(oneValueArgs)
    set(multiValueArgs MESSAGES)

    # Read them...
    cmake_parse_arguments(
        LOGE
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
        )

    if(SHIRABE_CONFIG__DEBUG_LOG)
        foreach(MSG ${LOGE_MESSAGES})
            message(SEND_ERROR ${MSG})
        endforeach(MSG)
    endif(SHIRABE_CONFIG__DEBUG_LOG)
    # END
endfunction(LogError)
#----------------------------------------------------------------------------------


#----------------------------------------------------------------------------------
# recursivelyFindFilesWithExtensions:
#    --> Recursively scan <ROOT_DIR> for all files with any of the <EXTENSIONS>
#        and exclude all files (relative to <ROOT_DIR>) listed in <EXCLUSIONS>.
#        The final list is stored in <RESULT_VARIABLE>, if any or in 
#         recursivelyFindFilesWithExtensions_RESULT if not provided.
#----------------------------------------------------------------------------------
# Options:
#
# OneValueArguments:
#     ROOT_DIR (MANDATORY)
#         - Root directory of recursive globbing.
#     RESULT_VARIABLE (OPTIONAL)
#         - Variable to store the found file-list into. 
#           Will be in recursivelyFindFilesWithExtensions_RESULT, if not provided.
#
# MultiValueArguments:
#     EXTENSIONS
#         - List of extensions to search for in ROOT_DIR
#     EXCLUSIONS
#         - List of filenames relative to ROOT_DIR to exclude from the final list.
#
# Result:
#     (1) List of found files stored in RESULT_VARIABLE, if provided or in 
#         recursivelyFindFilesWithExtensions_RESULT, if not.
#     (2) 0 on error, stored in RESULT_VARIABLE, if provided or in 
#         recursivelyFindFilesWithExtensions_RESULT, if not.
#----------------------------------------------------------------------------------
function(
        recursivelyFindFilesWithExtensions
        )
    # BEGIN
    # Define and register options, one and multi value argument keywords.
    set(options)
    set(oneValueArgs   RESULT_VARIABLE)
    set(multiValueArgs ROOT_DIR EXTENSIONS EXCLUSIONS)
    cmake_parse_arguments(
        RECFINDBYEXT
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
        )

    LogStatus(
        MESSAGES
        "Searching for files in:"
        "${RECFINDBYEXT_ROOT_DIR}"
        " "
        "Extensions required:"
        "${RECFINDBYEXT_EXTENSIONS}"
        " "
        "Excluded:"
        "${RECFINDBYEXT_EXCLUSIONS}"
        " "
        " "
        )

    # Generate formatted list of globbing-expressions for target file-types
    set(EXTENSION_GLOB_EXPRESSIONS)
	foreach(ROOT ${RECFINDBYEXT_ROOT_DIR})
            foreach(EXTENSION ${RECFINDBYEXT_EXTENSIONS})
                append(
                    EXTENSION_GLOB_EXPRESSIONS
                    "${ROOT}/*${EXTENSION}"
                )
            endforeach()
	endforeach()

    LogStatus(
        MESSAGES
        "Generated Globbing Expressions:"
        ${EXTENSION_GLOB_EXPRESSIONS}
        " "
        )

    # Get 'em files
    file(
        GLOB_RECURSE
        FOUND_FILES
        FOLLOW_SYMLINKS
        ${EXTENSION_GLOB_EXPRESSIONS}
        )

    # Create a source-dir-prepended list of all files to be excluded
    set(EXCLUSION_LIST)
    foreach(EXCLUDE_SOURCE ${RECFINDBYEXT_EXCLUSIONS})
        append(
            EXCLUSION_LIST
            ${RECFINDBYEXT_ROOT_DIR}/${EXCLUDE_SOURCE}
            )
    endforeach()

    LogStatus(
        MESSAGES
        "Files to exclude:"
        ${EXCLUSION_LIST}
        " "
        )

    # Now remove all items to be excluded from the list
    if(EXCLUSION_LIST)
        list(
            REMOVE_ITEM
            FOUND_FILES
            ${EXCLUSION_LIST}
            )
    endif()

    # If a result variable is defined, write the output to it,
    # Otherwise, write to "recursivelyFindFilesWithExtensions_RESULT"
    if(RECFINDBYEXT_RESULT_VARIABLE)
        set(
            ${RECFINDBYEXT_RESULT_VARIABLE} # TO VAR
            ${FOUND_FILES}                  # VALUES
            PARENT_SCOPE                    # IN PARENT SCOPE
            )
    else()
        set(
            recursivelyFindFilesWithExtensions_RESULT # TO VAR
            ${FOUND_FILES}                            # VALUES
            PARENT_SCOPE                              # IN PARENT SCOPE
            )
    endif()

    # END
endfunction(recursivelyFindFilesWithExtensions)
#----------------------------------------------------------------------------------

#----------------------------------------------------------------------------------
# formatPlatformConfigName: 
#     --> Take the target configuration and generate a smartray output filename,
#         to be stored in <OUT_NAME>, or formatPlatformConfigName_RESULT if not provided.
#         E.g.:
#             <MODULE_NAME>=SHIRABE_kernel
#             1. <IS_X64_BUILD>=NO,  <PLATFORM_CONFIG>=Debug   --> <OUT_NAME>=SHIRABE_kerneld
#             2. <IS_X64_BUILD>=NO,  <PLATFORM_CONFIG>=Release --> <OUT_NAME>=SHIRABE_kernel
#             3. <IS_X64_BUILD>=YES, <PLATFORM_CONFIG>=Debug   --> <OUT_NAME>=SHIRABE_kernel_x64d
#             4. <IS_X64_BUILD>=YES, <PLATFORM_CONFIG>=Release --> <OUT_NAME>=SHIRABE_kernel_x64
#----------------------------------------------------------------------------------
macro(
        formatPlatformConfigName
        MODULE_NAME
        IS_X64_BUILD
        PLATFORM_CONFIG
        FMT_AS_STATIC_LIB
        OUT_NAME
        )
    # BEGIN
    
    # Append '_x64' in case of a 64-bit build.
    set(TMP_MODULE_NAME ${MODULE_NAME})
    if(${IS_X64_BUILD})
        set(TMP_MODULE_NAME
            "${TMP_MODULE_NAME}_x64"
            )
    endif()
	    
    # Append 'd' in case of debug, but AFTER appending x64!
    
    if(SHIRABE_DEBUG)
        set(TMP_MODULE_NAME
            "${TMP_MODULE_NAME}d"
            )
    endif()

    # Finally append static library suffix to distinguish it from an
    # import library of shared objects.
    if(${FMT_AS_STATIC_LIB})
        set(
            TMP_MODULE_NAME
            "${TMP_MODULE_NAME}_a"
        )
    endif()

    # If a result variable is defined, write the output to it,
    # Otherwise, write to "formatPlatformConfigName_RESULT"
    if(${OUT_NAME})
        set(
            ${OUT_NAME}        # TO VAR
            ${TMP_MODULE_NAME} # VALUES
            )
			
    else()
        set(
            formatPlatformConfigName_RESULT # TO VAR
            ${TMP_MODULE_NAME}              # VALUES
            )
    endif()
    # END
endmacro(formatPlatformConfigName)
#----------------------------------------------------------------------------------

#----------------------------------------------------------------------------------
# setupTargetAndConfig: 
#     --> Use <MODULE_NAME>, <SHIRABE_REQUEST_x64_BUILD_CFG> and <SHIRABE_DEBUG>
#          to determine basic project properties:
#            SHIRABE_REQUEST_x64_BUILD: Are we building for x64?
#            SHIRABE_PLATFORM_TARGET:   Win32|Win64|Linux32|Linux64
#            SHIRABE_PLATFORM_CONFIG:   Debug|Release
#            SHIRABE_PLATFORM_CONFIG_SUFFIX:
#               Suffix-path used in many many cases.
#               E.g.: Win32/Debug
#            SHIRABE_MODULE_OUTPUT_NAME:
#               The "output-name" of compilation, i.e. the name of the 
#               final binary.
#----------------------------------------------------------------------------------
macro(setupTargetAndConfig MODULE_NAME) 
    # BEGIN

	# REMARKS: Since this function is to be exclusively called from within 
	#           a project script context, the variables used were declared 
	#           and defined in a previous step in the script.
	#           They are assumed to be "environment variables".

    #
    # Define the build target
    #
    set(SHIRABE_REQUEST_x64_BUILD OFF)
    if(SHIRABE_REQUEST_x64_BUILD_CFG)
        set(SHIRABE_REQUEST_x64_BUILD ON)
        LogStatus(MESSAGES "Building x64")
    endif(SHIRABE_REQUEST_x64_BUILD_CFG)

    #
    # Define architecture
    #
    set(SHIRABE_PROJECT_CROSS_BUILD_ARMHF OFF)
    set(SHIRABE_PROJECT_ARCHITECTURE)
    if(SHIRABE_ARCH)
        if("${SHIRABE_ARCH}" STREQUAL "armhf")
            set(SHIRABE_PROJECT_CROSS_BUILD_ARMHF ON)
        endif()

        set(SHIRABE_PROJECT_ARCHITECTURE ${SHIRABE_ARCH})
    else()
        if(SHIRABE_REQUEST_x64_BUILD)
            set(SHIRABE_PROJECT_ARCHITECTURE x86)
        else()
            set(SHIRABE_PROJECT_ARCHITECTURE x86_64)
        endif()
    endif()

    #
    # Generic build script specific
    #
    set(SHIRABE_PLATFORM_PREFIX)       # Win|Linux?
    set(SHIRABE_PLATFORM_ADDRESS_SIZE) # 32|64?
    set(SHIRABE_PLATFORM_TARGET)       # Win32|Win64|Linux32|Linux64|?
    set(SHIRABE_PLATFORM_CONFIG)       # Debug|Release [ + Test]?

    if(SHIRABE_PROJECT_CROSS_BUILD_ARMHF)
        set(SHIRABE_PLATFORM_PREFIX armhf)
    elseif(WIN32)
        set(SHIRABE_PLATFORM_PREFIX win)
    elseif(UNIX AND NOT APPLE)
        set(SHIRABE_PLATFORM_PREFIX linux)
    endif()    
    
    if(SHIRABE_REQUEST_x64_BUILD)
        set(SHIRABE_PLATFORM_ADDRESS_SIZE 64)
    else()
        set(SHIRABE_PLATFORM_ADDRESS_SIZE 32)
    endif()
    set(SHIRABE_PLATFORM_TARGET ${SHIRABE_PLATFORM_PREFIX}${SHIRABE_PLATFORM_ADDRESS_SIZE})
    
    if(SHIRABE_DEBUG)
        set(SHIRABE_PLATFORM_CONFIG debug)   # Debug|Release
    else()
        set(SHIRABE_PLATFORM_CONFIG release) # Debug|Release
    endif()

        set(SHIRABE_PLATFORM_CONFIG_SUFFIX ${SHIRABE_PLATFORM_TARGET}/${SHIRABE_PLATFORM_CONFIG})

	
    #
    # Define the module name
    # 
        set(SHIRABE_MODULE_OUTPUT_NAME ${MODULE_NAME}) # Predeclare basic name to have the subsequent function work.
    formatPlatformConfigName(
        ${MODULE_NAME}
        SHIRABE_REQUEST_x64_BUILD
        SHIRABE_PLATFORM_CONFIG
        SHIRABE_BUILD_STATICLIB
        SHIRABE_MODULE_OUTPUT_NAME
        )

    LogStatus(
        MESSAGES
        "Determined platform target: ${SHIRABE_PLATFORM_TARGET}"
        "Determined platform config: ${SHIRABE_PLATFORM_CONFIG}"
        "Determined new module name: ${SHIRABE_MODULE_OUTPUT_NAME}"
        " ")

endmacro(setupTargetAndConfig)
#----------------------------------------------------------------------------------

#----------------------------------------------------------------------------------
# installHeadersStructured: 
#     --> Installs headers retaining the subdirectory structure of the header-files.
#          Regular header installs, will just drop everything plainly in one directory
#          flattening the filesystem-hierarchy.
#----------------------------------------------------------------------------------
macro(installHeadersStructured TARGET_DIR INCLUDE_DIR HEADERS)
    # BEGIN

    LogStatus(MESSAGES "Structured header installation:")
    foreach(HEADER ${${HEADERS}})
		# string(REPLACE <match> <replacement> <out-var> <input>)
        string(REPLACE ${INCLUDE_DIR}/ "" relative_header ${HEADER})

		# Extract and format the relative subdirectory for the file to be installed.
        get_filename_component(dir ${relative_header} DIRECTORY)
        if(NOT "${dir}" STREQUAL "")
            set(dir ${dir}/)
        endif()

		# Extract the filename without path.
        get_filename_component(fn ${relative_header} NAME)

        LogStatus(MESSAGES "--> Installing: ${TARGET_DIR}/${dir}${fn}")
		# install(FILES <filename_with_path> DESTINATION <output_dir_appended_by_subdir>)
        install(FILES ${HEADER} DESTINATION ${TARGET_DIR}/${dir})
    endforeach()

    LogStatus(MESSAGES " ")

    # END
endmacro(installHeadersStructured)
#----------------------------------------------------------------------------------

macro(combine_arguments _variable)
  set(_result "")
  foreach(_element ${${_variable}})
    set(_result "${_result} \"${_element}\"")
  endforeach()
  string(STRIP "${_result}" _result)
  set(${_variable} "${_result}")
endmacro()

function(export_all_flags _filename)
  set(_include_directories "$<TARGET_PROPERTY:${_target},INCLUDE_DIRECTORIES>")
  set(_compile_definitions "$<TARGET_PROPERTY:${_target},COMPILE_DEFINITIONS>")
  set(_compile_flags "$<TARGET_PROPERTY:${_target},COMPILE_FLAGS>")
  set(_compile_options "$<TARGET_PROPERTY:${_target},COMPILE_OPTIONS>")
  set(_include_directories "$<$<BOOL:${_include_directories}>:-I$<JOIN:${_include_directories},\n-I>\n>")
  set(_compile_definitions "$<$<BOOL:${_compile_definitions}>:-D$<JOIN:${_compile_definitions},\n-D>\n>")
  set(_compile_flags "$<$<BOOL:${_compile_flags}>:$<JOIN:${_compile_flags},\n>\n>")
  set(_compile_options "$<$<BOOL:${_compile_options}>:$<JOIN:${_compile_options},\n>\n>")
  file(GENERATE OUTPUT "${_filename}" CONTENT "${_compile_definitions}${_include_directories}${_compile_flags}${_compile_options}\n")
endfunction()

function(add_precompiled_header _target _input)
  cmake_parse_arguments(_PCH "FORCEINCLUDE" "SOURCE_CXX:SOURCE_C" "" ${ARGN})

  get_filename_component(_input_we ${_input} NAME_WE)
  if(NOT _PCH_SOURCE_CXX)
    set(_PCH_SOURCE_CXX "${_input_we}.cpp")
  endif()
  if(NOT _PCH_SOURCE_C)
    set(_PCH_SOURCE_C "${_input_we}.c")
  endif()

  if(MSVC)

    set(_cxx_path "${CMAKE_CURRENT_BINARY_DIR}/${_target}_cxx_pch")
    set(_c_path "${CMAKE_CURRENT_BINARY_DIR}/${_target}_c_pch")
    make_directory("${_cxx_path}")
    make_directory("${_c_path}")
    set(_pch_cxx_header "${_cxx_path}/${_input}")
    set(_pch_cxx_pch "${_cxx_path}/${_input_we}.pch")
    set(_pch_c_header "${_c_path}/${_input}")
    set(_pch_c_pch "${_c_path}/${_input_we}.pch")

    get_target_property(sources ${_target} SOURCES)
    foreach(_source ${sources})
      set(_pch_compile_flags "")
      if(_source MATCHES \\.\(cc|cxx|cpp|c\)$)
    if(_source MATCHES \\.\(cpp|cxx|cc\)$)
      set(_pch_header "${_input}")
      set(_pch "${_pch_cxx_pch}")
    else()
      set(_pch_header "${_input}")
      set(_pch "${_pch_c_pch}")
    endif()

    if(_source STREQUAL "${_PCH_SOURCE_CXX}")
      set(_pch_compile_flags "${_pch_compile_flags} \"/Fp${_pch_cxx_pch}\" /Yc${_input}")
      set(_pch_source_cxx_found TRUE)
    elseif(_source STREQUAL "${_PCH_SOURCE_C}")
      set(_pch_compile_flags "${_pch_compile_flags} \"/Fp${_pch_c_pch}\" /Yc${_input}")
      set(_pch_source_c_found TRUE)
    else()
      if(_source MATCHES \\.\(cpp|cxx|cc\)$)
        set(_pch_compile_flags "${_pch_compile_flags} \"/Fp${_pch_cxx_pch}\" /Yu${_input}")
        set(_pch_source_cxx_needed TRUE)
      else()
        set(_pch_compile_flags "${_pch_compile_flags} \"/Fp${_pch_c_pch}\" /Yu${_input}")
        set(_pch_source_c_needed TRUE)
      endif()
      if(_PCH_FORCEINCLUDE)
        set(_pch_compile_flags "${_pch_compile_flags} /FI${_input}")
      endif(_PCH_FORCEINCLUDE)
    endif()

    get_source_file_property(_object_depends "${_source}" OBJECT_DEPENDS)
    if(NOT _object_depends)
      set(_object_depends)
    endif()
    if(_PCH_FORCEINCLUDE)
      if(_source MATCHES \\.\(cc|cxx|cpp\)$)
        list(APPEND _object_depends "${_pch_header}")
      else()
        list(APPEND _object_depends "${_pch_header}")
      endif()
    endif()

    set_source_files_properties(${_source} PROPERTIES
      COMPILE_FLAGS "${_pch_compile_flags}"
      OBJECT_DEPENDS "${_object_depends}")
      endif()
    endforeach()

    if(_pch_source_cxx_needed AND NOT _pch_source_cxx_found)
      message(FATAL_ERROR "A source file ${_PCH_SOURCE_CXX} for ${_input} is required for MSVC builds. Can be set with the SOURCE_CXX option.")
    endif()
    if(_pch_source_c_needed AND NOT _pch_source_c_found)
      message(FATAL_ERROR "A source file ${_PCH_SOURCE_C} for ${_input} is required for MSVC builds. Can be set with the SOURCE_C option.")
    endif()
  endif(MSVC)

  if(CMAKE_COMPILER_IS_GNUCXX)
    get_filename_component(_name ${_input} NAME)
    set(_pch_header "${CMAKE_CURRENT_SOURCE_DIR}/${_input}")
    set(_pch_binary_dir "${CMAKE_CURRENT_BINARY_DIR}/${_target}_pch")
    set(_pchfile "${_pch_binary_dir}/${_input}")
    set(_outdir "${CMAKE_CURRENT_BINARY_DIR}/${_target}_pch/${_name}.gch")
    make_directory(${_outdir})
    set(_output_cxx "${_outdir}/.c++")
    set(_output_c "${_outdir}/.c")

    set(_pch_flags_file "${_pch_binary_dir}/compile_flags.rsp")
    export_all_flags("${_pch_flags_file}")
    set(_compiler_FLAGS "@${_pch_flags_file}")
    add_custom_command(
      OUTPUT "${_pchfile}"
      COMMAND "${CMAKE_COMMAND}" -E copy "${_pch_header}" "${_pchfile}"
      DEPENDS "${_pch_header}"
      COMMENT "Updating ${_name}")
    add_custom_command(
      OUTPUT "${_output_cxx}"
      COMMAND "${CMAKE_CXX_COMPILER}" ${_compiler_FLAGS} -x c++-header -o "${_output_cxx}" "${_pchfile}"
      DEPENDS "${_pchfile}" "${_pch_flags_file}"
      COMMENT "Precompiling ${_name} for ${_target} (C++)")
    add_custom_command(
      OUTPUT "${_output_c}"
      COMMAND "${CMAKE_C_COMPILER}" ${_compiler_FLAGS} -x c-header -o "${_output_c}" "${_pchfile}"
      DEPENDS "${_pchfile}" "${_pch_flags_file}"
      COMMENT "Precompiling ${_name} for ${_target} (C)")

    get_property(_sources TARGET ${_target} PROPERTY SOURCES)
    foreach(_source ${_sources})
      set(_pch_compile_flags "")

      if(_source MATCHES \\.\(cc|cxx|cpp|c\)$)
    get_source_file_property(_pch_compile_flags "${_source}" COMPILE_FLAGS)
    if(NOT _pch_compile_flags)
      set(_pch_compile_flags)
    endif()
    separate_arguments(_pch_compile_flags)
    list(APPEND _pch_compile_flags -Winvalid-pch)
    if(_PCH_FORCEINCLUDE)
      list(APPEND _pch_compile_flags -include "${_pchfile}")
    else(_PCH_FORCEINCLUDE)
      list(APPEND _pch_compile_flags "-I${_pch_binary_dir}")
    endif(_PCH_FORCEINCLUDE)

    get_source_file_property(_object_depends "${_source}" OBJECT_DEPENDS)
    if(NOT _object_depends)
      set(_object_depends)
    endif()
    list(APPEND _object_depends "${_pchfile}")
    if(_source MATCHES \\.\(cc|cxx|cpp\)$)
      list(APPEND _object_depends "${_output_cxx}")
    else()
      list(APPEND _object_depends "${_output_c}")
    endif()

    combine_arguments(_pch_compile_flags)
    message("${_source}" ${_pch_compile_flags})
    set_source_files_properties(${_source} PROPERTIES
      COMPILE_FLAGS "${_pch_compile_flags}"
      OBJECT_DEPENDS "${_object_depends}")
      endif()
    endforeach()
  endif(CMAKE_COMPILER_IS_GNUCXX)
endfunction()
