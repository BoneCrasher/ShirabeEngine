function(linkBoost)
    #
    # Since Boost itself comes in different packages for both platforms
    # we have to identify the proper platform subdirectory id.
    #
    set(
        PROJECT_PLATFORM_ID
        "windows"
        )
    if(UNIX AND NOT APPLE)
        set(
            PROJECT_PLATFORM_ID
            "linux"
            )
    endif()

    # Convenience... :D
    set(
        BOOST_DIR
        "${THIRD_PARTY_DIR}/boost"
        )

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

    set(TARGET_DIR ${PLATFORM_PREFIX}${PLATFORM_SUFFIX}/${CONFIG})

    # -I
    append_parentscope(
        SR_PROJECT_INCLUDEPATH
        ${BOOST_DIR}/include
        )

    # ?
    append_parentscope(
        SR_PROJECT_DEPENDPATH
        ${BOOST_DIR}/${TARGET_DIR}/lib
        )

    set(BOOST_LIBRARIES)
    
        # -l
        # set(BOOST_TOOLSET vc100)
		set(BOOST_TOOLSET vc141)

		set(BOOST_VERSION_SUFFIX    1_66)
		set(BOOTS_VERSION_SO_SUFFIX 1.66)
	#
    # Boost core, linked for all projects specifying "LINK_BOOST"
    #
    if(WIN32)
        # -L
        append_parentscope(
            SR_PROJECT_LIBRARY_DIRECTORIES
            ${BOOST_DIR}/${TARGET_DIR}/lib
            )


        set(BOOST_DEBUG)
		LogStatus(MESSAGES "Debug is ${SR_DEBUG}")
        if(SR_DEBUG)
            set(BOOST_DEBUG -gd)
        endif()

        append(
            BOOST_LIBRARIES
            boost_atomic-${BOOST_TOOLSET}-mt${BOOST_DEBUG}-x${PLATFORM_SUFFIX}-${BOOST_VERSION_SUFFIX}
            boost_chrono-${BOOST_TOOLSET}-mt${BOOST_DEBUG}-x${PLATFORM_SUFFIX}-${BOOST_VERSION_SUFFIX}
            boost_date_time-${BOOST_TOOLSET}-mt${BOOST_DEBUG}-x${PLATFORM_SUFFIX}-${BOOST_VERSION_SUFFIX}
            boost_filesystem-${BOOST_TOOLSET}-mt${BOOST_DEBUG}-x${PLATFORM_SUFFIX}-${BOOST_VERSION_SUFFIX}
            boost_iostreams-${BOOST_TOOLSET}-mt${BOOST_DEBUG}-x${PLATFORM_SUFFIX}-${BOOST_VERSION_SUFFIX}
            boost_log-${BOOST_TOOLSET}-mt${BOOST_DEBUG}-x${PLATFORM_SUFFIX}-${BOOST_VERSION_SUFFIX}
            boost_locale-${BOOST_TOOLSET}-mt${BOOST_DEBUG}-x${PLATFORM_SUFFIX}-${BOOST_VERSION_SUFFIX}
            boost_system-${BOOST_TOOLSET}-mt${BOOST_DEBUG}-x${PLATFORM_SUFFIX}-${BOOST_VERSION_SUFFIX}
            boost_thread-${BOOST_TOOLSET}-mt${BOOST_DEBUG}-x${PLATFORM_SUFFIX}-${BOOST_VERSION_SUFFIX}
            boost_timer-${BOOST_TOOLSET}-mt${BOOST_DEBUG}-x${PLATFORM_SUFFIX}-${BOOST_VERSION_SUFFIX}
            )

        #
        # Additional "non-core" modules-configuration!
        #
        if(SR_LINK_BOOST_RANDOM)
            append(
                BOOST_LIBRARIES
                boost_random-${BOOST_TOOLSET}-mt${BOOST_DEBUG}-x${PLATFORM_SUFFIX}-${BOOST_VERSION_SUFFIX}
                )
        endif(SR_LINK_BOOST_RANDOM)
    elseif(UNIX AND NOT APPLE)
        append(
            BOOST_LIBRARIES
            ${BOOST_DIR}/${TARGET_DIR}/lib/libboost_atomic.so.${BOOST_VERSION_SO_SUFFIX}.0
            ${BOOST_DIR}/${TARGET_DIR}/lib/libboost_chrono.so.${BOOST_VERSION_SO_SUFFIX}.0
            ${BOOST_DIR}/${TARGET_DIR}/lib/libboost_date_time.so.${BOOST_VERSION_SO_SUFFIX}.0
            ${BOOST_DIR}/${TARGET_DIR}/lib/libboost_filesystem.so.${BOOST_VERSION_SO_SUFFIX}.0
            ${BOOST_DIR}/${TARGET_DIR}/lib/libboost_log.so.${BOOST_VERSION_SO_SUFFIX}.0
            ${BOOST_DIR}/${TARGET_DIR}/lib/libboost_locale.so.${BOOST_VERSION_SO_SUFFIX}.0
            ${BOOST_DIR}/${TARGET_DIR}/lib/libboost_system.so.${BOOST_VERSION_SO_SUFFIX}.0
            ${BOOST_DIR}/${TARGET_DIR}/lib/libboost_thread.so.${BOOST_VERSION_SO_SUFFIX}.0
            ${BOOST_DIR}/${TARGET_DIR}/lib/libboost_timer.so.${BOOST_VERSION_SO_SUFFIX}.0
            )

        #
        # Additional "non-core" modules-configuration!
        #
        if(SR_LINK_BOOST_RANDOM)
            append(
                BOOST_LIBRARIES
                ${BOOST_DIR}/${TARGET_DIR}/lib/libboost_random.so.${BOOST_VERSION_SO_SUFFIX}.0
                )
        endif(SR_LINK_BOOST_RANDOM)
    endif()


    LogStatus(MESSAGES "Boost Libs: ${BOOST_LIBRARIES}")

    append_parentscope(
        SR_PROJECT_LIBRARY_TARGETS
        ${BOOST_LIBRARIES}
        )
endfunction(linkBoost)
