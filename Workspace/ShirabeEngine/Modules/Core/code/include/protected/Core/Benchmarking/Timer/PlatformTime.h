#ifndef __SHIRABE_TIMEBASE_H__
#define __SHIRABE_TIMEBASE_H__

#include <map>
#include <iostream>   // for printf

#include "Platform/Platform.h"
#include "Core/EngineStatus.h"

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace Engine
{	
	/// <summary>
	/// Specifies valid time-units to be used for converting the internal time values.
	/// </summary>
	enum class ETimeUnit
	{
		NanoSeconds = 0,
		MicroSeconds,
		MilliSeconds,
		Seconds,
		Minutes,
		Hours,
		Day
	};

	/**************************************************************************************************
	 * Class:	PlatformTime
	 *
	 * Summary:
	 *  The PlatformTime-class is used to provide a common base interface and
	 *       functionality for platform time access, storage and conversions. Therefore it cannot be
	 *       instantiated directly, must be overridden for each to be supported platform and has to
	 *       implement the getTimestamp(), getConversionConstant()
	 *       and requestSetConversionFactors() methods. Since each platform has its' own internal
	 *       value type to hold a specific time value, the type need be provided as the template
	 *       parameter T.
	 *
	 * Typeparams:
	 * T - 	Internal time value data-type to represent a point in time numerically.
	 **************************************************************************************************/
	template <typename T>
	class PlatformTime
	{

	protected:
		using ConversionFactorMap = std::map<ETimeUnit, double>;

	public:
		typedef typename PlatformTime<T> timer_type;
		typedef typename T               internal_time_value_type;
		
	protected: 		

		/**************************************************************************************************
		 * Fn:	PlatformTime::PlatformTime() = default;
		 *
		 * Summary:	Default constructor.
		 **************************************************************************************************/
		PlatformTime() = default;
	public:    		

		/**************************************************************************************************
		 * Fn:	virtual PlatformTime::~PlatformTime() = default;
		 *
		 * Summary:	Destructor.
		 **************************************************************************************************/
		virtual ~PlatformTime() = default;

		/**************************************************************************************************
		 * Fn:	EEngineStatus PlatformTime::initialize();
		 *
		 * Summary:
		 *  Initializes this instance by requesting the currently valid conversion
		 *  	factors for each permitted value of ETimeUnit.
		 *
		 * Returns:	The EEngineStatus.
		 **************************************************************************************************/
		EEngineStatus initialize();		

		/**************************************************************************************************
		 * Fn:	double PlatformTime::getConversionMask(ETimeUnit unit = ETimeUnit::Seconds);
		 *
		 * Summary:	Determine the proper conversion factor to convert from.
		 *
		 * Parameters:
		 * unit - 	(Optional) The unit.
		 *
		 * Returns:
		 *  Valid conversion factor from seconds to @unit.
		 *          1.0 if the requested target unit is not supported.
		 **************************************************************************************************/
		double getConversionMask(ETimeUnit unit = ETimeUnit::Seconds);

		/**************************************************************************************************
		 * Fn:	virtual EEngineStatus PlatformTime::getTimestamp(T& buffer) const = 0;
		 *
		 * Summary:
		 *  Fetch the current platform implementation timestamp to perform all necessary computations.
		 *
		 * Parameters:
		 * buffer - 	[in,out] Reference to a field holding the value if successful.
		 *
		 * Returns:	EEngineStatus::Ok if successful. An error Of EEngineStatus otherwise.
		 **************************************************************************************************/
		virtual EEngineStatus getTimestamp(T& buffer) const = 0;	

		/**************************************************************************************************
		 * Fn:	virtual EEngineStatus PlatformTime::getConversionConstant(T& buffer) const = 0;
		 *
		 * Summary:
		 *  Fetch the base conversion constant from the platform implementation to convert from platform
		 *  Low-level time resolution to Seconds.
		 *
		 * Parameters:
		 * buffer - 	[in,out] Reference to a field holding the value if successful.
		 *
		 * Returns:	EEngineStatus::Ok if successful. An error Of EEngineStatus otherwise.
		 **************************************************************************************************/
		virtual EEngineStatus getConversionConstant(T& buffer) const = 0;

		/**************************************************************************************************
		 * Fn:	virtual void PlatformTime::requestSetConversionFactors(ConversionFactorMap& map) = 0;
		 *
		 * Summary:
		 *  Requests derivate implementations to set all valid conversion factors
		 *  	for each supported ETimeUnit.
		 *
		 * Parameters:
		 * map - 	[in,out] Assignment-container to hold all supported conversion factors.
		 **************************************************************************************************/
		virtual void requestSetConversionFactors(ConversionFactorMap& map) = 0;
		
		/** Summary: Container holding all valid mappings of <time unit> to <conversion factor>. */
		ConversionFactorMap m_conversionFactors;
	};

	template <typename T>
	EEngineStatus PlatformTime<T>::initialize() {
		requestSetConversionFactors(m_conversionFactors);
		return EEngineStatus::Ok;
	}


	template <typename T>
	double PlatformTime<T>::getConversionMask(ETimeUnit unit) {
		ConversionFactorMap::iterator it = m_conversionFactors.find(unit);
		if (it != m_conversionFactors.end())
			return it->second;
		else
			return 1.0;
	};
}

#endif