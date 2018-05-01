function(linkQT5)
	
	set(QT5_ROOT $ENV{Qt5_ROOT})

	# Release/Debug?
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
        set(CONFIG release)
        set(QT_CONFIG)
		set(CONFIG_SUFFIX)
    else()
        set(CONFIG debug)
        set(QT_CONFIG)
		set(CONFIG_SUFFIX d)
    endif()
	
    if(SR_REQUEST_x64_BUILD)
		set(ENV{Qt5_DIR} ${QT5_ROOT}/Qt-5.10.0_msvc2017_x64${QT_CONFIG})
    else()
		set(ENV{Qt5_DIR} ${QT5_ROOT}/Qt-5.10.0_msvc2017_x86${QT_CONFIG})
    endif()
	
	message(STATUS "ROOT : ${QT5_ROOT}")
	message(STATUS "DIR : ${QT5_DIR}")

	set(
		SR_QT5_COMPONENTS
			Charts
			Core
			Concurrent
			Gui
			Network
			OpenGL
			PrintSupport
			Svg
			Xml
			Widgets
	)

	LogStatus(MESSAGES "Including Qt5-libraries:" ${SR_QT5_COMPONENTS})

	# Determine Qt5 
	find_package(
		Qt5
		5.10.0
		COMPONENTS
			${SR_QT5_COMPONENTS}
		REQUIRED
		PATHS 
			$ENV{Qt5_DIR})
	
	# Qt5 specific file config
    set(QT5_CMAKE_AUTOMOC ON PARENT_SCOPE)
    set(QT5_CMAKE_AUTOUIC ON PARENT_SCOPE)
	set(QT5_CMAKE_AUTORCC ON PARENT_SCOPE)
    set(QT5_AUTOMOC       ON PARENT_SCOPE)
    set(QT5_AUTOUIC       ON PARENT_SCOPE)
	set(QT5_AUTORCC       ON PARENT_SCOPE)

	set(QT5_AUTOUIC_SEARCH_PATHS ${SR_PROJECT_UI_DIR} PARENT_SCOPE)
	set(QT5_AUTORCC_SEARCH_PATHS ${SR_PROJECT_RES_DIR} PARENT_SCOPE)
	set(QT5_AUTOGEN_BUILD_DIR    ${SR_PROJECT_INC_DIR}/protected/generated PARENT_SCOPE)
	set(QT5_AUTORCC_OPTIONS      --verbose --binary PARENT_SCOPE)

	# QT5_ADD_RESOURCES(SR_QT5_RESOURCES ${SR_QT5_RESOURCE_INPUT})
	# QT5_WRAP_UI(SR_QT5_UIHEADERS       ${SR_QT5_UIS})
	# QT5_WRAP_CPP(SR_QT5_MOCSOURCES     ${SR_QT5_MOC})
	
	# -I
	set(QT5_INCLUDE_PATHS)
	set(QT5_LIBRARY_DIRECTORIES)
	set(QT5_LIBRARY_TARGETS)
	set(SR_QT5_LOCAL_DEFINITIONS)
	set(SR_QT5_LOCAL_COMPILE_DEFINITIONS)
	set(SR_QT5_LOCAL_EXECUTABLE_COMPILE_FLAGS)
	foreach(COMPONENT ${SR_QT5_COMPONENTS})
		message(STATUS "Qt5-Package: Qt5${COMPONENT} -> Include: ${Qt5${COMPONENT}_INCLUDE_DIRS}, Lib: ${Qt5${COMPONENT}_LIBRARIES}")
		foreach(INCLUDE ${Qt5${COMPONENT}_INCLUDE_DIRS})
			message(STATUS "Include: ${INCLUDE}")
			append(
				QT5_INCLUDE_PATHS
					"${INCLUDE}"
			)
		endforeach()
		foreach(DEF ${Qt5${COMPONENT}_DEFINITIONS})
			message(STATUS "Definitions: ${DEF}")
			set(
				SR_QT5_LOCAL_DEFINITIONS
					${SR_QT5_LOCAL_DEFINITIONS}
					${DEF}
			)
		endforeach()		
		set(SR_QT5_DEFINITIONS ${SR_QT5_LOCAL_DEFINITIONS} PARENT_SCOPE)
		foreach(COMPILE_DEF ${Qt5${COMPONENT}_COMPILE_DEFINITIONS})
			message(STATUS "Compile-Definitions: ${COMPILE_DEF}")
			set(
				SR_QT5_LOCAL_COMPILE_DEFINITIONS
				    ${SR_QT5_LOCAL_COMPILE_DEFINITIONS}
					${COMPILE_DEF}
			)
		endforeach()
		set(SR_QT5_COMPILE_DEFINITIONS ${SR_QT5_LOCAL_COMPILE_DEFINITIONS} PARENT_SCOPE)

		foreach(EXEC_COMPILE_FLAGS ${Qt5${COMPONENT}_EXECUTABLE_COMPILE_FLAGS})
			message(STATUS "Executable Compile Flags: ${EXEC_COMPILE_FLAGS}")
			set(
				SR_QT5_LOCAL_EXECUTABLE_COMPILE_FLAGS
					${SR_QT5_LOCAL_EXECUTABLE_COMPILE_FLAGS}
					${EXEC_COMPILE_FLAGS}
			)
		endforeach()
		set(SR_QT5_EXECUTABLE_COMPILE_FLAGS ${SR_QT5_LOCAL_EXECUTABLE_COMPILE_FLAGS} PARENT_SCOPE)

		foreach(LIB ${Qt5${COMPONENT}_LIBRARIES})
			message(STATUS "Lib: ${LIB}")
			get_target_property(Qt5${COMPONENT}_location ${LIB} LOCATION)
			# LOCATION returns the filename to the .dll-file in QT_DIR/bin/<filename>.
			# Try to find QT_DIR based on the CMake discovery results.
			get_filename_component(Qt5${COMPONENT}_location_path ${Qt5${COMPONENT}_location} DIRECTORY)
			get_filename_component(Qt5${COMPONENT}_location_path ${Qt5${COMPONENT}_location_path} DIRECTORY)
			append(
				QT5_LIBRARY_DIRECTORIES
					${Qt5${COMPONENT}_location_path}/lib
			)
			append(
				QT5_LIBRARY_TARGETS
					"${LIB}"
			)
		endforeach()
	endforeach()

	append_parentscope(
		SR_PROJECT_INCLUDEPATH
			${QT5_INCLUDE_PATHS}
			${SR_PROJECT_PUBLIC_DEPLOY_DIR}/include/protected/generated/include_${CONFIG}
			${SR_PROJECT_INC_DIR}/protected/generated/include_${CONFIG}
			${THIRD_PARTY_DIR}/libQGLviewer_2.7.1/include
	)
	
	message(STATUS "Current Include-Paths: ${SR_PROJECT_INCLUDEPATH}")
    # -L
	append_parentscope(
		SR_PROJECT_LIBRARY_DIRECTORIES	
			${QT5_LIBRARY_DIRECTORIES}
			${THIRD_PARTY_DIR}/libQGLviewer_2.7.1/lib
	)
    # -l
    if(MSVC)
		append_parentscope(
			SR_PROJECT_LIBRARY_TARGETS			
				qtmain${CONFIG_SUFFIX}
				QGLViewer${CONFIG_SUFFIX}2
				${QT5_LIBRARY_TARGETS}
				opengl32
				glu32
		)
	else()
	endif()
endfunction(linkQT5)
