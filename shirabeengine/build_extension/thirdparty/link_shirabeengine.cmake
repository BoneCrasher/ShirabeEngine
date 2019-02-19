function(linkLibrary)

    set(MODULE_DIR ${SHIRABE_WORKSPACE_ROOT_DIR}/shirabeengine)

    set(BINARY_NAME shirabeengine)
    set(LINK_STATIC OFF)

    formatPlatformConfigName(
        ${BINARY_NAME}
        SHIRABE_ADDRESSMODEL_64BIT
        SHIRABE_PLATFORM_CONFIG
        OFF
        BINARY_NAME
        )

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
		${BINARY_NAME}
		)

endfunction(linkLibrary)
