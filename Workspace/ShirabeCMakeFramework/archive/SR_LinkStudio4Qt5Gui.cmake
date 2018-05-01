function(linkStudio4Qt5Gui)
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
        ${SRK_DIR}/include/public
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
    set(SR_TMP "INVALID_STUDIO4SRSENSOR_LIB")
    formatPlatformConfigName(
        sr_gui_qt5
        SR_REQUEST_x64_BUILD
        SR_PLATFORM_CONFIG
        SR_TMP
        )
	set(SR_TMP_TARGET ${SR_TMP}_a)

    # Now link
    append_parentscope(
        SR_PROJECT_LIBRARY_TARGETS
        ${SR_TMP_TARGET}
        )

    # Add dependency to this target to ensure proper compilation order
    append_parentscope(
        SR_PROJECT_INTERNAL_DEPENDENCIES
        ${SR_TMP}
        )

endfunction(linkStudio4Qt5Gui)
