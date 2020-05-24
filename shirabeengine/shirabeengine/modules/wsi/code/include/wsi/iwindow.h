#ifndef __SHIRABE_IWINDOW_H__
#define __SHIRABE_IWINDOW_H__

// #include "Compiler/MSVCWarnings.h"

#include <string>

#include <core/enginetypehelper.h>
#include <core/enginestatus.h>
#include <core/basictypes.h>
#include "wsi/windowhandlewrapper.h"

namespace engine
{
    namespace wsi
    {
        /**
         * The IWindow-interface declares the minimum contract required for a window instance inside
         * the engine window manager.
         */
        class IWindow
        {
            SHIRABE_DECLARE_INTERFACE(IWindow)

        public_interfaces:
            /**
             * The IEventCallback-interface declares the minimum contract required for a window
             * to provide window events to engine client components.
             */
            class IEventCallback
            {
                SHIRABE_DECLARE_INTERFACE(IEventCallback)

            public_api:
                /**
                 * Invoked, when the window starts operating, i.e. receives and handles messages from the operating
                 * system and engine window manager.
                 *
                 * @param aWindow The window which resumed operation.
                 */
                virtual void onResume(Shared<IWindow> const &aWindow) = 0;

                /**
                 * Invoked, when the window is shown in the operating system window manager, i.e. takes part
                 * in the rendering cycle of the OS and can be interacted with.
                 *
                 * @param aWindow The window which was shown.
                 */
                virtual void onShow(Shared<IWindow> const &aWindow) = 0;

                /**
                 * Invoked, when the window is moved or resized.
                 *
                 * @param aWindow    The moved and/or resized window.
                 * @param aNewBounds The new bounding rectangle of the window, relative to the operating
                 *                   systems coordinate origin.
                 */
                virtual void onBoundsChanged(
                        Shared<IWindow> const &aWindow,
                        CRect                    const &aNewBounds) = 0;

                /**
                 * Invoked, when the window is hidden in the operating system window manger, i.e.
                 * when it won't be rendered anymore and can no more be interacted with.
                 *
                 * @param aWindow The window which was hidden.
                 */
                virtual void onHide(Shared<IWindow> const &aWindow) = 0;

                /**
                 * Invoked, when the window pauses operation, i.e. does not more receive and/or handle messages from the
                 * operating system and engine window manager.
                 *
                 * @param aWindow The paused window.
                 */
                virtual void onPause(Shared<IWindow> const &aWindow) = 0;

                /**
                 * Invoked, just before a window is closed to perform any kind of shutdown and cleanup operations.
                 *
                 * @param aWindow The window to be closed.
                 */
                virtual void onClose(Shared<IWindow> const &aWindow) = 0;

                /**
                 * Invoked, just before a window instance is destroyed.
                 *
                 * @param aWindow The window to be destroyed.
                 */
                virtual void onDestroy(Shared<IWindow> const &aWindow) = 0;
            };

        public_typedefs:
            SHIRABE_DECLARE_LIST_OF_TYPE(Shared<IEventCallback>, IEventCallback) // IEventCallbackList


        public_api:
            /**
             * Return the window handle of the window.
             *
             * @return See brief.
             */
            virtual const wsi::CWindowHandleWrapper::Handle_t& handle() const = 0;

            /**
             * Return the name of the window.
             *
             * @return See brief.
             */
            virtual const std::string &name() const = 0;

            /**
             * Return the current bounds of the window relative to the operating
             * systems coordinate origin.
             *
             * @return See brief.
             */
            virtual const CRect &bounds() const = 0;

            /**
             * Show this window.
             *
             * @return EEngineStatus::Ok if successful. An error code otherwise.
             */
            virtual EEngineStatus show() = 0;

            /**
             * Hide this window.
             *
             * @return EEngineStatus::Ok if successful. An error code otherwise.
             */
            virtual EEngineStatus hide() = 0;

            /**
             * Resume operatin in this window.
             *
             * @return EEngineStatus::Ok if successful. An error code otherwise.
             */
            virtual EEngineStatus resume() = 0;

            /**
             * Update this window.
             *
             * @return EEngineStatus::Ok if successful. An error code otherwise.
             */
            virtual EEngineStatus update() = 0;

            /**
             * Pause this window.
             *
             * @return EEngineStatus::Ok if successful. An error code otherwise.
             */
            virtual EEngineStatus pause()  = 0;

            /**
             * Register a new event callback handler for this window.
             *
             * @param aCallback The callback listener to hook in.
             * @return          EEngineStatus::Ok if successful. An error code otherwise.
             */
            virtual EEngineStatus registerCallback(Shared<IWindow::IEventCallback> aCallback) = 0;

            /**
             * Register a new event callback handler for this window.
             *
             * @param aCallback The callback listener to hook in.
             * @return          EEngineStatus::Ok if successful. An error code otherwise.
             */
            virtual EEngineStatus unregisterCallback(Shared<IWindow::IEventCallback> aCallback) = 0;
        };

        SHIRABE_DECLARE_LIST_OF_TYPE(Shared<IWindow>, IWindow)

    }
}

#endif
