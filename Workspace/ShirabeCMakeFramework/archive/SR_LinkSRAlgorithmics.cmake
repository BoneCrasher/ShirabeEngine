function(linkSRAlgorithmics)
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
        set(CONFIG Release)
    else()
        set(CONFIG Debug)
    endif()

    # Some convenience
    set(TARGET_DIR deploy/${PLATFORM_PREFIX}${PLATFORM_SUFFIX}/${CONFIG})
    set(SRK_DIR    ${SR_WORKSPACE_ROOT_DIR}/${TARGET_DIR})

    # -I
    append_parentscope(
        SR_PROJECT_INCLUDEPATH
        ${SRK_DIR}/include/protected
        )

    # ?
    append_parentscope(
        SR_PROJECT_DEPENDPATH
        ${SRK_DIR}/bin
        ${SRK_DIR}/lib
        )

    # -L
    append_parentscope(
        SR_PROJECT_LIBRARY_DIRECTORIES
        ${SRK_DIR}/lib
        )

    # -l
    # Format name
    set(SR_TMP "INVALID_ALGORITHMICS_LIB")
    formatPlatformConfigName(
        sr_algorithmics
        SR_REQUEST_x64_BUILD
        SR_PLATFORM_CONFIG
        SR_TMP
        )

    # Now link
    append_parentscope(
        SR_PROJECT_LIBRARY_TARGETS
        sr_algorithmics
        )

    # Add dependency to this target to ensure proper compilation order
    append_parentscope(
        SR_PROJECT_INTERNAL_DEPENDENCIES
        ${SR_TMP}
        )
		
endfunction(linkSRAlgorithmics)
