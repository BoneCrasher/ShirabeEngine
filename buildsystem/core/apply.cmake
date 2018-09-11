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
# SHIRABE_ADDRESSMODEL_64BIT               True, if SHIRABE_REQUEST_x64_BUILD_CFG is passed as TRUE from the CMakeSettings for the
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
# SHIRABE_CMAKE_CXXLDFLAGS                Joint list of all linker flags for output linkage and the CXX-Dialects.
#                                     On MSVC, this will be empty and appended on the compiler flags by "/link"
# SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS                         All preprocessor macros used for compilation.
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

include(common_helper) # Includes macro: installHeadersStructured

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

if(SHIRABE_BUILD_SHAREDLIB AND SHIRABE_PROJECT_LIBRARY_MODULES)
    foreach(MODULE ${SHIRABE_PROJECT_LIBRARY_MODULES})
        set(SHIRABE_TRANSFORMED_CXXFLAGS "${SHIRABE_TRANSFORMED_CXXFLAGS} -Wl,--whole-archive ${MODULE}")
    endforeach()
    set(SHIRABE_TRANSFORMED_CXXFLAGS "${SHIRABE_TRANSFORMED_CXXFLAGS} -Wl,--no-whole-archive,-fPIC")
    set(SHIRABE_TRANSFORMED_CXXFLAGS "${SHIRABE_TRANSFORMED_CXXFLAGS} -fkeep-inline-functions")
    set(SHIRABE_TRANSFORMED_CXXFLAGS "${SHIRABE_TRANSFORMED_CXXFLAGS} -fkeep-static-functions")
endif()

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

set(CMAKE_CFLAGS    ${SHIRABE_TRANSFORMED_CFLAGS})
set(CMAKE_CXX_FLAGS ${SHIRABE_TRANSFORMED_CXXFLAGS})
		
set(SHIRABE_CMAKE_JOINED_LINKER_FLAGS
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
# Convert output suffixes to CMake variables for each output type 
#-----------------------------------------------------------------------------------------
set(CMAKE_STATIC_LIBRARY_SUFFIX ${SHIRABE_PROJECT_TARGET_SUFFIX_STATIC}) # [.lib/.a]
set(CMAKE_SHARED_LIBRARY_SUFFIX ${SHIRABE_PROJECT_TARGET_SUFFIX_SHARED}) # [.dll/.so]
set(CMAKE_EXECUTABLE_SUFFIX     ${SHIRABE_PROJECT_TARGET_SUFFIX_APP})    # [.exe/-]
		
#-----------------------------------------------------------------------------------------
# Add -L Options for the linker!
# IMPORTANT: The link directories have to be added BEFORE the targets are created!
#-----------------------------------------------------------------------------------------
# -L
if(SHIRABE_PROJECT_LIBRARY_TARGETS)
    LogStatus(MESSAGES "Appending library directories:")
    foreach(LIB_DIR ${SHIRABE_PROJECT_LIBRARY_DIRECTORIES})
        LogStatus(MESSAGES "-> ${LIB_DIR}")
    endforeach(LIB_DIR)
    LogStatus(MESSAGES " ")

    link_directories(
        ${SHIRABE_PROJECT_LIBRARY_DIRECTORIES}
    )
endif()
#-----------------------------------------------------------------------------------------


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SHIRABE_PROJECT_DEPLOY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${SHIRABE_PROJECT_DEPLOY_DIR}/lib)

#-----------------------------------------------------------------------------------------
LogStatus(
    MESSAGES
    "Determining CMAKE Target..."
    " ")

