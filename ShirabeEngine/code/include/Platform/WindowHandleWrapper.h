#ifndef __SHIRABE_WINDOWHANDLEWRAPPER_H__
#define __SHIRABE_WINDOWHANDLEWRAPPER_H__

#include "Platform/ApplicationEnvironment.h"

namespace Platform {
	namespace Window {

		class WindowHandleWrapper {
		public:
#ifdef PLATFORM_WINDOWS
			typedef HWND Handle;
#else 
			typedef unsigned int Handle;
#endif

			inline explicit WindowHandleWrapper(const Handle& handle)
				: _handle(handle) {}

			inline const Handle& handle() const { return _handle; }

		private:
			Handle _handle;
		};

	}
}

#endif