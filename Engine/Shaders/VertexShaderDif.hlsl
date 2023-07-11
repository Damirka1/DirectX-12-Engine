cbuffer C :  register(b0)
{
    float4x4 Pos;
    float4x4 View;
    float4x4 Proj;
    float4x4 PosViewProj;
    float3 ViewPos;
}

struct VSOUT
{
    float4 Pos : SV_Position;
    float3 PsPos : Positions;
    float3 Norm : NormCoord;
    float3 ViewPos: ViewPos;
    float2 Tex : Textures;
};

struct VSIN
{
    float3 Pos : Position;
    float3 Norm : NormCoord;
    float3 Tang : TanCoord;
    float3 Bitang : BiTanCoord;
    float2 Tex : TexCoord;
};

VSOUT main(VSIN vsin)
{
    VSOUT vs;
    vs.Pos = mul(float4(vsin.Pos, 1.0f), PosViewProj);
    vs.PsPos = mul(float4(vsin.Pos, 1.0f), Pos);
    vs.Norm = mul(vsin.Norm, Pos);
    vs.ViewPos = ViewPos;
    vs.Tex = vsin.Tex;

    return vs;
}