struct VSOut
{
	float3 Color : Color;
	float4 pos : SV_POSITION;
};

cbuffer C {
    float3 Color;
}

VSOut main(float2 pos : Position)
{
	VSOut vsout;
	vsout.pos = float4(pos, 1.0f, 1.0f);
	vsout.Color = Color;
	return vsout;
}