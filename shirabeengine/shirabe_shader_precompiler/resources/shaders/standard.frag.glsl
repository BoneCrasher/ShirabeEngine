#version 440 core

layout (location=0)
in struct_Input
{
    vec3 vertex_position;
    vec3 vertex_normal;
    vec3 vertex_tangent;
    vec2 vertex_texcoord;
} Input;

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inputColor;
layout (input_attachment_index = 1, set = 1, binding = 1) uniform subpassInput inputDepth;

layout (location=0) out vec4 fragment_color_0;

void main()
{
    fragment_color_0 = 0.5 * (gl_FragCoord + 1.0);
} 
