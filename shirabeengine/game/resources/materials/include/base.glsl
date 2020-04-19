#extension GL_ARB_separate_shader_objects : enable

struct struct_vertexData_base
{
    vec3 vertex_position;
    vec3 vertex_color;
};

struct struct_vertexData_extended
{
    vec3 vertex_position;
    vec3 vertex_normal;
    vec3 vertex_texcoord;
};

struct struct_vertexData_full
{
    vec4 vertex_color;
    vec3 vertex_position;
    vec3 vertex_normal;
    vec3 vertex_tangent;
    vec3 vertex_bitangent;
    vec2 vertex_texcoord;
};

struct struct_globalData
{
    float time;
};

struct struct_cameraMatrices
{
    mat4 view;
    mat4 projection;
};

struct struct_modelMatrices_default
{
    mat4 world;
    mat4 inverseTransposeWorld;
};

#define SHIRABE_MAXNUM_SKIN_MATRICES 72
struct struct_modelMatrices_skin
{
    mat4 skin[SHIRABE_MAXNUM_SKIN_MATRICES];
};

struct struct_modelMatrices_animation
{
    mat4 animationState[SHIRABE_MAXNUM_SKIN_MATRICES];
};

//
// Set 0: Global Application defined data
//
layout (std140, set = 0, binding = 0)
uniform struct_systemData
{
    struct_globalData global;
} systemData;

//
// Uniform Buffer to hold the camera matrices
//
layout (std140, set = 1, binding = 0)
uniform struct_graphicsData
{
    struct_cameraMatrices primaryCamera;
} graphicsData;

//
// Set 1: Reserved for future use
//
layout (std140, set = 1, binding = 1)
uniform Reserved
{
    float  _UNUSED;
    vec2   _UNUSED2;
    vec3   _UNUSED3;
    vec4   _UNUSED4;
    mat4   _UNUSED5;
}
reserved;

vec3 unpack_normal(vec3 aInput)
{
    return ((2.0f * aInput) - 1.0f);
}
