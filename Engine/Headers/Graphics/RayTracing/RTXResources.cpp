#include "RTXResources.h"

#include "nv_helpers_dx12/BottomLevelASGenerator.h"
#include "nv_helpers_dx12/DXRHelper.h"
#include "nv_helpers_dx12/RaytracingPipelineGenerator.h"
#include "nv_helpers_dx12/RootSignatureGenerator.h"

//#include "../Resources/DrawableMesh.h"
#include "../../Scene/Components/StaticMeshComponent.h"

#include "../../ResourceManager.h"

RTXResources::RTXResources(Graphics* pGraphics, ResourceManager* pRM)
{
	this->pGraphics = pGraphics;
	this->pRM = pRM;

	CBuffer b{};

	pConstBuffer = pRM->CreateConstBuffer(&b, sizeof(b), 0);
}

void RTXResources::PrepareModelForRtx(StaticMeshComponent* model, unsigned int hitGroup)
{
	RTResource resource;
	resource.Model = model;
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
	// TODO: make resource releasing
	//ReleaseStructures();

	CreateBottomLevelAS();
	CreateTopLevelAS();

	if (!pRtStateObject)
	{
		CreateRaytracingPipeline();
		CreateRaytracingOutputBuffer();
	}

	CreateShaderResourceHeap();
	CreateShaderBindingTable();

	NeedUpdate = false;
}

void RTXResources::EndInitialize()
{
	/*for (int i = 0; i < BlasInstances.size(); i++)
	{
		BlasInstances[i].Buffers.pScratch->Release();
		BlasInstances[i].Buffers.pScratch = nullptr;
	}*/
}

void RTXResources::CopyBuffer()
{
	pGraphics->CopyFromRenderTarget(pOutputResource);
}

