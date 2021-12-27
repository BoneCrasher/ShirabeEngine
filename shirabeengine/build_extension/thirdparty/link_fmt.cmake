function(linkLibrary)

    set(DIR ${THIRD_PARTY_DIR}/_deploy/fmt/${SHIRABE_PLATFORM_PREFIX}${SHIRABE_PLATFORM_ADDRESS_SIZE}/${SHIRABE_PLATFORM_CONFIG})

    set(LIBRARY_NAME fmt)
    if(SHIRABE_DEBUG)
        set(LIBRARY_NAME fmtd)
    endif()

    # -I
    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
        ${DIR}/include
        )

    # -L
    append_parentscope(
            SHIRABE_PROJECT_LIBRARY_DIRECTORIES
            ${DIR}/lib
    )

    # -l
    append_parentscope(
            SHIRABE_PROJECT_LIBRARY_TARGETS
            ${LIBRARY_NAME})

endfunction(linkLibrary)
