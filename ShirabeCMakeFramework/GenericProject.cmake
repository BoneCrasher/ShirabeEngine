#-------------------------------------------------
# Basic project setup
#-------------------------------------------------

# Enable verbose logging. Set 0 to deactivate.
set(SHIRABE_CONFIG__DEBUG_LOG 1)

cmake_minimum_required(VERSION 3.9) # Bail out for unsupported versions automatically.

# Select the current source's directory name as the PROJECT_ID.
get_filename_component(SHIRABE_PROJECT_ID ${CMAKE_CURRENT_SOURCE_DIR} NAME)

#
# Check, if the project is added as a subcomponent in another project's context.
# If so, do not use the "project(...)" macro, to avoid multiple entries for the 
# same subproject in the target list.
#
get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
  message(STATUS "Project ${SHIRABE_PROJECT_ID} is sub-project. Not adding project(...)-command.")
else()
  project(${SHIRABE_PROJECT_ID})
endif()

# Some basic conditions: Where?, What?, How?
set(SHIRABE_PROJECT_DIR               ${CMAKE_CURRENT_SOURCE_DIR}) # Obvious...
set(SHIRABE_MODULE_NAME               ${SHIRABE_PROJECT_ID})       # This module's name is simply equal to the project id...
set(SHIRABE_MODULE_TARGET_OUTPUT_NAME ${SHIRABE_PROJECT_ID})       # The module name selected should serve as the base for target output naming!
set(SHIRABE_TEMPLATE                  ${SHIRABE_TEMPLATE})         # Application | StaticLib | SharedLib
set(SHIRABE_LANGUAGE                  CXX)                         # Always assume C++

# Take the upper case module name as library key for build related module aware macros.
string(TOUPPER ${SHIRABE_MODULE_NAME} SHIRABE_LIBRARY_KEY)

#---------------------------------------------------------------------------------------------------------------------------------------------------
# Basic environments
#---------------------------------------------------------------------------------------------------------------------------------------------------

LogStatus(MESSAGES "Searching for modules in: ${CMAKE_MODULE_PATH}")
LogStatus(MESSAGES "Working directory of project '${SHIRABE_PROJECT_ID}': ${SHIRABE_PROJECT_DIR}")

LogStatus(
	MESSAGES
	    " "
		"----------------------------------- Generic Project Script for ${SHIRABE_PROJECT_ID} -----------------------------------"
		" "
)
#---------------------------------------------------------------------------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------
# Determine, wether we build a static lib or a shared lib and derive properties
#-----------------------------------------------------------------------------------------
set(SHIRABE_BUILD_APPLICATION OFF)
set(SHIRABE_BUILD_STATICLIB   OFF)
set(SHIRABE_BUILD_SHAREDLIB   OFF)
set(SHIRABE_HEADER_ONLY       OFF)

if("${SHIRABE_TEMPLATE}" STREQUAL "Application")
	set(SHIRABE_BUILD_APPLICATION ON)
elseif("${SHIRABE_TEMPLATE}" STREQUAL "StaticLib")
	set(SHIRABE_BUILD_STATICLIB  ON)
elseif("${SHIRABE_TEMPLATE}" STREQUAL "SharedLib")
	set(SHIRABE_BUILD_SHAREDLIB ON)
elseif("${SHIRABE_TEMPLATE}" STREQUAL "HeaderOnly")
	set(SHIRABE_HEADER_ONLY ON)
endif()

LogStatus(
	MESSAGES
	" "
	"Build Mode: "
	"Build App:       ${SHIRABE_BUILD_APPLICATION}"
	"Build SharedLib: ${SHIRABE_BUILD_SHAREDLIB}"
	"Build StaticLib: ${SHIRABE_BUILD_STATICLIB}"
	" ")

if(WIN32)
    set(SHIRABE_PROJECT_TARGET_SUFFIX_STATIC .lib)
    set(SHIRABE_PROJECT_TARGET_SUFFIX_SHARED .dll)
    set(SHIRABE_PROJECT_TARGET_SUFFIX_APP    .exe)
else()
    set(SHIRABE_PROJECT_TARGET_SUFFIX_STATIC .a)
    set(SHIRABE_PROJECT_TARGET_SUFFIX_SHARED .so)
    set(SHIRABE_PROJECT_TARGET_SUFFIX_APP    )
