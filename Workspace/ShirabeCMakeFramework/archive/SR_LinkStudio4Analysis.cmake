function(linkStudio4Analysis)
    set(PLATFORM_PREFIX)
    set(PLATFORM_SUFFIX)
    set(CONFIG)

    # Windows/Linux?
    if(WIN32)
        set(PLATFORM_PREFIX Win)
    elseif(UNIX AND NOT APPLE)
        set(PLATFORM_PREFIX Linux)
    else()
        message(ERROR Unsupported platform or missing platform/compiler definitions.)
    endif()

    # 32/64-bit?
    if(SR_REQUEST_x64_BUILD)
        set(PLATFORM_SUFFIX 64)
    else()
        set(PLATFORM_SUFFIX 32)
    endif()

    # Release/Debug?
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
        set(CONFIG )
    else()
        set(CONFIG d)
    endif()

    # Some convenience
    set(TARGET_DIR analysis)
    set(SRK_DIR    ${THIRD_PARTY_DIR}/${TARGET_DIR})

    # -I
    append_parentscope(
        SR_PROJECT_INCLUDEPATH
        ${SRK_DIR}/include
        )
		
		# -L
    append_parentscope(
        SR_PROJECT_LIBRARY_DIRECTORIES
        ${SRK_DIR}/lib
        )

    # -l
    # Format name
    # Now link
    append_parentscope(
        SR_PROJECT_LIBRARY_TARGETS
			analysis
        )
endfunction(linkStudio4Analysis)