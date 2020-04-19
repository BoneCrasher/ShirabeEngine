function(linkLibrary)
    
    set(DIR ${THIRD_PARTY_DIR}/_deploy/vulkan_sdk/${SHIRABE_PLATFORM_PREFIX}${SHIRABE_PLATFORM_ADDRESS_SIZE}/${SHIRABE_PLATFORM_CONFIG})
    file(TO_CMAKE_PATH ${DIR} DIR)

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
        vulkan)
endfunction(linkLibrary)
