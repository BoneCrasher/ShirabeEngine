function(linkLibrary)

    set(DIR ${THIRD_PARTY_DIR}/_deploy/fxgltf/${SHIRABE_PLATFORM_PREFIX}${SHIRABE_PLATFORM_ADDRESS_SIZE}/${SHIRABE_PLATFORM_CONFIG})

    # -I
    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
        ${DIR}/include
        )

endfunction(linkLibrary)
