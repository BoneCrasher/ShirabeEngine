function(linkLibrary)
    set(DIR ${THIRD_PARTY_DIR}/_deploy/stb/${SHIRABE_PLATFORM_PREFIX}${SHIRABE_PLATFORM_ADDRESS_SIZE}/${SHIRABE_PLATFORM_CONFIG}/include)

    # -I
    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
        ${DIR}
    )

endfunction(linkLibrary)
