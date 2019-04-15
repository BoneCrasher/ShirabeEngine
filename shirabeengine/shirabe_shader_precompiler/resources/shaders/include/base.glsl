#extension GL_ARB_separate_shader_objects : enable

//
// Set 0: Global Application defined data
//
layout (std140, set = 0, binding = 0)
uniform GlobalData
{
    float time;
}
globalData;

//
// Set 1: Reserved for future use
//
// layout (std140, set = 1, binding = 0)
// uniform Reserved
// {
//     float _UNUSED;
// }
reserved;
