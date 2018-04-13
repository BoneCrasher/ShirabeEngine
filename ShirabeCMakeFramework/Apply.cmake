# Variables that have been set by the generic buld script:
#
# SHIRABE_CONFIG__DEBUG_LOG               1, If verbose logging should happen
# SHIRABE_PROJECT_ID                      The base name of the module without target and platform info.
# SHIRABE_PROJECT_DIR                     The directory containing the CMakeLists with the most recent call to project()
# SHIRABE_MODULE_NAME                     The final module name containing platform and config info.
# SHIRABE_TEMPLATE                        The target type to be compiled, changing behavior of the build script.
# SHIRABE_BUILD_APPLICATION               True, if SHIRABE_TEMPLATE is Application
# SHIRABE_BUILD_STATICLIB                 True, if SHIRABE_TEMPLATE is StaticLib
# SHIRABE_BUILD_SHAREDLIB                 True, if SHIRABE_TEMPLATE is SharedLib
# SHIRABE_PROJECT_TARGET_SUFFIX_SHARED    .dll/.so
# SHIRABE_PROJECT_TARGET_SUFFIX_STATIC    .lib/.a
# SHIRABE_PROJECT_TARGET_SUFFIX_APP       .app
# SHIRABE_REQUEST_x64_BUILD               True, if SHIRABE_REQUEST_x64_BUILD_CFG is passed as TRUE from the CMakeSettings for the
#                                     current VS config.
# SHIRABE_PLATFORM_TARGET                 Win32|Win64|Linux32|Linux64
# SHIRABE_PLATFORM_CONFIG                 Debug|Release
# SHIRABE_PLATFORM_CONFIG_SUFFIX          Suffix in the format: <Target>/<Config>, e.g. Win32/Debug
# SHIRABE_PROJECT_CODE_DIR                The /code-directory of the current project
# SHIRABE_PROJECT_INC_DIR                 The /include-directory below the code-dir
# SHIRABE_PROJECT_SRC_DIR                 The /source-directory below the code-dir.
# SHIRABE_PROJECT_BUILD_DIR               The /build/<target>-directory of the current project containing all
#                                     all build, intermediate and output files.
# SHIRABE_PROJECT_INTERMEDIATE_DIR        The /build/<target>/intermediate-directory of the current project 
#                                     containing all intermediate data.
# SHIRABE_PROJECT_OBJECTS_DIR             The /build/<target>/intermediate/obj-directory containing all .O-files.
# SHIRABE_PROJECT_DEPLOY_DIR              The target directory where the build-results should be  
#                                     copied to.
# SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR       The target directory where the build-results should be  
#                                     copied to.
#								     
# SHIRABE_CMAKE_CFLAGS                    Joint list of all compiler flags for the C-Language.
# SHIRABE_CMAKE_CXXFLAGS                  Joint list of all compiler flags for the CXX-Dialects.
# SHIRABE_CMAKE_CLDFLAGS                  Joint list of all linker flags for output linkage and the C-Language. 
#                                     On MSVC, this will be empty and appended on the compiler flags by "/link"
# SHIRABE_CMAKE_CCXXLDFLAGS               Joint list of all linker flags for output linkage and the CXX-Dialects. 
#                                     On MSVC, this will be empty and appended on the compiler flags by "/link"
# SHIRABE_DEFINES                         All preprocessor macros used for compilation.
# SHIRABE_PRECOMPILED_HEADER              Contains the PCH-filename, if SHIRABE_PCH_ENABLED is true. 
# SHIRABE_PROJECT_INCLUDEPATH             Contains a list of all -I-Options/Paths.
# SHIRABE_PROJECT_DEPENDPATH              Contains a list of all addition dependency resolving paths.
# SHIRABE_PROJECT_LIBRARY_DIRECTORIES     Contains a list of all -L-Options/Paths.
# SHIRABE_PROJECT_LIBRARY_TARGETS         Contains a list of all -l-Options/Paths.
# SHIRABE_PROJECT_SOURCES                 Contains all .c/.cpp-files to be included for compilation.
# SHIRABE_PROJECT_HEADERS                 Contains all .h/.hpp-files to be included for compilation.
# SHIRABE_PROJECT_<MODE>_EXPORTED_HEADERS Contains all headers to be exported with the binary, using MODE [PUBLIC|PROTECTED|PRIVATE]
# 
# ## 3rd Party ## 
# LINK~                              [True/False] - Tells, whether specific libraries are linked in or not. 
# 
# ## QT5 Specific ##
# SHIRABE_QT5_EXECUTABLE_COMPILE_FLAGS    Qt5 defined exe compile flags 
# SHIRABE_QT5_DEFINITIONS                 Qt5 defined preprocessor definitions
# QT5_COMPILE_DEFINITIONS            Qt5 defined preprocessor definitions specific for compilation
#

