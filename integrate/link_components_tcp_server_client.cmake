function(linkLibrary)

    LogStatus(MESSAGES "Linking core_windows")
    
    set(DIR ${SR_WORKSPACE_ROOT_DIR}/_deploy/${SR_PLATFORM_TARGET}/${SR_PLATFORM_CONFIG})

    # -I
    append_parentscope(
        SR_PROJECT_INCLUDEPATH
        ${DIR}/include )

    # -L
    append_parentscope(
        SR_PROJECT_LIBRARY_DIRECTORIES
        ${DIR}/lib )

    # -l
    # Format name
    set(SR_TMP     "INVALID")
    set(LINK_STATIC ON)
    formatPlatformConfigName(
        tcp_server_client
        SR_REQUEST_x64_BUILD
        SR_PLATFORM_CONFIG
        LINK_STATIC
        SR_TMP)

    # Now link
    append_parentscope(
        SR_PROJECT_LIBRARY_TARGETS
        ${SR_TMP})

endfunction(linkLibrary)
