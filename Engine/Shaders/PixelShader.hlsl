Texture2D tex;
SamplerState smp;

float4 main(float2 UV : UV) : SV_TARGET
{
    return tex.Sample(smp, UV);
}