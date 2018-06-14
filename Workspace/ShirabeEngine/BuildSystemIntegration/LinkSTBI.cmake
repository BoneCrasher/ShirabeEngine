function(linkLibrary)
    # -I
    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
        ${SHIRABE_THIRD_PARTY_DIR}/stb/include
    )

endfunction(linkLibrary)
