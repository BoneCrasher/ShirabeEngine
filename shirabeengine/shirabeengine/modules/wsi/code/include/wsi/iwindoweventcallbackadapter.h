#ifndef __SHIRABE_IWINDOWEVENTCALLBACKADAPTER_H__
#define __SHIRABE_IWINDOWEVENTCALLBACKADAPTER_H__

#include <mutex>
#include <functional>

#include "wsi/iwindow.h"

namespace engine
{
    namespace wsi
    {
        /**
         * The CWindowEventCallbackAdapter class default implements the IEventCallback
         * interface to store a list of callacks and forward all events to the respective listeners.
         */
        class CWindowEventCallbackAdapter
                : public IWindow::IEventCallback
        {
        public_constructors:
            /**
             * Default construct a CWindowEventCallbackAdapter
             */
            CWindowEventCallbackAdapter();

        public_destructors:
            /**
             * Destroy and run..
             */
            ~CWindowEventCallbackAdapter();

        public_methods:
            /**
             * Register a new event callback handler for this window.
             *
             * @param aCallback The callback listener to hook in.
             * @return          EEngineStatus::Ok if successful. An error code otherwise.
             */
            EEngineStatus registerCallback(CStdSharedPtr_t<IWindow::IEventCallback> aCallback);

            /**
             * Register a new event callback handler for this window.
             *
             * @param aCallback The callback listener to hook in.
             * @return          EEngineStatus::Ok if successful. An error code otherwise.
             */
            EEngineStatus unregisterCallback(CStdSharedPtr_t<IWindow::IEventCallback> aCallback);

            /**
             * Return the current list of registered callbacks.
             *
             * @return See brief.
             */
            SHIRABE_INLINE IWindow::IEventCallbackList const &callbacks() const
            {
                return mEventCallbacks;
            }

            /**
             * Invoked, when the window starts operating, i.e. receives and handles messages from the operating
             * system and engine window manager.
             *
             * @param aWindow The window which resumed operation.
             */
            SHIRABE_INLINE void onResume(CStdSharedPtr_t<IWindow> const &aWindow)
            {
                notifyIteratively(mEventCallbacks, &IWindow::IEventCallback::onResume, aWindow);
            }

            /**
             * Invoked, when the window is shown in the operating system window manager, i.e. takes part
             * in the rendering cycle of the OS and can be interacted with.
             *
             * @param aWindow The window which was shown.
             */
            SHIRABE_INLINE void onShow(CStdSharedPtr_t<IWindow> const &aWindow)
            {
                notifyIteratively(mEventCallbacks, &IWindow::IEventCallback::onShow, aWindow);
            }

            /**
             * Invoked, when the window is moved or resized.
             *
             * @param aWindow    The moved and/or resized window.
             * @param aNewBounds The new bounding rectangle of the window, relative to the operating
             *                   systems coordinate origin.
             */
            SHIRABE_INLINE void onBoundsChanged(
                    CStdSharedPtr_t<IWindow> const &aWindow,
                    CRect                    const &aBounds)
            {
                notifyIteratively(mEventCallbacks, &IWindow::IEventCallback::onBoundsChanged, aWindow, aBounds);
            }

            /**
             * Invoked, when the window is hidden in the operating system window manger, i.e.
             * when it won't be rendered anymore and can no more be interacted with.
             *
             * @param aWindow The window which was hidden.
             */
            SHIRABE_INLINE void onHide(CStdSharedPtr_t<IWindow> const &aWindow)
            {
                notifyIteratively(mEventCallbacks, &IWindow::IEventCallback::onHide, aWindow);
            }

            /**
             * Invoked, when the window pauses operation, i.e. does not more receive and/or handle messages from the
             * operating system and engine window manager.
             *
             * @param aWindow The paused window.
             */
            SHIRABE_INLINE void onPause(CStdSharedPtr_t<IWindow> const &aWindow)
            {
                notifyIteratively(mEventCallbacks, &IWindow::IEventCallback::onPause, aWindow);
            }

            /**
             * Invoked, just before a window is closed and destroyed to perform any kind of shutdown and cleanup operations.
             *
             * @param aWindow The window to be closed.
             */
            SHIRABE_INLINE void onClose(CStdSharedPtr_t<IWindow> const &aWindow)
            {
                notifyIteratively(mEventCallbacks, &IWindow::IEventCallback::onClose, aWindow);
            }

        private_typedefs:
            /**
             * Convenience alias to declare a notification callback function.
             */
            template<
                    typename    TFnReturn,
                    typename ...TFnArgs
                    >
            using NotificationCallback_t = std::function<TFnReturn(TFnArgs&&...)>;

        private_methods:
            /**
             * Loop over the list of event callback handlers and invoke a callback
             * function forwarding the provided arguments.
             *
             * @param aCollection Collection of callback handlers.
             * @param aFunction   The function to be invoked on the callback.
             * @param aArgs       Arguments to be forwarded.
             */
            template <typename    TFnReturn,
                      typename ...TFnArgs>
            SHIRABE_INLINE void notifyIteratively(
                    const IWindow::IEventCallbackList  &aCollection,
                    TFnReturn(IWindow::IEventCallback::*aFunction)(TFnArgs...),
                    TFnArgs                        &&...aArgs)
            {
                std::lock_guard<std::mutex> guard(mEventCallbackMutex);

                for(CStdSharedPtr_t<IWindow::IEventCallback> const &callback : aCollection)
                {
                    NotificationCallback_t<TFnReturn, TFnArgs...> const bound = std::bind(*callback.get(), aFunction);
                    if(bound)
                    {
                        bound(std::forward<TFnArgs>(aArgs)...);
                    }
                }
            }

            /**
             * Return the current list of registered callbacks.
             *
             * @return See brief.
             */
            SHIRABE_INLINE IWindow::IEventCallbackList& mutableCallbacks()
            {
                return mEventCallbacks;
            }

        private_members:
            std::mutex                  mEventCallbackMutex;
            IWindow::IEventCallbackList mEventCallbacks;
        };
    }
}

#endif
