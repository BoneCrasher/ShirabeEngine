function(linkGLib)
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
    set(GLIB_DIR   ${THIRD_PARTY_DIR}/glib/${TARGET_DIR})

    # -I
    append_parentscope(
        SR_PROJECT_INCLUDEPATH
        ${GLIB_DIR}/include/glib-2.0
        ${GLIB_DIR}/lib/glib-2.0/include
        )

    # ?
    append_parentscope(
        SR_PROJECT_DEPENDPATH
        ${GLIB_DIR}/bin
        PARENT_SCOPE
        )

    if(WIN32 AND MSVC)
        # -L
        append_parentscope(
            SR_PROJECT_LIBRARY_DIRECTORIES
            ${GLIB_DIR}/lib
            )

        # -l
        append_parentscope(
            SR_PROJECT_LIBRARY_TARGETS
            ${GLIB_DIR}/lib/gio-2.0.lib
            ${GLIB_DIR}/lib/glib-2.0.lib
            ${GLIB_DIR}/lib/gmodule-2.0.lib
            ${GLIB_DIR}/lib/gobject-2.0.lib
            ${GLIB_DIR}/lib/gthread-2.0.lib
            )
    else() # Linux        
        # -l
        append_parentscope(
            SR_PROJECT_LIBRARY_TARGETS
            ${GLIB_DIR}/lib/libgio-2.0.so.0.3800.2
            ${GLIB_DIR}/lib/libglib-2.0.so.0.3800.2
            ${GLIB_DIR}/lib/libgmodule-2.0.so.0.3800.2
            ${GLIB_DIR}/lib/libgobject-2.0.so.0.3800.2
            ${GLIB_DIR}/lib/libgthread-2.0.so.0.3800.2
            )
    endif()
endfunction(linkGLib)