endif(WIN32)
#-----------------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------
# Debug or not, das ist hier die Frage...
#
# Important: Visual Studio 2017 CMake will not specify CMAKE_BUILD_TYPE properly.
#            It uses some strange custom bullshit, which is why we have to define 
#            it manually invoking CMake!
#-----------------------------------------------------------------------------------------
set(SHIRABE_DEBUG OFF)
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	set(SHIRABE_DEBUG ON)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
	set(SHIRABE_DEBUG OFF)
endif()
message(STATUS "Debug-Build?: ${SHIRABE_DEBUG}")
#-----------------------------------------------------------------------------------------

#-------------------------------------------------
# Determine final module name, platform and config
#
# Will write values into:
#  SHIRABE_ADDRESSMODEL_64BIT
#  SHIRABE_PLATFORM_TARGET
#  SHIRABE_PLATFORM_CONFIG
#  SHIRABE_MODULE_OUTPUT_NAME
#-------------------------------------------------
setupTargetAndConfig(${SHIRABE_MODULE_NAME})

set(SHIRABE_MODULE_TARGET_OUTPUT_NAME ${SHIRABE_MODULE_OUTPUT_NAME})

LogStatus(
	MESSAGES
		""
		"SHIRABE_PLATFORM_TARGET:        ${SHIRABE_PLATFORM_TARGET}"
		"SHIRABE_PLATFORM_CONFIG:        ${SHIRABE_PLATFORM_CONFIG}"
		"SHIRABE_MODULE_NAME:            ${SHIRABE_MODULE_NAME}"
		"SHIRABE_MODULE_OUTPUT_NAME:     ${SHIRABE_MODULE_TARGET_OUTPUT_NAME}"
		"SHIRABE_PLATFORM_CONFIG_SUFFIX: ${SHIRABE_PLATFORM_CONFIG_SUFFIX}"
		""
)
#-------------------------------------------------

#-------------------------------------------------
# Path setup for SmartRay-project
#-------------------------------------------------
set(SHIRABE_PROJECT_CONF_DIR         ${SHIRABE_PROJECT_DIR}/config)
set(SHIRABE_PROJECT_CODE_DIR         ${SHIRABE_PROJECT_DIR}/code)
set(SHIRABE_PROJECT_INC_DIR          ${SHIRABE_PROJECT_DIR}/code/include)
set(SHIRABE_PROJECT_SRC_DIR          ${SHIRABE_PROJECT_DIR}/code/source)
set(SHIRABE_PROJECT_UI_DIR           ${SHIRABE_PROJECT_DIR}/ui)
set(SHIRABE_PROJECT_RES_DIR          ${SHIRABE_PROJECT_DIR}/resources)

set(SHIRABE_PROJECT_BUILD_DIR        ${SHIRABE_PROJECT_DIR}/_build/${SHIRABE_PLATFORM_CONFIG_SUFFIX})
set(SHIRABE_PROJECT_INTERMEDIATE_DIR ${SHIRABE_PUBLIC_BUILD_ROOT}/intermediate)
set(SHIRABE_PROJECT_OBJECTS_DIR      ${SHIRABE_PROJECT_INTERMEDIATE_DIR}/obj)
set(SHIRABE_PROJECT_GEN_DIR          ${SHIRABE_PROJECT_INTERMEDIATE_DIR}/gen)

set(SHIRABE_PROJECT_DEPLOY_DIR       ${SHIRABE_PROJECT_DIR}/_deploy/${SHIRABE_PLATFORM_CONFIG_SUFFIX})
# Only used for public installation! (YES I know I should be consistent with the above "private" deploy dir...)
set(SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR ${SHIRABE_PUBLIC_DEPLOY_ROOT}/${SHIRABE_PLATFORM_CONFIG_SUFFIX})
if(CMAKE_INSTALL_PREFIX)
	set(SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR ${CMAKE_INSTALL_PREFIX}/${SHIRABE_PLATFORM_CONFIG_SUFFIX})
endif(CMAKE_INSTALL_PREFIX)

set(EXECUTABLE_OUTPUT_PATH ${SHIRABE_PROJECT_DEPLOY_DIR})
set(LIBRARY_OUTPUT_PATH    ${SHIRABE_PROJECT_DEPLOY_DIR})
set(CMAKE_INTDIR           ${SHIRABE_PROJECT_INTERMEDIATE_DIR})

