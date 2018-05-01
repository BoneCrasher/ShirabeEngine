function(linkPoco)
    set(PLATFORM_PREFIX)
    set(PLATFORM_SUFFIX)
    set(LIB_PLATFORM_SUFFIX)
    set(LIB_CONFIG_SUFFIX)

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
        set(PLATFORM_SUFFIX     64)
        set(LIB_PLATFORM_SUFFIX 64)
    else()
        set(PLATFORM_SUFFIX     32)
        set(LIB_PLATFORM_SUFFIX)
    endif()

    # Release/Debug?
    if(SR_DEBUG)
        set(LIB_CONFIG_SUFFIX d)
        set(CONFIG Debug)
    else()
        set(LIB_CONFIG_SUFFIX)
        set(CONFIG Release)
    endif()

    set(POCO_DIR ${THIRD_PARTY_DIR}/poco)

    # -I
    append_parentscope(
        SR_PROJECT_INCLUDEPATH
        ${POCO_DIR}/include/Foundation/include
        )

    # ?
    append_parentscope(
        SR_PROJECT_DEPENDPATH
        ${POCO_DIR}/${PLATFORM_PREFIX}${PLATFORM_SUFFIX}/bin
        )

    # -L
    if(WIN32)
        append_parentscope(
            SR_PROJECT_LIBRARY_DIRECTORIES
            ${POCO_DIR}/${PLATFORM_PREFIX}${PLATFORM_SUFFIX}/lib
            )
    elseif(UNIX AND NOT APPLE)
        append_parentscope(
            SR_PROJECT_LIBRARY_DIRECTORIES
            ${POCO_DIR}/${PLATFORM_PREFIX}${PLATFORM_SUFFIX}/${CONFIG}/lib
            )
    endif()

    # -l
    append_parentscope(
        SR_PROJECT_LIBRARY_TARGETS
        PocoFoundation${LIB_CONFIG_SUFFIX}
        )
endfunction(linkPoco)
