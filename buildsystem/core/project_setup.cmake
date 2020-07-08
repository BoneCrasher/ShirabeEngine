
#-------------------------------------------------
# Predeclaration of additional definitions "-D"
#-------------------------------------------------
set(SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS)

#---------------------------------------------------------------------------------------------------------------------------------------------------
# PRECOMPILED HEADER
#
# The precompiled header options must be determined here already, since 
# the compiler and linker options evaluation requires the setting.
#---------------------------------------------------------------------------------------------------------------------------------------------------
LogStatus(
    MESSAGES
    " "
    "#### PRECOMPILED HEADERS ####"
    " ")


set(SHIRABE_PROJECT_PCH_FILENAME)
if(SHIRABE_PCH_ENABLED)  # SHIRABE_PCH_ENABLED := "TRUE" | "FALSE"
    # The PCH must be located 
    set(SHIRABE_PROJECT_PCH_FILENAME "${SHIRABE_PCH_FILENAME}")
        if(SHIRABE_PROJECT_PCH_FILENAME)
        # Additional PCH setup in case we use the PCH
        append(SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS SHIRABE_PCH_ENABLED)
    endif(SHIRABE_PROJECT_PCH_FILENAME)
endif(SHIRABE_PCH_ENABLED)

LogStatus(
    MESSAGES
    "Using PCH:    ${SHIRABE_PCH_ENABLED}"
    "PCH-Filename: ${SHIRABE_PROJECT_PCH_FILENAME}"
    " ")

#---------------------------------------------------------------------------------------------------------------------------------------------------
# COMPILER & LINKER OPTIONS
#---------------------------------------------------------------------------------------------------------------------------------------------------

LogStatus(
    MESSAGES
    " "
    "#### COMPILER & LINKER OPTIONS ####"
    " ")

#
# Load Meta-Options and map project specific overrides
# to the respective internal variables, if any.
#
include(compiler_options_default)
include(compiler_options_mapuseroverrides)

#
# Convert Meta-Options to native build-system compiler/linker options.
#
if(WIN32)
    include(compiler_options_apply_msvc)
elseif(UNIX AND NOT APPLE)
    include(compiler_options_apply_gcc)
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
    append(
        SHIRABE_CMAKE_CFLAGS
            ${SHIRABE_MSVC_CL_COMMON_FLAGS}
            ${SHIRABE_MSVC_CL_CFLAGS}
            ${SHIRABE_ADDITIONAL_MSVC_CL_COMMON_FLAGS}
            ${SHIRABE_ADDITIONAL_MSVC_CL_CFLAGS}
    )
    # Add additional compiler flags for the MSVC CXX-compiler "cl.exe"
    append(
        SHIRABE_CMAKE_CXXFLAGS
            ${SHIRABE_MSVC_CL_COMMON_FLAGS}
            ${SHIRABE_MSVC_CL_CXXFLAGS}
            ${SHIRABE_ADDITIONAL_MSVC_CL_COMMON_FLAGS}
            ${SHIRABE_ADDITIONAL_MSVC_CL_CXXFLAGS}
    )
	
    append(
        SHIRABE_CMAKE_CLDFLAGS
            ${SHIRABE_MSVC_LD_COMMON_FLAGS}
            ${SHIRABE_MSVC_LD_CFLAGS}
            ${SHIRABE_ADDITIONAL_MSVC_LD_COMMON_FLAGS}
            ${SHIRABE_ADDITIONAL_MSVC_LD_CFLAGS}
    )

    append(
        SHIRABE_CMAKE_CXXLDFLAGS
            ${SHIRABE_MSVC_LD_COMMON_FLAGS}
            ${SHIRABE_MSVC_LD_CXXFLAGS}
            ${SHIRABE_ADDITIONAL_MSVC_LD_COMMON_FLAGS}
            ${SHIRABE_ADDITIONAL_MSVC_LD_CXXFLAGS}
    )
elseif(UNIX AND NOT APPLE)

    if(SHIRABE_WERRORMODE)
        append(SHIRABE_CMAKE_CXXFLAGS -Werror)
    endif(SHIRABE_WERRORMODE)

	# Add additional compiler flags for the GCC C-compiler "gcc"
    append(
        SHIRABE_CMAKE_CFLAGS
            ${SHIRABE_GCC_CL_COMMON_FLAGS}
            ${SHIRABE_GCC_CL_CFLAGS}
            ${SHIRABE_ADDITIONAL_GCC_CL_COMMON_FLAGS}
            ${SHIRABE_ADDITIONAL_GCC_CL_CFLAGS}
    )
    # Add additional compiler flags for the GCC CXX-compiler "g++"
    append(
        SHIRABE_CMAKE_CXXFLAGS
            ${SHIRABE_GCC_CL_COMMON_FLAGS}
            ${SHIRABE_GCC_CL_CXXFLAGS}
            ${SHIRABE_ADDITIONAL_GCC_CL_COMMON_FLAGS}
            ${SHIRABE_ADDITIONAL_GCC_CL_CXXFLAGS}
    )

    append(
        SHIRABE_CMAKE_CLDFLAGS
            ${SHIRABE_GCC_LD_COMMON_FLAGS}
            ${SHIRABE_GCC_LD_CFLAGS}
            ${SHIRABE_ADDITIONAL_GCC_LD_COMMON_FLAGS}
            ${SHIRABE_ADDITIONAL_GCC_LD_CFLAGS}
    )
    append(
        SHIRABE_CMAKE_CXXLDFLAGS
            ${SHIRABE_GCC_LD_COMMON_FLAGS}
            ${SHIRABE_GCC_LD_CXXFLAGS}
            ${SHIRABE_ADDITIONAL_GCC_LD_COMMON_FLAGS}
            ${SHIRABE_ADDITIONAL_GCC_LD_CXXFLAGS}
    )