LogStatus(
	MESSAGES
	    " "
		"-----------------------------------------------------------------------------------"
		"Project-Directory-Setup:"
		"-----------------------------------------------------------------------------------"
		"${NAME} - Base Directory:            ${SHIRABE_PROJECT_DIR}"
		"${NAME} - Config Directory:          ${SHIRABE_PROJECT_CONF_DIR}"
		"${NAME} - Code Directory:            ${SHIRABE_PROJECT_CODE_DIR}"
		"${NAME} - Include Directory:         ${SHIRABE_PROJECT_INC_DIR}"
		"${NAME} - Source Directory:          ${SHIRABE_PROJECT_SRC_DIR}"
		"${NAME} - Ui Files Directory:        ${SHIRABE_PROJECT_UI_DIR}"
		"${NAME} - Resources Directory:       ${SHIRABE_PROJECT_RES_DIR}"
		""
		"${NAME} - Build Directory:           ${SHIRABE_PROJECT_BUILD_DIR}"
		"${NAME} - Intermediate Directory:    ${SHIRABE_PROJECT_INTERMEDIATE_DIR}"
		"${NAME} - Objects Directory:         ${SHIRABE_PROJECT_OBJECTS_DIR}"
		"${NAME} - Generated Files Directory: ${SHIRABE_PROJECT_GEN_DIR}"
		""
		"${NAME} - Deploy Directory:          ${SHIRABE_PROJECT_DEPLOY_DIR}"
		"${NAME} - Public Deploy Directory:   ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}"
		"-----------------------------------------------------------------------------------"
		" "
)
#-------------------------------------------------

#-------------------------------------------------
# Predeclaration of additional definitions "-D"
#-------------------------------------------------
set(SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS)

#-------------------------------------------------
# Precompiled header
#
# The precompiled header options must be determined here already, since 
# the compiler and linker options evaluation requires the setting.
#-------------------------------------------------
set(SHIRABE_PROJECT_PCH_FILENAME)
if(SHIRABE_PCH_ENABLED)  # SHIRABE_PCH_ENABLED := "TRUE" | "FALSE"
    # The PCH must be located 
    set(SHIRABE_PROJECT_PCH_FILENAME "${SHIRABE_PCH_FILENAME}")
	if(SHIRABE_PROJECT_PCH_FILENAME)
		# Additional PCH setup in case we use the PCH
        append(SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS SHIRABE_PCH_ENABLED)
    endif(SHIRABE_PROJECT_PCH_FILENAME)
endif(SHIRABE_PCH_ENABLED)
#-------------------------------------------------

#-------------------------------------------------
# Compiler and linker options
#-------------------------------------------------
#
# Load Meta-Options and map project specific overrides
# to the respective internal variables, if any.
#
include(HandleCompilerOptions_Defaults)
include(HandleCompilerOptions_Overrides)

#
# Convert Meta-Options to native build-system compiler/linker options.
#
if(WIN32)
	include(HandleCompilerOptions_Windows)
elseif(UNIX AND NOT APPLE)
	include(HandleCompilerOptions_Linux)
else()
	LogError(MESSAGES "Unsupported platform.")
endif()

# 
# Join together the various sources of compiler options defined.
# 
# For the C and CXX compiler there are:
#   1. Common flags   => applicable to both compilers
#   2. Specific flags => for each language individually
#   3. Additional Common and/or Specific flags provided from the project's
#      CMakeLists.txt
#
# Similarly, this has to be done for the LDFLAGS
set(SHIRABE_CMAKE_CFLAGS)
set(SHIRABE_CMAKE_CXXFLAGS)

set(SHIRABE_CMAKE_CLDFLAGS)
set(SHIRABE_CMAKE_CXXLDFLAGS)

if(MSVC)

	# Add additional compiler flags for the MSVC C-compiler "cl.exe"
	set(
		SHIRABE_CMAKE_CFLAGS 
			${SHIRABE_MSVC_CL_COMMON_FLAGS}
			${SHIRABE_MSVC_CL_CFLAGS}
	)
	# Add additional compiler flags for the MSVC CXX-compiler "cl.exe"
    set(
		SHIRABE_CMAKE_CXXFLAGS 
			${SHIRABE_MSVC_CL_COMMON_FLAGS}
			${SHIRABE_MSVC_CL_CXXFLAGS}
	)

	set(
		SHIRABE_CMAKE_CLDFLAGS
			${SHIRABE_MSVC_LD_COMMON_FLAGS}
			${SHIRABE_MSVC_LD_CFLAGS}
			${SHIRABE_ADDITIONAL_LD_COMMON_FLAGS}
			${SHIRABE_ADDITIONAL_LD_CFLAGS}
	)

	set(
		SHIRABE_CMAKE_CXXLDFLAGS
			${SHIRABE_MSVC_LD_COMMON_FLAGS}
			${SHIRABE_MSVC_LD_CXXFLAGS}
			${SHIRABE_ADDITIONAL_LD_COMMON_FLAGS}
			${SHIRABE_ADDITIONAL_LD_CXXFLAGS}
	)

