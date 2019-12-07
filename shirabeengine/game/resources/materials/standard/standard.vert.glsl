#version 450 core
#extension GL_GOOGLE_include_directive : require
#include "base.glsl"

//
// Model specific matrices.
//
layout (std140, set = 2, binding = 0)
uniform struct_modelMatrices
{
    mat4  world;
    mat4  inverseTransposeWorld;
    float scale[2];
} modelMatrices;

struct test0
{
    mat4 mat00;
    mat4 mat01;
};
struct test1
{
    mat4 mats[10];
};
struct test2
{
    mat4 mat00;
    vec3 vec00;
    vec2 vec05;
    test1 test;
};
struct test3
{
    mat4 mat00;
    vec2 vec00;
};

layout (std140, set = 2, binding = 1)
uniform struct_structureTest0
{
    test0 test;
} structureTest0;

layout (std140, set = 2, binding = 2)
uniform struct_structureTest1
{
    test1 test;
} structureTest1;

layout (std140, set = 2, binding = 3)
uniform struct_structureTest3
{
    test2 test;
    test1 test1;
} structureTest3;

layout (std140, set = 2, binding = 4)
uniform struct_structureTest4
{
    test2 test[5];
} structureTest4;

layout (std140, set = 2, binding = 5)
uniform struct_structureTest5
{
    vec4  vec00;
    test2 test;
    vec3  vec01;
} structureTest5;

layout (std140, set = 2, binding = 6)
uniform struct_structureTest6
{
    test3 test00[7];
} structuretest6;

layout (std140, set = 2, binding = 7)
uniform struct_structureTest7
{
    mat4 mat;
} structureTest7[6];

//
// Input description
//
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec3 vertex_tangent;
layout (location = 3) in vec2 vertex_texcoord;

//
// Vertex shader output
//
layout (location = 0)
out struct_vertexData_full shader_output;

void main()
{
    vec4 position = vec4(0.0, 0.0, 0.0, 0.0);
    switch(gl_VertexIndex)
    {
        case 0:
        position = vec4(0.0, -0.5, 0.0, 1.0); // vec4(vertex_position, 1.0);
        shader_output.vertex_color = vec3(1.0f, 0.0f, 0.0f);
        break;
        case 1:
        position = vec4(0.5, 0.5, 0.0, 1.0); // vec4(vertex_position, 1.0);
        shader_output.vertex_color = vec3(0.0f, 1.0f, 0.0f);
        break;
        case 2:
        position = vec4(-0.5, 0.5, 0.0, 1.0); // vec4(vertex_position, 1.0);
        shader_output.vertex_color = vec3(0.0f, 0.0f, 1.0f);
        break;
    }

    position.x *= modelMatrices.scale[0];
    position.y *= modelMatrices.scale[1];
    gl_Position = position;

    shader_output.vertex_position = gl_Position.xyz;
    shader_output.vertex_normal   = vec3(1.0, 0.0,  0.0);
    shader_output.vertex_tangent  = vec3(0.0, 0.0, -1.0);
    shader_output.vertex_texcoord = vec2(gl_Position.xy);

}
