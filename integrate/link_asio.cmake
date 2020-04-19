function(linkLibrary)

    set(DIR ${SR_WORKSPACE_ROOT_DIR}/thirdparty/_deploy/asio)
    
    # -I
    append_parentscope(
        SR_PROJECT_INCLUDEPATH
        ${DIR}/include/asio
    )

endfunction(linkLibrary)
