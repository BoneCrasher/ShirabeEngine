#-------------------------------------------------
#-------------------------------------------------
# Compiler and linker options
#-------------------------------------------------
#-------------------------------------------------

#-------------------------------------------------
# Options and Defaults
#-------------------------------------------------
# All Options below exist in two variants:
#  1. SHIRABE_CC__<Name>:
#      The internal name used for compiler option conversion.
#     These names will be predefined with appropriate defaults.
#  2. SHIRABE_COMPILER__<Name>:
#      The external names to be defined OPTIONALLY, which will
#     override the corresponding value of SHIRABE_CC__<Name>
#      None of the SHIRABE_COMPILER__~ values is predefined and
#     a value different from the default must be specified in 
#     the respective project file.
#-------------------------------------------------

#-------------------------------------------------
# SHIRABE_CC__DFLT_CALLING_CONVENTION
#-------------------------------------------------
# Default subsystem definition for the MSVC compiler.
# Constraints: (MSVC only)
# Remarks:     For linux builds, it should be POSIX!
# Allowed Values: NONE|CONSOLE|WINDOWS|NATIVE|POSIX
#-------------------------------------------------
set(SHIRABE_CC__SUBSYSTEM)
if(WIN32 AND MSVC) 
        set(SHIRABE_CC__SUBSYSTEM WINDOWS)
elseif(NOT WIN32)
        set(SHIRABE_CC__SUBSYSTEM NONE)
endif()
#-------------------------------------------------

#-------------------------------------------------
# SHIRABE_CC__DFLT_CALLING_CONVENTION
#-------------------------------------------------
# Default GLOBAL calling convention used for all non-cpp-member-functions 
# not explicitly overriding the calling convention with a function attribute.
# Constraints: (MSVC ONLY); Linux does not support global default conventions.
# Allowed Values: CDECL|FASTCALL|STDCALL|VECTORCALL
#-------------------------------------------------
set(SHIRABE_CC__DFLT_CALLING_CONVENTION CDECL)
#-------------------------------------------------

#-------------------------------------------------
# SHIRABE_CC__SUPPORT_RTTI
#-------------------------------------------------
# Should RunTime Type Information be generated for runtime reflection?
# Constraints: 
# Allowed Values: YES|NO
#-------------------------------------------------
set(SHIRABE_CC__SUPPORT_RTTI YES)
#-------------------------------------------------

#-------------------------------------------------
# SHIRABE_CC__SUPPORT_EXCEPTIONS
#-------------------------------------------------
# Tells the compiler to activate exception support!
# Constraints: 
# Allowed Values: YES NO
#-------------------------------------------------
set(SHIRABE_CC__SUPPORT_EXCEPTIONS YES)

#-------------------------------------------------
# SHIRABE_CC__TREAT_C_AS_CPP_CODE
#-------------------------------------------------
# Tell the compiler to treat all files as CPP files, regardless
# of the effective contents.
# This is used to override the C compiler for C-files.
# Remarks:
#  Usually, it is possible to specify the behaviour on a per-file 
#   scope and also the other way round, i.e. CPP as C.
#  For the SR projects, forcing C-files being handled as CPP-files 
#   is currently enough.
# Constraints: -
# Allowed Values: YES|NO
#-------------------------------------------------
set(SHIRABE_CC__TREAT_C_AS_CPP_CODE YES)
#-------------------------------------------------


#-------------------------------------------------
# SHIRABE_CC__MULTITHREADED
#-------------------------------------------------
# Do we use a multiple processors to compile the code?
# Constraints: (MSVC Only)
# Allowed Values: YES|NO
#-------------------------------------------------
set(SHIRABE_CC__MULTIPROCESSOR_COMPILATION YES)
#-------------------------------------------------


#-------------------------------------------------
# SHIRABE_CC__MULTITHREADED
#-------------------------------------------------
# Do we use a multithreaded dynamic/static library
# of the MSVC runtime?
# Constraints: (MSVC Only)
# Allowed Values: YES|NO
#-------------------------------------------------
set(SHIRABE_CC__USE_MULTITHREADED_RT YES)
#-------------------------------------------------

#-------------------------------------------------
# SHIRABE_CC__LINK_MT_RT_STATIC
#-------------------------------------------------
# Should the MSVC runtime be linked statically?
# Constraints: (MSVC Only)
# Allowed Values: YES|NO
#-------------------------------------------------
set(SHIRABE_CC__LINK_MT_RT_STATIC NO)
#-------------------------------------------------

#-------------------------------------------------
# SHIRABE_CC__ENABLE_RUNTIME_CHECKS
#-------------------------------------------------
# Sets whether any kind of runtime check should be 
# generated to protect from common mistakes and 
# dangerous exploits.
# Constraints: -
# Allowed Values: YES|NO
#-------------------------------------------------
set(SHIRABE_CC__ENABLE_RUNTIME_CHECKS YES)
#-------------------------------------------------

#-------------------------------------------------
# SHIRABE_CC__ENABLE_RUNTIME_BUFFER_OVERRUN_PROTECTION
#-------------------------------------------------
# Generates a "cookie" to protect from buffer overrun exploits.
# Basically this cookie is "buffer memory" allocated, which 
# allows a virtual stack growth and protects from out of bound
# stack writes.
# Constraints: -
# Allowed Values: YES|NO
#-------------------------------------------------
set(SHIRABE_CC__ENABLE_RUNTIME_BUFFER_OVERRUN_PROTECTION YES)
#-------------------------------------------------

