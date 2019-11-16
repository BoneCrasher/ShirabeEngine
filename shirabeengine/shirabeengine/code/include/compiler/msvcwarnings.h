#ifndef __SHIRABE_MSVCWARNINGS_H__
#define __SHIRABE_MSVCWARNINGS_H__

//
// C4522 (Level 3) --> Multiple assignment operators defines
//
// Only informational warning. Will spam the compilation output during 
// interface compilation for no reason. DISABLE.
//
// See https://msdn.microsoft.com/de-de/library/fd45k287.aspx
//
#pragma warning(disable:4522) 

#endif