include(SHIRABE_CMake_Helper) # Includes macro: installHeadersStructured

#-----------------------------------------------------------------------------------------
# Prebuild Events?
#-----------------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------
# Build Events?
#-----------------------------------------------------------------------------------------
	
#-----------------------------------------------------------------------------------------
# Add desired compilate, compiler and linker flags
#-----------------------------------------------------------------------------------------
set(SHIRABE_TRANSFORMED_CFLAGS)
set(SHIRABE_TRANSFORMED_CXXFLAGS)
set(SHIRABE_TRANSFORMED_CLDFLAGS)
set(SHIRABE_TRANSFORMED_CXXLDFLAGS)

foreach(CFLAG ${SHIRABE_CMAKE_CFLAGS})
	set(SHIRABE_TRANSFORMED_CFLAGS "${SHIRABE_TRANSFORMED_CFLAGS} ${CFLAG}")
endforeach()

foreach(CXXFLAG ${SHIRABE_CMAKE_CXXFLAGS})
	set(SHIRABE_TRANSFORMED_CXXFLAGS "${SHIRABE_TRANSFORMED_CXXFLAGS} ${CXXFLAG}")
endforeach()

foreach(CLDFLAG ${SHIRABE_CMAKE_CLDFLAGS})
	set(SHIRABE_TRANSFORMED_CLDFLAGS "${SHIRABE_TRANSFORMED_CLDFLAGS} ${CLDFLAG}")
endforeach()

foreach(CXXLDFLAG ${SHIRABE_CMAKE_CXXLDFLAGS})
	set(SHIRABE_TRANSFORMED_CXXLDFLAGS "${SHIRABE_TRANSFORMED_CXXLDFLAGS} ${CXXLDFLAG}")
endforeach()

set(
	SHIRABE_CMAKE_CLDFLAGS
		${SHIRABE_TRANSFORMED_CLDFLAGS}
)
set(
	SHIRABE_CMAKE_CXXLDFLAGS
		${SHIRABE_TRANSFORMED_CXXLDFLAGS}
)

LogStatus(
	MESSAGES
	"Transformed C-flags: "      ${SHIRABE_TRANSFORMED_CFLAGS}
	"Transformed CXX-flags: "    ${SHIRABE_TRANSFORMED_CXXFLAGS}
	"Transformed LD C-flags: "   ${SHIRABE_TRANSFORMED_CLDFLAGS}
	"Transformed LD CXX-flags: " ${SHIRABE_TRANSFORMED_CXXLDFLAGS}
	)

set(CMAKE_CFLAGS   ${SHIRABE_TRANSFORMED_CFLAGS})
set(CMAKE_CXX_FLAGS ${SHIRABE_TRANSFORMED_CXXFLAGS})
		
