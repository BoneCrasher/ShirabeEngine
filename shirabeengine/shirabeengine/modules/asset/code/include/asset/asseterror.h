#ifndef __SHIRABE_ASSET_ERROR_H__
#define __SHIRABE_ASSET_ERROR_H__

#include <exception>
#include <string>

#include <core/enginetypehelper.h>

namespace engine
{
    namespace asset
    {
        /**
         * The EAssetErrorCode enum describes all possible states of asset interaction.
         */
        enum class EAssetErrorCode
        {
            Ok,
            AssetNotFound,
            AssetAlreadyAdded,
        };

        /**
         * The CAssetError class wraps an error message and asset error code as an
         * std::exception compatible type.
         */
        class CAssetError
                : public std::runtime_error
        {
        public_constructors:
            SHIRABE_INLINE CAssetError(
                    std::string     const &aMessage,
                    EAssetErrorCode const &aErrorCode)
                : std::runtime_error(aMessage)
                , mCode(aErrorCode)
            {}

        public_members:
            SHIRABE_INLINE EAssetErrorCode code() const
            {
                return mCode;
            }

        private_members:
            EAssetErrorCode mCode;
        };

    }
}

#endif
