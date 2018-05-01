function(linkGStreamer)
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
    set(TARGET_DIR    ${PLATFORM_PREFIX}${PLATFORM_SUFFIX}/${CONFIG})
    set(GSTREAMER_DIR ${THIRD_PARTY_DIR}/gstreamer/${TARGET_DIR})
    # -I
    append_parentscope(
        SR_PROJECT_INCLUDEPATH
        ${GSTREAMER_DIR}/include/gstreamer-1.0
        )

    # ?
    append_parentscope(
        SR_PROJECT_DEPENDPATH
        ${GSTREAMER_DIR}/bin
        )

    # -L
    append_parentscope(
        SR_PROJECT_LIBRARY_DIRECTORIES
        ${GSTREAMER_DIR}/lib
        )

    # -l
    append_parentscope(
        SR_PROJECT_LIBRARY_TARGETS

        )
endfunction(linkCC)
