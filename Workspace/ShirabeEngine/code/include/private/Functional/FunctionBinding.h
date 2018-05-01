#ifndef __SHIRABE_FUNCTIONBINDING_H__
#define __SHIRABE_FUNCTIONBINDING_H__

#include <functional>

namespace Functional {
	
	template <typename    T,         // Instance type containing a member function
              typename    TFnReturn, // Member function return-type
              typename... TFnParams> // Member function parameters
		/**
		* @brief bind a member function of type T with a variable number of arguments and a specific instance of
		*        type T together as a callable std::function.
		* @param instance
		* @return
		*/
		static std::function<TFnReturn(TFnParams...)> bind(
			T& instance,
			TFnReturn(T::*fn)(TFnParams...)
		) {
		return [&instance, fn](TFnParams... args) mutable -> TFnReturn { return (instance.*fn)(args...); };
	}
}

#endif