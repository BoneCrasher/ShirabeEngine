#version 440 core
#extension GL_GOOGLE_include_directive : require
#include "include/base.glsl"

// Attachment binding
layout(input_attachment_index = 0, set = 0, binding = 0)
uniform subpassInput gbuffer0;
layout(input_attachment_index = 1, set = 0, binding = 1)
uniform subpassInput gbuffer1;
layout(input_attachment_index = 2, set = 0, binding = 2)
uniform subpassInput gbuffer2;
layout(input_attachment_index = 3, set = 0, binding = 3)
uniform subpassInput gbuffer3;

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
layout (location = 0)
out vec4 fragment_color_0;

void main()
{
    fragment_color_0 = 0.5 * (gl_FragCoord + 1.0);
} 
