function(linkLibWebsockets)
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
    set(TARGET_DIR ${PLATFORM_PREFIX}${PLATFORM_SUFFIX}/${CONFIG})
    set(LWS_DIR    "${THIRD_PARTY_DIR}/libWebsockets/${TARGET_DIR}")
    set(WPP_DIR    "${THIRD_PARTY_DIR}/websocketpp")

    # -I
    append_parentscope(
        SR_PROJECT_INCLUDEPATH
        ${LWS_DIR}/include
        ${WPP_DIR}
        )

    # ?
    append_parentscope(
        SR_PROJECT_DEPENDPATH
        ${LWS_DIR}/bin
        )

    # -L
    append_parentscope(
        SR_PROJECT_LIBRARY_DIRECTORIES
        ${LWS_DIR}/lib
        )

    if(MSVC)
        # -l
        append_parentscope(
            SR_PROJECT_LIBRARY_TARGETS
            websockets_static
            )
    else()
        # -l
        append_parentscope(
            SR_PROJECT_LIBRARY_TARGETS
            websockets
            )
    endif()
endfunction(linkLibWebsockets)
