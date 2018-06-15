#
# Here all settings will be converted to specific compiler options
# for the MSVC compiler.
#
# For an explanation of all input options, see the file:
#    SHIRABE_CMake_HandleCompilerOptions_Default.cmake
#

# Default empty initialization
# Compiler
set(SHIRABE_MSVC_CL_COMMON_FLAGS)
set(SHIRABE_MSVC_CL_CFLAGS)
set(SHIRABE_MSVC_CL_CXXFLAGS)

# Linker
set(SHIRABE_MSVC_LD_COMMON_FLAGS)
set(SHIRABE_MSVC_LD_CFLAGS)
set(SHIRABE_MSVC_LD_CXXFLAGS)

#
# Set default non-configurable options
#
set(
	SHIRABE_MSVC_CL_COMMON_FLAGS
		# /Gm-                           # Minimal recompilation:      NO, to allow MP
		/Oy-                             # Suppress frame pointers:    NO
		/Zc:wchar_t                      # Wchar_t as:                 Internal Type
		/Zc:forScope                     # Force match in for-loop:    True (MSVC extension)		
		/analyze-                        # Do not perform code analysis
		/Fo"${SHIRABE_PROJECT_OBJECTS_DIR}/"  # Where to store .O/.OBJ-files?
		/MP12
		/std:c++latest
		# debug...
		# /Zm1000
		# /Zi
)

#
# Convert SHIRABE_CC_SUBSYSTEM
#
if(SHIRABE_CC__SUBSYSTEM 
		AND NOT "${SHIRABE_CC__SUBSYSTEM}" STREQUAL "NONE"
)	
	if("${SHIRABE_CC__SUBSYSTEM}" STREQUAL "CONSOLE")
		set(SHIRABE_MSVC_LD_COMMON_FLAGS "/SUBSYSTEM:CONSOLE")
		if(SHIRABE_BUILD_APPLICATION) 
			append(SHIRABE_MSVC_LD_COMMON_FLAGS "/ENTRY:mainCRTStartup")
		endif()
	elseif("${SHIRABE_CC__SUBSYSTEM}" STREQUAL "WINDOWS")
		set(SHIRABE_MSVC_LD_COMMON_FLAGS "/SUBSYSTEM:WINDOWS")
		if(SHIRABE_BUILD_APPLICATION) 
			append(SHIRABE_MSVC_LD_COMMON_FLAGS "/ENTRY:WinMainCRTStartup")
		endif()
	endif()
endif()


set(
	SHIRABE_MSVC_LD_COMMON_FLAGS
		${SHIRABE_MSVC_LD_COMMON_FLAGS}
		/LARGEADDRESSAWARE               # x86 with addresses >32bit?
		/SAFESEH:NO                      # Deactivate due to external libraries...
		/DYNAMICBASE                     # Activate address space randomization
		/TLBID:1                         # Resource ID for compiler type lib in case of attributes being used in code.
		/Fe:"${SHIRABE_PROJECT_BUILD_DIR}/${SHIRABE_MODULE_NAME}${SHIRABE_PROJECT_TARGET_SUFFIX}"
		/MANIFEST                        # Generate manifest for parallel binaries
		# /ALLOWISOLATION                  # Force load DLL with manifest file. Append :NO to deactivate this.
		/NXCOMPAT                        # Required for UAC
		/VERBOSE
		/INCREMENTAL                     # Incremental link
		#/DEBUG
		#/OPT:REF 
		#/OPT:ICF
)

if(SHIRABE_BUILD_SHAREDLIB)
	append(
		SHIRABE_MSVC_LD_COMMON_FLAGS
			"/OPT:NOREF"
	)
	foreach(MODULE ${SHIRABE_EMBED_MODULES})
		append(
			SHIRABE_MSVC_LD_COMMON_FLAGS
				"/WHOLEARCHIVE:${MODULE}"
		)
	endforeach()
endif()
 
# 
# Other fixed pipeline additions
# 
if(SHIRABE_DEBUG) 
	append(
		SHIRABE_MSVC_LD_COMMON_FLAGS 
			/DEBUG)
endif(SHIRABE_DEBUG)


	append(
		SHIRABE_MSVC_LD_COMMON_FLAGS 
			/DEBUG
			/PDB:"${SHIRABE_PROJECT_DEPLOY_DIR}/bin/${SHIRABE_MODULE_NAME}.pdb"
			/PGD:"${SHIRABE_PROJECT_DEPLOY_DIR}/bin/${SHIRABE_MODULE_NAME}.pgd"
	) # /ZI implies /FC and /Gy

# sif(SHIRABE_REQUEST_x64_BUILD)
# s	append(SHIRABE_MSVC_LD_COMMON_FLAGS "/MACHINE:X64")
# selse()
# s	append(SHIRABE_MSVC_LD_COMMON_FLAGS "/MACHINE:X86")
# sendif(SHIRABE_REQUEST_x64_BUILD)

