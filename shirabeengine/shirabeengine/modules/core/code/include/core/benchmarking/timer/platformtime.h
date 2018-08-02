#ifndef __SHIRABE_TIMEBASE_H__
#define __SHIRABE_TIMEBASE_H__

#include <map>
#include <iostream>   // for printf

#include "platform/platform.h"
#include "core/enginestatus.h"

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace Engine
{
    /**
     * Specifies valid time-units to be used for converting the internal time values.
     */
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




    /**
     * The PlatformTime-class is used to provide a common base interface and
     * functionality for platform time access, storage and conversions. Therefore it cannot be
     * instantiated directly, must be overridden for each to be supported platform and has to
     * implement the getTimestamp(), getConversionConstant()
     * and requestSetConversionFactors() methods. Since each platform has its' own internal
     * value type to hold a specific time value, the type need be provided as the template
     * parameter T.
     *
     * @tparam T Internal time value data-type to represent a point in time numerically.
     */
	template <typename T>
    class CPlatformTime
    {
    public_typedefs:
        using TimerType_t      = CPlatformTime<T>;
        using TimerValueType_t = T               ;

    public_destructors:

        /**
         * Destroy and run
         */
        virtual ~CPlatformTime() = default;

        /**
         * Initializes this instance by requesting the currently valid conversion
         * factors for each permitted value of ETimeUnit.
		 *
         * @return An EEngineStatus flag describing the operation result.
         */
		EEngineStatus initialize();		

        /**
         * Determine the proper conversion factor to convert from.
         *
         * @param unit (Optional) The unit.
         * @return  Valid conversion factor from seconds to @unit.
		 *          1.0 if the requested target unit is not supported.
         */
        double getConversionMask(ETimeUnit aUnit = ETimeUnit::Seconds);

        /**
         * Fetch the current platform implementation timestamp to perform all necessary computations.
         *
         * @param buffer [in,out] Reference to a field holding the value if successful.
         * @return                EEngineStatus::Ok if successful. An error Of EEngineStatus otherwise.
         */
        virtual EEngineStatus getTimestamp(T &aBuffer) const = 0;

        /**
         * Fetch the base conversion constant from the platform implementation to convert from platform
         * Low-level time resolution to Seconds.
         *
         * @param buffer [in,out] Reference to a field holding the value if successful.
         * @return       EEngineStatus::Ok if successful. An error Of EEngineStatus otherwise.
		 **************************************************************************************************/
        virtual EEngineStatus getConversionConstant(T &aBuffer) const = 0;


    protected_typedefs:
        using ConversionFactorMap_t = std::map<ETimeUnit, double>;

    protected_constructors:
        /**
         * Run and destroy
         */
        CPlatformTime() = default;

    protected_methods:
        /**
         * Requests derivate implementations to set all valid conversion factors
         * for each supported ETimeUnit.
         *
         * @param map [in,out] Assignment-container to hold all supported conversion factors.
         **************************************************************************************************/
        virtual void requestSetConversionFactors(ConversionFactorMap_t &aMap) = 0;
		
    private_members:
        /**
         * Container holding all valid mappings of <time unit> to <conversion factor>.
         */
        ConversionFactorMap_t mConversionFactors;
	};
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
	template <typename T>
    EEngineStatus CPlatformTime<T>::initialize()
    {
        requestSetConversionFactors(mConversionFactors);
		return EEngineStatus::Ok;
	}
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //<
    //<-----------------------------------------------------------------------------
	template <typename T>
    double CPlatformTime<T>::getConversionMask(ETimeUnit unit)
    {
        ConversionFactorMap_t::iterator it = mConversionFactors.find(unit);
        if (it != mConversionFactors.end())
			return it->second;
		else
			return 1.0;
	};
    //<-----------------------------------------------------------------------------
}

#endif
