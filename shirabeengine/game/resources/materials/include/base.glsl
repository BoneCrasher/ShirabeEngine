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

struct struct_timingData
{
    float timeElapsedTotalSeconds;
    float timeElapsedSinceLastFrameSeconds;
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

layout (constant_id = 0) const int SHIRABE_MAXNUM_SKIN_MATRICES = 72;
struct struct_modelMatrices_skin
{
    mat4 skin[SHIRABE_MAXNUM_SKIN_MATRICES];
};

struct struct_modelMatrices_animation
{
    mat4 animationState[SHIRABE_MAXNUM_SKIN_MATRICES];
};

// @nnotation:SHIRABE_SHARING_MODE(buffer_id="globalData", mode="global")
layout (std140, set = 0, binding = 0)
uniform GlobalDataBuffer
{
    struct_timingData data;

    int cameraIndex;
}
globalData;

// @nnotation:SHIRABE_SHARING_MODE(buffer_id="graphicsData", mode="global")
layout (constant_id = 1) const int SHIRABE_MAXNUM_CAMERAS = 64;
layout (std140, set = 0, binding = 1)
readonly buffer SceneDataStorage
{
    struct_cameraMatrices cameras[SHIRABE_MAXNUM_CAMERAS];
} 
sceneData;

// @nnotation:SHIRABE_SHARING_MODE(buffer_id="transforms", mode="global")
layout (constant_id = 2) const int SHIRABE_MAXNUM_OBJECT_TRANSFORMS = 2048;
layout (std140, set = 0, binding = 2)
readonly buffer ObjectTransformStorage
{
    mat4 storage[SHIRABE_MAXNUM_OBJECT_TRANSFORMS];
}
transforms;

// @nnotation:SHIRABE_SHARING_MODE(buffer_id="bindposes", mode="global")
layout (constant_id = 3) const int SHIRABE_MAXNUM_BINDPOSE_TRANSFORMS = 2048;
layout (std140, set = 0, binding = 3)
readonly buffer BindPoseTransformStorage
{
    mat4 storage[SHIRABE_MAXNUM_BINDPOSE_TRANSFORMS];
}
bindposes;

// @nnotation:SHIRABE_SHARING_MODE(buffer_id="animations", mode="global")
layout (constant_id = 4) const int SHIRABE_MAXNUM_ANIMATION_TRANSFORMS = 2048;
layout (std140, set = 0, binding = 4)
readonly buffer AnimationTransformStorage
{
    mat4 storage[SHIRABE_MAXNUM_ANIMATION_TRANSFORMS];
}
animations;

layout (constant_id = 5) const int SHIRABE_MAXNUM_TEXTURES = 256;
layout (set = 0, binding = 5)
uniform sampler2D textures[SHIRABE_MAXNUM_TEXTURES];

vec3 unpack_normal(vec3 aInput)
{
    return ((2.0f * aInput) - 1.0f);
}
