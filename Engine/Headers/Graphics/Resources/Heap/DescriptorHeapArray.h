#pragma once
#include <d3d12.h>
#include <vector>
#include <memory>

#include "Descriptor.h"
#include "../../Error_Check.h"

class Graphics;

class DescriptorHeapArray
{
public:
	DescriptorHeapArray(D3D12_DESCRIPTOR_HEAP_TYPE Type);

	~DescriptorHeapArray();

	void IncrementDescriptorsCount();

	void DecrementDescriptorsCount();

	ID3D12DescriptorHeap* GetPtr();

	void Initialize(Graphics* pGraphics);

	void SetResource(std::shared_ptr<Resource> pResource);

	bool IsNeedUpdate();

	void Update(Graphics* pGraphics);


private:
	bool Initialized = false;
	bool NeedUpdate = false;
	UINT InitDescCount = 0;

	ID3D12DescriptorHeap* pDescriptorsHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC Desc;

	std::vector<std::unique_ptr<Descriptor>> Descriptors;
};