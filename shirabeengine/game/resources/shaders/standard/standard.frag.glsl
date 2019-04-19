#version 450 core
#extension GL_GOOGLE_include_directive : require
#include "base.glsl"

// Attachment binding; Set=1 is mandatory!
layout(input_attachment_index = 0, set = 1, binding = 0) uniform subpassInput gbuffer0;
layout(input_attachment_index = 1, set = 1, binding = 1) uniform subpassInput gbuffer1;
layout(input_attachment_index = 2, set = 1, binding = 2) uniform subpassInput gbuffer2;
layout(input_attachment_index = 3, set = 1, binding = 3) uniform subpassInput gbuffer3;

layout(set = 3, binding = 0) uniform sampler2D diffuseTexture;
layout(set = 3, binding = 1) uniform sampler2D specularReflectanceGlossTexture;
layout(set = 3, binding = 2) uniform sampler2D normalTexture;

// Input
layout (location = 0)
in struct_Input
{
    vec3 vertex_position;
    vec3 vertex_normal;
    vec3 vertex_tangent;
    vec2 vertex_texcoord;
} shader_input;

// Output
layout (location = 0) out vec4 fragment_color_0;

void main()
{
    fragment_color_0 = 0.5 * (gl_FragCoord + 1.0);
} 