elseif(UNIX AND NOT APPLE)

	# Add additional compiler flags for the GCC C-compiler "gcc"
    append(
		SHIRABE_CMAKE_CFLAGS
			${SHIRABE_GCC_CL_COMMON_FLAGS}
			${SHIRABE_GCC_CL_CFLAGS}
	)
	# Add additional compiler flags for the GCC CXX-compiler "g++"
    append(
		SHIRABE_CMAKE_CXXFLAGS
			${SHIRABE_GCC_CL_COMMON_FLAGS}
			${SHIRABE_GCC_CL_CXXFLAGS}
	)

	set(
		SHIRABE_CMAKE_CLDFLAGS
			${SHIRABE_GCC_LD_COMMON_FLAGS}
			${SHIRABE_GCC_LD_CFLAGS}
			${SHIRABE_ADDITIONAL_LD_COMMON_FLAGS}
			${SHIRABE_ADDITIONAL_LD_CFLAGS}
	)
	set(
		SHIRABE_CMAKE_CXXLDFLAGS
			${SHIRABE_GCC_LD_COMMON_FLAGS}
			${SHIRABE_GCC_LD_CXXFLAGS}
			${SHIRABE_ADDITIONAL_LD_COMMON_FLAGS}
			${SHIRABE_ADDITIONAL_LD_CXXFLAGS}
	)
endif()

LogStatus(
	MESSAGES
	    " "
		"-----------------------------------------------------------------------------------"
		"Compiler/Linker flags:"
		"-----------------------------------------------------------------------------------"
		"CFLAGS:     " ${SHIRABE_CMAKE_CFLAGS}
		"CXXFLAGS:   " ${SHIRABE_CMAKE_CXXFLAGS}
		"CLDFLAGS:   " ${SHIRABE_CMAKE_CLDFLAGS} 
		"CPPLDFLAGS: " ${SHIRABE_CMAKE_CXXLDFLAGS} 
		"-----------------------------------------------------------------------------------"
		" "
)
#-------------------------------------------------


#-------------------------------------------------
# Preprocessor fun...
#-------------------------------------------------

# Handle platform specific preprocessor stuff
if(WIN32)
	append(
		SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS
			WINDOWS
			_CRT_SECURE_NO_WARNINGS
	)

	if(SHIRABE_BUILD_SHAREDLIB)
		append(
			SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS
				_WINDLL
				_USRDLL
		)
	endif(SHIRABE_BUILD_SHAREDLIB)

	append(
		SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS
			WIN32
			_WIN32
	)

	# For compatibility reasons also append WIN32 and hope for the best...
	if("${SHIRABE_PLATFORM_TARGET}" STREQUAL "Win64")
		append(
			SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS
				WIN64
				_WIN64
		)
	endif()
endif(WIN32)

if(UNIX AND NOT APPLE)
	append(
		SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS
			LINUX
	)

	if(SHIRABE_BUILD_SHAREDLIB)
		append(
			SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS
				_LINUX_SHARED_LIB
		)
	endif()
endif()

# Character encodings
if(SHIRABE_UNICODE_ENABLED)
	append(
		SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS
			_UNICODE
			UNICODE
	)
endif(SHIRABE_UNICODE_ENABLED)

# Platform independent config
if(SHIRABE_BUILD_SHAREDLIB) 
	append(
		SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS
			BUILD_DLL
			SHIRABE_BUILD_DLL
	)
	message(STATUS "Setting SHIRABE_BUILD_DLL")
elseif(SHIRABE_BUILD_STATICLIB)
	append(
		SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS
			SHIRABE_BUILD_STATIC
	)
	message(STATUS "Setting SHIRABE_BUILD_STATIC")
else() 
	append(
		SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS
			SHIRABE_LINK_STATIC
			SHIRABE_LINK_DLL
	)
	message(STATUS "Setting SHIRABE_LINK_STATIC & SHIRABE_LINK_DLL")
endif(SHIRABE_BUILD_SHAREDLIB)

if("${SHIRABE_PLATFORM_CONFIG}" STREQUAL "Release")
	append(
		SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS
			_RELEASE
    )
