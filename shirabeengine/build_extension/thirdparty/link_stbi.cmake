function(linkLibrary)
    set(DIR ${THIRD_PARTY_DIR}/_deploy/stb/include)

    # -I
    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
        ${DIR}
    )

endfunction(linkLibrary)
