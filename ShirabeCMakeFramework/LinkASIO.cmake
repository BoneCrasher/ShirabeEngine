function(linkASIO)
    # -I
    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
        ${THIRD_PARTY_DIR}/asio/include
    )

endfunction(linkASIO)
