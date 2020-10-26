struct VSOut
{
	float3 Color : Color;
	float4 pos : SV_POSITION;
};

VSOut main(float2 pos : Position, float3 c : Color)
{
	VSOut vsout;
	vsout.pos = float4(pos, 1.0f, 1.0f);
	vsout.Color = c;
	return vsout;
}