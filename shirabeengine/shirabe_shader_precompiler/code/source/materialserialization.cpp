#include "materialserialization.h"

namespace shader_precompiler
{
    namespace serialization
    {

        bool CMaterialSerializer::serialize(SShaderCompilationUnit const &aUnit, CStdSharedPtr_t<IResult> &aOutResult)
        {
            aUnit.acceptSerializer(*this);
        }

        bool CMaterialSerializer::serializeUnit(SShaderCompilationUnit const &aUnit)
        {}

        bool CMaterialSerializer::serializeStage(SShaderCompilationElement const &aStage)
        {
            aStage.acceptSerializer(*this);
        }

        bool CMaterialSerializer::serializeInputDescription(std::vector<spirv_cross::Resource> const &aResource)
        {}

        bool CMaterialSerializer::serializeSubpassInputs(std::vector<spirv_cross::Resource> const &aResource)
        {}

        bool CMaterialSerializer::serializeSubpassOutputs(std::vector<spirv_cross::Resource> const &aResource)
        {}

        bool CMaterialSerializer::serializeUniformBuffers(std::vector<spirv_cross::Resource> const &aResource)
        {}

        bool CMaterialSerializer::serializeSampledImages(std::vector<spirv_cross::Resource> const &aResource)
        {}

    }
}
