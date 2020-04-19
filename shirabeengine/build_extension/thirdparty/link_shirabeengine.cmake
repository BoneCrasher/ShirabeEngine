function(linkLibrary)

    set(MODULE_DIR ${SHIRABE_WORKSPACE_ROOT_DIR}/shirabeengine)

    set(TARGET_NAME shirabeengine)
    set(LINK_STATIC OFF)

    # -I
    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
        ${MODULE_DIR}/code/include
        ${SHIRABE_PROJECT_DEPLOY_DIR}/include
        )
		
	# -L
    append_parentscope(
        SHIRABE_PROJECT_LIBRARY_DIRECTORIES
        ${SHIRABE_PROJECT_DEPLOY_DIR}/lib
        )

	# -l
	append_parentscope(
		SHIRABE_PROJECT_LIBRARY_TARGETS
		${TARGET_NAME}
		)

endfunction(linkLibrary)
