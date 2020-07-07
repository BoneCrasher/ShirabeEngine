#include "asset/buffers/declaration.h"

namespace engine::buffers
{
//<-----------------------------------------------------------------------------
//
//<-----------------------------------------------------------------------------
    Shared <CBufferInstance> CBufferInstance::fromAsset(SBufferAsset const &aAsset)
    {
        auto instance = makeShared<CBufferInstance>();

        instance->mName           = aAsset.name;
        instance->mBufferSize     = aAsset.bufferSize;
        instance->mBinaryFilename = aAsset.binaryFilename;

        return instance;
    }
}
