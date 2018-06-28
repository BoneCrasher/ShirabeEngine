function(linkLibrary)
    # -I
    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
        ${THIRD_PARTY_DIR}/stb/include
    )

endfunction(linkLibrary)
