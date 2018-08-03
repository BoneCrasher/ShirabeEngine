#ifndef __SHIRABE_ENGINE_MEMORY_TYPES_H__ 
#define __SHIRABE_ENGINE_MEMORY_TYPES_H__ 

#include <tchar.h>

namespace engine {
	namespace Memory {

		/* ShirabeEngine Datatype Definition
		 *
		 * This file defines custom typedefs for ANSI-C99 Std datatypes as well as
		 * platform/compiler specific datatypes.
		 * The consequent use of the subsequent types is recommended throughout the engine.
		 *
		 * In general the amount of bits available for the integral types depends on the
		 * architecture compiled on and the OS compiled for.
		 *
		 * Usually the following sizes can be assumed:
		 *  -> bool:     1 Bit
		 *  -> char:     1 Byte
		 *  -> short:    2 Bytes
		 *  -> int:      4 Bytes
		 *  -> long int: 8 Bytes
		 *  -> float:    4 Bytes ( IEEE 754 )
		 *  -> double:   8 Bytes ( IEEE 754 )
		 *
		 * ANSI-C99 Std: http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1124.pdf
		 *
		 * For windows specific datatypedefs see:
		 *   -> http://msdn.microsoft.com/en-us/library/windows/desktop/aa383751%28v=vs.85%29.aspx
		 *   -> http://msdn.microsoft.com/en-us/library/s3f49ktz.aspx
		 *   -> http://msdn.microsoft.com/en-us/library/29dh1w7z.aspx
		 */

		 // IEEE 754 Std Floating-Point Numbers
		 // -> Caluclation: x = <sign> * <mantissa> * (<base>^<bias - characteristic>)
		 //
		 // Single-Precision: <s0, c7,  ..., c0, m23, ..., m0>; 
		 //  Bit-Mapping: sign = 1, characteristics = 8, mantissa = 24, bias = 127
		 //  Exponentvalues: -126 <= e <= 127
		 //  Range (Normalized): 2^(-126) .. (1−2^(-24)) * 2^128
		typedef float     tSingle;    // 4 Bytes, signed, 
		// Double-Precision: <s0, c11, ..., c0, m52, ..., m0>;
		//  Bit-Mapping: sign = 1, characteristics = 11, mantissa = 53, bias = 1023
		//  Exponentvalues: -1022 <= e <= 1023
		//  Range (Normalized): 2^(-1022) .. (1−2^(-53)) * 2^1024
		typedef double    tDouble;    // 8 Bytes, signed, 

#if defined(_MSC_VER) // Defined by the visual c++ compiler!

		typedef bool               tBool;      // 1 Bit,              0 .. 1
		typedef unsigned char      tUChar;     // 1 Byte,  unsigned,  0 .. 255
		typedef          char      tChar;      // 1 Byte,  signed,   -128 .. 0 .. 127
		typedef          wchar_t   tWChar;     // at least 2 bytes up to 4 Bytes, unsigned, wide char for unicode rep.
		//typedef          char16_t  tChar16;    // 2 Bytes, unsigned, equivalent to tUShort/tUInt16
		//typedef          char32_t  tChar32;    // 4 Bytes, unsigned, equivalent to tUInt/tUInt32
		typedef unsigned short     tUShort;    // 2 Bytes, unsigned,  0 .. 2^16
		typedef signed   short     tShort;     // 2 Bytes, signed,   -2^15 .. 0 .. (2^15 - 1)
		typedef unsigned int       tUInt;      // 4 Bytes, unsigned,  0 .. 2^32
		typedef signed   int       tInt;       // 4 Bytes, signed,   -2^31 .. 0 .. (2^31 - 1)
		typedef unsigned long      tULong;     // 4 Bytes, signed,    0 .. 2^32
		typedef signed   long      tLong;      // 4 Bytes, unsigned  -2^31 .. 0 .. (2^31 - 1)
		typedef unsigned long long tULongLong; // 8 Bytes, unsigned,  0 .. 2^64
		typedef signed   long long tLongLong;  // 8 Bytes, signed,   -2^63 .. 0 .. (2^63 - 1)  

		// typedef unsigned t8  tUInt8;  // ANSI: unsigned char
		// typedef signed   t8  tInt8;   // ANSI: signed   char
		// typedef unsigned t16 tUInt16; // ANSI: unsigned short
		// typedef signed   t16 tInt16;  // ANSI: signed   short
		// typedef unsigned t32 tUInt32; // ANSI: unsigned int
		// typedef signed   t32 tInt32;  // ANSI: signed int
		// typedef unsigned t64 tUInt64; // ANSI: long long
		// typedef signed   t64 tInt64;  // ANSI: long long

#elif defined (__GNUC__) // GCC compiler specific

		typedef bool               tBool;      // 1 Bit,              0 .. 1
		typedef unsigned char      tUChar;     // 1 Byte,  unsigned,  0 .. 255
		typedef signed   char      tChar;      // 1 Byte,  signed,   -128 .. 0 .. 127
		typedef          wchar_t   tWChar;     // at least 2 bytes up to 4 Bytes, unsigned, wide char for unicode rep.
		typedef unsigned short     tChar16;    // 2 Bytes, unsigned, equivalent to tUShort/tUInt16
		typedef unsigned int       tChar32;    // 4 Bytes, unsigned, equivalent to tUInt/tUInt32
		typedef unsigned short     tUShort;    // 2 Bytes, unsigned,  0 .. 2^16
		typedef signed   short     tShort;     // 2 Bytes, signed,   -2^15 .. 0 .. (2^15 - 1)
		typedef unsigned int       tUInt;      // 4 Bytes, unsigned,  0 .. 2^32
		typedef signed   int       tInt;       // 4 Bytes, signed,   -2^31 .. 0 .. (2^31 - 1)
		typedef unsigned long      tULong;     // 4 Bytes, signed,    0 .. 2^32
		typedef signed   long      tLong;      // 4 Bytes, unsigned  -2^31 .. 0 .. (2^31 - 1)
		typedef unsigned long long tULongLong; // 8 Bytes, unsigned,  0 .. 2^64
		typedef signed   long long tLongLong;  // 8 Bytes, signed,   -2^63 .. 0 .. (2^63 - 1)

		// ANSI C99 compliant
		typedef unsigned char      tUInt8;
		typedef signed   char      tInt8;
		typedef unsigned short     tUInt16;
		typedef signed   short     tInt16;
		typedef unsigned int       tUInt32;
		typedef signed   int       tInt32;
		typedef unsigned long int  tUInt64;
		typedef signed   long int  tInt64;

#endif

	}
}

#endif