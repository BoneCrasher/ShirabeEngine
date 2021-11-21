#include <log/log.h>
#include <base/string.h>

#include "wsi/x11/x11window.h"

namespace engine
{
    namespace wsi
    {
        namespace x11
        {
            //<-----------------------------------------------------------------------------
            //
            //<-----------------------------------------------------------------------------
            CX11Window::CX11Window(
                    Display           *aDisplay,
                    Window      const &aWindow,
                    std::string const &aName,
                    CRect       const &aInitialBounds)
                : IWindow()
                , IX11Adapter()
                , mWindowManager(nullptr)
                , mDisplay(aDisplay)
                , mName(aName)
                , mBounds(aInitialBounds)
                , mActive(false)
                , mHandleWrapper(aWindow)
                , mCallbackAdapter()
            { }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            CX11Window::~CX11Window()
            {
                if(mActive.load())
                {
                    mActive.store(false);
                }
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            wsi::CWindowHandleWrapper::Handle_t const &CX11Window::handle() const
            {
                return mHandleWrapper.handle();
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            std::string const &CX11Window::name() const
            {
                return mName;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            CRect const &CX11Window::bounds() const
            {
                return mBounds;
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            EEngineStatus CX11Window::show()
            {
                try
                {
                    Window const &window = mHandleWrapper.handle();

                    XMapWindow(mDisplay, window);

                    return EEngineStatus::Ok;
                }
                catch(...)
                {
                    CLog::Error(logTag(), StaticStringHelpers::format("Failed to show window '{}'", name()));
                    return EEngineStatus::WindowEventError;
                }
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            EEngineStatus CX11Window::hide()
            {
                try
                {
                    Window const &window = mHandleWrapper.handle();

                    XUnmapWindow(mDisplay, window);

                    return EEngineStatus::Ok;
                }
                catch(...)
                {
                    CLog::Error(logTag(), StaticStringHelpers::format("Failed to hide window '{}'", name()));
                    return EEngineStatus::WindowEventError;
                }
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            EEngineStatus CX11Window::resume()
            {
                try
                {
                    mActive.store(true);
                    mCallbackAdapter.onResume(makeSharedFromInstance(this));

                    return EEngineStatus::Ok;
                }
                catch(...)
                {
                    CLog::Error(logTag(), StaticStringHelpers::format("Failed to show window '{}'", name()));
                    return EEngineStatus::WindowEventError;
                }
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            EEngineStatus CX11Window::update()
            {
                if(!mActive.load())
                    return EEngineStatus::Ok;

                Window const &window = mHandleWrapper.handle();

                XEvent event = {};

                try
                {
                    // Asynchronously, there could be more and more events coming in,
                    // but we select a snapshot of events here and process only the next
                    // 'eventCount' events in this update cycle.
                    int32_t const eventCount = XPending(mDisplay);
                    for(int32_t k=0; k<eventCount; ++k)
                    {
                        XEvent event{};

                        XNextEvent(mDisplay, &event);

                        bool const handled = handleEvent(mDisplay, window, event);
                        if(!handled)
                        {
                            // ??? further handling?
                        }
                    }

                    return EEngineStatus::Ok;
                }
                catch(...)
                {
                    CLog::Error(logTag(), StaticStringHelpers::format("Failed to update message queue of window '{}'", name()));
                    return EEngineStatus::WindowMessageHandlerError;
                }
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            bool CX11Window::handleEvent(
                    Display       *aDisplay,
                    Window  const &aWindow,
                    XEvent  const &aEvent)
            {
                if(CreateNotify == aEvent.type)
                {
                    XCreateWindowEvent const &event = aEvent.xcreatewindow;

                    onCreate(event.window);

                    return true;
                }

                if(DestroyNotify == aEvent.type)
                {
                    onDestroy();

                    return true;
                }

                if(ReparentNotify == aEvent.type)
                {

                    return true;
                }

                if(MapNotify == aEvent.type)
                {
                    onShow();

                    return true;
                }

                if(UnmapNotify == aEvent.type)
                {
                    onHide();

                    return true;
                }

                if(ConfigureNotify == aEvent.type)
                {
                    uint32_t const windowOffsetX = aEvent.xconfigure.x;
                    uint32_t const windowOffsetY = aEvent.xconfigure.y;
                    uint32_t const windowWidth   = aEvent.xconfigure.width;
                    uint32_t const windowHeight  = aEvent.xconfigure.height;

                    onMove(windowOffsetX, windowOffsetY);
                    onResize(windowWidth, windowHeight);

                    return true;
                }

                if(Expose == aEvent.type)
                {
                    // Window was exposed. Redraw it.
                    if(0 == aEvent.xexpose.count)
                    {
                        // If the expose count is zero, no more expose events
                        // will follow, thus rerender entirely.

                    }

                    return true;
                }



                return false; // Unhandled
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            EEngineStatus CX11Window::pause()
            {
                try
                {
                    mActive.store(false);
                    mCallbackAdapter.onPause(makeSharedFromInstance(this));

                    return EEngineStatus::Ok;
                }
                catch(...)
                {
                    CLog::Error(logTag(), StaticStringHelpers::format("Failed to show window '{}'", name()));
                    return EEngineStatus::WindowEventError;
                }
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            EEngineStatus CX11Window::registerCallback(Shared<IWindow::IEventCallback> aCallback)
            {
                return mCallbackAdapter.registerCallback(aCallback);
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            EEngineStatus CX11Window::unregisterCallback(Shared<IWindow::IEventCallback> aCallback)
            {
                return mCallbackAdapter.unregisterCallback(aCallback);
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            void CX11Window::onCreate(uint64_t const &aScreenHandle)
            {
                mHandleWrapper = CWindowHandleWrapper((OSHandle_t)aScreenHandle);
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            void CX11Window::onEnabled()
            {

            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            void CX11Window::onShow()
            {
                mCallbackAdapter.onShow(makeSharedFromInstance(this));
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            void CX11Window::onHide()
            {
                mCallbackAdapter.onHide(makeSharedFromInstance(this));
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            void CX11Window::onDisabled()
            {

            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            void CX11Window::onClose()
            {
                mCallbackAdapter.onClose(makeSharedFromInstance(this));
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            void CX11Window::onDestroy()
            {
                mCallbackAdapter.onDestroy(makeSharedFromInstance(this));
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            void CX11Window::onMove(
                    uint32_t const &aPositionX,
                    uint32_t const &aPositionY)
            {
                mBounds.position.x(aPositionX);
                mBounds.position.y(aPositionY);

                mCallbackAdapter.onBoundsChanged(makeSharedFromInstance(this), mBounds);
            }
            //<-----------------------------------------------------------------------------

            //<-----------------------------------------------------------------------------
            //<
            //<-----------------------------------------------------------------------------
            void CX11Window::onResize(
                    uint32_t const &aWidth,
                    uint32_t const &aHeight)
            {
                mBounds.size.x(aWidth);
                mBounds.size.y(aHeight);

                mCallbackAdapter.onBoundsChanged(makeSharedFromInstance(this), mBounds);
            }
            //<-----------------------------------------------------------------------------
        }
    }
}
