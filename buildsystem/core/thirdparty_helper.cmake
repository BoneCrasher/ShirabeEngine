#
# Include useful macros to handle filesystem and platform config
#
include(common_helper)

function(linkLinuxOSEnvironment) 
    # Define and register options, one and multi value argument keywords.
    set(options)
    set(oneValueArgs)
    set(multiValueArgs
            OS_INCLUDE_PATHS
            OS_DEPEND_PATHS
            OS_LIBRARY_DIRECTORIES
            OS_LIBRARY_LINK_TARGETS)

    # Read them...
    cmake_parse_arguments(
        LIBLNX
            "${options}"
            "${oneValueArgs}"
            "${multiValueArgs}"
            ${ARGN})

    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
            /usr/include)

    append_parentscope(
        SHIRABE_PROJECT_LIBRARY_DIRECTORIES
            /usr/lib${SHIRABE_PLATFORM_ADDRESS_SIZE})
    #
    # Append all provided additional values for -I,-L,-l and depend paths.
    #

    # -I, ?, -L, -l
    append_parentscope(SHIRABE_PROJECT_INCLUDEPATH         ${LIBLNX_OS_INCLUDE_PATHS}       )
    append_parentscope(SHIRABE_PROJECT_DEPENDPATH          ${LIBLNX_OS_DEPEND_PATHS}        )
    append_parentscope(SHIRABE_PROJECT_LIBRARY_DIRECTORIES ${LIBLNX_OS_LIBRARY_DIRECTORIES} )
    append_parentscope(SHIRABE_PROJECT_LIBRARY_TARGETS     ${LIBLNX_OS_LIBRARY_LINK_TARGETS} 
                                                           stdc++fs
                                                           gcc
                                                           )

endfunction(linkLinuxOSEnvironment)

function(linkWindowsOSEnvironment) 
    # Define and register options, one and multi value argument keywords.
    set(options)
    set(oneValueArgs)
    set(multiValueArgs
            OS_INCLUDE_PATHS
            OS_DEPEND_PATHS
            OS_LIBRARY_DIRECTORIES
            OS_LIBRARY_LINK_TARGETS)

    # Read them...
    cmake_parse_arguments(
        LIBWIN
            "${options}"
            "${oneValueArgs}"
            "${multiValueArgs}"
            ${ARGN})

    set(WIN_TARGET_SUFFIX)
    if(SHIRABE_REQUEST_x64_BUILD)
        set(WIN_TARGET_SUFFIX x${SHIRABE_PLATFORM_ADDRESS_SIZE})
    endif()

    # Get the windows SDK dependencies to make sure Ws2_32.lib as well as the most
    # fundamental windows libs/dlls (kernel32/ole32/...) can be found!
    file(TO_CMAKE_PATH "$ENV{WindowsSdkDir}include" WIN_SDK_DIR__INCLUDE)
    file(TO_CMAKE_PATH "$ENV{WindowsSdkDir}include" WIN_SDK_DIR__LIB)

    append_parentscope(
        SHIRABE_PROJECT_INCLUDEPATH
        "${WIN_SDK_DIR__INCLUDE}")

    append_parentscope(
        SHIRABE_PROJECT_LIBRARY_DIRECTORIES
            "${WIN_SDK_DIR__LIB}/${WIN_TARGET_SUFFIX}")

    #
    # Append all provided additional values for -I,-L,-l and depend paths.
    #

    # -I, ?, -L, -l
    append_parentscope(SHIRABE_PROJECT_INCLUDEPATH         ${LIBWIN_OS_INCLUDE_PATHS}       )
    append_parentscope(SHIRABE_PROJECT_DEPENDPATH          ${LIBWIN_OS_DEPEND_PATHS}        )
    append_parentscope(SHIRABE_PROJECT_LIBRARY_DIRECTORIES ${LIBWIN_OS_LIBRARY_DIRECTORIES} )
    append_parentscope(SHIRABE_PROJECT_LIBRARY_TARGETS     ${LIBWIN_OS_LIBRARY_LINK_TARGETS})

endfunction(linkWindowsOSEnvironment)

