#ifndef _FUNCTORPROPERTYCHANGELISTENER_H_
#define _FUNCTORPROPERTYCHANGELISTENER_H_

#include "IPropertyChangeListener.hpp"
#include "ValueChangedArguments.hpp"

namespace DataBinding {

    template <typename T, typename Functor>
    // A generic functor listener, which encapsulates either a function pointer or a lambda-function.
    // Using the template type parameter allows to cover both with a single implementation.
    class FunctorPropertyChangeListener
            : public IPropertyChangeListener < T > {
    public:
        FunctorPropertyChangeListener(Functor functor)
            : IPropertyChangeListener<T>(),
              m_functor(functor)
        {}

        ~FunctorPropertyChangeListener() {
            m_functor = nullptr;
        }

        // Invoke the functor.
        // @Remarks:
        //			Since the functor can also be a lambda-functor, a nullptr-check is no more possible.
        void onPropertyChanged(const char *propertyName, const ValueChangedArguments<T>& arguments) {
            m_functor(propertyName, arguments);
        }

        inline const Functor& internalFunctor() const { return m_functor; }

    private:
        Functor m_functor;
    };

}

#endif
