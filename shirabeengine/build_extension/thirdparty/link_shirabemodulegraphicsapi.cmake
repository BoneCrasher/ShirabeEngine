function(linkLibrary)
    
	LogStatus(MESSAGES 
	
        ${SHIRABE_ADDRESSMODEL_64BIT}
        ${SHIRABE_PLATFORM_CONFIG})

    set(BINARY_NAME graphicsapi)
	set(LINK_STATIC OFF)

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
        ${SHIRABE_PROJECT_DEPLOY_DIR}/include
        )
		
	# -L
    append_parentscope(
        SHIRABE_PROJECT_LIBRARY_DIRECTORIES
        ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/lib
        )

    # -l
    if(SHIRABE_BUILD_STATICLIB)
        append_parentscope(
                SHIRABE_PROJECT_LIBRARY_TARGETS
                ${BINARY_NAME}
                )
    else()
        append_parentscope(
                SHIRABE_PROJECT_LIBRARY_MODULES
                ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/lib/lib${BINARY_NAME}.a
                )
    endif()

endfunction(linkLibrary)
