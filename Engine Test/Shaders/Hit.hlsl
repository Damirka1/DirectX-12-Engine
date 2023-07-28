#include "Common.hlsl"

//// Raytracing acceleration structure, accessed as a SRV
//RaytracingAccelerationStructure SceneBVH : register(t0);

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

struct VertexLayout
{
    float3 vertex;
    float3 normal;
    float3 tan;
    float3 bitan;
    float2 tex;
};

StructuredBuffer<VertexLayout> VertexBuffer : register(t0);

StructuredBuffer<uint> IndexBuffer : register(t1);

cbuffer ConstBuffer : register(b0)
{
    float3 color;
}

[shader("closesthit")]
void ClosestHit(inout HitInfo payload, Attributes attrib)
{
    float3 barycentrics = float3(1.f - attrib.bary.x - attrib.bary.y, attrib.bary.x, attrib.bary.y);
    
    uint vertId = IndexBuffer[PrimitiveIndex() * 3];
    
    VertexLayout v = VertexBuffer[vertId];
    
    float3 localVertex = v.vertex * barycentrics.x + v.vertex * barycentrics.y + v.vertex * barycentrics.z;
    float3 localNormal = v.normal * barycentrics.x + v.normal * barycentrics.y + v.normal * barycentrics.z;
    
    payload.vertex = mul(localVertex, ObjectToWorld3x4());
    payload.normal = mul(localNormal, ObjectToWorld3x4());
    
    payload.hitOrigin = WorldRayOrigin() + RayTCurrent() * WorldRayDirection();
    payload.colorAndDistance = float4(color, RayTCurrent());
}