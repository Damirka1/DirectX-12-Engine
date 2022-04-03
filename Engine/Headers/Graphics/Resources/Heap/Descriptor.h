#pragma once
#include <d3d12.h>
#include <memory>

class Resource;

class Descriptor
{
public:
	bool SetResource(std::shared_ptr<Resource> pResource);
	void RemoveResource();
	void Initialize(D3D12_GPU_DESCRIPTOR_HANDLE& pGpu, D3D12_CPU_DESCRIPTOR_HANDLE& pCpu);

	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle();
	~Descriptor();

private:
	bool InUse = false;
	bool Initialized = false;

	std::shared_ptr<Resource> pResource;

	D3D12_GPU_DESCRIPTOR_HANDLE pGpuHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE pCpuHandle;
};