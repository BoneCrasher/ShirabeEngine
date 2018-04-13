function(linkIconv)
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
    set(ICONV_DIR  ${THIRD_PARTY_DIR}/libiconv/${TARGET_DIR})

    # -I
    append_parentscope(
        SR_PROJECT_INCLUDEPATH
        ${ICONV_DIR}/include
        )

    # ?
    append_parentscope(
        SR_PROJECT_DEPENDPATH
        ${ICONV_DIR}/bin
        )

    # -L
    append_parentscope(
        SR_PROJECT_LIBRARY_DIRECTORIES
        ${ICONV_DIR}/lib
        )

    # -l
	append_parentscope(
		SR_PROJECT_LIBRARY_TARGETS
		charset
		iconv
		)
endfunction(linkIconv)
