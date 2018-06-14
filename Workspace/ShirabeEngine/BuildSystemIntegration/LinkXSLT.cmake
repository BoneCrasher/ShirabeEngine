function(linkLibrary)
    
    set(TARGET_DIR ${SHIRABE_PLATFORM_PREFIX}${SHIRABE_PLATFORM_ADDRESS_SIZE}/${SHIRABE_PLATFORM_CONFIG})
    set(XSLT_DIR   ${SHIRABE_THIRD_PARTY_DIR}/libxslt/${TARGET_DIR})

    # -I
    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
        ${XSLT_DIR}/include
        )

    # -L
    append_parentscope(
        SHIRABE_PROJECT_LIBRARY_DIRECTORIES
        ${XSLT_DIR}/lib
        )

    # -l
    if(MSVC)
		append_parentscope(
			SHIRABE_PROJECT_LIBRARY_TARGETS
			libexslt_a
			libxslt_a
			)
	else()
		append_parentscope(
			SHIRABE_PROJECT_LIBRARY_TARGETS
			exslt
			xslt
			)
	endif()
endfunction(linkLibrary)
