#pragma once
#ifndef RESOURCE_HEADER
#define RESOURCE_HEADER
#include "..\..\Graphics.h"

class Resource
{
	friend class ResourceManager;
public:
	virtual void Bind(Graphics* pGraphics)
	{

	};
protected:
	virtual void Initialize(Graphics* pGraphics, D3D12_CPU_DESCRIPTOR_HANDLE& pHandle)
	{

	};

	virtual ~Resource(){}

	void SetHeapIndex(UINT Index) noexcept
	{
		this->Index = Index;
	}

	bool Initialized = false;
	UINT Index = 0u;

	D3D12_GPU_DESCRIPTOR_HANDLE pGpuHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE pCpuHandle;
};

#endif