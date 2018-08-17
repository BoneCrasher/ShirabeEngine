#include <algorithm>
#include <functional>

#include <platform/platform.h>
#include <core/string.h>
#include "wsi/windowmanager.h"

#ifdef SHIRABE_PLATFORM_WINDOWS
// #include "WSI/Windows/WindowsWindowFactory.h"
#endif // SHIRABE_PLATFORM_WINDOWS

namespace engine
{
    namespace wsi
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CWindowManager::CWindowManager()
            : mWindows(),
              mWindowFactory(nullptr)
        {

        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CWindowManager::~CWindowManager()
        {
            if(!mWindows.empty())
            {
                for(CStdSharedPtr_t<IWindow> const &window : mWindows)
                {
                    CLog::Warning(logTag(), CString::format("Pending, non-finalized window instance found with name '%0'", window->name()));
                }

                mWindows.clear();
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CWindowManager::EWindowManagerError CWindowManager::initialize(os::SApplicationEnvironment const &aApplicationEnvironment)
        {
#ifdef SHIRABE_PLATFORM_WINDOWS
            mWindowFactory = makeCStdSharedPtr<WSI::Windows::WindowsWindowFactory>((HINSTANCE)aApplicationEnvironment.instanceHandle);
#endif // SHIRABE_PLATFORM_WINDOWS

            if(!mWindowFactory)
            {
                CLog::Error(logTag(), "Failed to initialize the window factory.");

                return EWindowManagerError::InitializationFailed;
            }

            return EWindowManagerError::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CWindowManager::EWindowManagerError CWindowManager::deinitialize()
        {
            mWindowFactory = nullptr;
            // if (!<condition>) {
            // 	Log::Error(logTag(), "Failed to initialize the window factory.");
            // 	return EWindowManagerError::InitializationFailed;
            // }

            return EWindowManagerError::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CWindowManager::EWindowManagerError CWindowManager::update()
        {
            EWindowManagerError error = EWindowManagerError::Ok;

            for(CStdSharedPtr_t<IWindow> const &window : mWindows)
            {
                if(CheckEngineError(window->update()))
                {
                    CLog::Warning(logTag(), CString::format("Window '%0' failed to update.", window->name()));

                    error = EWindowManagerError::UpdateFailed;
                    continue;
                }
            }

            return error;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CStdSharedPtr_t<IWindow> CWindowManager::createWindow(
                std::string const &aName,
                CRect       const &aInitialBounds)
        {
            CStdSharedPtr_t<IWindow> window = mWindowFactory->createWindow(aName, aInitialBounds);
            if(!window)
            {
                CLog::Warning(logTag(), CString::format("Failed to create window '%0' with bounds x/y/w/h --> %1/%2/%3/%4",
                                                      aName.c_str(),
                                                      aInitialBounds.position.x(), aInitialBounds.position.y(),
                                                      aInitialBounds.size.x(),     aInitialBounds.size.y()));
                return nullptr;
            }

            mWindows.push_back(window);

            return window;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CStdSharedPtr_t<IWindow> CWindowManager::getWindowByName(std::string const &aName)
        {
            for(CStdSharedPtr_t<IWindow> const &window : mWindows)
            {
                if(window->name().compare(aName) == 0)
                {
                    return window;
                }
            }

            return nullptr;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CStdSharedPtr_t<IWindow> CWindowManager::getWindowByHandle(os::CWindowHandleWrapper::Handle_t const &aHandle)
        {
            auto const predicate = [&] (CStdSharedPtr_t<IWindow> const &aCompare) -> bool
            {
                return aCompare->handle() == aHandle;
            };

            IWindowList::iterator it = std::find_if(mWindows.begin(), mWindows.end(), predicate);

            return (it == mWindows.end()) ? nullptr : (*it);

            // One line version:
            // return ((it = std::find_if(_windows.begin(), m_windows.end(), [&](const IWindowPtr& cmp) -> bool { return cmp->handle() == handle; })) == m_windows.end) ? nullptr : (*it);

            // Below code can also be used. But above is a educational demonstration of std::find_if.
            // for (const IWindowPtr& pWindow : m_windows) {
            // 	if (pWindow->handle() == handle)
            // 		return pWindow;
            // }
            // return nullptr;
        }
        //<-----------------------------------------------------------------------------
    }
}
