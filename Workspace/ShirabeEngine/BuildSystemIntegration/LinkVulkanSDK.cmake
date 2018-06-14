function(linkLibrary)
    
    set(DIR $ENV{VK_SDK_PATH})
	file(TO_CMAKE_PATH ${DIR} DIR)
	
	LogStatus(MESSAGES "Vulkan-Dir: ${DIR}")

	set(LIB_SUFFIX)
	if(SHIRABE_DEBUG)
		set(LIB_SUFFIX)
	endif()

	set(ADDRESS_SUFFIX)
	if(NOT SHIRABE_ADDRESSMODEL_64BIT)
		set(ADDRESS_SUFFIX 32)
	endif()
	
    # -I
    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
        ${DIR}/Include
        )

        # -L
    append_parentscope(
        SHIRABE_PROJECT_LIBRARY_DIRECTORIES
        ${DIR}/Lib${ADDRESS_SUFFIX}
        )
	# -l
	append_parentscope(
		SHIRABE_PROJECT_LIBRARY_TARGETS
		vulkan-1
		)
endfunction(linkLibrary)
