#version 450 core
#extension GL_GOOGLE_include_directive : require
#include "base.glsl"

// Attachment binding; Set=1 is mandatory!
layout(input_attachment_index = 0, set = 2, binding = 0) uniform subpassInput gbuffer0;
layout(input_attachment_index = 1, set = 2, binding = 1) uniform subpassInput gbuffer1;
layout(input_attachment_index = 2, set = 2, binding = 2) uniform subpassInput gbuffer2;
layout(input_attachment_index = 3, set = 2, binding = 3) uniform subpassInput gbuffer3;
layout(input_attachment_index = 4, set = 2, binding = 4) uniform subpassInput depth;

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
    vec4 position_f_spec   = subpassLoad(gbuffer0);
    vec4 normal_f_spec_exp = subpassLoad(gbuffer1);
    vec4 diffuse           = subpassLoad(gbuffer2);
    vec4 other_1           = subpassLoad(gbuffer3);
    vec4 depth             = subpassLoad(depth);

    vec3  N = normalize(normal_f_spec_exp.xyz);

    vec3  E = vec3(0.0f,  0.0f,  0.0f);
    vec3  L = vec3(0.0f,  1.0f,  -0.5f);
    // vec3  L = vec3(0.0f,  0.0f,  -1.0f);
    vec3  V = -position_f_spec.xyz;
    vec3  R = -normalize(reflect(L, N));

    float f_lambert  = dot(normalize(L), N);
    vec3  I_specular = vec3(1.0f, 1.0f, 1.0f);
    float f_specular = 0.0f;

    if(0.0f < f_lambert)
    {
        f_specular = clamp(dot(R, V), 0.0f, 1.0f);
        f_specular = ((normal_f_spec_exp.a + 2.0f) / (2.0f * 3.14152f)) * position_f_spec.a * pow(f_specular, normal_f_spec_exp.a + 1.0f);
    }

    // fragment_light_0 = vec4( clamp((f * C), 0.0f, 1.0f), 1.0f );
    fragment_light_0 = vec4( clamp(f_specular * I_specular, 0.0f, 1.0f), clamp(f_lambert, 0.0f, 1.0f));
} 