#
# link() --> Handle all LINK_~-settings from a project script and call the respective
#            setup function.
#
function(link)
    LogStatus(MESSAGES
        "Setting up 3rd party libraries for ${SHIRABE_MODULE_NAME}"
        " ")

    if(SHIRABE_PROJECT_REQUESTED_LINK_TARGETS)
        foreach(TARGET ${SHIRABE_PROJECT_REQUESTED_LINK_TARGETS})

            string(TOLOWER ${TARGET} TARGET_LC)

            set(TARGET_SYSTEMDIR ${SHIRABE_BUILD_SYSTEM_DIR}/thirdparty/link_${TARGET_LC}.cmake)
            set(TARGET_EXTDIR    ${SHIRABE_BUILD_SYSTEM_EXT_DIR}/thirdparty/link_${TARGET_LC}.cmake)

            if(EXISTS "${TARGET_SYSTEMDIR}")
                include(${TARGET_SYSTEMDIR})
            elseif(EXISTS "${TARGET_EXTDIR}")
                include(${TARGET_EXTDIR})
            else()
                LogError(MESSAGES "Failed to determine link-script for requested library ${TARGET}.")
            endif()

            linkLibrary()
        endforeach()
    endif()

#    # Automated LINK-File inclusion and linkage
#    #
#    #  Scans for files with the filename scheme "link_~.cmake".
#    #  For each detected file: removes the "link_"-prefix and converts the remainder to upper case.
#    #  Finally the upper case module key is joined with "LINK_" and the script checks, whether
#    #  "LINK_<Key>" is set to true.
#    #  If so, the module file's "linkLibrary()" function is called.
#    FILE(GLOB LINK_MODULES_SHARED ${SHIRABE_BUILD_SYSTEM_DIR}/thirdparty/link_*[.]cmake)
#    FILE(GLOB LINK_MODULES_EXT    ${SHIRABE_BUILD_SYSTEM_EXT_DIR}/thirdparty/link_*[.]cmake)

#    LogStatus(
#        MESSAGES
#        "Shared link modules found:" ${LINK_MODULES_SHARED}
#        " "
#        "For expression:" "${SHIRABE_BUILD_SYSTEM_DIR}/thirdparty/link_*[.]cmake"
#        " ")
#    LogStatus(
#        MESSAGES
#        "Extension link modules found:" ${LINK_MODULES_EXT}
#        " "
#        "For expression" "${SHIRABE_BUILD_SYSTEM_EXT_DIR}/thirdparty/link_*[.]cmake"
#        " ")

#    set(LINK_MODULES
#            ${LINK_MODULES_SHARED}
#            ${LINK_MODULES_EXT})

#    foreach(LINK_MODULE ${LINK_MODULES})
#        get_filename_component(
#            LINK_MODULE_NAME # RESULT
#            ${LINK_MODULE}   # INPUT
#            NAME_WE)         # Filename w/o path and extension
#        string(TOUPPER ${LINK_MODULE_NAME} LINK_MODULE_KEY_UC)
#        if(${LINK_MODULE_KEY_UC})

#            LogStatus(
#                MESSAGES
#                "Linking in: ${LINK_MODULE}"
#                " ")

#            include(${LINK_MODULE_NAME})
#            linkLibrary()

