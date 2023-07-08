#include "Common.hlsl"

// Raytracing acceleration structure, accessed as a SRV
RaytracingAccelerationStructure SceneBVH : register(t0);

[shader("closesthit")]
void ClosestHit(inout HitInfo payload, Attributes attrib)
{
    if (payload.colorAndDistance.w >= 1)
    {
        float3 lightPos = float3(20, 0, -2);
        // Find the world - space hit position 
        float3 worldOrigin = WorldRayOrigin() + RayTCurrent() * WorldRayDirection();
        float3 lightDir = normalize(lightPos - worldOrigin);
        // Fire a shadow ray. The direction is hard-coded here, but can be fetched 
        // from a constant-buffer 
        RayDesc ray;
        ray.Origin = worldOrigin;
        ray.Direction = lightDir;
        ray.TMin = 0.01;
        ray.TMax = 100000;
        // Initialize the ray payload 
        HitInfo shadowPayload;
        shadowPayload.colorAndDistance = float4(0,0,0, RayTCurrent());
        // Trace the ray 
        TraceRay(
            SceneBVH,
            RAY_FLAG_NONE,
            0xFF,
            0,
            0,
            0,
            ray,
            shadowPayload);

        if (shadowPayload.colorAndDistance.w > -1)
        {
            payload.colorAndDistance = float4(0,0,0, RayTCurrent());
        }
        else
        {
            payload.colorAndDistance = float4(shadowPayload.colorAndDistance.rgb, RayTCurrent());
        }

    }
	else
        payload.colorAndDistance = float4(0,0,0, RayTCurrent());
}