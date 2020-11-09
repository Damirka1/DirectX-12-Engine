struct VSOut
{
	float3 Color : Color;
	float4 pos : SV_POSITION;
};

cbuffer C {
    float4 Color;
    matrix Pos;
}

VSOut main(float2 pos : Position)
{
	VSOut vsout;
    vsout.pos = mul(float4(pos, 1.0f, 1.0f), Pos);
    vsout.Color = float3(Color.xyz);
	return vsout;
}