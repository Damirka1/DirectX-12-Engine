#pragma once

#include <d3d12.h>
#include <dxcapi.h>
#include <memory>
#include <unordered_map>

#include "nv_helpers_dx12/TopLevelASGenerator.h"
#include "../Resources/Buffers/Buffers.h"
#include "nv_helpers_dx12/ShaderBindingTableGenerator.h"

#include "../../Timer.h"

class DrawableMesh;
class Camera;

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
		std::vector<DrawableMesh*> meshes;
		AccelerationStructureBuffers Buffers;
		//unsigned int InstanceId;
		unsigned int HitGroup;
	};

	struct CBuffer 
	{
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
		DirectX::XMMATRIX IView;
		DirectX::XMMATRIX IProjection;
		DirectX::XMFLOAT3 LightPos;
	};

public:

	RTXResources(Graphics* pGraphics, ResourceManager* pRM);

	void PrepareMeshForRtx(DrawableMesh* mesh, unsigned int hitGroup);

	bool IsNeedUpdate();

	bool IsNeedRelease();

	void Initialize();

	void ReleaseScratch();

	void CopyBuffer();

	void DispatchRays();

	void Update(Camera* pCamera);

	~RTXResources();

private:
	Timer timer;
	DirectX::XMFLOAT3 LightPos = { 20, 0, -2 };

	bool NeedUpdate = false;
	bool NeedRelease = false;

	Graphics* pGraphics;
	ResourceManager* pRM;

	std::shared_ptr<ConstantBuffer> pConstBuffer;

	std::unordered_map<std::string, std::shared_ptr<RTResource>> BlasResources;

	void CreateBottomLevelAS();

	/// Create the main acceleration structure that holds
	/// all instances of the scene
	/// \param     instances : pair of BLAS and transform
	void CreateTopLevelAS();

	void UpdateTopLevelAS();

	ID3D12RootSignature* CreateRayGenSignature();
	ID3D12RootSignature* CreateHitSignature();
	ID3D12RootSignature* CreateMissSignature();
	
	nv_helpers_dx12::TopLevelASGenerator TopLevelASGenerator;
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
	nv_helpers_dx12::ShaderBindingTableGenerator SbtHelper;
	ID3D12Resource* pSbtStorage;

	void ReleaseStructures();
};