endif()

# Clean out duplicate options
if(SHIRABE_CMAKE_CFLAGS)
    list(REMOVE_DUPLICATES SHIRABE_CMAKE_CFLAGS)
endif()
if(SHIRABE_CMAKE_CXXFLAGS)
    list(REMOVE_DUPLICATES SHIRABE_CMAKE_CXXFLAGS)
endif()
if(SHIRABE_CMAKE_CLDFLAGS)
    list(REMOVE_DUPLICATES SHIRABE_CMAKE_CLDFLAGS)
endif()
if(SHIRABE_CMAKE_CXXLDFLAGS)
    list(REMOVE_DUPLICATES SHIRABE_CMAKE_CXXLDFLAGS)
endif()

LogStatus(
    MESSAGES
        "-----------------------------------------------------------------------------------"
        "Compiler/Linker flags:"
        "-----------------------------------------------------------------------------------"
        "C-Compiler flags:   " ${SHIRABE_CMAKE_CFLAGS}     " "
        "C++-Compiler flags: " ${SHIRABE_CMAKE_CXXFLAGS}   " "
        "C-Linker flags:     " ${SHIRABE_CMAKE_CLDFLAGS}   " "
        "C++-Linker flags:   " ${SHIRABE_CMAKE_CXXLDFLAGS}
        "-----------------------------------------------------------------------------------"
        " "
)


#---------------------------------------------------------------------------------------------------------------------------------------------------
# PREPROCESSOR
#---------------------------------------------------------------------------------------------------------------------------------------------------

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
                LINUX_SHARED_LIB
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
            SHIRABE_BUILD_DLL
    )
elseif(SHIRABE_BUILD_STATICLIB)
    append(
        SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS
            SHIRABE_BUILD_STATIC
    )
endif(SHIRABE_BUILD_SHAREDLIB)

if(NOT SHIRABE_DEBUG)
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

# Finally add up custom project definitions
if(SHIRABE_ADDITIONAL_PROJECT_PREPROCESSOR_DEFINITIONS)
    append(
        SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS
            ${SHIRABE_ADDITIONAL_PROJECT_PREPROCESSOR_DEFINITIONS}
    )
endif()
if(SHIRABE_DEBUG)
if(SHIRABE_ADDITIONAL_PROJECT_PREPROCESSOR_DEFINITIONS_DEBUG)
    append(
        SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS
            ${SHIRABE_ADDITIONAL_PROJECT_PREPROCESSOR_DEFINITIONS_DEBUG}
    )
endif()
else()
    if(SHIRABE_ADDITIONAL_PROJECT_PREPROCESSOR_DEFINITIONS_RELEASE)
        append(
            SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS
                ${SHIRABE_ADDITIONAL_PROJECT_PREPROCESSOR_DEFINITIONS_RELEASE}
        )
    endif()
endif()

#---------------------------------------------------------------------------------------------------------------------------------------------------
# INCLUDE PATH
#---------------------------------------------------------------------------------------------------------------------------------------------------

set(
    SHIRABE_PROJECT_INCLUDEPATH
        ${SHIRABE_PROJECT_CODE_DIR}/include
        ${SHIRABE_PROJECT_CODE_DIR}/source
        ${SHIRABE_ADDITIONAL_INCLUDEPATHS}
)


#---------------------------------------------------------------------------------------------------------------------------------------------------
# HEADER & SOURCE GENERATION OPERATIONS
#---------------------------------------------------------------------------------------------------------------------------------------------------

LogStatus(
    MESSAGES
    " "
    "#### HEADER & SOURCE GENERATION ####"
    " ")

