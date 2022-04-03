#pragma once
#ifndef HEAP_HEADER
#define HEAP_HEADER
#include "../../Graphics.h"
#include <array>

class RS_Layout;

class GlobalHeap
{
public:
	GlobalHeap() noexcept = default;
	GlobalHeap(GlobalHeap&) = delete;

	void Add_CBV_SHR_UAV_Desc(UINT Count) noexcept;
	void Add_Samplers_Desc(UINT Count) noexcept;

	void Initialize(Graphics* pGraphics);

	// Start pointer to cbv, srv, uav heap.
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUStartPtr() noexcept;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUStartPtr() noexcept;

	// Start pinter to sampler's heap.
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUStartPtrForSAMPLERS() noexcept;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUStartPtrForSAMPLERS() noexcept;

	bool isHeapForCbvInitialized() noexcept;

	bool isHeapForSamplersInitialized() noexcept;

	void Bind(Graphics* pGraphics);

	// TODO: make thread safe!
	void Clear(Graphics* pGraphics);

	bool IsNeedUpdate();

	~GlobalHeap();

private:
	std::array<ID3D12DescriptorHeap*, 2> pHeaps;
	UINT CBV_SHR_UAV_Desc_count = 0;
	UINT SAMPLERS_Desc_count = 0;
	char ActiveHeaps = 0;
	bool Initialized = false;
	bool HeapCBVInitialized = false;
	bool HeapSamplersInitialized = false;
	bool NeedUpdate = false;
};
#endif