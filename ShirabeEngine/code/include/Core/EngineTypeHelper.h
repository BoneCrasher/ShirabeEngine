#ifndef __SHIRABE_ENGINETYPEHELPER_H__
#define __SHIRABE_ENGINETYPEHELPER_H__

#include <any>
#include <memory>
#include <functional>
#include <vector>

namespace Engine {


	template <typename T>
	using Ptr = std::shared_ptr<T>;

	#define DeclareSharedPointerType(type) \
            using type##Ptr = Ptr<type>;

#define DeclarePrefixedSharedPointerType(prefix, type) \
            using prefix##Ptr = Ptr<type>;

    #define DeclareSharedPointerTypeCustomDeleter(type) \
            using type##Ptr = Ptr<type>;

    #define Template(...) __VA_ARGS__

    #define DeclareTemplatedSharedPointerType(prefix, type) \
	        using prefix##Ptr = Ptr<type>;

	template <typename TUnderlyingType, typename ... TArgs>
	static inline std::shared_ptr<TUnderlyingType> MakeSharedPointerType(TArgs&&... args) {
		return std::make_shared<TUnderlyingType>(std::forward<TArgs>(args)...);
	}

	template <typename T>
	using SharedPtrDeleterFn = std::function<void(T*)>;

	template <typename T, typename TDeleter>
	static inline Ptr<T>
		MakeSharedPointerTypeCustomDeleter(
			T          *pInstance,
		    TDeleter    deleter) {
		return Ptr<T>(pInstance, deleter);
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

	template <typename TEnum>
	static inline
		std::underlying_type_t<TEnum> EToUnderlying(const TEnum& r) {
		return static_cast<std::underlying_type_t<TEnum>>(r);
	}

	template <typename TEnum>
	static inline 
		std::underlying_type_t<TEnum> operator |(
			const TEnum& l,
			const TEnum& r
			) {
		return (EToUnderlying<TEnum>(l) | EToUnderlying<TEnum>(r));
	}

	template <typename TEnum>
	static inline 
		std::underlying_type_t<TEnum> operator |(
			const std::underlying_type_t<TEnum>& l,
			const TEnum& r
			) {
		return (l | EToUnderlying<TEnum>(r));
	}
	
	template <typename T, T... Rest>
	struct CombineEnumClassFlags;

	template <typename T>
	struct CombineEnumClassFlags<T> {
		static const std::underlying_type_t<T> value = 0;
	};
	
	template <typename T, T First, T... Rest>
	struct CombineEnumClassFlags<T, First, Rest...> {
		static const
			std::underlying_type_t<T> value
			= (((std::underlying_type_t<T>)First) | CombineEnumClassFlags<T, Rest...>::value);
	};

	template <typename T>
	struct EnumClassFlags {
		static inline 
		std::underlying_type_t<T>
			append(const std::underlying_type_t<T>& f,
				const T& a) {
			return (f | a);
		}
	};

	template <typename T>
	static inline bool checkFlag
	(
		const typename std::underlying_type<
		                  typename std::enable_if<std::is_enum<T>::value, T>::type
		               >::type &flags,   
		               const T &test
	) {
		return ((flags & ((typename std::underlying_type<T>::type) test)) == ((typename std::underlying_type<T>::type) test));
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


	#define DenyCopyAndMove(type, alias)              \
		type(const alias&)              = delete; \
		type(alias&&)                   = delete; \
		alias& operator =(const alias&) = delete; \
		alias& operator =(alias&&)      = delete; 

	#define DeclareInterface(name)           \
		class name {                         \
            public:                          \
                virtual ~name() = default;   \
                                             \
                DenyCopyAndMove(name, name); \
            protected:                       \
                name() = default;            \
            public:

	#define DeclareTemplatedInterface(name, alias)            \
    class name {                                         	  \
            public:                 						  \
				typedef alias my_type;						  \
															  \
                virtual ~name() = default;               	  \
                                                         	  \
                DenyCopyAndMove(name, alias);                 \
            protected:                                  	  \
                name() = default;                       	  \
            public:

	#define DeclareDerivedInterface(name, base) \
		class name 								\
	        : public base {                     \
            public:                             \
                virtual ~name() = default;      \
                                                \
                DenyCopyAndMove(name, name);    \
            protected:                          \
                name() = default;               \
            public:

	#define DeclareDerivedTemplatedInterface(name, alias, base) \
		class name 								\
	        : public base {                     \
            public:                             \
				typedef alias my_type;		    \
												\
                virtual ~name() = default;      \
                                                \
                DenyCopyAndMove(name, alias);   \
            protected:                          \
                name() = default;               \
            public:

	#define DeclareInterfaceEnd(name) };

}

#endif
