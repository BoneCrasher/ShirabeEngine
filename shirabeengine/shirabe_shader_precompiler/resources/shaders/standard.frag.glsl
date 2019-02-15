#version 440 core

in struct_Input
{

} Input;

void main()
{
    gl_FragColor = 0.5 * (gl_Position + 1.0);
} 
