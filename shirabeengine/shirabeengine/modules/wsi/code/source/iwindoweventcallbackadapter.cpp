#include "WSI/IWindowEventCallbackAdapter.h"

namespace engine {
  namespace WSI {

    namespace __Private {
      static bool __checkContainsCallbackPtr(
        IWindow::IEventCallbackList                 const&collection,
        IWindow::IEventCallbackPtr                  const&element,
        IWindow::IEventCallbackList::const_iterator      *iterator)
      {
        IWindow::IEventCallbackList::const_iterator it;
        it = std::find(collection.begin(), collection.end(), element);

        if(iterator) *iterator = it;

        return (it != collection.end());
      }
    }

    WindowEventCallbackAdapter::WindowEventCallbackAdapter()
      : m_eventCallbacks()
    {}

    WindowEventCallbackAdapter::~WindowEventCallbackAdapter() {
      if(!callbacks().empty()) {
        mutableCallbacks().clear();
      }
    }

    EEngineStatus WindowEventCallbackAdapter::registerCallback(
      IWindow::IEventCallbackPtr const&cb
    ) {
      if(!cb)
        return EEngineStatus::NullPointer;

      if(__Private::__checkContainsCallbackPtr(callbacks(), cb, nullptr)) {
        return EEngineStatus::ObjectAlreadyAddedToCollection;
      }

      try {
        std::lock_guard<std::mutex> guard(m_eventCallbackMutex);
        mutableCallbacks().insert(m_eventCallbacks.end(), cb);
        return EEngineStatus::Ok;
      }
      catch(...) {
        return EEngineStatus::CollectionInsertException;
      }
    }

    EEngineStatus WindowEventCallbackAdapter::unregisterCallback(
      IWindow::IEventCallbackPtr const&cb)
    {
      if(!cb)
        return EEngineStatus::NullPointer;

      IWindow::IEventCallbackList::const_iterator it;
      if(__Private::__checkContainsCallbackPtr(callbacks(), cb, &it)) {
        return EEngineStatus::ObjectNotAddedToCollection;
      }

      try {
        std::lock_guard<std::mutex> guard(m_eventCallbackMutex);
        mutableCallbacks().erase(it);
        return EEngineStatus::Ok;
      }
      catch(...) {
        return EEngineStatus::CollectionEraseException;
      }
    }
  }
}