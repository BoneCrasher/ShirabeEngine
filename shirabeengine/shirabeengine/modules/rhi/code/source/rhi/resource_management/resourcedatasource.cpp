#include "rhi/resource_management/resourcedatasource.h"

namespace engine
{
    namespace rhi
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CResourceDataSource::CResourceDataSource()
            : mAccessorFunction(nullptr)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CResourceDataSource::CResourceDataSource(ResourceAccessFn_t const &aFunction)
            : mAccessorFunction(aFunction)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        ByteBuffer CResourceDataSource::getData() const
        {
            if(mAccessorFunction)
                return mAccessorFunction();
            else
                return ByteBuffer{ };
        }
        //<-----------------------------------------------------------------------------
    }
}
