#ifndef __SHIRABE_WSI_WINDOWSWINDOW_H__
#define __SHIRABE_WSI_WINDOWSWINDOW_H__

#include <atomic>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#ifdef Status
#undef Status
#endif

#ifdef index
    #undef index
#endif

#ifdef Always
    #undef Always
#endif

#include <log/log.h>

#include "wsi/windowhandlewrapper.h"
#include "wsi/windowmanager.h"
#include "wsi/iwindow.h"
#include "wsi/iwindoweventcallbackadapter.h"
#include "wsi/x11/x11display.h"

namespace engine
{
    namespace wsi
    {
        namespace x11
        {
            using namespace engine;
            using namespace os;

            /**
             * The IX11Adapter class provides a window system event handling mechanism implemented by
             * a window to receive window events.
             */
            class IX11Adapter
            {
                SHIRABE_DECLARE_INTERFACE(IX11Adapter);

            public_api:
                /**
                 * Invoked, when a window was created and assigned a screen handle.
                 *
                 * @param aScreenHandle The window handle assigned from the window manager.
                 */
                virtual void onCreate(uint64_t const &aScreenHandle) = 0;
                /**
                 * Invoked, when the window was enabled in the system and can operate.
                 */
                virtual void onEnabled() = 0;
                /**
                 * Invoked, when the window is shown on the screen.
                 */
                virtual void onShow() = 0;
                /**
                 * Invoked, when the window is being hidden from the screen.
                 */
                virtual void onHide() = 0;
                /**
                 * Invoked, when the window is being disabled an won't be functional anymore.
                 */
                virtual void onDisabled() = 0;
                /**
                 * Invoked, when a window is being closed.
                 */
                virtual void onClose() = 0;
                /**
                 * Invoked, when a window instances is being destroyed.
                 */
                virtual void onDestroy() = 0;

                /**
                 * Invoked, when a window was moved.
                 *
                 * @param aPositionX New x position of the windows top left corner
                 * @param aPositionY New y position of the windows top left corner
                 */
                virtual void onMove(
                        uint32_t const &aPositionX,
                        uint32_t const &aPositionY) = 0;

                /**
                 * Invoked, when a window is resized.
                 *
                 * @param aWidth  The new width of the window in pixels.
                 * @param aHeight The new height of the window in pixels.
                 */
                virtual void onResize(
                        uint32_t const &aWidth,
                        uint32_t const &aHeight) = 0;
            };

            /**
             * THe CX11Window class implements IWindow and IX11Adapter for the X11 framework on compatible
             * unix systems.
             */
            class SHIRABE_LIBRARY_EXPORT CX11Window
                    : public wsi::IWindow
                    , public IX11Adapter
            {
                SHIRABE_DECLARE_LOG_TAG(CX11Window);

            public_static_constants:
                constexpr static const char* sWindowDefaultName = "X11Window";

            public_constructors:
                /**
                 * Construct a new X11 window.
                 *
                 * @param aDisplay       The X11 display the window is attached to.
                 * @param aWindow        Underlying X11 window handle.
                 * @param aName          Name of the window to create.
                 * @param aInitialBounds Initial bounding rectangle of the window to create.
                 */
                CX11Window(
                        Display           *aDisplay,
                        Window      const &aWindow,
                        std::string const &aName,
                        CRect       const &aInitialBounds);

            public_destructors:
                /**
                 * Destroy and run...
                 */
                ~CX11Window();

            public_methods:
                /**
                 * Return the window handle of the window.
                 *
                 * @return See brief.
                 */
                wsi::CWindowHandleWrapper::Handle_t const &handle() const;

                /**
                 * Return the name of the window.
                 *
                 * @return See brief.
                 */
                std::string const &name()   const;
                /**
                 * Return the current bounds of the window relative to the operating
                 * systems coordinate origin.
                 *
                 * @return See brief.
                 */
                CRect const &bounds() const;

                /**
                 * Show this window.
                 *
                 * @return EEngineStatus::Ok if successful. An error code otherwise.
                 */
                EEngineStatus show();
                /**
                 * Hide this window.
                 *
                 * @return EEngineStatus::Ok if successful. An error code otherwise.
                 */
                EEngineStatus hide();
                /**
                 * Resume operatin in this window.
                 *
                 * @return EEngineStatus::Ok if successful. An error code otherwise.
                 */
                EEngineStatus resume();
                /**
                 * Update this window.
                 *
                 * @return EEngineStatus::Ok if successful. An error code otherwise.
                 */
                EEngineStatus update();
                /**
                 * Pause this window.
                 *
                 * @return EEngineStatus::Ok if successful. An error code otherwise.
                 */
                EEngineStatus pause();

                /**
                 * Register a new event callback handler for this window.
                 *
                 * @param aCallback The callback listener to hook in.
                 * @return          EEngineStatus::Ok if successful. An error code otherwise.
                 */
                EEngineStatus registerCallback(Shared<IWindow::IEventCallback> aCallback);
                /**
                 * Register a new event callback handler for this window.
                 *
                 * @param aCallback The callback listener to hook in.
                 * @return          EEngineStatus::Ok if successful. An error code otherwise.
                 */
                EEngineStatus unregisterCallback(Shared<IWindow::IEventCallback> aCallback);

                /**
                 * Invoked, when a window was created and assigned a screen handle.
                 *
                 * @param aScreenHandle The window handle assigned from the window manager.
                 */
                void onCreate(uint64_t const &aScreenHandle);
                /**
                 * Invoked, when the window was enabled in the system and can operate.
                 */
                void onEnabled();
                /**
                 * Invoked, when the window is shown on the screen.
                 */
                void onShow();
                /**
                 * Invoked, when the window is being hidden from the screen.
                 */
                void onHide();
                /**
                 * Invoked, when the window is being disabled an won't be functional anymore.
                 */
                void onDisabled();
                /**
                 * Invoked, when a window is being closed.
                 */
                void onClose();
                /**
                 * Invoked, when a window instances is being destroyed.
                 */
                void onDestroy();

                /**
                 * Invoked, when a window was moved.
                 *
                 * @param aPositionX New x position of the windows top left corner
                 * @param aPositionY New y position of the windows top left corner
                 */
                void onMove(
                        uint32_t const &aPositionX,
                        uint32_t const &aPositionY);

                /**
                 * Invoked, when a window is resized.
                 *
                 * @param aWidth  The new width of the window in pixels.
                 * @param aHeight The new height of the window in pixels.
                 */
                void onResize(
                        uint32_t const &aWidth,
                        uint32_t const &aHeight);

            private_methods:
                bool handleEvent(
                        Display       *aDisplay,
                        Window  const &aWindow,
                        XEvent  const &aEvent);

            private_members:
                Shared<CWindowManager> mWindowManager;
                Display                        *mDisplay;
                std::string                     mName;
                CRect                           mBounds;
                std::atomic_bool                mActive;
                CWindowHandleWrapper            mHandleWrapper;
                CWindowEventCallbackAdapter     mCallbackAdapter;

            };
        }
    }
}
#endif
