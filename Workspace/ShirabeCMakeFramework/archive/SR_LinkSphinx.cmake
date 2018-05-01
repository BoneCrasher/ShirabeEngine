function(linkSphinx)
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
        set(LIB_PF_SUFFIX   _x64)
    else()
        set(PLATFORM_SUFFIX 32)
        set(LIB_PF_SUFFIX)
    endif()

    # Release/Debug?
    if(SR_DEBUG)
        set(CONFIG d)
        set(CONFIG_STR Debug)
    else()
        set(CONFIG)
        set(CONFIG_STR Release)
    endif()

    set(TARGET deploy/${PLATFORM_PREFIX}${PLATFORM_SUFFIX}/${CONFIG_STR})
    set(SPHINX_DIR ${SR_WORKSPACE_ROOT_DIR}/${TARGET})

    # -I
    append_parentscope(
        SR_PROJECT_INCLUDEPATH
        ${SPHINX_DIR}/include
        )

    # ?
    append_parentscope(
        SR_PROJECT_DEPENDPATH
        ${SPHINX_DIR}/bin
        )

    # -L
    append_parentscope(
        SR_PROJECT_LIBRARY_DIRECTORIES
        ${SPHINX_DIR}/lib
        )

    if(WIN32 AND MSVC)
        # -l
        append_parentscope(
            SR_PROJECT_LIBRARY_TARGETS
            SphinxLib
            )
    else()
        # -l
        append_parentscope(
            SR_PROJECT_LIBRARY_TARGETS
            Sphinx${LIB_PF_SUFFIX}${CONFIG}
            MathParser${LIB_PF_SUFFIX}
            )
    endif()

endfunction(linkSphinx)
