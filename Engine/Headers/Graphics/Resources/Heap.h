#pragma once
#ifndef HEAP_HEADER
#define HEAP_HEADER
#include "Bindable.h"
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



// TODO: move to another file
class HeapDescriptorArray : public Bindable
{
	class RootParameter
	{
		friend HeapDescriptorArray;
		class Range
		{
			friend HeapDescriptorArray;
		public:
			Range(D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT Nums) noexcept;
			CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT Index) noexcept;

		private:
			D3D12_DESCRIPTOR_RANGE_TYPE Type;
			UINT NumDescriptors;
			CD3DX12_CPU_DESCRIPTOR_HANDLE CPU_Offset;
			UINT Size = 0u;
		};
	public:
		RootParameter(UINT Index) noexcept;
		virtual void Bind(ID3D12GraphicsCommandList* pCommandList);

	protected:
		UINT Index = 0u;
		CD3DX12_GPU_DESCRIPTOR_HANDLE GPU_OffsetFromStart;
		UINT Size = 0u;
		std::vector<Range> Ranges;
	};

	class DescriptorTable : public RootParameter
	{
	public:
		DescriptorTable(UINT Index) noexcept;

		void Bind(ID3D12GraphicsCommandList* pCommandList) override;

	};

public:
	enum class Type
	{
		CBV,
		SRV,
		UAV,
		DSV,
		RTV,
		SAMPLER
	};

	HeapDescriptorArray();
	void Bind(Graphics* pGraphics) override;
	void Initialize(RS_Layout& Lay) noexcept;
	void InitializePointers(Graphics* pGraphics, CD3DX12_CPU_DESCRIPTOR_HANDLE& pCPU, CD3DX12_GPU_DESCRIPTOR_HANDLE& pGPU, 
		CD3DX12_CPU_DESCRIPTOR_HANDLE& pCPUForSamplers, CD3DX12_GPU_DESCRIPTOR_HANDLE& pGPUForSamplers) noexcept;
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT RootParam, UINT Range, UINT RangeIndex) noexcept;
	~HeapDescriptorArray() override;
private:
	std::vector<RootParameter*> Parameters;
};
#endif