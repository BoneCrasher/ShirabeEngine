function(linkLibrary)

    set(TARGET_NAME graphicsapi)
    set(BINARY_NAME graphicsapi)
    set(LINK_STATIC OFF)

    set(MODULE_DIR ${SHIRABE_WORKSPACE_ROOT_DIR}/shirabeengine/modules/graphicsapi)

    # -I
    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
        ${MODULE_DIR}/code/include
        )
		
	# -L
    append_parentscope(
        SHIRABE_PROJECT_LIBRARY_DIRECTORIES
        ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/lib
        )

    # -l

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