#-------------------------------------------------
# SHIRABE_CC__ENABLE_RUNTIME_CHECK_TYPE_NARROWING
#-------------------------------------------------
# Throws a warning, if the programmer narrows a type  
# to a smaller type causing possible data loss.
# Constraints: -
# Allowed Values: YES|NO
#-------------------------------------------------
set(SHIRABE_CC__ENABLE_RUNTIME_CHECK_TYPE_NARROWING NO)
#-------------------------------------------------

#-------------------------------------------------
# SHIRABE_CC__ENABLE_RUNTIME_CHECK_STACK
#-------------------------------------------------
# Insert runtime checks to protect the stack.
# Constraints: -
# Allowed Values: YES|NO
#-------------------------------------------------
set(SHIRABE_CC__ENABLE_RUNTIME_CHECK_STACK YES)
#-------------------------------------------------

#-------------------------------------------------
# SHIRABE_CC__ENABLE_RUNTIME_CHECK_INITIALIZATION
#-------------------------------------------------
# Insert checks for uninitialized memory/variables.
# Constraints: -
# Allowed Values: YES|NO
#-------------------------------------------------
set(SHIRABE_CC__ENABLE_RUNTIME_CHECK_INITIALIZATION YES)
#-------------------------------------------------


#-------------------------------------------------
# SHIRABE_CC__TREAT_WARNINGS_AS_ERRORS
#-------------------------------------------------
# On warning, immediately throw errors.
# Constraints: -
# Allowed Values: YES|NO
#-------------------------------------------------
set(SHIRABE_CC__TREAT_WARNINGS_AS_ERRORS NO)
#-------------------------------------------------

#-------------------------------------------------
# SHIRABE_CC__WARNING_LEVEL
#-------------------------------------------------
# Set the desired warning level, the higher, the more warnings
# will be generated and the more strict warnings will become.
# Constraints: -
# Allowed Values: 0|1|2|3|4|
# --> Theoretically also All, but its incompatible with 
#     the standard headers.
#-------------------------------------------------
set(SHIRABE_CC__WARNING_LEVEL 4)
#-------------------------------------------------

#-------------------------------------------------
# SHIRABE_CC__OPTIMIZATION_LEVEL
#-------------------------------------------------
# ...
# Constraints: -
# Allowed Values: O_Size|O_Speed|O_Debug|-
#-------------------------------------------------
set(SHIRABE_CC__OPTIMIZATION_LEVEL O_Debug)
#-------------------------------------------------

#-------------------------------------------------
# SHIRABE_CC__ALLOW_SYSTEM_INTERNAL_FN
#-------------------------------------------------
# Allow the replacement of functions with more performant
# possibly not standard conformant or behaviourally expected
# functions by the compiler. Increases speed.
# Constraints: (MSVC Only)
# Allowed Values: YES|NO
#-------------------------------------------------
set(SHIRABE_CC__ALLOW_SYSTEM_INTERNAL_FN NO)
#-------------------------------------------------

#-------------------------------------------------
# SHIRABE_CC__OPTIMIZE_ENTIRE_APPLICATION
#-------------------------------------------------
# Cross-Module Optimization activation.
# Usually, optimization happens "per module".
# Constraints: (MSVC Only)
# Remarks: Implies link-time code generation /LTCG
# Allowed Values: YES|NO
#-------------------------------------------------
set(SHIRABE_CC__OPTIMIZE_ENTIRE_APPLICATION NO)
#-------------------------------------------------

#-------------------------------------------------
# SHIRABE_CC__ENABLE_FUNCTION_LEVEL_LINKING
#-------------------------------------------------
# Allow Non-inline and non-clas functions be packaged as COMDATs.
# Will allow exclusion or reordering of binary function storage 
# using /OPT and /ORDER (See below)
# Constraints: (MSVC Only)
# Allowed Values: YES|NO
#-------------------------------------------------
set(SHIRABE_CC__ENABLE_FUNCTION_LEVEL_LINKING YES)
#-------------------------------------------------

#-------------------------------------------------
# SHIRABE_CC__ENABLE_FUNCTION_LEVEL_LINKING
#-------------------------------------------------
# Allow removal of unused functions and redundant COMDAT entities
# in the final binary output.
#
# /OPT:REF    Removes unused functions. 
#    ~:NOREF  Keeps them, e.g. for debugging.
# /OPT:ICF=#  Iterate # to detect and remove redundant COMDAT-entities.
#    ~:NOICF  Opposite of ICF, no checks.
#      (!)->  ICF is implied by ~:REF, BUT will not fold read-only entities.
#             Explicit ~:REF,ICF options will enable full folding.
# /OPT:LBR    Reduces ARM-architecture code-islands for long branch-routes. 
#    ~:NOLBR  Leaves things as is.
# /ORDER:@filename -> Reorders COMDATs in the final binaries to optimize the 
#                     binary layout and the application itself.
# Constraints: (MSVC Only), Requires SHIRABE_CC__ENABLE_FUNCTION_LEVEL_LINKING=YES
# Allowed Values: YES|NO
#-------------------------------------------------
set(SHIRABE_CC__ENABLE_BINARY_OUTPUT_OPTIMIZATION YES)
#-------------------------------------------------

#-------------------------------------------------
# SHIRABE_CC__CREATE_BIGOBJ
#-------------------------------------------------
# Increase the addressable space in the OBJ-file
#
# Constraints: (MSVC Only)
# Allowed Values: YES|NO
#-------------------------------------------------
set(SHIRABE_CC__CREATE_BIGOBJ NO)
#-------------------------------------------------
