#version 450 core
#extension GL_GOOGLE_include_directive : require
#include "base.glsl"

// Attachment binding; Set=1 is mandatory!
layout(input_attachment_index = 0, set = 2, binding = 0) uniform subpassInput gbuffer0;
layout(input_attachment_index = 1, set = 2, binding = 1) uniform subpassInput gbuffer1;
layout(input_attachment_index = 2, set = 2, binding = 2) uniform subpassInput gbuffer2;
layout(input_attachment_index = 3, set = 2, binding = 3) uniform subpassInput depth;

// layout(std140, set = 3, binding = 0)
// uniform struct_screenInfo
// {
//     vec2 extent;
// } screenInfo;

// Input
// layout (location = 0)
// in vec2 texcoord;

// Output
layout (location = 0) out vec4 fragment_light_0;

void main()
{
    vec4 diffuse = subpassLoad(gbuffer0);
    vec4 normal  = subpassLoad(gbuffer1);
    vec4 other   = subpassLoad(gbuffer2);
    vec4 depth   = subpassLoad(depth);

    fragment_light_0 = diffuse; // Just forward...
} 
