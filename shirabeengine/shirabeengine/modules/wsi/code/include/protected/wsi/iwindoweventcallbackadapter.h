#ifndef __SHIRABE_IWINDOWEVENTCALLBACKADAPTER_H__
#define __SHIRABE_IWINDOWEVENTCALLBACKADAPTER_H__

#include <mutex>

#include "Core/Functional/FunctionBinding.h"
#include "WSI/IWindow.h"

namespace engine {
  namespace WSI {

    class WindowEventCallbackAdapter
      : public IWindow::IEventCallback {
    public:
      WindowEventCallbackAdapter();
      ~WindowEventCallbackAdapter();

      EEngineStatus registerCallback(const IWindow::IEventCallbackPtr& cb);
      EEngineStatus unregisterCallback(const IWindow::IEventCallbackPtr& cb);

      inline const IWindow::IEventCallbackList& callbacks() const { return m_eventCallbacks; }

      //
      // IEventCallbackPtr implementation
      //
      inline void onResume(const IWindowPtr& pWindow) { notifyIteratively(m_eventCallbacks, &IWindow::IEventCallback::onResume, pWindow); }
      inline void onShow(const IWindowPtr& pWindow) { notifyIteratively(m_eventCallbacks, &IWindow::IEventCallback::onShow, pWindow); }
      inline void onBoundsChanged(const IWindowPtr& pWindow,
        const Rect&       bounds) {
        notifyIteratively(m_eventCallbacks, &IWindow::IEventCallback::onBoundsChanged, pWindow, bounds);
      }
      inline void onHide(const IWindowPtr& pWindow) { notifyIteratively(m_eventCallbacks, &IWindow::IEventCallback::onHide, pWindow); }
      inline void onPause(const IWindowPtr& pWindow) { notifyIteratively(m_eventCallbacks, &IWindow::IEventCallback::onPause, pWindow); }
      inline void onClose(const IWindowPtr& pWindow) { notifyIteratively(m_eventCallbacks, &IWindow::IEventCallback::onClose, pWindow); }

    private:
      template <typename    TFnReturn,
        typename ...TFnArgs>
        using NotificationCallback_t = std::function<TFnReturn(TFnArgs&&...)>;

      template <typename    TFnReturn,
        typename ...TFnArgs>
        inline void notifyIteratively(
          const IWindow::IEventCallbackList  &collection,
          TFnReturn(IWindow::IEventCallback::*fn)(TFnArgs...),
          TFnArgs                        &&...args
        ) {
        std::lock_guard<std::mutex> guard(m_eventCallbackMutex);

        NotificationCallback_t<TFnReturn, TFnArgs...> bound;
        for(const IWindow::IEventCallbackPtr& cb : collection) {
          bound = Functional::bind(*cb.get(), fn);
          if(bound)
            bound(std::forward<TFnArgs>(args)...);
        }
      }

      inline IWindow::IEventCallbackList& mutableCallbacks() { return m_eventCallbacks; }

      std::mutex                  m_eventCallbackMutex;
      IWindow::IEventCallbackList m_eventCallbacks;
    };
  }
}

#endif