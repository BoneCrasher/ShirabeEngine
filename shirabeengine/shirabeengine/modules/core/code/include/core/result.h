#ifndef __SHIRABE_RESULT_H__
#define __SHIRABE_RESULT_H__

#include <optional>
#include <base/declaration.h>


namespace engine
{
    /**
     * The AResult abstract class is a result tuple used to return an
     * error code combined with the result data associated with the function call.
     *
     * The purpose of this struct is to unify the way functions return errors, while
     * reducing the amount of exception throwing and out parameter usage.
     */
    template <typename TResult, typename TData>
    class AResult
    {
        template <typename T>
        friend T std::get();

    public_typedefs:
        typedef TData value_type;

    public_constructors:
        /**
         * Create an empty result.
         */
        SHIRABE_INLINE AResult()
            : mResult(TResult())
            , mData(TData())
        {}

        /**
         * Copy-Construct a result from another.
         *
         * @param aOther
         */
        explicit SHIRABE_INLINE AResult(AResult<TResult, TData> const &aOther)
            : mResult(aOther.mResult)
            , mData(aOther.mData)
        {}

        /**
         * Move-Construct a result from another.
         *
         * @param aOther
         */
        explicit SHIRABE_INLINE AResult(AResult<TResult, TData> &&aOther)
            : mResult(aOther.mResult)
            , mData(std::move(aOther.mData))
        {}

        /**
         * Convert-Copy-Construct this result from another containing any kind of assignable data.
         *
         * @param aOther Another result instance of datatype assignable to TData.
         */
        template <typename TOtherData>
        SHIRABE_INLINE AResult(AResult<TResult, TOtherData> const &aOther)
            : mResult(aOther.mResult)
            , mData(aOther.mData)
        {
            static_assert (std::is_assignable_v<TData, TOtherData>, "Can't assign TOtherData to TData.");
        }

        /**
         * Convert-Move-Construct this result from another containing any kind of assignable data.
         *
         * @param aOther Another result instance of datatype assignable to TData.
         */
        template <typename TOtherData>
        SHIRABE_INLINE AResult(AResult<TResult, TOtherData> && aOther)
            : mResult(aOther.mResult)
            , mData(std::move(aOther.mData))
        {
            static_assert(std::is_assignable_v<TData, TOtherData>, "Can't assign TOtherData to TData.");
        }

        /**
         * Create a result from a result code.
         *
         * @param aResult
         * @param aData
         */
        SHIRABE_INLINE AResult(TResult const &aResult)
            : mResult(aResult)
            , mData(TData())
        {}

        /**
         * Create a result from a result code and data by copy.
         *
         * @param aResult
         * @param aData
         */
        SHIRABE_INLINE AResult(TResult const &aResult, TData const &aData)
            : mResult(aResult)
            , mData(aData)
        {}

        /**
         * Create a result from a result code and data by move-semantics.
         *
         * @param aResult
         * @param aData
         */
        SHIRABE_INLINE AResult(TResult const &aResult, TData &&aData)
            : mResult(aResult)
            , mData(std::move(aData))
        {}

    public_destructors:
        /**
         * Destroy and run...
         */
        virtual ~AResult() = default;

    public_operators:
        /**
         * Copy-Assign another result to this instance.
         *
         * @param aOther
         * @return
         */
        SHIRABE_INLINE AResult<TResult, TData> &operator=(AResult<TResult, TData> const &aOther)
        {
            mResult = aOther.mResult;
            mData   = aOther.mData;

            return (*this);
        }

        /**
         * Move-Assign another result to this instance.
         *
         * @param aOther
         * @return
         */
        SHIRABE_INLINE AResult<TResult, TData> &operator=(AResult<TResult, TData> &&aOther)
        {
            mResult = aOther.mResult;
            mData   = std::move(aOther.mData);

            return (*this);
        }

        template <typename TOtherData>
        SHIRABE_INLINE AResult<TResult, TData> &operator=(AResult<TResult, TOtherData> const &aOther)
        {
            static_assert (std::is_assignable_v<TData, TOtherData>);

            mResult = aOther.mResult;
            mData   = aOther.mData;

            return (*this);
        }

        template <typename TOtherData>
        SHIRABE_INLINE AResult<TResult, TData> &operator=(AResult<TResult, TOtherData> &&aOther)
        {
            static_assert (std::is_assignable_v<TData, TOtherData>);

            mResult = aOther.mResult;
            mData   = std::move(aOther.mData);

            return (*this);
        }

