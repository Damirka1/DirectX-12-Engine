struct VSOut
{
	float4 pos : SV_POSITION;
};

cbuffer C
{
    matrix Projection;
    matrix View;
    matrix Pos;
}

float4 main(float3 pos : Position) : SV_POSITION
{
    return mul(mul(mul(float4(pos, 1.0f), Pos), View), Projection);
}