# Important:
#   Since the autogeneration of Qt somehow sucks trying to append custom cmd switches,
#   we manually invoke the RCC tool in order to not embed the resources, but put them
#   into a shared resource library with the suffix .srdrl
#   SmartRay Dynamic Resources Library)d
set(SHIRABE_PROJECT_QT5_RCC_FILES)
if(SHIRABE_PROJECT_QT5_RESOURCES)
    message(STATUS "Compiling QT5 resources")

    # Read the environment variable Qt5 Root! Make sure it is specified...
    file(TO_CMAKE_PATH $ENV{Qt5_ROOT} QT5_RCC_PATH)

    # This path is unfortunately hardcoded but will also ensure, that
    # the correct Qt5 custom build will be used!
    set(QT5_RCC_EXEC ${QT5_RCC_PATH}x86/${SHIRABE_PLATFORM_CONFIG}/bin/rcc.exe)
    if(SHIRABE_ADDRESSMODEL_64BIT)
        set(QT5_RCC_EXEC ${QT5_RCC_PATH}/amd64/${SHIRABE_PLATFORM_CONFIG}/bin/rcc.exe)
    endif()

    message(STATUS "--> RCC: ${QT5_RCC_EXEC}")

    set(SHIRABE_PROJECT_QT5_GENERATED_RESOURCES)
    foreach(QT5_RESOURCE ${SHIRABE_PROJECT_QT5_RESOURCES})
        message(STATUS "----> File: ${QT5_RESOURCE}")

        get_filename_component(QT5_RESOURCE_BASEDIR  ${QT5_RESOURCE} DIRECTORY)
        get_filename_component(QT5_RESOURCE_BASENAME ${QT5_RESOURCE} NAME_WE)

        set(
            SHIRABE_PROJECT_QT5_SRDRL_PATH
            ${QT5_RESOURCE_BASEDIR}/${QT5_RESOURCE_BASENAME}.srdrl)

        execute_process(
            COMMAND
            ${QT5_RCC_EXEC} --verbose --binary -o ${SHIRABE_PROJECT_QT5_SRDRL_PATH} ${QT5_RESOURCE})

        append(
            SHIRABE_PROJECT_QT5_RCC_FILES
            ${SHIRABE_PROJECT_QT5_SRDRL_PATH})

        LogStatus(MESSAGES "--> SRDRL Output Files: " ${SHIRABE_PROJECT_QT5_RCC_FILES} "")
    endforeach()

    append(
        SHIRABE_PROJECT_SOURCES
            ${SHIRABE_PROJECT_QT5_GENERATED_RESOURCES})
endif()

#---------------------------------------------------------------------------------------------------------------------------------------------------
# HEADERS & SOURCES INCLUSION
#---------------------------------------------------------------------------------------------------------------------------------------------------

LogStatus(
    MESSAGES
    " "
    "#### HEADER & SOURCE INCLUSION ####"
    " ")


# Recursively find all .h/.hpp-files
set(SHIRABE_PROJECT_HEADERS)
recursivelyFindFilesWithExtensions(
    ROOT_DIR
        ${SHIRABE_PROJECT_INC_DIR}
        ${SHIRABE_PROJECT_SRC_DIR}
    EXTENSIONS
        .h;.hpp;.tpp
    EXCLUSIONS
        ${SHIRABE_EXCLUDED_HEADERS}
    RESULT_VARIABLE
        SHIRABE_PROJECT_HEADERS
)

# Recursively find all .c/.cpp-files;
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
        "Adding headers for compilation:"
        "-----------------------------------------------------------------------------------"
        ${SHIRABE_PROJECT_HEADERS}
        "-----------------------------------------------------------------------------------"
        " "
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


# Define, which headers shall be exported in which way for the library.
set(SHIRABE_PROJECT_PUBLIC_EXPORTED_HEADERS  ${SHIRABE_PROJECT_HEADERS})
set(SHIRABE_PROJECT_PRIVATE_EXPORTED_HEADERS ${SHIRABE_PROJECT_HEADERS})

if(SHIRABE_PROJECT_PUBLIC_EXPORTED_HEADERS)
    list(FILTER SHIRABE_PROJECT_PUBLIC_EXPORTED_HEADERS  INCLUDE REGEX "^${SHIRABE_PROJECT_INC_DIR}/*")
endif()
if(SHIRABE_PROJECT_PRIVATE_EXPORTED_HEADERS)
    list(FILTER SHIRABE_PROJECT_PRIVATE_EXPORTED_HEADERS INCLUDE REGEX "^${SHIRABE_PROJECT_SRC_DIR}/*")
endif()

LogStatus(
    MESSAGES
        " "
        "Publicly Exported Headers:"    ${SHIRABE_PROJECT_PUBLIC_EXPORTED_HEADERS}
        " "
        "Privately Exported Headers:"   ${SHIRABE_PROJECT_PRIVATE_EXPORTED_HEADERS}
        " "
)

#---------------------------------------------------------------------------------------------------------------------------------------------------
# EXTERNAL LIBRARY LINKAGE
#---------------------------------------------------------------------------------------------------------------------------------------------------
LogStatus(
    MESSAGES
    " "
    "#### EXTERNAL LIBRARY LINKAGE ####"
    " ")

link()

#---------------------------------------------------------------------------------------------------------------------------------------------------
# APPLY
#---------------------------------------------------------------------------------------------------------------------------------------------------

# Apply all SHIRABE_~-variables to CMAKE_~-variables, setup all targets, links and installs
include(apply)
