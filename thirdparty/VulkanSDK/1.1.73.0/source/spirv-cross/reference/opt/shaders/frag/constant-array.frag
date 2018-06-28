#version 310 es
precision mediump float;
precision highp int;

struct Foobar
{
    float a;
    float b;
};

layout(location = 0) out vec4 FragColor;
layout(location = 0) flat in mediump int index;

void main()
{
    highp vec4 indexable[3] = vec4[](vec4(1.0), vec4(2.0), vec4(3.0));
    highp vec4 indexable_1[2][2] = vec4[][](vec4[](vec4(1.0), vec4(2.0)), vec4[](vec4(8.0), vec4(10.0)));
    Foobar indexable_2[2] = Foobar[](Foobar(10.0, 40.0), Foobar(90.0, 70.0));
    FragColor = ((indexable[index] + (indexable_1[index][index + 1])) + vec4(30.0)) + vec4(indexable_2[index].a + indexable_2[index].b);
}