set(
	SHIRABE_CMAKE_JOINED_LINKER_FLAGS 
		${SHIRABE_CMAKE_CLDFLAGS} 
		${SHIRABE_CMAKE_CXXLDFLAGS}
)
set(CMAKE_STATIC_LINKER_FLAGS ${SHIRABE_CMAKE_JOINED_LINKER_FLAGS})
set(CMAKE_SHARED_LINKER_FLAGS ${SHIRABE_CMAKE_JOINED_LINKER_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS    ${SHIRABE_CMAKE_JOINED_LINKER_FLAGS})
#-----------------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------
# Any additional tweaks on behaviour the compiler and linker options couldn't do?
#-----------------------------------------------------------------------------------------
if(SHIRABE_CC__TREAT_C_AS_CPP_CODE) # Supportive setting in addition to the compiler flags
	SET_SOURCE_FILES_PROPERTIES(${SHIRABE_PROJECT_SOURCES} PROPERTIES LANGUAGE CXX)
endif()
#-----------------------------------------------------------------------------------------
	
#-----------------------------------------------------------------------------------------
# Add -L Options for the linker!
#-----------------------------------------------------------------------------------------
# -L
LogStatus(MESSAGES "Defining -L-flags...")
if(SHIRABE_PROJECT_LIBRARY_TARGETS)
	LogStatus(MESSAGES "-L:" ${SHIRABE_PROJECT_LIBRARY_DIRECTORIES})
	link_directories(
		${SHIRABE_PROJECT_LIBRARY_DIRECTORIES}
		)
endif()
#-----------------------------------------------------------------------------------------
	
#-----------------------------------------------------------------------------------------
# Convert output suffixes to CMake variables for each output type 
#-----------------------------------------------------------------------------------------
set(CMAKE_STATIC_LIBRARY_SUFFIX ${SHIRABE_PROJECT_TARGET_SUFFIX_STATIC}) # [.lib/.a]
set(CMAKE_SHARED_LIBRARY_SUFFIX ${SHIRABE_PROJECT_TARGET_SUFFIX_SHARED}) # [.dll/.so]
set(CMAKE_EXECUTABLE_SUFFIX     ${SHIRABE_PROJECT_TARGET_SUFFIX_APP})    # [.exe/-]
		
#-----------------------------------------------------------------------------------------

if(SHIRABE_BUILD_APPLICATION)
	add_executable(
		${SHIRABE_MODULE_NAME}
		${SHIRABE_PROJECT_HEADERS}
		${SHIRABE_PROJECT_SOURCES}
		)
	set_target_properties(${SHIRABE_MODULE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${SHIRABE_PROJECT_DEPLOY_DIR}/bin)
	set_target_properties(${SHIRABE_MODULE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${SHIRABE_PROJECT_DEPLOY_DIR}/bin)
elseif(SHIRABE_BUILD_STATICLIB)
	add_library(
		${SHIRABE_MODULE_NAME}
		STATIC
		${SHIRABE_PROJECT_HEADERS}
		${SHIRABE_PROJECT_SOURCES}
		)
	set_target_properties(${SHIRABE_MODULE_NAME} PROPERTIES ARCHIVE_OUTPUT_DEBUG_DIRECTORY   ${SHIRABE_PROJECT_DEPLOY_DIR}/lib)
	set_target_properties(${SHIRABE_MODULE_NAME} PROPERTIES ARCHIVE_OUTPUT_RELEASE_DIRECTORY ${SHIRABE_PROJECT_DEPLOY_DIR}/lib)
elseif(SHIRABE_BUILD_SHAREDLIB)
	add_library(
		${SHIRABE_MODULE_NAME}
		SHARED
		${SHIRABE_PROJECT_HEADERS}
		${SHIRABE_PROJECT_SOURCES}
		)
	set_target_properties(${SHIRABE_MODULE_NAME} PROPERTIES LIBRARY_OUTPUT_DEBUG_DIRECTORY   ${SHIRABE_PROJECT_DEPLOY_DIR}/bin)
	set_target_properties(${SHIRABE_MODULE_NAME} PROPERTIES LIBRARY_OUTPUT_RELEASE_DIRECTORY ${SHIRABE_PROJECT_DEPLOY_DIR}/bin)
endif()

#-----------------------------------------------------------------------------------------
# Setup working directory of debugger
#-----------------------------------------------------------------------------------------
file(TO_NATIVE_PATH "${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin/" VSWORKINGDIR)
set_target_properties(${SHIRABE_MODULE_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${VSWORKINGDIR}")

#-----------------------------------------------------------------------------------------
# QT5 specific madness required for generated files!
#-----------------------------------------------------------------------------------------
if(LINK_QT5)
	message(STATUS "Qt5-Autogen: ${AUTOGEN_BUILD_DIR}")
	set_target_properties(${SHIRABE_MODULE_NAME} PROPERTIES AUTOGEN_BUILD_DIR "${AUTOGEN_BUILD_DIR}")

	LogStatus(MESSAGES "Compile Definitions for Qt5:" ${QT5_COMPILE_DEFINITIONS})

	add_definitions(${SHIRABE_QT5_DEFINITIONS})
 
	if(SHIRABE_BUILD_APPLICATION)	
		set(CMAKE_CFLAGS   
				${CMAKE_CFLAGS}
					${SHIRABE_QT5_EXECUTABLE_COMPILE_FLAGS})
		set(CMAKE_CXX_FLAGS 
				${CMAKE_CXX_FLAGS}
					${SHIRABE_QT5_EXECUTABLE_COMPILE_FLAGS})
	endif(SHIRABE_BUILD_APPLICATION)
endif(LINK_QT5)
#-----------------------------------------------------------------------------------------
# Append the preprocessor defines to CMake
#-----------------------------------------------------------------------------------------
LogStatus(MESSAGES "Appending definitions...")
target_compile_definitions(${SHIRABE_MODULE_NAME} PUBLIC ${SHIRABE_DEFINES})

foreach(DEFINE ${SHIRABE_DEFINES})
	LogStatus(MESSAGES "${DEFINE}")
endforeach(DEFINE)
#-----------------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------
# Append the includepaths to CMake
#-----------------------------------------------------------------------------------------
LogStatus(MESSAGES "Defining -I-flags...")
if(SHIRABE_PROJECT_INCLUDEPATH)

	set(TRANSFORMED_INCLUDES)

	foreach(INC_TMP ${SHIRABE_PROJECT_INCLUDEPATH})
		set(TMP "${INC_TMP}/")
		file(TO_CMAKE_PATH "${INC_TMP}/" TMP)

		set(
			TRANSFORMED_INCLUDES
			${TRANSFORMED_INCLUDES}
			${TMP}
		)
	endforeach()
	foreach(INC_TMP ${SHIRABE_PROJECT_ADDITIONAL_INCLUDEPATH})
		set(TMP "${INC_TMP}/")
		file(TO_CMAKE_PATH "${INC_TMP}/" TMP)

		set(
			TRANSFORMED_INCLUDES
			${TRANSFORMED_INCLUDES}
			${TMP}
		)
	endforeach()

	target_include_directories(${SHIRABE_MODULE_NAME} BEFORE PUBLIC  ${SHIRABE_PROJECT_INCLUDEPATH})
endif()
LogStatus(MESSAGES "-I:" ${SHIRABE_PROJECT_INCLUDEPATH})

get_target_property(CURR_INC_DIRS ${SHIRABE_MODULE_NAME} INCLUDE_DIRECTORIES)
LogStatus(MESSAGES "Include directories queried from target after set: " ${CURR_INC_DIRS})
#-----------------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------
# Set -l flags
#-----------------------------------------------------------------------------------------
LogStatus(MESSAGES "Defining -l-flags...")
if(SHIRABE_PROJECT_LIBRARY_TARGETS)
	LogStatus(MESSAGES "-l:" ${SHIRABE_PROJECT_LIBRARY_TARGETS})
	target_link_libraries(
		${SHIRABE_MODULE_NAME}
		${SHIRABE_PROJECT_LIBRARY_TARGETS}
		)
endif()

#-----------------------------------------------------------------------------------------
# Fix output filename to contain the x64 and d suffixes.
#-----------------------------------------------------------------------------------------
set_target_properties(${SHIRABE_MODULE_NAME} PROPERTIES OUTPUT_NAME ${SHIRABE_MODULE_TARGET_OUTPUT_NAME})
#-----------------------------------------------------------------------------------------
		
#-----------------------------------------------------------------------------------------
# Fix linker language
#-----------------------------------------------------------------------------------------
set_target_properties(${SHIRABE_MODULE_NAME} PROPERTIES LINKER_LANGUAGE ${SHIRABE_LANGUAGE})
#-----------------------------------------------------------------------------------------


#-----------------------------------------------------------------------------------------
# Apply internal dependencies explicitely given from outside. This should only be used 
#  for dependencies a specific project is aware of, i.e. subprojects of the respective
#  project.
#  Do not refer to projects outside the local source sub-tree.
#-----------------------------------------------------------------------------------------
# if(SHIRABE_PROJECT_INTERNAL_DEPENDENCIES)
# 	foreach(INTERNAL_DEPENDENCY ${SHIRABE_PROJECT_INTERNAL_DEPENDENCIES})
# 		add_dependencies(
# 			${SHIRABE_MODULE_NAME}
# 			${INTERNAL_DEPENDENCY}
# 			)
# 	endforeach()
# endif(SHIRABE_PROJECT_INTERNAL_DEPENDENCIES)
#-----------------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------
# Install configuration
#  This sets-up all output of headers and binaries.
#  Here we also export to PUBLIC, PROTECTED, PRIVATE! (TODO)
#-----------------------------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIR 
			${SHIRABE_PROJECT_DEPLOY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIR 
			${SHIRABE_PROJECT_DEPLOY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIR 
			${SHIRABE_PROJECT_DEPLOY_DIR}/lib)

message(STATUS "Deploy dir: ${SHIRABE_PROJECT_DEPLOY_DIR}")
if(SHIRABE_BUILD_APPLICATION)
	#
	# PRIVATE EXPORT
	#
	LogStatus(MESSAGES "Setting up PRIVATE EXPORT")

	# Exe
	install(
		TARGETS
		${SHIRABE_MODULE_NAME}
		RUNTIME DESTINATION
		${SHIRABE_PROJECT_DEPLOY_DIR}/bin
		)

	#
	# PUBLIC EXPORT
	#
	LogStatus(MESSAGES "Setting up PUBLIC EXPORT")

	# Exe
	install(
		TARGETS
		${SHIRABE_MODULE_NAME}
		RUNTIME DESTINATION
		${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin
		)
elseif(SHIRABE_BUILD_SHAREDLIB)
	#
	# PRIVATE EXPORT
	#
	LogStatus(MESSAGES "Setting up PRIVATE EXPORT")

	if(WIN32)
		# DLL
		install(
			TARGETS
			${SHIRABE_MODULE_NAME}
			RUNTIME DESTINATION
			${SHIRABE_PROJECT_DEPLOY_DIR}/bin
			)
		# Import-Lib
		install(
			TARGETS
			${SHIRABE_MODULE_NAME}
			ARCHIVE DESTINATION
			${SHIRABE_PROJECT_DEPLOY_DIR}/lib
			)
	elseif(UNIX AND NOT APPLE)
		install(
			TARGETS
			${SHIRABE_MODULE_NAME}
			LIBRARY DESTINATION
			${SHIRABE_PROJECT_DEPLOY_DIR}/lib
			)
	endif()

	# Headers
	installHeadersStructured(
		${SHIRABE_PROJECT_DEPLOY_DIR}/include
		${SHIRABE_PROJECT_INC_DIR}
		SHIRABE_PROJECT_PRIVATE_EXPORTED_HEADERS
		)
	installHeadersStructured(
		${SHIRABE_PROJECT_DEPLOY_DIR}/include
		${SHIRABE_PROJECT_INC_DIR}
		SHIRABE_PROJECT_PUBLIC_EXPORTED_HEADERS
		)

	#
	# PUBLIC EXPORT
	#
	LogStatus(MESSAGES "Setting up PUBLIC EXPORT")

	if(WIN32)
		# DLL
		install(
			TARGETS
			${SHIRABE_MODULE_NAME}
			RUNTIME DESTINATION
			${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin
			)
		# Import-Lib
		install(
			TARGETS
			${SHIRABE_MODULE_NAME}
			ARCHIVE DESTINATION
			${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/lib
			)
	elseif(UNIX AND NOT APPLE)
		install(
			TARGETS
			${SHIRABE_MODULE_NAME}
			LIBRARY DESTINATION
			${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/lib
			)
	endif()
	
	installHeadersStructured(
		${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/include
		${SHIRABE_PROJECT_INC_DIR}
		SHIRABE_PROJECT_PRIVATE_EXPORTED_HEADERS
		)
	installHeadersStructured(
		${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/include
		${SHIRABE_PROJECT_INC_DIR}
		SHIRABE_PROJECT_PUBLIC_EXPORTED_HEADERS
		)

	# Make sure to copy over the exact PDB file on windows
	if(WIN32)
		if("${SHIRABE_PLATFORM_CONFIG}" STREQUAL "Debug")
			install(FILES "${SHIRABE_PROJECT_DEPLOY_DIR}/bin/${SHIRABE_MODULE_NAME}.pdb" DESTINATION "${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin")
		endif()
	endif()

elseif(SHIRABE_BUILD_STATICLIB)
	#
	# PRIVATE EXPORT
	#
	LogStatus(MESSAGES "Setting up PRIVATE EXPORT")

	# Headers
	installHeadersStructured(
		${SHIRABE_PROJECT_DEPLOY_DIR}/include
		${SHIRABE_PROJECT_INC_DIR}
		SHIRABE_PROJECT_PRIVATE_EXPORTED_HEADERS
		)
	installHeadersStructured(
		${SHIRABE_PROJECT_DEPLOY_DIR}/include
		${SHIRABE_PROJECT_INC_DIR}
		SHIRABE_PROJECT_PUBLIC_EXPORTED_HEADERS
		)
	# Static-Lib
	install(
		TARGETS
		${SHIRABE_MODULE_NAME}
		ARCHIVE DESTINATION
		${SHIRABE_PROJECT_DEPLOY_DIR}/lib
		)

	#
	# PUBLIC EXPORT
	#
	LogStatus(MESSAGES "Setting up PUBLIC EXPORT")

	# Static-Lib
	install(
		TARGETS
		${SHIRABE_MODULE_NAME}
		ARCHIVE DESTINATION
		${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/lib
		)
		
	installHeadersStructured(
		${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/include
		${SHIRABE_PROJECT_INC_DIR}
		SHIRABE_PROJECT_PRIVATE_EXPORTED_HEADERS
		)
	installHeadersStructured(
		${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/include
		${SHIRABE_PROJECT_INC_DIR}
		SHIRABE_PROJECT_PUBLIC_EXPORTED_HEADERS
		)
endif()

if(LINK_QT5)
	if(SHIRABE_PROJECT_QT5_RCC_FILES)
		install(
			FILES
				${SHIRABE_PROJECT_QT5_RCC_FILES}
			DESTINATION
				${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin
		)

		install(
			FILES
				${SHIRABE_PROJECT_QT5_RCC_FILES}
			DESTINATION
				${SHIRABE_PROJECT_DEPLOY_DIR}/bin
		)
	endif(SHIRABE_PROJECT_QT5_RCC_FILES)

	install(
		DIRECTORY
			${SHIRABE_PROJECT_INC_DIR}/protected/generated/include_${SHIRABE_PLATFORM_CONFIG}/
		DESTINATION
			${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/include/protected
	)
endif(LINK_QT5)

if(SHIRABE_LANGUAGE)
    set_target_properties(${SHIRABE_MODULE_NAME} PROPERTIES LINKER_LANGUAGE ${SHIRABE_LANGUAGE})
endif()
#-----------------------------------------------------------------------------------------
# Add custom INSTALL_~-task to allow "only build and install single target" instead of default "ALL"
#-----------------------------------------------------------------------------------------
ADD_CUSTOM_TARGET(INSTALL_${SHIRABE_MODULE_NAME}
	${CMAKE_COMMAND}
	-DBUILD_TYPE=${CMAKE_BUILD_TYPE}
	-P ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake)
ADD_DEPENDENCIES(INSTALL_${SHIRABE_MODULE_NAME} ${SHIRABE_MODULE_NAME})
#-----------------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------
# Postbuild Events?
#-----------------------------------------------------------------------------------------