if(SHIRABE_BUILD_APPLICATION)
    add_executable(
        ${SHIRABE_MODULE_NAME}
        ${SHIRABE_PROJECT_HEADERS}
        ${SHIRABE_PROJECT_SOURCES}
    )

    set_target_properties(${SHIRABE_MODULE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${SHIRABE_PROJECT_DEPLOY_DIR}/bin)
    set_target_properties(${SHIRABE_MODULE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${SHIRABE_PROJECT_DEPLOY_DIR}/bin)

    LogStatus(
        MESSAGES
        "--> Application"
        " ")

elseif(SHIRABE_BUILD_STATICLIB)
    add_library(
        ${SHIRABE_MODULE_NAME}
        STATIC        
        ${SHIRABE_PROJECT_HEADERS}
        ${SHIRABE_PROJECT_SOURCES}
    )

    set_target_properties(${SHIRABE_MODULE_NAME} PROPERTIES ARCHIVE_OUTPUT_DEBUG_DIRECTORY   ${SHIRABE_PROJECT_DEPLOY_DIR}/lib)
    set_target_properties(${SHIRABE_MODULE_NAME} PROPERTIES ARCHIVE_OUTPUT_RELEASE_DIRECTORY ${SHIRABE_PROJECT_DEPLOY_DIR}/lib)

    # Append this, in order to have the automatic export header generation properly flag static library's export behaviour.
    # (See below)
    append(SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS ${SHIRABE_LIBRARY_KEY}_STATIC_BUILD)

    set_property(TARGET  ${SHIRABE_MODULE_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

    LogStatus(
        MESSAGES
        "--> Static Library"
        " ")
elseif(SHIRABE_BUILD_SHAREDLIB)
    add_library(
        ${SHIRABE_MODULE_NAME}
        SHARED
        ${SHIRABE_PROJECT_HEADERS}
        ${SHIRABE_PROJECT_SOURCES}
    )
    set_target_properties(${SHIRABE_MODULE_NAME} PROPERTIES LIBRARY_OUTPUT_DEBUG_DIRECTORY   ${SHIRABE_PROJECT_DEPLOY_DIR}/bin)
    set_target_properties(${SHIRABE_MODULE_NAME} PROPERTIES LIBRARY_OUTPUT_RELEASE_DIRECTORY ${SHIRABE_PROJECT_DEPLOY_DIR}/bin)

    set_property(TARGET  ${SHIRABE_MODULE_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
    LogStatus(
        MESSAGES
        "--> Shared Library"
        " ")
elseif(SHIRABE_HEADER_ONLY)
    add_custom_target(${SHIRABE_MODULE_NAME})

    LogStatus(
        MESSAGES
        "--> Header Only"
        " ")
endif()

# add_precompiled_header(${SHIRABE_MODULE_NAME} ${SHIRABE_PROJECT_SRC_DIR}/${SHIRABE_PROJECT_PCH_FILENAME})

if(SHIRABE_PCH_ENABLED)
    set(SHIRABE_PROJECT_PCH_OUTPUT_DIR ${SHIRABE_PROJECT_INTERMEDIATE_DIR}/${SHIRABE_MODULE_NAME})

	# Add PCH generation target for GCC and Clang only.
	if(NOT MSVC)
                string(REPLACE ".h" "" SHIRABE_PROJECT_PCH_FILENAME ${SHIRABE_PROJECT_PCH_FILENAME})

                set(SHIRABE_PCH_INCLUDE_PATHS)
                foreach(INC ${SHIRABE_PROJECT_INCLUDEPATH})
			LogStatus(MESSAGES "INCDIR: ${INC}")
                        set(SHIRABE_PCH_INCLUDE_PATHS
                                "${SHIRABE_PCH_INCLUDE_PATHS} -I${INC}")
		endforeach()

        set(SHIRABE_PROJECT_PCH_SOURCE ${SHIRABE_PROJECT_SRC_DIR}/${SHIRABE_PROJECT_PCH_FILENAME})

		set(
                        SHIRABE_PCH_GEN_CMD_ARGS
                "${CMAKE_CXX_FLAGS} ${SHIRABE_PCH_INCLUDE_PATHS} -o ${SHIRABE_PROJECT_PCH_OUTPUT_DIR}/${SHIRABE_PROJECT_PCH_FILENAME}.h.pch ${SHIRABE_PROJECT_PCH_SOURCE}.h")

                string(REPLACE " " ";" SHIRABE_PCH_GEN_CMD_ARGS ${SHIRABE_PCH_GEN_CMD_ARGS})
                set(SHIRABE_PCH_GEN_CMD ${CMAKE_CXX_COMPILER} ${SHIRABE_PCH_GEN_CMD_ARGS})
    
		LogStatus(
			MESSAGES
			"Adding custom pre-build step to generate precompiled header."
                        "--> ${SHIRABE_PROJECT_PCH_SOURCE}"
                        "--> Command: ${SHIRABE_PCH_GEN_CMD}"
			" ")
	
		add_custom_target(
                        ${SHIRABE_MODULE_NAME}_GeneratePCH
			COMMAND
                                if [ ! -f ${SHIRABE_PROJECT_PCH_OUTPUT_DIR} ]; then mkdir -p "${SHIRABE_PROJECT_PCH_OUTPUT_DIR}" \; fi && ${SHIRABE_PCH_GEN_CMD}
		)

                add_dependencies(${SHIRABE_MODULE_NAME} ${SHIRABE_MODULE_NAME}_GeneratePCH)
	endif()

    set(SHIRABE_PROJECT_INCLUDEPATH ${SHIRABE_PROJECT_PCH_OUTPUT_DIR} ${SHIRABE_PROJECT_INCLUDEPATH})
endif()
#-----------------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------
# Setup working directory of debugger
#-----------------------------------------------------------------------------------------

if(MSVC)
    LogStatus(
        MESSAGES
        "Setting Visual Studio debugger working directory to:"
        "--> ${VSWORKINGDIR}."
        " ")

    file(TO_NATIVE_PATH "${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin/" VSWORKINGDIR)
    set_target_properties(${SHIRABE_MODULE_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${VSWORKINGDIR}")
endif()

#-----------------------------------------------------------------------------------------
# Qt5 compile definitions
#-----------------------------------------------------------------------------------------
if(LINK_QT5)

    set_target_properties(${SHIRABE_MODULE_NAME} PROPERTIES AUTOGEN_BUILD_DIR "${AUTOGEN_BUILD_DIR}")

    LogStatus(
        MESSAGES
        "Compile Definitions for Qt5:"
        ${QT5_COMPILE_DEFINITIONS}
        " ")

    add_definitions(${SHIRABE_QT5_DEFINITIONS})

    LogStatus(
        MESSAGES
        "Appending compiler flags for Qt5."
        " ")

    if(SHIRABE_BUILD_APPLICATION)
        append(
            CMAKE_CFLAGS
            ${SHIRABE_QT5_EXECUTABLE_COMPILE_FLAGS})
        append(
            CMAKE_CXX_FLAGS
            ${SHIRABE_QT5_EXECUTABLE_COMPILE_FLAGS})
    endif(SHIRABE_BUILD_APPLICATION)
endif(LINK_QT5)

#-----------------------------------------------------------------------------------------
# Append the preprocessor defines to CMake
#-----------------------------------------------------------------------------------------

set(
    SHIRABE_PROJECT_DEFINITIONS
        ${SHIRABE_PROJECT_PREPROCESSOR_DEFINITIONS}
        ${SHIRABE_PROJECT_LIBRARY_DEFINITIONS})

if(NOT SHIRABE_HEADER_ONLY)
    target_compile_definitions(
        ${SHIRABE_MODULE_NAME}
        PUBLIC
        ${SHIRABE_PROJECT_DEFINITIONS}
    )
endif()

LogStatus(MESSAGES "Appending preprocessor definitions:")
foreach(DEFINE ${SHIRABE_PROJECT_DEFINITIONS})
    LogStatus(MESSAGES "-> ${DEFINE}")
endforeach(DEFINE)
LogStatus(MESSAGES " ")
#-----------------------------------------------------------------------------------------


#-----------------------------------------------------------------------------------------
# Automatic export header generation.
#
# Will generate a header filename called "SHIRABE_<Module>_Export.h" containg a
# macro named SHIRABE_<Module>_LIBRARY_EXPORT defined empty, to __declspec(dllexport) or
# __declspec(dllimport) respectively.
#-----------------------------------------------------------------------------------------

if(SHIRABE_BUILD_SHAREDLIB OR SHIRABE_BUILD_STATICLIB)

    LogStatus(
        MESSAGES
        "Generating export headers"
        " ")

    include(GenerateExportHeader)
    generate_export_header(
        ${SHIRABE_MODULE_NAME}
            BASE_NAME         ${SHIRABE_LIBRARY_KEY}
            EXPORT_MACRO_NAME ${SHIRABE_LIBRARY_KEY}_LIBRARY_EXPORT
            EXPORT_FILE_NAME  ${SHIRABE_PROJECT_GEN_DIR}/export_headers/protected/${SHIRABE_LIBRARY_KEY}_Export.h)
endif()

# Even if the export headers are not generated, append the include-directory
# so that dependencies' export headers can be located.
append(SHIRABE_PROJECT_INCLUDEPATH ${SHIRABE_PROJECT_GEN_DIR}/export_headers/protected)
#-----------------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------
# Append the includepaths to CMake
#-----------------------------------------------------------------------------------------

set(TRANSFORMED_INCLUDES)
if(SHIRABE_PROJECT_INCLUDEPATH)
    set(
        INCLUDE_PATH_INPUT
        ${SHIRABE_PROJECT_INCLUDEPATH}
        ${SHIRABE_PROJECT_ADDITIONAL_INCLUDEPATH})

    set(TRANSFORMED_INCLUDES)

    foreach(INC_TMP ${INCLUDE_PATH_INPUT})
        set(TMP "${INC_TMP}/")
        file(TO_CMAKE_PATH "${INC_TMP}/" TMP)

        append(
            TRANSFORMED_INCLUDES
            ${TMP})
    endforeach()

    if(NOT SHIRABE_HEADER_ONLY)
        target_include_directories(${SHIRABE_MODULE_NAME} BEFORE PUBLIC ${SHIRABE_PROJECT_INCLUDEPATH})
    endif()
endif()

LogStatus(MESSAGES "Appending include paths:")
foreach(INC_PATH ${TRANSFORMED_INCLUDES})
    LogStatus(MESSAGES "-> ${INC_PATH}")
endforeach(INC_PATH)
LogStatus(MESSAGES " ")
#-----------------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------
# Set -l flags
#-----------------------------------------------------------------------------------------
if(SHIRABE_PROJECT_LIBRARY_TARGETS AND NOT SHIRABE_HEADER_ONLY)

    LogStatus(MESSAGES "Appending library targets:")
    foreach(LIB_TARGET ${SHIRABE_PROJECT_LIBRARY_TARGETS})
        LogStatus(MESSAGES "-> ${LIB_TARGET}")
    endforeach(LIB_TARGET)
    LogStatus(MESSAGES " ")

    target_link_libraries(
        ${SHIRABE_MODULE_NAME}
        ${SHIRABE_PROJECT_LIBRARY_TARGETS}
    )
endif()

#-----------------------------------------------------------------------------------------
# Fix output filename to contain the x64 and d suffixes.
#-----------------------------------------------------------------------------------------

LogStatus(
    MESSAGES
    "Setting compilate output name to:"
    "--> ${SHIRABE_MODULE_TARGET_OUTPUT_NAME}"
    " ")
set_target_properties(${SHIRABE_MODULE_NAME} PROPERTIES OUTPUT_NAME ${SHIRABE_MODULE_TARGET_OUTPUT_NAME})
#-----------------------------------------------------------------------------------------
		
#-----------------------------------------------------------------------------------------
# Fix linker language
#-----------------------------------------------------------------------------------------

LogStatus(
    MESSAGES
    "Forcing linker language to:"
    "--> ${SHIRABE_PROJECT_LANGUAGE}"
    " ")
set_target_properties(${SHIRABE_MODULE_NAME} PROPERTIES LINKER_LANGUAGE ${SHIRABE_PROJECT_LANGUAGE})
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

LogStatus(
    MESSAGES
    "Setting up INSTALL to:"
    "--> ${SHIRABE_PROJECT_DEPLOY_DIR}"
    " ")

if(SHIRABE_BUILD_APPLICATION)
    #
    # PRIVATE EXPORT
    #

    # Exe
    install(
        TARGETS
            ${SHIRABE_MODULE_NAME}
        RUNTIME DESTINATION
            ${SHIRABE_PROJECT_DEPLOY_DIR}/bin)

    #
    # PUBLIC EXPORT
    #

    # Exe
    install(
        TARGETS
            ${SHIRABE_MODULE_NAME}
        RUNTIME DESTINATION
            ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin)

    LogStatus(
        MESSAGES
        "Installing: ${SHIRABE_MODULE_NAME} executable to: ${SHIRABE_PROJECT_DEPLOY_DIR}/bin"
        " ")

elseif(SHIRABE_BUILD_SHAREDLIB)
    #
    # PROTECTED EXPORT
    #

    if(WIN32)
        # DLL
        install(
            TARGETS
                ${SHIRABE_MODULE_NAME}
            RUNTIME DESTINATION
                ${SHIRABE_PROJECT_DEPLOY_DIR}/bin)
        # Import-Lib
        install(
            TARGETS
                ${SHIRABE_MODULE_NAME}
            ARCHIVE DESTINATION
                ${SHIRABE_PROJECT_DEPLOY_DIR}/lib)
    elseif(UNIX AND NOT APPLE)
        install(
            TARGETS
                ${SHIRABE_MODULE_NAME}
            LIBRARY DESTINATION
                ${SHIRABE_PROJECT_DEPLOY_DIR}/lib)
    endif()

    # Headers
    installHeadersStructured(
        ${SHIRABE_PROJECT_DEPLOY_DIR}/include
        ${SHIRABE_PROJECT_INC_DIR}
        SHIRABE_PROJECT_PROTECTED_EXPORTED_HEADERS)
    installHeadersStructured(
        ${SHIRABE_PROJECT_DEPLOY_DIR}/include
        ${SHIRABE_PROJECT_INC_DIR}
        SHIRABE_PROJECT_PUBLIC_EXPORTED_HEADERS)

    # Make sure to copy over the exact PDB file on windows
    if(WIN32)
        if(SHIRABE_DEBUG)
            install(
                FILES
                    "${SHIRABE_PROJECT_DEPLOY_DIR}/bin/${SHIRABE_MODULE_NAME}.pdb"
                DESTINATION
                    "${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin")
        endif()
    endif()

    LogStatus(
        MESSAGES
        "Installing: ${SHIRABE_MODULE_NAME} DLL/so to:         ${SHIRABE_PROJECT_DEPLOY_DIR}/bin"
        "Installing: ${SHIRABE_MODULE_NAME} import library to: ${SHIRABE_PROJECT_DEPLOY_DIR}/lib"
        "Installing: ${SHIRABE_MODULE_NAME} PDB to:            ${SHIRABE_PROJECT_DEPLOY_DIR}/bin"
        " ")

elseif(SHIRABE_BUILD_STATICLIB)

    # Static-Lib
    install(
        TARGETS
            ${SHIRABE_MODULE_NAME}
        ARCHIVE DESTINATION
            ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/lib)

    installHeadersStructured(
        ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/include
        ${SHIRABE_PROJECT_INC_DIR}
        SHIRABE_PROJECT_PUBLIC_EXPORTED_HEADERS)

    LogStatus(
        MESSAGES
        "Installing: ${SHIRABE_MODULE_NAME} static library to: ${SHIRABE_PROJECT_DEPLOY_DIR}/lib"
        " ")

elseif(SHIRABE_HEADER_ONLY)
    installHeadersStructured(
        ${SHIRABE_PROJECT_DEPLOY_DIR}/include
        ${SHIRABE_PROJECT_INC_DIR}
        SHIRABE_PROJECT_PUBLIC_EXPORTED_HEADERS)

    installHeadersStructured(
        ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/include
        ${SHIRABE_PROJECT_INC_DIR}
        SHIRABE_PROJECT_PUBLIC_EXPORTED_HEADERS)
endif()

if(LINK_QT5)
    if(SHIRABE_PROJECT_QT5_RCC_FILES)
        install(
            FILES
                ${SHIRABE_PROJECT_QT5_RCC_FILES}
            DESTINATION
                ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/bin)

        install(
            FILES
                 ${SHIRABE_PROJECT_QT5_RCC_FILES}
            DESTINATION
                 ${SHIRABE_PROJECT_DEPLOY_DIR}/bin)
    endif(SHIRABE_PROJECT_QT5_RCC_FILES)

    install(
        DIRECTORY
            ${SHIRABE_PROJECT_GEN_DIR}/include_${SHIRABE_PLATFORM_CONFIG}/
        DESTINATION
             ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/include/protected)



     LogStatus(
         MESSAGES
         "Qt5-Install:"
         "Installing SRDRL-files to:        ${SHIRABE_PROJECT_DEPLOY_DIR}/lib"
         "Installing generated headeres to: ${SHIRABE_PROJECT_PUBLIC_DEPLOY_DIR}/include/protected"
         " ")

endif(LINK_QT5)

#-----------------------------------------------------------------------------------------
# Add custom INSTALL_~-task to allow "only build and install single target" instead of default "ALL"
#-----------------------------------------------------------------------------------------
ADD_CUSTOM_TARGET(
    INSTALL_${SHIRABE_MODULE_NAME}
    ${CMAKE_COMMAND}
    -DBUILD_TYPE=${CMAKE_BUILD_TYPE}
    -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_install.cmake)
ADD_DEPENDENCIES(INSTALL_${SHIRABE_MODULE_NAME} ${SHIRABE_MODULE_NAME})

LogStatus(
    MESSAGES
    "Created custom INSTALL-target: " "--> INSTALL_${SHIRABE_MODULE_NAME}"
    "Created implicit dependency:   " "--> INSTALL_${SHIRABE_MODULE_NAME} => ${SHIRABE_MODULE_NAME}"
    " ")

#-----------------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------
# Postbuild Events?
#-----------------------------------------------------------------------------------------
