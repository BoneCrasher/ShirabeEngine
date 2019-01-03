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
    public_constructors:
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

        /**
         * To be implemented by specific result classes.
         *
         * @return True, if the function call was successful.
         */
        virtual bool successful() const = 0;

    private_members:
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
        inline AResult(TResult const &aResult)
            : mResult(aResult)
        {}

    public_destructors:
        /**
         * Destroy and run...
         */
        virtual ~AResult() = default;

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
         * To be implemented by specific result classes.
         *
         * @return True, if the function call was successful.
         */
        virtual bool successful() const = 0;

    private_members:
        TResult mResult;
    };
}

#endif // RESULT_H
