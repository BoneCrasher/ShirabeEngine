function(linkLibrary)

    set(TARGET_DIR ${SHIRABE_PLATFORM_PREFIX}${SHIRABE_PLATFORM_ADDRESS_SIZE}/${SHIRABE_PLATFORM_CONFIG})
    set(ZLIB       ${THIRD_PARTY_DIR}/_deploy/zlib/${TARGET_DIR})

    # -I
    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
        ${ZLIB}/include
        )

    # -L
    append_parentscope(
        SHIRABE_PROJECT_LIBRARY_DIRECTORIES
        ${ZLIB}/lib
        )

    # -l
    append_parentscope(
        SHIRABE_PROJECT_LIBRARY_TARGETS
        z
        )

endfunction(linkLibrary)
