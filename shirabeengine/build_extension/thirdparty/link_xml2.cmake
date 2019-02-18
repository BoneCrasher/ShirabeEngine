function(linkLibrary)
    
    set(TARGET_DIR ${SHIRABE_PLATFORM_PREFIX}${SHIRABE_PLATFORM_ADDRESS_SIZE}/${SHIRABE_PLATFORM_CONFIG})
    set(XML2_DIR   ${THIRD_PARTY_DIR}/_deploy/libxml2/${TARGET_DIR})

    # -I
    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
        ${XML2_DIR}/include/libxml2
        )

    # -L
    append_parentscope(
        SHIRABE_PROJECT_LIBRARY_DIRECTORIES
        ${XML2_DIR}/lib
        )

    # -l
	if(MSVC)
		append_parentscope(
			SHIRABE_PROJECT_LIBRARY_TARGETS
			libxml2_a
			)
	else()
		append_parentscope(
			SHIRABE_PROJECT_LIBRARY_TARGETS
			xml2
			)
	endif()
endfunction(linkLibrary)
