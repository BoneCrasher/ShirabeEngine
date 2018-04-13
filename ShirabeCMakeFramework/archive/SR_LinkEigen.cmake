function(linkEigen)
    # Header only library

    # -I
    append_parentscope(
        SR_PROJECT_INCLUDEPATH
        ${THIRD_PARTY_DIR}/eigen
        )
endfunction(linkEigen)
