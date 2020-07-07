#include "asset/mesh/declaration.h"

namespace engine::mesh
{
    //<-----------------------------------------------------------------------------

    //<-----------------------------------------------------------------------------
    //
    //<-----------------------------------------------------------------------------
    Shared<CMeshInstance> CMeshInstance::fromAsset(SMeshAsset const &aMeshAsset)
    {
        auto mesh = makeShared<CMeshInstance>();

        mesh->mName                 = aMeshAsset.name;
        mesh->mAttributes           = aMeshAsset.attributes;
        mesh->mIndices              = aMeshAsset.indices;
        mesh->mAttributeSampleCount = aMeshAsset.attributeSampleCount;
        mesh->mIndexSampleCount     = aMeshAsset.indexSampleCount;
        mesh->mBinaryFilename       = aMeshAsset.binaryFilename;

        return mesh;
    }
}
