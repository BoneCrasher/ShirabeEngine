function(linkMathParser)
    # Only required on windows for debugging
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
    if(NOT SR_DEBUG)
        set(CONFIG Release)
    else()
        set(CONFIG Debug)
    endif()

    # Some convenience
    set(TARGET_DIR ${PLATFORM_PREFIX}${PLATFORM_SUFFIX}/${CONFIG})
    set(MP_DIR ${SR_WORKSPACE_ROOT_DIR}/gev_client/deploy/${TARGET_DIR})

    # -I
    append_parentscope(
        SR_PROJECT_INCLUDEPATH
        ${MP_DIR}/include
        )

    # ?
    append_parentscope(
        SR_PROJECT_DEPENDPATH
        ${MP_DIR}/bin
        )

    # -L
    append_parentscope(
        SR_PROJECT_LIBRARY_DIRECTORIES
        ${MP_DIR}/lib
        )

    # -l
    # Format name
    set(SR_TMP "INVALID_MATHPARSER_LIB")
    formatPlatformConfigName(
        MathParser
        SR_REQUEST_x64_BUILD
        SR_PLATFORM_CONFIG
        SR_TMP
        )
    # Now link
    #ppend_parentscope(
    #   SR_PROJECT_LIBRARY_TARGETS
    #   ${SR_TMP}
    #   )

    # Add dependency to this target to ensure proper compilation order
    append_parentscope(
        SR_PROJECT_INTERNAL_DEPENDENCIES
        ${SR_TMP}
        )
endfunction(linkMathParser)
