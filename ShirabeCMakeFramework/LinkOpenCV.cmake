function(linkOpenCV)
    
    set(TARGET_DIR  ${SHIRABE_PLATFORM_PREFIX}${SHIRABE_PLATFORM_ADDRESS_SIZE}/${SHIRABE_PLATFORM_CONFIG})
    set(OCV_DIR_INC ${THIRD_PARTY_DIR}/openCV/${TARGET_DIR}/include)
    set(OCV_DIR     ${THIRD_PARTY_DIR}/openCV/${TARGET_DIR})

    # -I
    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
        ${OCV_DIR_INC}
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
			opencv_core2413${CONFIG}
			opencv_imgproc2413${CONFIG}
			opencv_highgui2413${CONFIG}
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
endfunction(linkOpenCV)
