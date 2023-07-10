#include "Common.hlsl"

[shader("miss")]
void Miss(inout HitInfo payload : SV_RayPayload)
{
    payload.colorAndDistance = float4(0, 0, 0, -1.f);
}