else()
	append(
		SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS
			_DEBUG
    )
endif()

#-------------------------------------------------
# Finally add up custom project definitions
#-------------------------------------------------
if(SHIRABE_ADDITIONAL_PROJECT_PREPROCESSOR_DEFINITIONS)
	append(
		SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS
			${SHIRABE_ADDITIONAL_PROJECT_PREPROCESSOR_DEFINITIONS}
	)
endif()
#-------------------------------------------------

#-------------------------------------------------
# Make sure we can include our project headers
#-------------------------------------------------
set(
	SHIRABE_PROJECT_INCLUDEPATH
		${SHIRABE_PROJECT_CODE_DIR}/include/public 
		${SHIRABE_PROJECT_CODE_DIR}/include/protected 
		${SHIRABE_PROJECT_CODE_DIR}/include/private 
		${SHIRABE_ADDITIONAL_INCLUDEPATHS}
)
#-------------------------------------------------

#-------------------------------------------------
# Recursively find all .c/.cpp-files;
#-------------------------------------------------
set(SHIRABE_PROJECT_SOURCES)
recursivelyFindFilesWithExtensions(
	ROOT_DIR 
		${SHIRABE_PROJECT_SRC_DIR} 
	EXTENSIONS 
		.c;.cpp
	EXCLUSIONS
		${SHIRABE_EXCLUDED_SOURCES}
	RESULT_VARIABLE
		SHIRABE_PROJECT_SOURCES
)

LogStatus(
	MESSAGES
		"-----------------------------------------------------------------------------------"
		"Adding sources for compilation:"
		"-----------------------------------------------------------------------------------"
		${SHIRABE_PROJECT_SOURCES}
		"-----------------------------------------------------------------------------------"
		" "
)
#-------------------------------------------------

#-------------------------------------------------
# Recursively find all .h/.hpp-files
#-------------------------------------------------
set(SHIRABE_PROJECT_HEADERS)
recursivelyFindFilesWithExtensions(
	ROOT_DIR 
		${SHIRABE_PROJECT_INC_DIR} 
	EXTENSIONS 
		.h;.hpp
	EXCLUSIONS
		${SHIRABE_EXCLUDED_HEADERS}
	RESULT_VARIABLE
		SHIRABE_PROJECT_HEADERS
)

LogStatus(
	MESSAGES
		"-----------------------------------------------------------------------------------"
		"Adding headers for compilation:"
		"-----------------------------------------------------------------------------------"
		${SHIRABE_PROJECT_HEADERS}
		"-----------------------------------------------------------------------------------"
		" "
)
#-------------------------------------------------

#-------------------------------------------------
# Define, which headers shall be exported in which way for the library.
#-------------------------------------------------
set(SHIRABE_PROJECT_PUBLIC_EXPORTED_HEADERS    ${SHIRABE_PROJECT_HEADERS})
set(SHIRABE_PROJECT_PROTECTED_EXPORTED_HEADERS ${SHIRABE_PROJECT_HEADERS})
set(SHIRABE_PROJECT_PRIVATE_EXPORTED_HEADERS   ${SHIRABE_PROJECT_HEADERS})

list(FILTER SHIRABE_PROJECT_PUBLIC_EXPORTED_HEADERS    INCLUDE REGEX "^${SHIRABE_PROJECT_INC_DIR}/public/*")
list(FILTER SHIRABE_PROJECT_PROTECTED_EXPORTED_HEADERS INCLUDE REGEX "^${SHIRABE_PROJECT_INC_DIR}/protected/*")
list(FILTER SHIRABE_PROJECT_PRIVATE_EXPORTED_HEADERS   INCLUDE REGEX "^${SHIRABE_PROJECT_INC_DIR}/private/*")

LogStatus(
	MESSAGES 
		""
		"Public Exported Headers:"    ${SHIRABE_PROJECT_PUBLIC_EXPORTED_HEADERS}
		"Protected Exported Headers:" ${SHIRABE_PROJECT_PROTECTED_EXPORTED_HEADERS}
		"Private Exported Headers:"   ${SHIRABE_PROJECT_PRIVATE_EXPORTED_HEADERS}
		""
)
#-------------------------------------------------

#-------------------------------------------------
# 3RD-Party library setup
#-------------------------------------------------
link()
#-------------------------------------------------

#-------------------------------------------------
# Apply all SHIRABE_~-variables to CMAKE_~-variables, setup all targets, links and installs
#-------------------------------------------------
include(Apply)
#-------------------------------------------------
