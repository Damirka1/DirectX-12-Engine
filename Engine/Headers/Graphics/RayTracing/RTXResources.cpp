#include "RTXResources.h"

#include "nv_helpers_dx12/BottomLevelASGenerator.h"
#include "nv_helpers_dx12/DXRHelper.h"

#include "../Resources/DrawableMesh.h"

RTXResources::RTXResources(Graphics* pGraphics)
{
	this->pGraphics = pGraphics;
}

void RTXResources::PrepareMeshForRtx(DrawableMesh* mesh, unsigned int hitGroup)
{
	RTResource resource;
	resource.Mesh = mesh;
	resource.Buffers = { 0 };
	resource.InstanceId = BlasInstances.size();
	resource.HitGroup = hitGroup;

	BlasInstances.push_back(resource);

	NeedUpdate = true;
}

bool RTXResources::IsNeedUpdate()
{
	return NeedUpdate;
}

void RTXResources::StartInitialize()
{
	CreateBottomLevelAS();
}

void RTXResources::EndInitialize()
{
	for (int i = 0; i < BlasInstances.size(); i++)
		BlasInstances[i].Buffers.pScratch->Release();

	NeedUpdate = false;
}



RTXResources::~RTXResources()
{
	for (int i = 0; i < BlasInstances.size(); i++)
		BlasInstances[i].Buffers.pResult->Release();
}

void RTXResources::CreateBottomLevelAS()
{
	for (int i = 0; i < BlasInstances.size(); i++)
	{
		DrawableMesh* mesh = BlasInstances[i].Mesh;

		std::shared_ptr<VertexBuffer> pVertexBuffer = mesh->pVertexBuffer;
		std::shared_ptr<IndexBuffer> pIndexBuffer = mesh->pIndexBuffer;

		nv_helpers_dx12::BottomLevelASGenerator bottomLevelAS;

		// Adding all vertex buffers and not transforming their position.
		bottomLevelAS.AddVertexBuffer(pVertexBuffer->pBuffer, 0, pVertexBuffer->VertexCount, pVertexBuffer->Stride, pIndexBuffer->pBuffer, 0, pIndexBuffer->IndeciesCount, nullptr, 0);

		// The AS build requires some scratch space to store temporary information.
		// The amount of scratch memory is dependent on the scene complexity.
		UINT64 scratchSizeInBytes = 0;
		// The final AS also needs to be stored in addition to the existing vertex
		// buffers. It size is also dependent on the scene complexity.
		UINT64 resultSizeInBytes = 0;

		bottomLevelAS.ComputeASBufferSizes(pGraphics->GetDevice(), false, &scratchSizeInBytes,
			&resultSizeInBytes);

		// Once the sizes are obtained, the application is responsible for allocating
		// the necessary buffers. Since the entire generation will be done on the GPU,
		// we can directly allocate those on the default heap
		AccelerationStructureBuffers buffers;
		buffers.pScratch = nv_helpers_dx12::CreateBuffer(pGraphics->GetDevice(), scratchSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON, nv_helpers_dx12::kDefaultHeapProps);
		buffers.pResult = nv_helpers_dx12::CreateBuffer(pGraphics->GetDevice(), resultSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, nv_helpers_dx12::kDefaultHeapProps);

		// Build the acceleration structure. Note that this call integrates a barrier
		// on the generated AS, so that it can be used to compute a top-level AS right
		// after this method.
		bottomLevelAS.Generate(pGraphics->GetCommandList(), buffers.pScratch, buffers.pResult, false, nullptr);

		BlasInstances[i].Buffers = buffers;
	}
}

void RTXResources::CreateTopLevelAS()
{
	// Gather all the instances into the builder helper
	for (size_t i = 0; i < BlasInstances.size(); i++)
		TopLevelASGenerator.AddInstance(BlasInstances[i].Buffers.pResult, *BlasInstances[i].Mesh->transfrom, static_cast<UINT>(i), static_cast<UINT>(BlasInstances[i].HitGroup));

	// As for the bottom-level AS, the building the AS requires some scratch space
	// to store temporary data in addition to the actual AS. In the case of the
	// top-level AS, the instance descriptors also need to be stored in GPU
	// memory. This call outputs the memory requirements for each (scratch,
	// results, instance descriptors) so that the application can allocate the
	// corresponding memory
	UINT64 scratchSize, resultSize, instanceDescsSize;

	TopLevelASGenerator.ComputeASBufferSizes(pGraphics->GetDevice(), true, &scratchSize, &resultSize, &instanceDescsSize);

	// Create the scratch and result buffers. Since the build is all done on GPU,
	// those can be allocated on the default heap
	TopLevelASBuffers.pScratch = nv_helpers_dx12::CreateBuffer(pGraphics->GetDevice(), scratchSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nv_helpers_dx12::kDefaultHeapProps);
	TopLevelASBuffers.pResult = nv_helpers_dx12::CreateBuffer(pGraphics->GetDevice(), resultSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, nv_helpers_dx12::kDefaultHeapProps);

	// The buffer describing the instances: ID, shader binding information,
	// matrices ... Those will be copied into the buffer by the helper through
	// mapping, so the buffer has to be allocated on the upload heap.
	TopLevelASBuffers.pInstanceDesc = nv_helpers_dx12::CreateBuffer(pGraphics->GetDevice(), instanceDescsSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, nv_helpers_dx12::kUploadHeapProps);

	// After all the buffers are allocated, or if only an update is required, we
	// can build the acceleration structure. Note that in the case of the update
	// we also pass the existing AS as the 'previous' AS, so that it can be
	// refitted in place.
	TopLevelASGenerator.Generate(pGraphics->GetCommandList(), TopLevelASBuffers.pScratch, TopLevelASBuffers.pResult, TopLevelASBuffers.pInstanceDesc);

}
