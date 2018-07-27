#ifndef __SHIRABE__TIMER_H__
#define __SHIRABE__TIMER_H__

#include "core/enginetypehelper.h"
#include "core/benchmarking/measurementdatastore.h"
#include "core/benchmarking/timer/timespanunit.h"

#include <platform/platform.h>

#ifdef PLATFORM_WINDOWS
#pragma warning(push)
#pragma warning(disable:4820)
#include "core/benchmarking/timer/windowstime.h"
#else
    #include "time.h"
#endif

using namespace Engine::Benchmarking;

/**************************************************************************************************
 * Typedef:	MeasurementDataStore<MeasurementChunk<double, double>> FPSDataStore
 *
 * Summary:	Defines an alias representing the FPS data store.
 **************************************************************************************************/
typedef CMeasurementDataStore<CMeasurementChunk<double, double>> FPSDataStore_t;

namespace Engine {

#ifdef PLATFORM_WINDOWS
	typedef class Engine::WindowsTime Time, Time, *LPTime;
#elif
	typedef class Engine::Time_Base Time, Time, *LPTime;
#endif

	/**************************************************************************************************
	 * Class:	Timer
	 *
	 * Summary:
	 *  The timer class will use a specific PlatformTime specialization to hook into platform time
	 *  counters and evaluate total elapsed, elapsed since last frame and frames per second data.
	 **************************************************************************************************/
	class Timer {
	public:

		Timer();
		virtual ~Timer();

		EEngineStatus initialize();
		EEngineStatus update();
		EEngineStatus cleanup();

		double elapsed(ETimeUnit unit = ETimeUnit::Seconds);
		double total_elapsed(ETimeUnit unit = ETimeUnit::Seconds);

		float FPS();
		
	private:

#ifdef PLATFORM_WINDOWS
		typedef
			WindowsTime
			internal_time_type;
#elif defined(PLATFORM_LINUX)
		typedef
			LinuxTime
			internal_time_type;
#endif
		typedef
			typename internal_time_type::internal_time_value_type
			internal_time_value_type;
		typedef
			CStdSharedPtr_t<internal_time_type>
			TimePtr;

		void setTimeInterface(const TimePtr&);

		TimePtr                  m_timeInterface;
		internal_time_value_type m_conversionConstant;

		internal_time_value_type m_initial;
		internal_time_value_type m_current;
		internal_time_value_type m_elapsed;

		FPSDataStore m_dataStore;
		double       m_chunkPushCounter;
		int          m_frames;
	};

}

#ifdef PLATFORM_WINDOWS
#pragma warning(pop)
#endif

#endif
