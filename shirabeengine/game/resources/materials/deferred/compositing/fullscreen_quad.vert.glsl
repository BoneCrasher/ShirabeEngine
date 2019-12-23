#version 450 core
#extension GL_GOOGLE_include_directive : require
#include "base.glsl"

// layout (location=0)
// out vec2 texcoord;

void main()
{
    switch(gl_VertexIndex)
    {
        case 0:
            gl_Position = vec4(-1.0, -1.0, 0.0, 1.0);
            //texcoord    = gl_Position.xy;
            break;
        case 1:
        case 4:
            gl_Position = vec4(-1.0, 1.0, 0.0, 1.0);
            //texcoord    = gl_Position.xy;
            break;
        case 2:
        case 3:
            gl_Position = vec4(1.0, -1.0, 0.0, 1.0);
            //texcoord    = gl_Position.xy;
            break;
        case 5:
            gl_Position = vec4(1.0, 1.0, 0.0, 1.0);
           // texcoord    = gl_Position.xy;
            break;
    }
}
