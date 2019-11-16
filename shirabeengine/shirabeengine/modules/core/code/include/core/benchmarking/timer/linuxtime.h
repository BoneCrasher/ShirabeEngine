#ifndef __SHIRABE__TIME_LINUX_H__
#define __SHIRABE__TIME_LINUX_H__

#pragma warning(push)
#pragma warning(once:4514)
#pragma warning(once:4820)
#pragma warning(once:4668)
#pragma warning(once:4005)
#pragma warning(once:4350)

#include <time.h>

#include <platform/platform.h>
#include <log/log.h>
#include "core/enginestatus.h"
#include "core/enginetypehelper.h"
#include "core/benchmarking/timer/platformtime.h"

namespace engine
{
    /**
     * Linux implementation of a high resolution timer.
     */
    class CLinuxTime
        : public CPlatformTime<timespec>
	{
        SHIRABE_DECLARE_LOG_TAG(CLinuxTime)

    public_typedefs:
        using InternalTimeValueType_t = timespec;

    public_constructors:
        /**
         * @brief CLinuxTime
         */
        CLinuxTime();

    public_destructors:
        /**
         * Destroy and run...
         */
        ~CLinuxTime();

    public_methods:
        EEngineStatus getTimestamp(InternalTimeValueType_t &aBuffer)          const;
        EEngineStatus getConversionConstant(InternalTimeValueType_t &aBuffer) const;

        void requestSetConversionFactors(ConversionFactorMap_t& map);

	protected:		

        /**
         * Queries the current clock resolution used to convert from the platform internal
         *   	   time/counter representation format to a Seconds-value.
         *
         *  This value maps directly to the conversion constant.
         *
         * @param buffer
         * @return
         */
        EEngineStatus getClockResolution(InternalTimeValueType_t &aBuffer) const;

        /**
         *  Queries the current clock timestamp to fetch the current counter value. For time calculations
         *  this method has to be invoked at least once each frame!
         *
         * @param buffer
         * @return
         */
        EEngineStatus getClockTimestamp(InternalTimeValueType_t &buffer) const;
	};
}

#pragma warning(pop)

#endif
