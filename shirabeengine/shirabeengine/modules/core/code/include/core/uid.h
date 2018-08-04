#ifndef __SHIRABE_CORE_UID_H__
#define __SHIRABE_CORE_UID_H__

#include "core/enginetypehelper.h"

namespace engine
{
    namespace core
    {
        /**
         * Generic interface for any kind of random generator to be used.
         */
        template <typename TUIDUnderlyingType>
        class IUIDGenerator
        {
            SHIRABE_DECLARE_INTERFACE(IUIDGenerator);

        public_api:
            /**
             * Fetch the next random number from the underlying generator.
             *
             * @return
             */
            virtual TUIDUnderlyingType generate() = 0;
        };

    }
}

#endif
