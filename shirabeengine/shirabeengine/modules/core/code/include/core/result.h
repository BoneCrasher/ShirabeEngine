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
    template <typename TResult, typename TData = void*>
    class AResult
    {
    public_constructors:
        /**
         * Create a result from a result information and optional data.
         *
         * @param aResult
         * @param aData
         */
        SHIRABE_INLINE AResult(
                TResult              const &aResult,
                std::optional<TData> const &aData = std::optional<TData>())
            : mResult(aResult)
            , mData(aData)
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
        SHIRABE_INLINE std::optional<TData> const &data() const
        {
            return mData;
        }

        /**
         * Return the associated mutable function result data of this result struct.
         *
         * @return See brief.
         */
        SHIRABE_INLINE std::optional<TData> &data()
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
        TResult              mResult;
        std::optional<TData> mData;
    };


}

#endif // RESULT_H
