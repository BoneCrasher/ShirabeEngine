function(linkLibrary)

    LogStatus(MESSAGES "Linking <Generic>")
    
    set(DIR ${SHIRABE_WORKSPACE_ROOT_DIR}/_deploy/${SHIRABE_PLATFORM_PREFIX}${SHIRABE_PLATFORM_ADDRESS_SIZE}/${SHIRABE_PLATFORM_CONFIG})

    # -D
    append_parentscope(
        SHIRABE_PROJECT_LIBRARY_DEFINITIONS)

    # -I
    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
        ${DIR}/include)

    # -L
    append_parentscope(
        SHIRABE_PROJECT_LIBRARY_DIRECTORIES
        ${DIR}/lib)

    # -l
    # Format name

    # Now link
    append_parentscope(
        SHIRABE_PROJECT_LIBRARY_TARGETS
        <Generic>)

endfunction(linkLibrary)
