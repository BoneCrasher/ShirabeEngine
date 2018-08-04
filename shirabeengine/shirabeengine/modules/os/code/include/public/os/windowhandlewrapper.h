#ifndef __SHIRABE_OS_WINDOWHANDLEWRAPPER_H__
#define __SHIRABE_OS_WINDOWHANDLEWRAPPER_H__

#include "os/applicationenvironment.h"

namespace engine
{
    namespace os
    {
        /**
         * Wraps an operating system window handle.
         */
        class WindowHandleWrapper
        {
		public:
            using Handle_t = OSHandle_t;

            SHIRABE_INLINE explicit WindowHandleWrapper(Handle_t const &handle)
                : m_handle(handle)
            {}

            inline const Handle_t &handle() const
            {
                return mHandle;
            }

		private:
            Handle_t mHandle;
		};

	}
}

#endif
