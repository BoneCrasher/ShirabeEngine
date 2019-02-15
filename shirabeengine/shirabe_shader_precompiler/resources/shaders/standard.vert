#version 440 core
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec3 vertex_tangent;
layout (location = 3) in vec3 vertex_texcoord;
  
out struct_Output
{

} Output;

void main()
{
    gl_Position = vec4(vertex_position, 1.0);
}
