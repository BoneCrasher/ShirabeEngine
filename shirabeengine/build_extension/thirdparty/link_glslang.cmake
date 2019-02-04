function(linkLibrary)
    
    set(DIR ${THIRD_PARTY_DIR}/glslang/${SHIRABE_PLATFORM_PREFIX}${SHIRABE_PLATFORM_ADDRESS_SIZE}/${SHIRABE_PLATFORM_CONFIG})

    LogStatus(MESSAGES "GlSlang-Cross-Dir: ${DIR}")
	
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
        OGLCompiler
        OSDependent
        glslang
        HLSL
        SPIRV
        SPRRemapper)
endfunction(linkLibrary)
