#ifndef __SHIRABE_OS_WINDOWHANDLEWRAPPER_H__
#define __SHIRABE_OS_WINDOWHANDLEWRAPPER_H__

#include "OS/ApplicationEnvironment.h"

namespace Engine {
	namespace OS {

		class WindowHandleWrapper {
		public:
			typedef OSHandle Handle;

			inline explicit WindowHandleWrapper(const Handle& handle)
				: m_handle(handle) {}

			inline const Handle& handle() const { return m_handle; }

		private:
			Handle m_handle;
		};

	}
}

#endif