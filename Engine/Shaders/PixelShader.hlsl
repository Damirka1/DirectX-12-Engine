cbuffer C
{
    float3 color;
}

float4 main() : SV_TARGET
{
    return float4(color.xyz, 1.0f);
}