    public_methods:
        /**
         * Return the associated result information of this result struct.
         *
         * @return See brief
         */
        SHIRABE_INLINE TResult const &result() const
        {
            return mResult;
        }

        /**
         * Return the associated immutable function result data of this result struct.
         *
         * @return See brief.
         */
        SHIRABE_INLINE TData const &data() const
        {
            return mData;
        }

        /**
         * Return the associated mutable function result data of this result struct.
         *
         * @return See brief.
         */
        SHIRABE_INLINE TData &data()
        {
            return mData;
        }

        template <std::size_t N>
        decltype(auto) get() const
        {
            if      constexpr (N == 0) return mResult;
            else if constexpr (N == 1) return mData;
        }

        /**
         * To be implemented by specific result classes.
         *
         * @return True, if the function call was successful.
         */
        virtual bool successful() const = 0;

    private_members:

        template <typename TSameResult, typename TOtherData> friend class AResult;

        TResult mResult;
        TData   mData;
    };

    /**
     * The AResult abstract class is a result tuple used to return an
     * error code combined with the result data associated with the function call.
     *
     * The purpose of this struct is to unify the way functions return errors, while
     * reducing the amount of exception throwing and out parameter usage.
     */
    template <typename TResult>
    class AResult<TResult, void>
    {
    public_constructors:
        /**
         * Create a result from a result code.
         *
         * @param aResult
         */
        SHIRABE_INLINE AResult(TResult const &aResult)
            : mResult(aResult)
        {}

        SHIRABE_INLINE AResult(AResult<TResult, void> const &aOther)
            : mResult(aOther.mResult)
        {}

        SHIRABE_INLINE AResult(AResult<TResult, void> &&aOther)
        {
            mResult = aOther.result();
        }

    public_destructors:
        /**
         * Destroy and run...
         */
        virtual ~AResult() = default;

    public_operators:
        SHIRABE_INLINE AResult<TResult, void> &operator=(AResult<TResult, void> const &aOther)
        {
            mResult = aOther.mResult;

            return (*this);
        }

        SHIRABE_INLINE AResult<TResult, void> &operator=(AResult<TResult, void> &&aOther)
        {
            mResult = aOther.mResult;

            return (*this);
        }

    public_methods:
        /**
         * Return the associated result information of this result struct.
         *
         * @return See brief
         */
        SHIRABE_INLINE TResult const &result() const
        {
            return mResult;
        }

        template <std::size_t N>
        decltype(auto) get() const
        {
            if constexpr (N == 0) return mResult;
        }

        /**
         * To be implemented by specific result classes.
         *
         * @return True, if the function call was successful.
         */
        virtual bool successful() const = 0;

    private_members:
        TResult mResult;
    };

    /**
     * Default Result implementation for bool data types.
     */
    template <typename TData>
    class CResult
            : public AResult<bool, TData>
    {
    public_constructors:
        using AResult<bool, TData>::AResult;

        /**
         * Create a result from a data value.
         *
         * @param aData
         */
        SHIRABE_INLINE CResult(TData const &aData)
            : AResult<bool, TData>(true, aData)
        {}

    public_operators:
        using AResult<bool, TData>::operator=;

    public_methods:
        SHIRABE_INLINE bool successful() const
        {
            return (AResult<bool, TData>::result());
        }
    };

}

// Make AResult decomposable...
namespace std
{
    template <typename TResult, typename TData>
    struct tuple_size<engine::AResult<TResult, TData>>
        : std::integral_constant<std::size_t, 2>
    {};

    template <typename TResult>
    struct tuple_size<engine::AResult<TResult, void>>
        : std::integral_constant<std::size_t, 1>
    {};

    template <typename TResult, typename TData>
    struct tuple_element<0, engine::AResult<TResult, TData>>
    {
        using type = TResult;
    };

    template <typename TResult, typename TData>
    struct tuple_element<1, engine::AResult<TResult, TData>>
    {
        using type = TData;
    };

    template <typename TResult>
    struct tuple_element<0, engine::AResult<TResult, void>>
    {
        using type = TResult;
    };

    // template <std::size_t N, typename TResult, typename TData>
    // struct tuple_element<N, engine::AResult<TResult, TData>>
    // {
    //     using type = decltype(std::declval<engine::AResult<TResult, TData>>().get<N>());
    // };
    //
    // template <std::size_t N, typename TResult>
    // struct tuple_element<N, engine::AResult<TResult, void>>
    // {
    //     using type = decltype(std::declval<engine::AResult<TResult, void>>().get<N>());
    // };
}

#endif // RESULT_H
