function(linkLibrary)

    SET(TARGET_NAME platform)
    set(BINARY_NAME ${TARGET_NAME})
    set(LINK_STATIC OFF)

    set(MODULE_DIR ${SHIRABE_WORKSPACE_ROOT_DIR}/shirabeengine/modules/platform)

    # -I
    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
        ${MODULE_DIR}/code/include
        )
		
	## -L
    #append_parentscope(
    #    SHIRABE_PROJECT_LIBRARY_DIRECTORIES
    #    ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/lib
    #    )
	#
	# -l

    # -l
    append_parentscope(
            SHIRABE_PROJECT_LIBRARY_TARGETS
            ${TARGET_NAME}
    )

endfunction(linkLibrary)
