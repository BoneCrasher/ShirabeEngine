function(linkLibrary)
    
    set(TARGET_DIR  ${SHIRABE_PLATFORM_PREFIX}${SHIRABE_PLATFORM_ADDRESS_SIZE}/${SHIRABE_PLATFORM_CONFIG})
    set(OCV_DIR     ${SHIRABE_THIRD_PARTY_DIR}/openCV/${TARGET_DIR})
	
	set(LIB_SUFFIX)
	if(SHIRABE_DEBUG)
		set(LIB_SUFFIX d)
	endif()
	
    # -I
    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
        ${OCV_DIR}/include
        )

    if(MSVC)
        # -L
        append_parentscope(
            SHIRABE_PROJECT_LIBRARY_DIRECTORIES
            ${OCV_DIR}/lib
            )
		# -l
		append_parentscope(
			SHIRABE_PROJECT_LIBRARY_TARGETS
			opencv_core2413${LIB_SUFFIX}
			opencv_imgproc2413${LIB_SUFFIX}
			opencv_highgui2413${LIB_SUFFIX}
			)
	elseif(UNIX AND NOT APPLE)
		# -l
		append_parentscope(
			SHIRABE_PROJECT_LIBRARY_TARGETS
			${OCV_DIR}/lib/libopencv_core.so.2.4.13
			${OCV_DIR}/lib/libopencv_imgproc.so.2.4.13
			${OCV_DIR}/lib/libopencv_highgui.so.2.4.13
			)
	endif()
endfunction(linkLibrary)
