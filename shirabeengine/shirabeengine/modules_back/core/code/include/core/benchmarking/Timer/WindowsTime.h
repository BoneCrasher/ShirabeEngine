#ifndef __SHIRABE__TIME_WIN32_H__
#define __SHIRABE__TIME_WIN32_H__

#pragma warning(push)
#pragma warning(once:4514)
#pragma warning(once:4820)
#pragma warning(once:4668)
#pragma warning(once:4005)
#pragma warning(once:4350)

#include "Platform/Platform.h"

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include "core/enginestatus.h"
#include "core/enginetypehelper.h"
#include "Core/Benchmarking/Timer/PlatformTime.h"

namespace Engine
{
	/**************************************************************************************************
	 * Class:	WindowsTime
	 *
	 * Summary:
	 *  The WindowsTime-class implements PlatformTime{LONGLONG} using
	 *  		the internal PerformanceCounter and PerformanceFrequency functionality.
	 *
	 * See:
	 * PlatformTime{LONGLONG} - 
	 * https://msdn.microsoft.com/de-de/library/windows/desktop/ms644904(v=vs.85).aspx
	 * https://msdn.microsoft.com/de-de/library/windows/desktop/ms644905(v=vs.85).aspx
	 **************************************************************************************************/
	class WindowsTime
		: public PlatformTime<LONGLONG>
	{
	public:
		typedef LONGLONG internal_time_value_type;

		/**************************************************************************************************
		 * Fn:	WindowsTime::WindowsTime();
		 *
		 * Summary:	Default constructor.
		 **************************************************************************************************/
		WindowsTime();

		/**************************************************************************************************
		 * Fn:	WindowsTime::~WindowsTime();
		 *
		 * Summary:	Destructor.
		 **************************************************************************************************/
		~WindowsTime();

		EEngineStatus getTimestamp(internal_time_value_type &buffer)           const;
		EEngineStatus getConversionConstant(internal_time_value_type &bBuffer) const;

		void requestSetConversionFactors(ConversionFactorMap& map);
	protected:		

		/**************************************************************************************************
		 * Fn:	EEngineStatus WindowsTime::queryPerformanceFrequency(internal_time_value_type &buffer) const;
		 *
		 * Summary:
		 *  Queries the performance frequency used to convert from the platform internal
		 *   	   time/counter representation format to a Seconds-value.
		 *  
		 *  This value maps directly to the conversion constant.
		 *
		 * Parameters:
		 * buffer - 	[in,out] LONGLONG-buffer to hold the current counter value.
		 *
		 * Returns:	EEngineStatus.Ok, if successful. An error code of EEngineStatus otherwise.
		 **************************************************************************************************/
		EEngineStatus queryPerformanceFrequency(internal_time_value_type &buffer) const;

		/**************************************************************************************************
		 * Fn:	EEngineStatus WindowsTime::queryPerformanceCounter(internal_time_value_type &buffer) const;
		 *
		 * Summary:
		 *  Queries the performance counter to fetch the current counter value. For time calculations
		 *  this method has to be invoked at least once each frame!
		 *
		 * Parameters:
		 * buffer - 	[in,out] LONGLONG-buffer to hold the current counter value.
		 *
		 * Returns:	EEngineStatus.Ok, if successful. An error code of EEngineStatus otherwise.
		 **************************************************************************************************/
		EEngineStatus queryPerformanceCounter(internal_time_value_type &buffer)   const;
	};
	DeclareSharedPointerType(WindowsTime)
}

#pragma warning(pop)

#endif