if(SHIRABE_PCH_ENABLED)
	append(
		SHIRABE_MSVC_CL_COMMON_FLAGS 
			/Yu"${SHIRABE_PRECOMPILED_HEADER}"
			/Fp"${SHIRABE_PROJECT_INTERMEDIATE_DIR}/${SHIRABE_MODULE_NAME}.pch" 
	)
else()
endif(SHIRABE_PCH_ENABLED)

#
# In case of a shared lib, we need to specify /DLL
#
if(SHIRABE_BUILD_SHAREDLIB)
	if(SHIRABE_DEBUG)
		append(SHIRABE_MSVC_CL_COMMON_FLAGS /LDd)		
	else(SHIRABE_DEBUG)
		append(SHIRABE_MSVC_CL_COMMON_FLAGS /LD)
	endif(SHIRABE_DEBUG)
endif(SHIRABE_BUILD_SHAREDLIB)

#
# Convert SHIRABE_CC__DFLT_CALLING_CONVENTION (MSVC ONLY)
#
if(SHIRABE_CC__DFLT_CALLING_CONVENTION)
	LogStatus(MESSAGES "Handling calling convention...")
	if("${SHIRABE_CC__DFLT_CALLING_CONVENTION}" STREQUAL "CDECL")
		LogStatus(MESSAGES "--> ${SHIRABE_CC__DFLT_CALLING_CONVENTION}")
		append(SHIRABE_MSVC_CL_COMMON_FLAGS /Gd)
	elseif("${SHIRABE_CC__DFLT_CALLING_CONVENTION}" STREQUAL "FASTCALL")
		LogStatus(MESSAGES "--> ${SHIRABE_CC__DFLT_CALLING_CONVENTION}")
		append(SHIRABE_MSVC_CL_COMMON_FLAGS /Gr)
	elseif("${SHIRABE_CC__DFLT_CALLING_CONVENTION}" STREQUAL "STDCALL")
		LogStatus(MESSAGES "--> ${SHIRABE_CC__DFLT_CALLING_CONVENTION}")
		append(SHIRABE_MSVC_CL_COMMON_FLAGS /Gz)
	elseif("${SHIRABE_CC__DFLT_CALLING_CONVENTION}" STREQUAL "THISCALL")
		LogStatus(MESSAGES "--> ${SHIRABE_CC__DFLT_CALLING_CONVENTION}")
		append(SHIRABE_MSVC_CL_COMMON_FLAGS /Gv)
	endif()
else()
	# Default: 
	append(SHIRABE_MSVC_CL_COMMON_FLAGS /Gd)
endif()

#
# Convert SHIRABE_CC__SUPPORT_RTTI
# 
# Enabled in MSVC by default. Check for !RTTI?
if(NOT SHIRABE_CC__SUPPORT_RTTI)
	append(SHIRABE_MSVC_CL_CXXFLAGS /GR-)
endif()

#
# Convert SHIRABE_CC__SUPPORT_EXCEPTIONS
# 
# Enabled in MSVC by default: /EHa. Cannot deactivate it anyway.
if(SHIRABE_CC__SUPPORT_EXCEPTIONS) 
	append(SHIRABE_MSVC_CL_CXXFLAGS /EHsc)
endif()

#
# Convert SHIRABE_CC__TREAT_C_AS_CPP_CODE
# 
if(SHIRABE_CC__TREAT_C_AS_CPP_CODE)
	append(SHIRABE_MSVC_CL_COMMON_FLAGS /TP)
endif()


#
# Convert SHIRABE_CC__MULTIPROCESSOR_COMPILATION
#
if(SHIRABE_CC__MULTIPROCESSOR_COMPILATION)
	append(SHIRABE_MSVC_CL_COMMON_FLAGS /MP8) # Use 8 parallel processes max.
endif()  

#
# Convert SHIRABE_CC__MULTITHREADED_RT
#
if(SHIRABE_CC__USE_MULTITHREADED_RT)
	set(FLAG M)

	# if(SHIRABE_CC__LINK_MT_RT_STATIC)
	# if(SHIRABE_BUILD_STATICLIB)
	#	set(FLAG ${FLAG}T)
	# else()
		set(FLAG ${FLAG}D)
	# endif()
	
	if(FLAG) # If we don't build a library, flag will be unset.
		if(SHIRABE_DEBUG)
			set(FLAG ${FLAG}d)
		endif()
		
		append(SHIRABE_MSVC_CL_COMMON_FLAGS /${FLAG})
	endif()	
endif()  

