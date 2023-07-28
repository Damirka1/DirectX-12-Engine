#include "Common.hlsl"

//// Raytracing acceleration structure, accessed as a SRV
//RaytracingAccelerationStructure SceneBVH : register(t0);

cbuffer C : register(b0)
{
    float3 color;
}

//[shader("closesthit")]
//void ClosestHit(inout HitInfo payload, Attributes attrib)
//{
//    if (payload.colorAndDistance.w >= 1)
//    {
//        float3 lightPos[] = { float3(-10, 10, -10), float3(10, 10, 10) };
//        // Find the world - space hit position 
//        float3 worldOrigin = WorldRayOrigin() + RayTCurrent() * WorldRayDirection();
//        for (int i = 0; i < 2; i++) 
//        {
//            float3 vecToLight = lightPos[i] - worldOrigin;
//            float3 lightDir = normalize(vecToLight);
//            // Fire a shadow ray. The direction is hard-coded here, but can be fetched 
//            // from a constant-buffer 
//            RayDesc ray;
//            ray.Origin = worldOrigin;
//            ray.Direction = lightDir;
//            ray.TMin = 0.01;
//            ray.TMax = 100000;
//            // Initialize the ray payload 
//            HitInfo shadowPayload;
//            shadowPayload.colorAndDistance = float4(0, 0, 0, -1);
//            // Trace the ray 
//            TraceRay(
//                SceneBVH,
//                RAY_FLAG_NONE,
//                0xFF,
//                0,
//                0,
//                0,
//                ray,
//                shadowPayload);
//
//            if (shadowPayload.colorAndDistance.w > -1)
//            {
//                payload.colorAndDistance.rgb *= float3(0.2f, 0.2f, 0.2f);
//                payload.colorAndDistance.w = -2;
//            }
//
//            else
//            {
//                payload.colorAndDistance.rgb *= float3(1.2f, 1.2f, 1.2f);
//            }
//        }
//    }
//    else
//        payload.colorAndDistance = float4(1, 1, 1, RayTCurrent());
//}

[shader("closesthit")]
void ClosestHit(inout HitInfo payload, Attributes attrib)
{
    payload.colorAndDistance = float4(color, RayTCurrent());
    //payload.colorAndDistance = float4(1,1,1, RayTCurrent());
}