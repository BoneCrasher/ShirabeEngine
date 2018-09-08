#
# Here all settings will be converted to specific compiler options
# for the gcc compiler.
#
# For an explanation of all input options, see the file:
#    SHIRABE_CMake_HandleCompilerOptions_Default.cmake
#

set(SHIRABE_GCC_CL_COMMON_FLAGS)
set(SHIRABE_GCC_CL_CFLAGS)
set(SHIRABE_GCC_CL_CXXFLAGS)

set(SHIRABE_GCC_LD_COMMON_FLAGS)
set(SHIRABE_GCC_LD_CFLAGS)
set(SHIRABE_GCC_LD_CXXFLAGS)
#
# Set default non-configurable options
#
set(
        SHIRABE_GCC_CL_COMMON_FLAGS
            "-v"            # Print to the max
            "-std=c++17"    # C++11 required
            "-fpermissive"  # Actually bad... Try to remove!
            "-fexceptions"  # Enable std exceptions
            "-pthread"      # Enforce to use of pthread, as all ext. libs do
            "-fPIC"         # Enable position independent code
            "-Wl,-fPIC"
)

# set(SHIRABE_GCC_CL_CXXFLAGS "--whole-archive")

set(
    SHIRABE_GCC_LD_COMMON_FLAGS
            )

# Make sure the compiler uses the correct address model
if(SHIRABE_PROJECT_CROSS_BUILD_ARMHF)
    append(SHIRABE_GCC_CL_COMMON_FLAGS "-marm")
elseif(SHIRABE_ADDRESSMODEL_64BIT)
    append(SHIRABE_GCC_CL_COMMON_FLAGS "-m64")
else()
    append(SHIRABE_GCC_CL_COMMON_FLAGS "-m32")
endif()

# Make sure that debug symbols are built
if(SHIRABE_DEBUG)
    append(SHIRABE_GCC_CL_COMMON_FLAGS "-ggdb3")
endif()

if(SHIRABE_PCH_ENABLED)
    # LogStatus(MESSAGES "Using PCH: ${SHIRABE_PROJECT_SRC_DIR}/${SHIRABE_PROJECT_PCH_FILENAME}.gch")
    # append(
    #     SHIRABE_GCC_CL_COMMON_FLAGS
    #         "-include ${SHIRABE_PROJECT_SRC_DIR}/${SHIRABE_PROJECT_PCH_FILENAME}.gch")
endif(SHIRABE_PCH_ENABLED)

#
# Convert SHIRABE_CC__TREAT_C_AS_CPP_CODE
# 
if(SHIRABE_CC__TREAT_C_AS_CPP_CODE)
    # Usually the below code was the right way to do it,
	# but I've realized that on GCC, constraining the compiler 
	# selection will lead to annoying, hardly solvable errors.
    # append(SHIRABE_GCC_CL_COMMON_FLAGS "-x c++")
endif()

#
# Convert SHIRABE_CC__SUPPORT_RTTI
# 
if(SHIRABE_CC__SUPPORT_RTTI)
endif()

#
# Convert SHIRABE_CC__SUPPORT_EXCEPTIONS
# 
if(SHIRABE_CC__SUPPORT_EXCEPTIONS)
endif()

#
# Convert SHIRABE_CC__ENABLE_COMPILETIME_CHECKS
#
if(SHIRABE_CC__ENABLE_COMPILETIME_CHECKS)
endif()

#
# Convert SHIRABE_CC__ENABLE_RUNTIME_CHECKS
#
if(SHIRABE_CC__ENABLE_RUNTIME_CHECKS)
endif()
	
#
# Convert SHIRABE_CC__TREAT_WARNINGS_AS_ERRORS
#
if(SHIRABE_CC__TREAT_WARNINGS_AS_ERRORS)
    append(SHIRABE_GCC_CL_COMMON_FLAGS "-Werror")
endif()
	
#
# Convert SHIRABE_CC__MULTITHREADED_DLL
#
if(SHIRABE_CC__MULTITHREADED_DLL)
endif()  
	
#
# Convert SHIRABE_CC__WARNING_LEVEL
#
if(SHIRABE_CC__WARNING_LEVEL)
        if(${SHIRABE_CC__WARNING_LEVEL} GREATER 2) # Activate even more complaints
        append(SHIRABE_GCC_CL_COMMON_FLAGS "-Wextra")
        elseif(${SHIRABE_CC__WARNING_LEVEL} GREATER 1) # Activate "all" warnings
         append(SHIRABE_GCC_CL_COMMON_FLAGS "-Wall")
	else()
	     # Default, nothing to add.
	endif()

	 # Additionally, enforce ISO-violation warnings and even more?
        if(${SHIRABE_CC__WARNING_LEVEL} GREATER 3)
        append(SHIRABE_GCC_CL_COMMON_FLAGS "-Wpedantic")
	endif()
else() # 0# Default. Don't change anything
        append(SHIRABE_GCC_CL_COMMON_FLAGS "-w")
endif()
	
#
# Convert SHIRABE_CC__OPTIMIZATION_LEVEL
#
if(SHIRABE_CC__OPTIMIZATION_LEVEL)
        if(SHIRABE_CC__OPTIMIZATION_LEVEL EQUAL O_Size)
                append(SHIRABE_MSVC_CL_COMMON_FLAGS "-Os")
        elseif(SHIRABE_CC__OPTIMIZATION_LEVEL EQUAL O_Speed)
                append(SHIRABE_MSVC_CL_COMMON_FLAGS "-Ofast")
        elseif(SHIRABE_CC__OPTIMIZATION_LEVEL EQUAL O_Debug)
            append(SHIRABE_MSVC_CL_COMMON_FLAGS "-Og")
	else() # Default
                append(SHIRABE_MSVC_CL_COMMON_FLAGS "-O1")
	endif()
endif() 
	
#
# Convert SHIRABE_CC__ALLOW_SYSTEM_INTERNAL_FN
#
if(SHIRABE_CC__ALLOW_SYSTEM_INTERNAL_FN)
endif()  
	
#
# Convert SHIRABE_CC__OPTIMIZE_ENTIRE_APPLICATION
#
if(SHIRABE_CC__OPTIMIZE_ENTIRE_APPLICATION)
endif()   
	
#
# Convert SHIRABE_CC__ENABLE_FUNCTION_LEVEL_LINKING
#
if(SHIRABE_CC__ENABLE_FUNCTION_LEVEL_LINKING)
endif() 
