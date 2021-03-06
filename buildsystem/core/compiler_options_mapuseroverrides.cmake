#-------------------------------------------------
# Handle compiler and linker option overrides from 
# the project specific script.
#-------------------------------------------------

macro(handleOverride target input)
	if(${input})
		set(
			${target}
				${${input}}
		)
	endif()
endmacro()

handleOverride(SHIRABE_CC__SUBSYSTEM                                SHIRABE_COMPILER__SUBSYSTEM)
handleOverride(SHIRABE_CC__DFLT_CALLING_CONVENTION                  SHIRABE_COMPILER__DFLT_CALLING_CONVENTION)
handleOverride(SHIRABE_CC__SUPPORT_RTTI                             SHIRABE_COMPILER__SUPPORT_RTTI)
handleOverride(SHIRABE_CC__SUPPORT_EXCEPTIONS                       SHIRABE_COMPILER__SUPPORT_EXCEPTIONS)
handleOverride(SHIRABE_CC__TREAT_C_AS_CPP_CODE                      SHIRABE_COMPILER__TREAT_C_AS_CPP_CODE)
handleOverride(SHIRABE_CC__MULTIPROCESSOR_COMPILATION               SHIRABE_COMPILER__MULTIPROCESSOR_COMPILATION)
handleOverride(SHIRABE_CC__USE_MULTITHREADED_RT                     SHIRABE_COMPILER__USE_MULTITHREADED_RT)
handleOverride(SHIRABE_CC__LINK_MT_RT_STATIC                        SHIRABE_COMPILER__LINK_MT_RT_STATIC)
handleOverride(SHIRABE_CC__ENABLE_RUNTIME_CHECKS                    SHIRABE_COMPILER__ENABLE_RUNTIME_CHECKS)
handleOverride(SHIRABE_CC__ENABLE_RUNTIME_BUFFER_OVERRUN_PROTECTION SHIRABE_COMPILER__ENABLE_RUNTIME_BUFFER_OVERRUN_PROTECTION)
handleOverride(SHIRABE_CC__ENABLE_RUNTIME_CHECK_TYPE_NARROWING      SHIRABE_COMPILER__ENABLE_RUNTIME_CHECK_TYPE_NARROWING)
handleOverride(SHIRABE_CC__ENABLE_RUNTIME_CHECK_STACK               SHIRABE_COMPILER__ENABLE_RUNTIME_CHECK_STACK)
handleOverride(SHIRABE_CC__ENABLE_RUNTIME_CHECK_INITIALIZATION      SHIRABE_COMPILER__ENABLE_RUNTIME_CHECK_INITIALIZATION)
handleOverride(SHIRABE_CC__TREAT_WARNINGS_AS_ERRORS                 SHIRABE_COMPILER__TREAT_WARNINGS_AS_ERRORS)
handleOverride(SHIRABE_CC__WARNING_LEVEL                            SHIRABE_COMPILER__WARNING_LEVEL)
handleOverride(SHIRABE_CC__OPTIMIZATION_LEVEL                       SHIRABE_COMPILER__OPTIMIZATION_LEVEL)
handleOverride(SHIRABE_CC__ALLOW_SYSTEM_INTERNAL_FN                 SHIRABE_COMPILER__ALLOW_SYSTEM_INTERNAL_FN)
handleOverride(SHIRABE_CC__OPTIMIZE_ENTIRE_APPLICATION              SHIRABE_COMPILER__OPTIMIZE_ENTIRE_APPLICATION)
handleOverride(SHIRABE_CC__ENABLE_FUNCTION_LEVEL_LINKING            SHIRABE_COMPILER__ENABLE_FUNCTION_LEVEL_LINKING)
handleOverride(SHIRABE_CC__ENABLE_BINARY_OUTPUT_OPTIMIZATION        SHIRABE_COMPILER__ENABLE_BINARY_OUTPUT_OPTIMIZATION)
handleOverride(SHIRABE_CC__CREATE_BIGOBJ                            SHIRABE_COMPILER__CREATE_BIGOBJ)

#-------------------------------------------------
