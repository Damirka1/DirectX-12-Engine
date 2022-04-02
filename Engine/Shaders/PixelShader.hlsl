cbuffer C
{
    float3 color;
}

float4 main() : SV_TARGET
{
    return float4(0.5, 0.5, 0.5, 1.0f);
}