void RTXResources::DispatchRays()
{
	auto commandList = pGraphics->GetCommandList();

	// Bind the descriptor heap giving access to the top-level acceleration
		// structure, as well as the raytracing output
	std::vector<ID3D12DescriptorHeap*> heaps = { pSrvUavHeap };
	commandList->SetDescriptorHeaps(static_cast<UINT>(heaps.size()), heaps.data());

	// On the last frame, the raytracing output was used as a copy source, to
	// copy its contents into the render target. Now we need to transition it to
	// a UAV so that the shaders can write in it.
	CD3DX12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(pOutputResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	commandList->ResourceBarrier(1, &transition);

	// Setup the raytracing task
	D3D12_DISPATCH_RAYS_DESC desc = {};
	// The layout of the SBT is as follows: ray generation shader, miss
	// shaders, hit groups. As described in the CreateShaderBindingTable method,
	// all SBT entries of a given type have the same size to allow a fixed
	// stride.

	// The ray generation shaders are always at the beginning of the SBT.
	uint32_t rayGenerationSectionSizeInBytes = SbtHelper.GetRayGenSectionSize();
	desc.RayGenerationShaderRecord.StartAddress = pSbtStorage->GetGPUVirtualAddress();
	desc.RayGenerationShaderRecord.SizeInBytes = rayGenerationSectionSizeInBytes;

	// The miss shaders are in the second SBT section, right after the ray
	// generation shader. We have one miss shader for the camera rays and one
	// for the shadow rays, so this section has a size of 2*m_sbtEntrySize. We
	// also indicate the stride between the two miss shaders, which is the size
	// of a SBT entry
	uint32_t missSectionSizeInBytes = SbtHelper.GetMissSectionSize();
	desc.MissShaderTable.StartAddress = pSbtStorage->GetGPUVirtualAddress() + rayGenerationSectionSizeInBytes;
	desc.MissShaderTable.SizeInBytes = missSectionSizeInBytes;
	desc.MissShaderTable.StrideInBytes = SbtHelper.GetMissEntrySize();

	// The hit groups section start after the miss shaders. In this sample we
	// have one 1 hit group for the triangle
	uint32_t hitGroupsSectionSize = SbtHelper.GetHitGroupSectionSize();
	desc.HitGroupTable.StartAddress = pSbtStorage->GetGPUVirtualAddress() + rayGenerationSectionSizeInBytes + missSectionSizeInBytes;
	desc.HitGroupTable.SizeInBytes = hitGroupsSectionSize;
	desc.HitGroupTable.StrideInBytes = SbtHelper.GetHitGroupEntrySize();

	auto res = pGraphics->GetResolution();

	// Dimensions of the image to render, identical to a kernel launch dimension
	desc.Width = res.first;
	desc.Height = res.second;
	desc.Depth = 1;

	// Bind the raytracing pipeline
	commandList->SetPipelineState1(pRtStateObject);
	// Dispatch the rays and write to the raytracing output
	commandList->DispatchRays(&desc);

	// The raytracing output needs to be copied to the actual render target used
	// for display. For this, we need to transition the raytracing output from a
	// UAV to a copy source, and the render target buffer to a copy destination.
	// We can then do the actual copy, before transitioning the render target
	// buffer into a render target, that will be then used to display the image
	transition = CD3DX12_RESOURCE_BARRIER::Transition(pOutputResource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	commandList->ResourceBarrier(1, &transition);

	pGraphics->CopyToRenderTarget(pOutputResource);

	transition = CD3DX12_RESOURCE_BARRIER::Transition(pOutputResource, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
	commandList->ResourceBarrier(1, &transition);
}

void RTXResources::Update(Camera* pCamera)
{
	UpdateTopLevelAS();

	CBuffer b;
	b.View = pCamera->GetView();
	b.Projection = pCamera->GetProjection();
	DirectX::XMVECTOR det;
	b.IView = DirectX::XMMatrixInverse(&det, b.View);
	b.IProjection = DirectX::XMMatrixInverse(&det, b.Projection);
	
	pConstBuffer->Update(&b, sizeof(b));
}


RTXResources::~RTXResources()
{
	ReleaseStructures();

	pRayGenLibrary->Release();
	pMissLibrary->Release();
	pHitLibrary->Release();

	pRayGenSignature->Release();
	pMissSignature->Release();
	pHitSignature->Release();

	pRtStateObjectProps->Release();
	pRtStateObject->Release();

	pOutputResource->Release();

	pSrvUavHeap->Release();

	pSbtStorage->Release();
}

void RTXResources::CreateBottomLevelAS()
{
	for (int i = 0; i < BlasInstances.size(); i++)
	{
		nv_helpers_dx12::BottomLevelASGenerator bottomLevelAS;

		for (int j = 0; j < BlasInstances[i].Model->Meshes.size(); j++)
		{
			std::shared_ptr<VertexBuffer> pVertexBuffer = BlasInstances[i].Model->Meshes[j].pVertexBuffer;
			std::shared_ptr<IndexBuffer> pIndexBuffer = BlasInstances[i].Model->Meshes[j].pIndexBuffer;

			// Adding all vertex buffers and not transforming their position.
			bottomLevelAS.AddVertexBuffer(pVertexBuffer->pBuffer, 0, pVertexBuffer->VertexCount, pVertexBuffer->Stride, pIndexBuffer->pBuffer, 0, pIndexBuffer->IndeciesCount, nullptr, 0);
		}

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
	nv_helpers_dx12::TopLevelASGenerator topLevelASGenerator;
	// Gather all the instances into the builder helper
	for (size_t i = 0; i < BlasInstances.size(); i++)
		topLevelASGenerator.AddInstance(BlasInstances[i].Buffers.pResult, BlasInstances[i].Model->GetPosMatrix(), static_cast<UINT>(i), static_cast<UINT>(BlasInstances[i].HitGroup));

	// As for the bottom-level AS, the building the AS requires some scratch space
	// to store temporary data in addition to the actual AS. In the case of the
	// top-level AS, the instance descriptors also need to be stored in GPU
	// memory. This call outputs the memory requirements for each (scratch,
	// results, instance descriptors) so that the application can allocate the
	// corresponding memory
	UINT64 scratchSize, resultSize, instanceDescsSize;

	topLevelASGenerator.ComputeASBufferSizes(pGraphics->GetDevice(), true, &scratchSize, &resultSize, &instanceDescsSize);

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
	topLevelASGenerator.Generate(pGraphics->GetCommandList(), TopLevelASBuffers.pScratch, TopLevelASBuffers.pResult, TopLevelASBuffers.pInstanceDesc);
	
	TopLevelASGenerator = std::move(topLevelASGenerator);
}

void RTXResources::UpdateTopLevelAS()
{
	TopLevelASGenerator.Generate(pGraphics->GetCommandList(), TopLevelASBuffers.pScratch, TopLevelASBuffers.pResult, TopLevelASBuffers.pInstanceDesc);
}

ID3D12RootSignature* RTXResources::CreateRayGenSignature()
{
	nv_helpers_dx12::RootSignatureGenerator rsc;
	rsc.AddHeapRangesParameter(
		{ {0 /*u0*/, 1 /*1 descriptor */, 0 /*use the implicit register space 0*/,
		  D3D12_DESCRIPTOR_RANGE_TYPE_UAV /* UAV representing the output buffer*/,
		  0 /*heap slot where the UAV is defined*/},
		 {0 /*t0*/, 1, 0,
		  D3D12_DESCRIPTOR_RANGE_TYPE_SRV /*Top-level acceleration structure*/,
		  1},
		{0 /*b0*/, 1, 0,
		  D3D12_DESCRIPTOR_RANGE_TYPE_CBV /* Const buffer */,
		  2} });

	return rsc.Generate(pGraphics->GetDevice(), true);
}

ID3D12RootSignature* RTXResources::CreateHitSignature()
{
	nv_helpers_dx12::RootSignatureGenerator rsc;
	return rsc.Generate(pGraphics->GetDevice(), true);
}

ID3D12RootSignature* RTXResources::CreateMissSignature()
{
	nv_helpers_dx12::RootSignatureGenerator rsc;
	return rsc.Generate(pGraphics->GetDevice(), true);
}

void RTXResources::CreateRaytracingPipeline()
{
	nv_helpers_dx12::RayTracingPipelineGenerator pipeline(pGraphics->GetDevice());

	// The pipeline contains the DXIL code of all the shaders potentially executed
	// during the raytracing process. This section compiles the HLSL code into a
	// set of DXIL libraries. We chose to separate the code in several libraries
	// by semantic (ray generation, hit, miss) for clarity. Any code layout can be
	// used.
	pRayGenLibrary = nv_helpers_dx12::CompileShaderLibrary(L"Shaders//RayGen.hlsl");
	pMissLibrary = nv_helpers_dx12::CompileShaderLibrary(L"Shaders//Miss.hlsl");
	pHitLibrary = nv_helpers_dx12::CompileShaderLibrary(L"Shaders//Hit.hlsl");

	// In a way similar to DLLs, each library is associated with a number of
	// exported symbols. This
	// has to be done explicitly in the lines below. Note that a single library
	// can contain an arbitrary number of symbols, whose semantic is given in HLSL
	// using the [shader("xxx")] syntax
	pipeline.AddLibrary(pRayGenLibrary, { L"RayGen" });
	pipeline.AddLibrary(pMissLibrary, { L"Miss" });
	pipeline.AddLibrary(pHitLibrary, { L"ClosestHit" });

	// To be used, each DX12 shader needs a root signature defining which
	// parameters and buffers will be accessed.
	pRayGenSignature = CreateRayGenSignature();
	pMissSignature = CreateMissSignature();
	pHitSignature = CreateHitSignature();

	// 3 different shaders can be invoked to obtain an intersection: an
	// intersection shader is called
	// when hitting the bounding box of non-triangular geometry. This is beyond
	// the scope of this tutorial. An any-hit shader is called on potential
	// intersections. This shader can, for example, perform alpha-testing and
	// discard some intersections. Finally, the closest-hit program is invoked on
	// the intersection point closest to the ray origin. Those 3 shaders are bound
	// together into a hit group.

	// Note that for triangular geometry the intersection shader is built-in. An
	// empty any-hit shader is also defined by default, so in our simple case each
	// hit group contains only the closest hit shader. Note that since the
	// exported symbols are defined above the shaders can be simply referred to by
	// name.

	// Hit group for the triangles, with a shader simply interpolating vertex
	// colors
	pipeline.AddHitGroup(L"HitGroup", L"ClosestHit");

	// The following section associates the root signature to each shader. Note
	// that we can explicitly show that some shaders share the same root signature
	// (eg. Miss and ShadowMiss). Note that the hit shaders are now only referred
	// to as hit groups, meaning that the underlying intersection, any-hit and
	// closest-hit shaders share the same root signature.
	pipeline.AddRootSignatureAssociation(pRayGenSignature, { L"RayGen" });
	pipeline.AddRootSignatureAssociation(pMissSignature, { L"Miss" });
	pipeline.AddRootSignatureAssociation(pHitSignature, { L"HitGroup" });

	// The payload size defines the maximum size of the data carried by the rays,
	// ie. the the data
	// exchanged between shaders, such as the HitInfo structure in the HLSL code.
	// It is important to keep this value as low as possible as a too high value
	// would result in unnecessary memory consumption and cache trashing.
	pipeline.SetMaxPayloadSize(4 * sizeof(float)); // RGB + distance

	// Upon hitting a surface, DXR can provide several attributes to the hit. In
	// our sample we just use the barycentric coordinates defined by the weights
	// u,v of the last two vertices of the triangle. The actual barycentrics can
	// be obtained using float3 barycentrics = float3(1.f-u-v, u, v);
	pipeline.SetMaxAttributeSize(2 * sizeof(float)); // barycentric coordinates

	// The raytracing process can shoot rays from existing hit points, resulting
	// in nested TraceRay calls. Our sample code traces only primary rays, which
	// then requires a trace depth of 1. Note that this recursion depth should be
	// kept to a minimum for best performance. Path tracing algorithms can be
	// easily flattened into a simple loop in the ray generation.
	pipeline.SetMaxRecursionDepth(1);

	// Compile the pipeline for execution on the GPU
	pRtStateObject = pipeline.Generate();

	// Cast the state object into a properties object, allowing to later access
	// the shader pointers by name
	Error_Check(pRtStateObject->QueryInterface(IID_PPV_ARGS(&pRtStateObjectProps)));
}

void RTXResources::CreateRaytracingOutputBuffer()
{
	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.DepthOrArraySize = 1;
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	// The backbuffer is actually DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, but sRGB
	// formats cannot be used with UAVs. For accuracy we should convert to sRGB
	// ourselves in the shader
	resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	auto res = pGraphics->GetResolution();

	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	resDesc.Width = res.first;
	resDesc.Height = res.second;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	Error_Check(
		pGraphics->GetDevice()->CreateCommittedResource(&nv_helpers_dx12::kDefaultHeapProps, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&pOutputResource))
	);
}

void RTXResources::CreateShaderResourceHeap()
{
	auto device = pGraphics->GetDevice();
	// Create a SRV/UAV/CBV descriptor heap. We need 2 entries - 1 UAV for the
	// raytracing output and 1 SRV for the TLAS
	pSrvUavHeap = nv_helpers_dx12::CreateDescriptorHeap(device, 3, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, true);

	// Get a handle to the heap memory on the CPU side, to be able to write the
	// descriptors directly
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = pSrvUavHeap->GetCPUDescriptorHandleForHeapStart();

	// Create the UAV. Based on the root signature we created it is the first
	// entry. The Create*View methods write the view information directly into
	// srvHandle
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	device->CreateUnorderedAccessView(pOutputResource, nullptr, &uavDesc, srvHandle);

	// Add the Top Level AS SRV right after the raytracing output buffer
	srvHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.RaytracingAccelerationStructure.Location = TopLevelASBuffers.pResult->GetGPUVirtualAddress();
	// Write the acceleration structure view in the heap
	device->CreateShaderResourceView(nullptr, &srvDesc, srvHandle);

	// Add the constant buffer for the camera after the TLAS
	srvHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// Describe and create a constant buffer view for the camera
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = pConstBuffer->pBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = pConstBuffer->DataSize;
	device->CreateConstantBufferView(&cbvDesc, srvHandle);
}

void RTXResources::CreateShaderBindingTable()
{
	// The SBT helper class collects calls to Add*Program.  If called several
	// times, the helper must be emptied before re-adding shaders.
	SbtHelper.Reset();

	// The pointer to the beginning of the heap is the only parameter required by
	// shaders without root parameters
	D3D12_GPU_DESCRIPTOR_HANDLE srvUavHeapHandle = pSrvUavHeap->GetGPUDescriptorHandleForHeapStart();

	// The helper treats both root parameter pointers and heap pointers as void*,
	// while DX12 uses the
	// D3D12_GPU_DESCRIPTOR_HANDLE to define heap pointers. The pointer in this
	// struct is a UINT64, which then has to be reinterpreted as a pointer.
	auto heapPointer = reinterpret_cast<UINT64*>(srvUavHeapHandle.ptr);

	// The ray generation only uses heap data
	SbtHelper.AddRayGenerationProgram(L"RayGen", { heapPointer });

	// The miss and hit shaders do not access any external resources: instead they
	// communicate their results through the ray payload
	SbtHelper.AddMissProgram(L"Miss", {});

	// Adding the triangle hit shader
	SbtHelper.AddHitGroup(L"HitGroup", {});

	// Compute the size of the SBT given the number of shaders and their
	// parameters
	uint32_t sbtSize = SbtHelper.ComputeSBTSize();

	// Create the SBT on the upload heap. This is required as the helper will use
	// mapping to write the SBT contents. After the SBT compilation it could be
	// copied to the default heap for performance.
	pSbtStorage = nv_helpers_dx12::CreateBuffer(pGraphics->GetDevice(), sbtSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_GENERIC_READ, nv_helpers_dx12::kUploadHeapProps);
	if (!pSbtStorage) {
		throw std::logic_error("Could not allocate the shader binding table");
	}
	// Compile the SBT from the shader and parameters info
	SbtHelper.Generate(pSbtStorage, pRtStateObjectProps);
}

void RTXResources::ReleaseStructures()
{
	for (int i = 0; i < BlasInstances.size(); i++)
	{
		if (BlasInstances[i].Buffers.pResult)
		{
			BlasInstances[i].Buffers.pResult->Release();
			BlasInstances[i].Buffers.pResult = nullptr;
			BlasInstances[i].Buffers.pScratch->Release();
			BlasInstances[i].Buffers.pScratch = nullptr;
		}
	}

	if (TopLevelASBuffers.pResult)
	{
		TopLevelASBuffers.pResult->Release();
		TopLevelASBuffers.pResult = nullptr;

		TopLevelASBuffers.pScratch->Release();
		TopLevelASBuffers.pScratch = nullptr;

		TopLevelASBuffers.pInstanceDesc->Release();
		TopLevelASBuffers.pInstanceDesc = nullptr;
	}
}
