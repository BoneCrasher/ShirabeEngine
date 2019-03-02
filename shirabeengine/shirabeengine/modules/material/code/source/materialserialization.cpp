#include "material/materialserialization.h"

namespace engine
{
    namespace serialization
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        EShaderStage stageFromString(std::string const &aString)
        {
            if(0 == aString.compare("vertex"))             return EShaderStage::Vertex;
            if(0 == aString.compare("tess_control_point")) return EShaderStage::TesselationControlPoint;
            if(0 == aString.compare("tess_evaluation"))    return EShaderStage::TesselationEvaluation;
            if(0 == aString.compare("geometry"))           return EShaderStage::Geometry;
            if(0 == aString.compare("fragment"))           return EShaderStage::Fragment;
            if(0 == aString.compare("compute"))            return EShaderStage::Compute;

            return EShaderStage::NotApplicable;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        std::string const stageToString(EShaderStage const &aStage)
        {
            switch(aStage)
            {
            case EShaderStage::Vertex:                  return "vertex";
            case EShaderStage::TesselationControlPoint: return "tess_control_point";
            case EShaderStage::TesselationEvaluation:   return "tess_evaluation";
            case EShaderStage::Geometry:                return "geometry";
            case EShaderStage::Fragment:                return "fragment";
            case EShaderStage::Compute:                 return "compute";
            default:                                    return "unknown";
            }
        }
        //<-----------------------------------------------------------------------------
    }
}
