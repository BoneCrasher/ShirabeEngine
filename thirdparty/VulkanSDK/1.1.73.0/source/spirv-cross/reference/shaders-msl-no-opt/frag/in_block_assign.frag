#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct VOUT
{
    float4 a;
};

struct main0_in
{
    float4 VOUT_a [[user(locn0)]];
};

struct main0_out
{
    float4 FragColor [[color(0)]];
};

fragment main0_out main0(main0_in in [[stage_in]])
{
    main0_out out = {};
    VOUT tmp;
    tmp.a = in.VOUT_a;
    tmp.a += float4(1.0);
    out.FragColor = tmp.a;
    return out;
}

