#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include <memory>
#include <list>
#include <algorithm>
#include <functional>

namespace QTG {

    template <typename TObserver>
    class Subject
    {
        public:
            using TPtr          = std::shared_ptr<TObserver>;
            using TPtrList      = std::list<TPtr>;

            template <typename... Args>
            using ObserverFwdFn = std::function<void(TPtr, Args&&...)>;

            Subject() : _observers() {}

            /**
             * @brief Register an TObserver-instance for notifications.
             * @param observer
             * @return
             */
            bool observe(TPtr observer) {
                typename TPtrList::const_iterator it = std::find(_observers.begin(), _observers.end(), observer);
                if(it != _observers.end())
                    return false;
                else {
                    _observers.push_back(observer);
                    return true;
                }
            }

            /**
             * @brief Unregister an TObserver-instance for notifications.
             * @param observer
             * @return
             */
            bool ignore(TPtr observer) {
                typename TPtrList::const_iterator it = std::find(_observers.begin(), _observers.end(), observer);
                if(it == _observers.end())
                    return false;
                else {
                    _observers.remove(observer);
                    return true;
                }
            }

            /**
             * @brief Iterates over all observers of that type and forwards it to a function that calls the appropriate method.
             * @param fn
             */
            template <typename... Args>
            void notify(ObserverFwdFn<Args...> fn, Args... args) {
                if(!fn) return;

                for(TPtr observer : _observers)
                    fn(observer, std::forward<Args>(args)...);
            }

        private:
            TPtrList _observers;

    };

    template <typename TObserver>
    using SubjectPtr = std::shared_ptr<Subject<TObserver>>;

}

#endif // OBSERVER_HPP
