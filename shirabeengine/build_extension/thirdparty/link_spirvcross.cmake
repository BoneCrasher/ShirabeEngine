function(linkLibrary)
    
    set(DIR ${THIRD_PARTY_DIR}/SPIRV-Cross/${SHIRABE_PLATFORM_PREFIX}${SHIRABE_PLATFORM_ADDRESS_SIZE}/${SHIRABE_PLATFORM_CONFIG})

    LogStatus(MESSAGES "SPIRV-Cross-Dir: ${DIR}")
	
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
        spirv-cross-core
        spirv-cross-cpp
        spirv-cross-glsl
        spirv-cross-msl
        spirv-cross-reflect
        spirv-cross-util)
endfunction(linkLibrary)