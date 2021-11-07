cbuffer C
{
    matrix PosViewProj;
}

float4 main(float3 pos : Position, float3 norm : NormCoord) : SV_Position
{
    return mul(float4(pos, 1.0f), PosViewProj);
}