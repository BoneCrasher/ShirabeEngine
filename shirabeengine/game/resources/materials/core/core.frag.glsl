#version 450 core
#extension GL_GOOGLE_include_directive : require
#include "base.glsl"

// Input
layout (location = 0)
in struct_vertexData_full shader_input;

// Output
layout (location = 0)
out vec4 fragment_color_0;

void main()
{
    fragment_color_0 = vec4(1.0, 1.0, 1.0, 1.0);
}
