#ifndef __SHIRABE_IRESOURCEMANAGER_H__
#define __SHIRABE_IRESOURCEMANAGER_H__

#include "Core/EngineTypeHelper.h"
#include "Core/EngineStatus.h"
#include "Core/DataBuffer.h"

#include "ResourceDomainTransfer.h"

#include "Resources/Core/ResourceDTO.h"
#include "Resources/Subsystems/GFXAPI/GFXAPIResourceBackend.h"

#include "Resources/Subsystems/GFXAPI/Types/SwapChain.h"
#include "Resources/Subsystems/GFXAPI/Types/Texture.h"
#include "Resources/Subsystems/GFXAPI/Types/DepthStencilState.h"
#include "Resources/Subsystems/GFXAPI/Types/RasterizerState.h"

namespace Engine {
  namespace Resources {

    /**
     * \class ResourceDataSource
     *
     * \brief A resource data source.
     **************************************************************************************************/
    class ResourceDataSource {
    public:
      using ResourceAccessFn_t = std::function<ByteBuffer()>;

      inline
        ResourceDataSource(ResourceAccessFn_t const&fn)
        : m_accessorFn(fn)
      {}

      inline 
        ByteBuffer getData() {
        if(m_accessorFn)
          return m_accessorFn();
        else
          return ByteBuffer{};
      }

    private:
      ResourceAccessFn_t m_accessorFn;
    };

    #define DeclareResourceMethods(resource)              \
                                                          \
      virtual EEngineStatus create##resource(             \
        resource::CreationRequest const&inRequest,        \
        PublicResourceId_t             &outId,            \
        bool                            deferLoad = false \
      ) = 0;                                              \
                                                          \
      virtual EEngineStatus load##resource(               \
        PublicResourceId_t const&inId                     \
      ) = 0;                                              \
                                                          \
      virtual EEngineStatus update##resource(             \
        PublicResourceId_t      const&inId,               \
        resource::UpdateRequest const&inRequest           \
      ) = 0;                                              \
                                                          \
      virtual EEngineStatus unload##resource(             \
        PublicResourceId_t const&inId                     \
      ) = 0;                                              \
                                                          \
      virtual EEngineStatus destroy##resource(            \
        PublicResourceId_t const&inId                     \
      ) = 0;                                   

    DeclareInterface(IResourceManager);

    virtual bool clear() = 0;

    virtual Ptr<BasicGFXAPIResourceBackend>& backend() = 0;

    DeclareResourceMethods(SwapChain);
    DeclareResourceMethods(Texture);
    DeclareResourceMethods(RenderTargetView);
    DeclareResourceMethods(ShaderResourceView);
    DeclareResourceMethods(DepthStencilView);
    DeclareResourceMethods(DepthStencilState);
    DeclareResourceMethods(RasterizerState);

    DeclareInterfaceEnd(IResourceManager);
    DeclareSharedPointerType(IResourceManager);
  }
}

#endif