struct VSOut
{
	float2 UV : UV;
	float4 pos : SV_POSITION;
};

cbuffer C
{
    matrix Pos;
}

VSOut main(float3 pos : Position, float2 UV : TexCoord)
{
	VSOut vsout;
    vsout.pos = mul(float4(pos, 1.0f), Pos);
    vsout.UV = UV;
	return vsout;
}