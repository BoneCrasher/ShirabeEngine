#include "wsi/iwindoweventcallbackadapter.h"

namespace engine
{
    namespace wsi
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        namespace PRIVATE
        {
            /**
             * Helper to check whether a callback is already registered or not.
             *
             * @param aCollection  The reference collection.
             * @param aElement     The element to check for.
             * @param aOutIterator The iterator referrering the found element or collection.end().
             * @return             True if found. False otherwise.
             */
            static bool checkContainsCallbackPtr(
                    IWindow::IEventCallbackList                 const &aCollection,
                    Shared<IWindow::IEventCallback>    const &aElement,
                    IWindow::IEventCallbackList::const_iterator       &aOutIterator)
            {
                aOutIterator = std::find(aCollection.begin(), aCollection.end(), aElement);

                bool const found = (aOutIterator != aCollection.end());
                return found;
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CWindowEventCallbackAdapter::CWindowEventCallbackAdapter()
            : mEventCallbacks()
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CWindowEventCallbackAdapter::~CWindowEventCallbackAdapter()
        {
            if(!callbacks().empty()) {
                mutableCallbacks().clear();
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CWindowEventCallbackAdapter::registerCallback(
                Shared<IWindow::IEventCallback> aCallback)
        {
            if(!aCallback)
            {
                return EEngineStatus::NullPointer;
            }

            IWindow::IEventCallbackList::const_iterator position{};
            if(PRIVATE::checkContainsCallbackPtr(callbacks(), aCallback, position))
            {
                return EEngineStatus::ObjectAlreadyAddedToCollection;
            }

            try
            {
                std::lock_guard<std::mutex> guard(mEventCallbackMutex);

                mutableCallbacks().insert(mEventCallbacks.end(), aCallback);
                return EEngineStatus::Ok;
            }
            catch(...)
            {
                return EEngineStatus::CollectionInsertException;
            }
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        EEngineStatus CWindowEventCallbackAdapter::unregisterCallback(
                Shared<IWindow::IEventCallback> aCallback)
        {
            if(!aCallback)
            {
                return EEngineStatus::NullPointer;
            }

            IWindow::IEventCallbackList::const_iterator position{};
            if(PRIVATE::checkContainsCallbackPtr(callbacks(), aCallback, position))
            {
                return EEngineStatus::ObjectNotAddedToCollection;
            }

            try
            {
                std::lock_guard<std::mutex> guard(mEventCallbackMutex);

                mutableCallbacks().erase(position);
                return EEngineStatus::Ok;
            }
            catch(...)
            {
                return EEngineStatus::CollectionEraseException;
            }
        }
        //<-----------------------------------------------------------------------------
    }
}
