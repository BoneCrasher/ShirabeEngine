function(linkLibrary)
    
    set(TARGET_DIR ${SHIRABE_PLATFORM_PREFIX}${SHIRABE_PLATFORM_ADDRESS_SIZE}/${SHIRABE_PLATFORM_CONFIG})
    set(DIR        ${SHIRABE_THIRD_PARTY_DIR}/gltf/${TARGET_DIR})

    # -I
    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
        ${DIR}/include
        )

endfunction(linkLibrary)
