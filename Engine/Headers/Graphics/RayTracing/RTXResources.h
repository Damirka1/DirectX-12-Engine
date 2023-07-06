#pragma once

#include <d3d12.h>
#include <dxcapi.h>
#include <memory>

#include "nv_helpers_dx12/TopLevelASGenerator.h"
#include "../Resources/Buffers/Buffers.h"
#include "nv_helpers_dx12/ShaderBindingTableGenerator.h"

class DrawableMesh;

class RTXResources 
{
	friend class ResourceManager;
	struct AccelerationStructureBuffers
	{
		ID3D12Resource* pScratch;      // Scratch memory for AS builder
		ID3D12Resource* pResult;       // Where the AS is
		ID3D12Resource* pInstanceDesc; // Hold the matrices of the instances
	};

	struct RTResource 
	{
		DrawableMesh* Mesh;
		AccelerationStructureBuffers Buffers;
		unsigned int InstanceId;
		unsigned int HitGroup;
	};

public:

	RTXResources(Graphics* pGraphics);

	void PrepareMeshForRtx(DrawableMesh* mesh, unsigned int hitGroup);

	bool IsNeedUpdate();

	void StartInitialize();

	void EndInitialize();

	~RTXResources();

private:

	bool NeedUpdate = false;

	Graphics* pGraphics;

	std::vector<RTResource> BlasInstances;

	void CreateBottomLevelAS();

	/// Create the main acceleration structure that holds
	/// all instances of the scene
	/// \param     instances : pair of BLAS and transform
	void CreateTopLevelAS();

	ID3D12RootSignature* CreateRayGenSignature();
	ID3D12RootSignature* CreateHitSignature();
	ID3D12RootSignature* CreateMissSignature();
	
	AccelerationStructureBuffers TopLevelASBuffers;

	IDxcBlob* pRayGenLibrary;
	IDxcBlob* pHitLibrary;
	IDxcBlob* pMissLibrary;

	ID3D12RootSignature* pRayGenSignature;
	ID3D12RootSignature* pHitSignature;
	ID3D12RootSignature* pMissSignature;
					   
	// Ray tracing pipeline state
	ID3D12StateObject* pRtStateObject;

	// Ray tracing pipeline state properties, retaining the shader identifiers
	// to use in the Shader Binding Table
	ID3D12StateObjectProperties* pRtStateObjectProps;
	
	void CreateRaytracingPipeline();
	void CreateRaytracingOutputBuffer();
	void CreateShaderResourceHeap();

	ID3D12Resource* pOutputResource;
	ID3D12DescriptorHeap* pSrvUavHeap;

	void CreateShaderBindingTable();
	nv_helpers_dx12::ShaderBindingTableGenerator pSbtHelper;
	ID3D12Resource* pSbtStorage;

	void ReleaseStructures();
};