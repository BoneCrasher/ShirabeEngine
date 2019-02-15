#version 440 core

layout (location=0)
in struct_Input
{
    vec3 vertex_position;
    vec3 vertex_normal;
    vec3 vertex_tangent;
    vec2 vertex_texcoord;
} Input;

layout (location=0) out vec4 fragment_color_0;

void main()
{
    fragment_color_0 = 0.5 * (gl_FragCoord + 1.0);
} 
