function(linkCC)
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
    set(TARGET_DIR     ${PLATFORM_PREFIX}${PLATFORM_SUFFIX}/${CONFIG})
    set(CFITSIO_DIR    ${THIRD_PARTY_DIR}/cfitsio/${TARGET_DIR})
    set(CCFITS_INC_DIR ${THIRD_PARTY_DIR}/CCfits/include)
    set(CCFITS_DIR     ${THIRD_PARTY_DIR}/CCfits/${TARGET_DIR})

    # -I
    append_parentscope(
        SR_PROJECT_INCLUDEPATH
        ${CFITSIO_DIR}/include
        ${CCFITS_INC_DIR}
        )

    # ?
    append_parentscope(
        SR_PROJECT_DEPENDPATH
        ${CFITSIO_DIR}
        ${CCFITS_DIR}
        )

    if(MSVC)
        # -L
        append_parentscope(
            SR_PROJECT_LIBRARY_DIRECTORIES
            ${CCFITS_DIR}
            ${CFITSIO_DIR}
            )
    else()
        # -L
        append_parentscope(
            SR_PROJECT_LIBRARY_DIRECTORIES
            ${CCFITS_DIR}
            ${CFITSIO_DIR}/lib
            )
    endif()

    # -l
    append_parentscope(
        SR_PROJECT_LIBRARY_TARGETS
        CCfits
        cfitsio
        )
endfunction(linkCC)
