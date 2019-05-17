function(linkLibrary)
    
    set(TARGET_DIR  ${SHIRABE_PLATFORM_PREFIX}${SHIRABE_PLATFORM_ADDRESS_SIZE}/${SHIRABE_PLATFORM_CONFIG})
    set(DIR         ${THIRD_PARTY_DIR}/_deploy/cryptopp/${TARGET_DIR})
		
    # -I
    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
        ${DIR}/include
        )

    # -L
    append_parentscope(
        SHIRABE_PROJECT_LIBRARY_DIRECTORIES
        ${DIR}/lib
        )
        
    if(MSVC)
		# -l
		append_parentscope(
			SHIRABE_PROJECT_LIBRARY_TARGETS
			cryptopp
			)
	elseif(UNIX AND NOT APPLE)
		# -l
		append_parentscope(
			SHIRABE_PROJECT_LIBRARY_TARGETS
			libcryptopp
			)
	endif()
endfunction(linkLibrary)
