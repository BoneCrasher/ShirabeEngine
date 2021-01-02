#version 450 core

// PushConstants
// @nnotation:SHIRABE_SHARING_MODE(buffer_id="animations", mode="global")
layout (push_constant)
uniform PushConstantStorage
{
    int instanceIndex;
} pushConstantStorage;

struct struct_transformIndices 
{
    int world;
    int invTransposeWorld;
};

struct struct_textureIndices
{   
    int diffuse;
    int specular;
    int normal;
};

struct struct_instanceData
{
    struct_transformIndices transforms;
    struct_textureIndices   textures;
};

// @nnotation:SHIRABE_SHARING_MODE(buffer_id="instanceData", mode="instance")
layout (constant_id = 20) const int SHIRABE_STANDARD_MAX_INSTANCES = 8;
layout (std140, set=2, binding=0)
readonly buffer InstanceData
{
    struct_instanceData data[SHIRABE_STANDARD_MAX_INSTANCES];
} instanceData;
