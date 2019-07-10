#include "graphicsapi/resources/types/mesh.h"

namespace engine
{
    namespace gfxapi
    {

        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        CMeshDeclaration::SDescriptor::SDescriptor()
            : SDescriptorImplBase<EResourceType::MESH, EResourceSubType::MESH_STATIC>()
            , name("")
        {}

        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string CMeshDeclaration::SDescriptor::toString() const
        {
            std::stringstream ss;
            ss
                    << "CMesh::SDescriptor ('" << name << "'): ";

            return ss.str();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CMeshDeclaration::CCreationRequest::CCreationRequest(SDescriptor const &aDescriptor)
            : CBaseDeclaration::CCreationRequestBase<SDescriptor>(aDescriptor)
        {}
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string CMeshDeclaration::CCreationRequest::toString() const
        {
            std::stringstream ss;
            ss
                    << "MeshCreationRequest: \n"
                    << std::endl;

            return ss.str();
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        CMesh::CMesh(CMesh::SDescriptor const &aDescriptor)
            : CMeshDeclaration()
            , CResourceDescriptorAdapter<CMeshDeclaration::SDescriptor>(aDescriptor)
        {}
        //<-----------------------------------------------------------------------------

    }
}
