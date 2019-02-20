#version 440 core

// Vertex Layout
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec3 vertex_tangent;
layout (location = 3) in vec2 vertex_texcoord;

layout (location=0)
out struct_Output
{
    vec3 vertex_position;
    vec3 vertex_normal;
    vec3 vertex_tangent;
    vec2 vertex_texcoord;
} Output;

void main()
{
    gl_Position = vec4(vertex_position, 1.0);
}
