
Texture2D tex;
SamplerState splr;

float4 main(float2 Tex : Textures) : SV_TARGET
{
    return tex.Sample(splr, Tex);
}