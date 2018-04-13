if(SHIRABE_CMAKE_DEBUG_LOG)
	message(STATUS "BEGIN: Parsing SHIRABE_3rdPartyLibrary_Helper.cmake")
endif()

#
# Include useful macros to handle filesystem and platform config
#
include(SHIRABE_CMake_Helper)

#
# Include all supported 3rd-party librarie's setup scripts
#

# Header only
include(SHIRABE_LinkASIO)
include(SHIRABE_LinkXML2)
include(SHIRABE_LinkXSLT)

function(linkLinuxOSEnvironment) 
	# Define and register options, one and multi value argument keywords.
	set(options)
	set(oneValueArgs)
	set(multiValueArgs
			OS_INCLUDE_PATHS
			OS_DEPEND_PATHS
			OS_LIBRARY_DIRECTORIES
			OS_LIBRARY_LINK_TARGETS
	)

	# Read them...
	cmake_parse_arguments(
		LIBLNX 
			"${options}" 
			"${oneValueArgs}" 
			"${multiValueArgs}" 
			${ARGN}
	)

    append_parentscope(
		SHIRABE_PROJECT_INCLUDEPATH
			/usr/include
	)

    append_parentscope(
		SHIRABE_PROJECT_LIBRARY_DIRECTORIES
			/usr/lib${SHIRABE_PLATFORM_ADDRESS_SIZE}
	)

	#
	# Append all provided additional values for -I,-L,-l and depend paths.
	#

	# -I, ?, -L, -l
	append_parentscope(SHIRABE_PROJECT_INCLUDEPATH         ${LIBLNX_OS_INCLUDE_PATHS}       )
	append_parentscope(SHIRABE_PROJECT_DEPENDPATH          ${LIBLNX_OS_DEPEND_PATHS}        )
	append_parentscope(SHIRABE_PROJECT_LIBRARY_DIRECTORIES ${LIBLNX_OS_LIBRARY_DIRECTORIES} )
    append_parentscope(SHIRABE_PROJECT_LIBRARY_TARGETS     ${LIBLNX_OS_LIBRARY_LINK_TARGETS} gcc)

endfunction(linkLinuxOSEnvironment)

function(linkWindowsOSEnvironment) 
# Define and register options, one and multi value argument keywords.
	set(options)
	set(oneValueArgs)
	set(multiValueArgs
			OS_INCLUDE_PATHS
			OS_DEPEND_PATHS
			OS_LIBRARY_DIRECTORIES
			OS_LIBRARY_LINK_TARGETS
	)

	# Read them...
	cmake_parse_arguments(
		LIBWIN 
			"${options}" 
			"${oneValueArgs}" 
			"${multiValueArgs}" 
			${ARGN}
	)

	set(WIN_TARGET_SUFFIX)
	if(SHIRABE_ADDRESSMODEL_64BIT)
        set(WIN_TARGET_SUFFIX x${SHIRABE_PLATFORM_ADDRESS_SIZE})
    endif()

	# Get the windows SDK dependencies to make sure Ws2_32.lib as well as the most 
	# fundamental windows libs/dlls (kernel32/ole32/...) can be found!
	file(TO_CMAKE_PATH "$ENV{WindowsSdkDir}include" WIN_SDK_DIR__INCLUDE)
	file(TO_CMAKE_PATH "$ENV{WindowsSdkDir}include" WIN_SDK_DIR__LIB)
    append_parentscope(
		SHIRABE_PROJECT_INCLUDEPATH
			"${WIN_SDK_DIR__INCLUDE}"
	)

    append_parentscope(
		SHIRABE_PROJECT_LIBRARY_DIRECTORIES
			"${WIN_SDK_DIR__LIB}/${WIN_TARGET_SUFFIX}"
	)

	#
	# Append all provided additional values for -I,-L,-l and depend paths.
	#
	
	# -I, ?, -L, -l
	append_parentscope(SHIRABE_PROJECT_INCLUDEPATH         ${LIBWIN_OS_INCLUDE_PATHS}       )
	append_parentscope(SHIRABE_PROJECT_DEPENDPATH          ${LIBWIN_OS_DEPEND_PATHS}        )
	append_parentscope(SHIRABE_PROJECT_LIBRARY_DIRECTORIES ${LIBWIN_OS_LIBRARY_DIRECTORIES} )
	append_parentscope(SHIRABE_PROJECT_LIBRARY_TARGETS     ${LIBWIN_OS_LIBRARY_LINK_TARGETS})
	#
endfunction(linkWindowsOSEnvironment)

#
# link() --> Handle all LINK_~-settings from a project script and call the respective
#            setup function.
#
function(link)
	message(STATUS "Setting up 3rd party libraries for ${SHIRABE_MODULE_NAME}")
	
	if(LINK_OPENCV)
		LogStatus(MESSAGES "Linking in OpenCV!")
		linkOpenCV()
	endif(LINK_OPENCV)

	if(LINK_XML2)
		LogStatus(MESSAGES "Linking in XML2")
		linkXML2()
	endif(LINK_XML2)

	if(LINK_XSLT)
		LogStatus(MESSAGES "Linking in XSLT")
		linkXSLT()
	endif(LINK_XSLT)
	
	if(LINK_ASIO)
		LogStatus(MESSAGES "Linking in ASIO")
		linkASIO()
	endif(LINK_ASIO)

	list(REMOVE_DUPLICATES SHIRABE_PROJECT_INCLUDEPATH)
	list(REMOVE_DUPLICATES SHIRABE_PROJECT_LIBRARY_DIRECTORIES)
	list(REMOVE_DUPLICATES SHIRABE_PROJECT_LIBRARY_TARGETS)

	#
	# Append platform specific ext. library setup
	#
	if(WIN32)
		linkWindowsOSEnvironment(
			SPECIFIC_LIBRARY_LINK_TARGETS 
				advapi32
				Ws2_32
				Iphlpapi
				version
		)
	elseif(UNIX AND NOT APPLE)
		linkLinuxOSEnvironment(
			SPECIFIC_LIBRARY_LINK_TARGETS
				dl
		)
	else() 
		message(STATUS "Unknown platform")
	endif()

	LogStatus(MESSAGES "link()::-I"   ${SHIRABE_PROJECT_INCLUDEPATH})
	LogStatus(MESSAGES "link()::-L"   ${SHIRABE_PROJECT_LIBRARY_DIRECTORIES})
	LogStatus(MESSAGES "link()::-l"   ${SHIRABE_PROJECT_LIBRARY_TARGETS})

	# Forward to parent scope making it usable for the generic build script
	set(SHIRABE_PROJECT_INCLUDEPATH         ${SHIRABE_PROJECT_INCLUDEPATH}         PARENT_SCOPE)
	set(SHIRABE_PROJECT_LIBRARY_DIRECTORIES ${SHIRABE_PROJECT_LIBRARY_DIRECTORIES} PARENT_SCOPE)
	set(SHIRABE_PROJECT_LIBRARY_TARGETS     ${SHIRABE_PROJECT_LIBRARY_TARGETS}     PARENT_SCOPE)
endfunction(link)

if(SHIRABE_CMAKE_DEBUG_LOG)
	message(STATUS "DONE:  Parsing SHIRABE_3rdPartyLibrary_Helper.cmake")
endif()

