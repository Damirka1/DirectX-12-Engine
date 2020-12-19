cbuffer C
{
    matrix PosViewProj;
}


float4 main(float2 pos : Position) : SV_POSITION
{
    //return float4(pos, 0.0f, 1.0f);
    return mul(float4(pos, 0.0f, 1.0f), PosViewProj);
}