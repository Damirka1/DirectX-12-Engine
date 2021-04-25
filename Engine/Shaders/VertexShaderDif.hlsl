cbuffer C
{
    matrix PosViewProj;
}

struct VSOUT
{
    float2 Tex : Textures;
    float4 Pos : SV_Position;
};

VSOUT main(float3 pos : Position, float3 norm : NormCoord, float2 tex : TexCoord)
{
    VSOUT VSout;
    VSout.Tex = tex;
    VSout.Pos = mul(float4(pos, 1.0f), PosViewProj);
    
    return VSout;
}