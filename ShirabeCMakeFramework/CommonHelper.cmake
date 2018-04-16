message(STATUS "BEGIN: Parsing CommonHelper.cmake")

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
    set(oneValueArgs   ROOT_DIR   RESULT_VARIABLE)
    set(multiValueArgs EXTENSIONS EXCLUSIONS)
    cmake_parse_arguments(
        RECFINDBYEXT
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
        )

    LogStatus(
        MESSAGES
        " "
        "-----------------------------------------------------------------------------------"
        "recursivelyFindFilesWithExtensions"
        "-----------------------------------------------------------------------------------"
        )

    LogStatus(
        MESSAGES
        "Searching for files in:"
        "${RECFINDBYEXT_ROOT_DIR}"
        "Extensions required:"
        "${RECFINDBYEXT_EXTENSIONS}"
        )

    # Generate formatted list of globbing-expressions for target file-types
    set(EXTENSION_GLOB_EXPRESSIONS)
    foreach(EXTENSION ${RECFINDBYEXT_EXTENSIONS})
        append(
            EXTENSION_GLOB_EXPRESSIONS
            "${RECFINDBYEXT_ROOT_DIR}/*${EXTENSION}"
            )
    endforeach()

    LogStatus(
        MESSAGES
        "Globbing Expressions:"
        ${EXTENSION_GLOB_EXPRESSIONS}
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
        message( FATAL_ERROR "No RESULT_VARIABLE specified to store results." )
    endif()

    LogStatus(
        MESSAGES
        "-----------------------------------------------------------------------------------"
        " "
    )
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
        OUT_NAME
        )
    # BEGIN
    
    # Append '_x64' in case of a 64-bit build.
    set(TMP_MODULE_NAME ${MODULE_NAME})
    if(${IS_X64_BUILD})
        set(
            TMP_MODULE_NAME
                "${TMP_MODULE_NAME}_x64"
        )
    endif()
	    
    # Append 'd' in case of debug, but AFTER appending x64!    
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(
            TMP_MODULE_NAME
                "${TMP_MODULE_NAME}d"
        )
    endif()

    # Finally, check for the static library suffix _a!
    if(SHIRABE_BUILD_STATICLIB)
	set(
		TMP_MODULE_NAME
		    "${TMP_MODULE_NAME}_a"
	)
    endif()

    LogStatus(MESSAGES "Composed new module name: ${TMP_MODULE_NAME}")

    # If a result variable is defined, write the output to it,
    # Otherwise, write to "formatPlatformConfigName_RESULT"
    if(${OUT_NAME})
        set(
            ${OUT_NAME}        # TO VAR
            ${TMP_MODULE_NAME} # VALUES
        )			
    else()
        message( FATAL_ERROR "No OUT_NAME specified to store results." )
    endif()
    # END
endmacro(formatPlatformConfigName)
#----------------------------------------------------------------------------------

#----------------------------------------------------------------------------------
# setupTargetAndConfig: 
#     --> Use <MODULE_NAME>, <SHIRABE_ADDRESSMODEL_64BIT_CFG> and <SHIRABE_DEBUG>
#          to determine basic project properties:
#            SHIRABE_ADDRESSMODEL_64BIT: Are we building for x64?
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
    # Define SHIRABE_ADDRESSMODEL_64BIT
    #
    set(SHIRABE_ADDRESSMODEL_64BIT OFF)
    if(SHIRABE_ADDRESSMODEL_64BIT_CFG} STREQUAL "ON")
        set(SHIRABE_ADDRESSMODEL_64BIT ON)
    endif(SHIRABE_ADDRESSMODEL_64BIT_CFG)
    LogStatus(MESSAGES "Building for 64-bit address model? ${SHIRABE_ADDRESSMODEL_64BIT}")

    #
    # Define SHIRABE_PLATFORM_PREFIX       => ( Win, Linux )
    # Define SHIRABE_PLATFORM_ADDRESS_SIZE => ( 32, 64, ... )
    # Define SHIRABE_PLATFORM_TARGET       => ( Win32, Win64, Linux32, Linux64 )
    # Define SHIRABE_PLATFORM_CONFIG       => ( [Debug|Release][Test|Profile] )=
    #
    set(SHIRABE_PLATFORM_PREFIX)      
    set(SHIRABE_PLATFORM_ADDRESS_SIZE)
    set(SHIRABE_PLATFORM_TARGET)      
    set(SHIRABE_PLATFORM_CONFIG)      

    if(WIN32)
        set(SHIRABE_PLATFORM_PREFIX Win)
    elseif(UNIX AND NOT APPLE)
        set(SHIRABE_PLATFORM_PREFIX Linux)
    endif()
    
    if(SHIRABE_ADDRESSMODEL_64BIT)
        set(SHIRABE_PLATFORM_ADDRESS_SIZE 64)
    else()
        set(SHIRABE_PLATFORM_ADDRESS_SIZE 32)
    endif()
    set(SHIRABE_PLATFORM_TARGET ${SHIRABE_PLATFORM_PREFIX}${SHIRABE_PLATFORM_ADDRESS_SIZE})

    if(SHIRABE_DEBUG)
        set(SHIRABE_PLATFORM_CONFIG Debug)   # Debug|Release
    else()
        set(SHIRABE_PLATFORM_CONFIG Release) # Debug|Release
    endif()

    set(SHIRABE_PLATFORM_CONFIG_SUFFIX ${SHIRABE_PLATFORM_TARGET}/${SHIRABE_PLATFORM_CONFIG})
    
    LogStatus(MESSAGES "Determined platform target: ${SHIRABE_PLATFORM_TARGET}")
    LogStatus(MESSAGES "Determined platform config: ${SHIRABE_PLATFORM_CONFIG}")
	
    #
    # Define SHIRABE_MODULE_OUTPUT_NAME
    # 
	set(SHIRABE_MODULE_OUTPUT_NAME ${MODULE_NAME}) # Predeclare basic name to have the subsequent function work.
    formatPlatformConfigName(
        ${MODULE_NAME}
        SHIRABE_ADDRESSMODEL_64BIT
        SHIRABE_PLATFORM_CONFIG
        SHIRABE_MODULE_OUTPUT_NAME
        )
    LogStatus(MESSAGES "Determined new module name: ${SHIRABE_MODULE_OUTPUT_NAME}")
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

    LogStatus(MESSAGES "Looping through HEADERS...")
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

        LogStatus(MESSAGES "Installing: ${TARGET_DIR}/${dir}${fn}")
		# install(FILES <filename_with_path> DESTINATION <output_dir_appended_by_subdir>)
        install(FILES ${HEADER} DESTINATION ${TARGET_DIR}/${dir})
    endforeach()

    # END
endmacro(installHeadersStructured)
#----------------------------------------------------------------------------------

message(STATUS "DONE:  Parsing CommonHelper.cmake")
