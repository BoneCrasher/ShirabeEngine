function(linkCVI)
    if(WIN32 AND SR_DEBUG)
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
        if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
            set(CONFIG Release)
        else()
            set(CONFIG Debug)
        endif()

        # Some convenience
        set(TARGET_DIR deploy/${PLATFORM_PREFIX}${PLATFORM_SUFFIX}/${CONFIG})
        set(CVI_DIR ${THIRD_PARTY_DIR}/cvi/${TARGET_DIR})

        # -I
        append_parentscope(
            SR_PROJECT_INCLUDEPATH
            ${CVI_DIR}/include
            )

        # ?
        append_parentscope(
            SR_PROJECT_DEPENDPATH
            ${CVI_DIR}/lib
            )

        # -L
        append_parentscope(
            SR_PROJECT_LIBRARY_DIRECTORIES
            ${CVI_DIR}/lib
            )

        # -l
        append_parentscope(
            SR_PROJECT_LIBRARY_TARGETS
            nivision
            )
    endif(WIN32)
endfunction(linkCVI)
