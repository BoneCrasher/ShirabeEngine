#ifndef _IPROPERTYCHANGELISTENER_H_
#define _IPROPERTYCHANGELISTENER_H_

#include <functional>
#include "ValueChangedArguments.hpp"

namespace DataBinding {
    template <typename T>
    // Provides a basic interface to create INotifyPropertyChange-compliant objects that can be registered for value change notifications.
    class IPropertyChangeListener {
    public:
        virtual ~IPropertyChangeListener() = default;

        // Define the onPropertyChanged signature as a function pointer type!
        // @See onPropertyChanged for more information.
        typedef typename std::function<void(const char*, const ValueChangedArguments<T>&)> callback_func_type;
        // void(*callback_func_type)(const char *propertyName, const ValueChangedArguments<T>& arguments);

        // Implement so that value change notifications can be provided for a specific property with corresponding event arguments.
        // @Param 'propertyName':
        //					The property name whose value has changed.
        // @Param 'arguments':
        //					A PropertyChangeArgumentContainer encapsulating an arbitrary object with additional data provided.
        virtual void onPropertyChanged(const char *propertyName, const ValueChangedArguments<T>& arguments) = 0;

        virtual const callback_func_type& internalFunctor() const = 0;

    protected:
        // Prevent direct instantiation
        IPropertyChangeListener() = default;
    };
}

#endif
