//
// Created by dottideveloper on 20.10.19.
//

#include "vulkan/resources/types/vulkanbufferresource.h"
#include "vulkan/resources/cvulkanprivateresourceobjectfactory.h"

namespace engine
{
    namespace vulkan
    {
        using namespace gfxapi;
        using namespace resources;

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanPrivateResourceObjectFactory::initialize()
        {
            Unique<IResourceObjectCreatorBase> p = makeUnique<CResourceObjectCreator<CBufferResource::SDescriptor>>(nullptr);
            setCreatorForType<CVulkanBufferResource>(std::move(p));

            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CEngineResult<> CVulkanPrivateResourceObjectFactory::deinitialize()
        {
            return EEngineStatus::Ok;
        }
        //<-----------------------------------------------------------------------------

    }
}
