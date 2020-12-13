cbuffer C
{
    matrix Pos;
    matrix Projection;
}



float4 main(float2 pos : Position) : SV_POSITION
{
    //return float4(pos, 0.0f, 1.0f);
    return mul(mul(float4(pos, 0.0f, 1.0f), Pos), Projection);
}