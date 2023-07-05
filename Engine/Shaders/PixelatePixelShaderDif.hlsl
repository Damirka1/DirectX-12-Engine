
Texture2D tex;
SamplerState splr;

float4 Pixelate(float2 uv)
{
    uint2 var = uint2(uv.x * 100, uv.y * 100);
    uv = float2((float)var.x / 100, (float)var.y / 100);

    return tex.Sample(splr, uv);
}

float4 main(float2 Tex : Textures) : SV_TARGET
{
    return Pixelate(Tex);
}