#
# Convert SHIRABE_CC__ENABLE_RUNTIME_CHECKS
# --> Only in Debug mode
#
if(SHIRABE_CC__ENABLE_RUNTIME_CHECKS AND SHIRABE_DEBUG)
	#
	# Perform buffer-overrun protection in functions.
	# Enable this for specially secure functions and 
	# avoid exploits due to buffer overruns.
	# Performance hit!	
	#
	if(SHIRABE_CC__ENABLE_RUNTIME_BUFFER_OVERRUN_PROTECTION)
		append(SHIRABE_MSVC_CL_COMMON_FLAGS /GS)
	endif()

	#
	# Now for the runtime check flags.
	#
	set(RTC_FLAGS)
	# Check for type narrowing and data loss due to it?
	# Append c
	if(SHIRABE_CC__ENABLE_RUNTIME_CHECK_TYPE_NARROWING)
		set(RTC_FLAGS ${RTC_FLAGS}c)
	endif()
	# Check for stack runtime issues?
	# Append s
	if(SHIRABE_CC__ENABLE_RUNTIME_CHECK_STACK)   
		set(RTC_FLAGS ${RTC_FLAGS}s)     
	endif()  
	# Check for uninitialized variables and their usage?
	# Append u
	if(SHIRABE_CC__ENABLE_RUNTIME_CHECK_INITIALIZATION) 
		set(RTC_FLAGS ${RTC_FLAGS}u)
	endif()

	# Possible combinations: RTCc|RTCs|RTCu|RTCcs|RTCcu|RTCsu|RTCcsu
	append(SHIRABE_MSVC_CL_COMMON_FLAGS /RTC${RTC_FLAGS})
endif()
	
#
# Convert SHIRABE_CC__TREAT_WARNINGS_AS_ERRORS
#
if(SHIRABE_CC__TREAT_WARNINGS_AS_ERRORS)
	append(SHIRABE_MSVC_CL_COMMON_FLAGS /WX)
else()	
	append(SHIRABE_MSVC_CL_COMMON_FLAGS /WX-)
endif()

#
# Convert SHIRABE_CC__WARNING_LEVEL
#
if(SHIRABE_CC__WARNING_LEVEL)
	append(SHIRABE_MSVC_CL_COMMON_FLAGS /W${SHIRABE_CC__WARNING_LEVEL})
else() # 0
	append(SHIRABE_MSVC_CL_COMMON_FLAGS /W0)
endif()
	
#
# Convert SHIRABE_CC__OPTIMIZATION_LEVEL
#
if(SHIRABE_CC__OPTIMIZATION_LEVEL)
	if(SHIRABE_CC__OPTIMIZATION_LEVEL EQUAL O_Size)		
		append(SHIRABE_MSVC_CL_COMMON_FLAGS /O1)
	elseif(SHIRABE_CC__OPTIMIZATION_LEVEL EQUAL O_Speed)		
		append(SHIRABE_MSVC_CL_COMMON_FLAGS /O2)
	else() # O_Debug or any other.
		append(SHIRABE_MSVC_CL_COMMON_FLAGS /Od)
	endif()
else()
	append(SHIRABE_MSVC_CL_COMMON_FLAGS /Od)
endif() 
	
#
# Convert SHIRABE_CC__ALLOW_SYSTEM_INTERNAL_FN
#
if(SHIRABE_CC__ALLOW_SYSTEM_INTERNAL_FN)
	append(SHIRABE_MSVC_CL_COMMON_FLAGS /Oi)	
endif()  
	
#
# Convert SHIRABE_CC__OPTIMIZE_ENTIRE_APPLICATION (Release only)
#
if(SHIRABE_CC__OPTIMIZE_ENTIRE_APPLICATION AND NOT SHIRABE_DEBUG)
	append(SHIRABE_MSVC_CL_COMMON_FLAGS /GL)
    append(SHIRABE_MSVC_CL_COMMON_FLAGS /LTCG) # Enhance linker performance using link-time code generation.
endif()   
	
#
# Convert SHIRABE_CC__ENABLE_FUNCTION_LEVEL_LINKING
#
if(SHIRABE_CC__ENABLE_FUNCTION_LEVEL_LINKING)
	append(SHIRABE_MSVC_CL_COMMON_FLAGS /Gy)
	
    #
    # Convert SHIRABE_CC__ENABLE_FUNCTION_LEVEL_LINKING
    #
	if(SHIRABE_CC__ENABLE_BINARY_OUTPUT_OPTIMIZATION)	
		# append(SHIRABE_MSVC_LD_COMMON_FLAGS /OPT:REF,ICF)
	endif(SHIRABE_CC__ENABLE_BINARY_OUTPUT_OPTIMIZATION)
else()
	append(SHIRABE_MSVC_CL_COMMON_FLAGS /Gy-)
endif()

if(SHIRABE_CC__CREATE_BIGOBJ)
	append(SHIRABE_MSVC_CL_COMMON_FLAGS /bigobj)
endif(SHIRABE_CC__CREATE_BIGOBJ)
