#ifndef __SHIRABE_ENGINETYPEHELPER_H__
#define __SHIRABE_ENGINETYPEHELPER_H__

#include <memory>
#include <functional>
#include <vector>

namespace Engine {
	#define DeclareSharedPointerType(type) \
            using type##Ptr = std::shared_ptr<type>;

    #define DeclareSharedPointerTypeCustomDeleter(type) \
            using type##Ptr = std::shared_ptr<type>;   

	template <typename TUnderlyingType, typename ... TArgs>
	static inline std::shared_ptr<TUnderlyingType> MakeSharedPointerType(TArgs&&... args) {
		return std::make_shared<TUnderlyingType>(std::forward<TArgs>(args)...);
	}

	template <typename T>
	using SharedPtrDeleterFn = std::function<void(T*)>;

	template <typename T, typename TDeleter>
	static inline std::shared_ptr<T> 
		MakeSharedPointerTypeCustomDeleter(
			T        *pInstance,
		    TDeleter  deleter) {
		return std::shared_ptr<T>(pInstance, deleter);
	}

	template <typename T, typename TPtr = std::shared_ptr<T>>
	static inline TPtr GetNonDeletingSelfPtrType(T* instance) {
		return TPtr(instance, [](T*) -> void {; /* Do not delete */ });
	}

	template <typename T, typename TPtr = std::shared_ptr<T>>
	static inline TPtr GetNonDeletingPtrType(TPtr ptr) {
		return TPtr(ptr.get(), [](T*) -> void {; /* Do not delete */ });
	}


	#define DeclareUniquePointerType(type) \
            using type##Ptr = std::unique_ptr<type>;

	template <typename TUnderlyingType, typename ... TArgs>
	static inline std::unique_ptr<TUnderlyingType> MakeUniquePointerType(TArgs&&... args) {
		return std::make_unique<TUnderlyingType>(std::forward<TArgs>(args)...);
	}

	#define DeclareEnumClassUnderlyingType(enumName, typeName) \
            typedef std::underlying_type<enumName>::type typeName;


	template <typename TEnum, typename TUnderlying = std::underlying_type<TEnum>::type>
	static inline bool CheckEnumFlag(
		const TEnum       &value,
		const TUnderlying &underlyingFlags) {
		TUnderlying underlyingValue = static_cast<TUnderlying>(value);
		return ((underlyingFlags & underlyingValue) == underlyingValue);
	}

#define DeclareListType(type, prefix) using prefix##List = std::vector<type>;

#define DeclareTemplateListType(type, prefix) \
             template <typename type>         \
	         using prefix##List = std::vector<std::shared_ptr<type>>;


	#define DeclareInterface(name)                      \
    class name {                                        \
            public:                                     \
                virtual ~name() = default;              \
                                                        \
                name(const name&)             = delete; \
                name(name&&)                  = delete; \
				name& operator =(const name&) = delete; \
                name& operator =(name&&)      = delete; \
            protected:                                  \
                name() = default;                       \
            public:

	#define DeclareInterfaceEnd(name)                   \
            };                                          
}

#endif
