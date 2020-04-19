#ifndef __SHIRABE_WSI_WINDOWHANDLEWRAPPER_H__
#define __SHIRABE_WSI_WINDOWHANDLEWRAPPER_H__

#include "os/applicationenvironment.h"

namespace engine
{
    namespace wsi
    {
        using engine::os::OSHandle_t;

        /**
         * Wraps an operating system window handle.
         */
        class CWindowHandleWrapper
        {
        public_typedefs:
            using Handle_t = OSHandle_t;

        public_constructors:
            /**
             * Construct a handle wrapper around a provided window handle.
             *
             * @param aHandle Handle to wrap.
             */
            SHIRABE_INLINE explicit CWindowHandleWrapper(Handle_t const &aHandle)
                : mHandle(aHandle)
            {}

        public_methods:
            /**
             * Return the currently stored window handle.
             *
             * @return See brief.
             */
            SHIRABE_INLINE const Handle_t &handle() const
            {
                return mHandle;
            }

		private:
            Handle_t mHandle;
		};

	}
}

#endif
