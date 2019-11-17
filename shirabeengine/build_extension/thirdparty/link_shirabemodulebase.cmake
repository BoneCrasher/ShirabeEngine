function(linkLibrary)

    set(TARGET_NAME base)
    set(BINARY_NAME base)
    set(LINK_STATIC OFF)

    set(MODULE_DIR ${SHIRABE_WORKSPACE_ROOT_DIR}/shirabeengine/modules/base)

    # -I
    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
        ${MODULE_DIR}/code/include
        )



    # -l
    if(SHIRABE_BUILD_STATICLIB)
        append_parentscope(
                SHIRABE_PROJECT_LIBRARY_TARGETS
                ${TARGET_NAME}
        )
    elseif(SHIRABE_BUILD_SHAREDLIB)
        append_parentscope(
                SHIRABE_PROJECT_LIBRARY_MODULES
                ${TARGET_NAME}
        )
    endif()

endfunction(linkLibrary)
