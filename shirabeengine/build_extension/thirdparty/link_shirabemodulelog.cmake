function(linkLibrary)

    set(BINARY_NAME log)
    set(LINK_STATIC OFF)

    set(MODULE_DIR ${SHIRABE_WORKSPACE_ROOT_DIR}/shirabeengine/modules/log)

    formatPlatformConfigName(
        ${BINARY_NAME}
        SHIRABE_ADDRESSMODEL_64BIT
        SHIRABE_PLATFORM_CONFIG
        ON
        BINARY_NAME
        )

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
	## -l
	#append_parentscope(
	#	SHIRABE_PROJECT_LIBRARY_TARGETS
	#	${BINARY_NAME}
	#	)

endfunction(linkLibrary)