#        endif()
#    endforeach()

    LogStatus(MESSAGES " ")

    # Additional post processing for Qt5 only
    if(LINK_QT5)
        LogStatus(MESSAGES "Linking in Qt5" " ")

        set(CMAKE_AUTOMOC              ${QT5_CMAKE_AUTOMOC}        PARENT_SCOPE)
        set(CMAKE_AUTOUIC              ${QT5_CMAKE_AUTOUIC}        PARENT_SCOPE)
        set(CMAKE_AUTORCC              ${QT5_CMAKE_AUTORCC}        PARENT_SCOPE)
        set(AUTOMOC                    ${QT5_AUTOMOC}              PARENT_SCOPE)
        set(AUTOUIC                    ${QT5_AUTOUIC}              PARENT_SCOPE)
        set(AUTORCC                    ${QT5_AUTORCC}              PARENT_SCOPE)
        set(CMAKE_AUTOUIC_SEARCH_PATHS ${QT5_AUTOUIC_SEARCH_PATHS} PARENT_SCOPE)
        set(CMAKE_AUTORCC_SEARCH_PATHS ${QT5_AUTORCC_SEARCH_PATHS} PARENT_SCOPE)
        set(CMAKE_AUTORCC_OPTIONS      ${QT5_AUTORCC_OPTIONS}      PARENT_SCOPE)
        set(AUTOGEN_BUILD_DIR          ${QT5_AUTOGEN_BUILD_DIR}    PARENT_SCOPE)

        set(SHIRABE_PROJECT_QT5_DEFINITIONS              ${SHIRABE_QT5_DEFINITIONS}              PARENT_SCOPE)
        set(SHIRABE_PROJECT_QT5_COMPILE_DEFINITIONS      ${SHIRABE_QT5_COMPILE_DEFINITIONS}      PARENT_SCOPE)
        set(SHIRABE_PROJECT_QT5_EXECUTABLE_COMPILE_FLAGS ${SHIRABE_QT5_EXECUTABLE_COMPILE_FLAGS} PARENT_SCOPE)

        append_parentscope(
            SHIRABE_PROJECT_INCLUDEPATH
                ${SHIRABE_PROJECT_GEN_DIR}/include_${CONFIG})
    endif(LINK_QT5)

    # Important: Some paths might be included double and twice due to Qt5 linkage...
    #            SELECT DISTINCT path FROM ...
    if(SHIRABE_PROJECT_INCLUDEPATH)
        list(REMOVE_DUPLICATES SHIRABE_PROJECT_INCLUDEPATH)
    endif()

    if(SHIRABE_PROJECT_LIBRARY_DIRECTORIES)
        list(REMOVE_DUPLICATES SHIRABE_PROJECT_LIBRARY_DIRECTORIES)
    endif()

    if(SHIRABE_PROJECT_LIBRARY_TARGETS)
        list(REMOVE_DUPLICATES SHIRABE_PROJECT_LIBRARY_TARGETS)
    endif()

    #
    # Append platform specific ext. library setup
    #
    if(WIN32)
        linkWindowsOSEnvironment(
            OS_LIBRARY_LINK_TARGETS
                advapi32
                Ws2_32
                Iphlpapi
                version)
    elseif(UNIX AND NOT APPLE)
        linkLinuxOSEnvironment(
            OS_LIBRARY_DIRECTORIES
                /usr/lib
            OS_LIBRARY_LINK_TARGETS
                #stdc++
                #m
                dl)
    else()
        message(STATUS "Unknown platform" " ")
    endif()

    # LogStatus(MESSAGES "link() -> Preprocessor definitions:" ${SHIRABE_PROJECT_LIBRARY_DEFINITIONS})
    # LogStatus(MESSAGES "link() -> Include paths:"            ${SHIRABE_PROJECT_INCLUDEPATH})
    # LogStatus(MESSAGES "link() -> Library directories:"      ${SHIRABE_PROJECT_LIBRARY_DIRECTORIES})
    # LogStatus(MESSAGES "link() -> Library targets:"          ${SHIRABE_PROJECT_LIBRARY_TARGETS})

    # Forward to parent scope making it usable for the generic build script
    set(SHIRABE_PROJECT_LIBRARY_DEFINITIONS ${SHIRABE_PROJECT_LIBRARY_DEFINITIONS} PARENT_SCOPE)
    set(SHIRABE_PROJECT_INCLUDEPATH         ${SHIRABE_PROJECT_INCLUDEPATH}         PARENT_SCOPE)
    set(SHIRABE_PROJECT_LIBRARY_DIRECTORIES ${SHIRABE_PROJECT_LIBRARY_DIRECTORIES} PARENT_SCOPE)
    set(SHIRABE_PROJECT_LIBRARY_TARGETS     ${SHIRABE_PROJECT_LIBRARY_TARGETS}     PARENT_SCOPE)
    set(SHIRABE_PROJECT_LIBRARY_MODULES     ${SHIRABE_PROJECT_LIBRARY_MODULES}     PARENT_SCOPE)
endfunction(link)
