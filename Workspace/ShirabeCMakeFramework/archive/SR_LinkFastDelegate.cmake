function(linkFastDelegate)
    # Header only library

    # -I
    append_parentscope(
        SR_PROJECT_INCLUDEPATH
        ${THIRD_PARTY_DIR}/FastDelegate
        )
endfunction(linkFastDelegate)
