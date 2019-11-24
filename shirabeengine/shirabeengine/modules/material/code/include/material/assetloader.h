//
// Created by dotti on 24.11.19.
//

#ifndef __SHIRABEDEVELOPMENT_ASSETLOADER_H__
#define __SHIRABEDEVELOPMENT_ASSETLOADER_H__

#include <asset/assetstorage.h>
#include <resources/resourcedescriptions.h>
#include <resources/resourcetypes.h>
#include <resources/cresourcemanager.h>

namespace engine::material
{
    using namespace resources;
    static Unique<CResourceFromAssetResourceObjectCreator<SMaterial>> getAssetLoader(Shared<resources::CResourceManager> const &aResourceManager
                                                                                   , Shared<asset::IAssetStorage>        const &aAssetStorage
                                                                                   , Shared<material::CMaterialLoader>   const &aMaterialLoader)
    {
        auto const loader = [=] (ResourceId_t const &aResourceId, AssetId_t const &aAssetId) -> Shared<ILogicalResourceObject>
        {
            auto const &[result, instance] = aMaterialLoader->loadMaterialInstance(aAssetId);
            if(CheckEngineError(result))
            {
                return nullptr;
            }

            Shared<CMaterialMaster> const &master    = instance; // instance->master();
            SMaterialSignature      const &signature = master  ->signature();
            CMaterialConfig         const &config    = instance->config();

            // Copy the template!
            SMaterialPipelineDescriptor pipelineDescriptor     = SMaterialPipelineDescriptor(instance->getPipelineDescriptor());
            SShaderModuleDescriptor     shaderModuleDescriptor = instance->getShaderModuleDescriptor();
            Vector<SBufferDescription>  bufferDescriptions     = instance->getBufferDescriptors();

            std::string              const renderPassHandle   = mCurrentRenderPassHandle;
            std::vector<std::string> const textureViewHandles = {}; // All texture view names are immediately based on the texture-names, no further decoration needed.
            std::vector<std::string> const bufferHandles      = {}; // All uniform buffer names are a compounds as <materialname>_<buffername>.

            CEngineResult<Shared<ILogicalResourceObject>> shaderModuleObject = aResourceManager->useAssetResource(shaderModuleDescriptor.name);

            for(auto const &bufferDesc : bufferDescriptions)
            {

            }

            pipelineDescriptor.name                  = aMaterial.readableName;
            pipelineDescriptor.referenceRenderPassId = renderPassHandle;
            pipelineDescriptor.subpass               = mCurrentSubpass;

            std::vector<std::string> pipelineDependencies {};
            pipelineDependencies.push_back(renderPassHandle);
            for(auto const &textureViewHandle : textureViewHandles)
            {
                pipelineDependencies.push_back(textureViewHandle);
            }
            for(auto const &bufferViewHandle : bufferHandles)
            {
                pipelineDependencies.push_back(fmt::format("{}_{}", pipelineDescriptor.name, bufferViewHandle));
            }
            CEngineResult<Shared<ILogicalResourceObject>> pipelineObject = mResourceManager->useDynamicResource<SPipeline>(pipelineDescriptor.name, pipelineDescriptor, std::move(pipelineDependencies));
            EngineStatusPrintOnError(pipelineObject.result(), logTag(), "Failed to create pipeline.");
        };

        return makeUnique<CResourceFromAssetResourceObjectCreator<SMaterial>>(loader);
    }
}

#endif //__SHIRABEDEVELOPMENT_ASSETLOADER_H__
