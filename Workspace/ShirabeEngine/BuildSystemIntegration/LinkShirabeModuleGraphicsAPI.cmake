function(linkLibrary)
    
	LogStatus(MESSAGES 
	
        ${SHIRABE_ADDRESSMODEL_64BIT}
        ${SHIRABE_PLATFORM_CONFIG})

	set(BINARY_NAME GraphicsAPI)
	set(LINK_STATIC OFF)

	formatPlatformConfigName(
        GraphicsAPI
        SHIRABE_ADDRESSMODEL_64BIT
        SHIRABE_PLATFORM_CONFIG
        OFF
        BINARY_NAME
        )

    # -I
    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
        ${SHIRABE_ENGINE_WS_DIR}/Modules/GraphicsAPI/code/include/protected
        ${SHIRABE_ENGINE_WS_DIR}/Modules/GraphicsAPI/code/include/public
        )
		
	# -L
    append_parentscope(
        SHIRABE_PROJECT_LIBRARY_DIRECTORIES
        ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/lib
        )

	# -l
	append_parentscope(
		SHIRABE_PROJECT_LIBRARY_TARGETS
		${BINARY_NAME}
		)

endfunction(linkLibrary)
