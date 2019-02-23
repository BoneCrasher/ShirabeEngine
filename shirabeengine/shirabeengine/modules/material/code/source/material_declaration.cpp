#include "material/material_declaration.h"

namespace engine
{
    namespace material
    {
        //<-----------------------------------------------------------------------------
        //
        //<-----------------------------------------------------------------------------
        bool SMaterialStage::acceptSerializer(serialization::IMaterialSerializer &aSerializer) const
        {
            return true;
        }
        //<-----------------------------------------------------------------------------

        //<-----------------------------------------------------------------------------
        //<
        //<-----------------------------------------------------------------------------
        bool SMaterial::acceptSerializer(serialization::IMaterialSerializer &aSerializer) const
        {
            return true;
        }
        //<-----------------------------------------------------------------------------
    }
}
