#pragma once
#ifndef HEAP_HEADER
#define HEAP_HEADER
#include "Bindable.h"
#include <array>

class RS_Layout;

class GlobalHeap
{
public:
	GlobalHeap() = default;
	GlobalHeap(GlobalHeap&) = delete;

	void Add_CBV_SHR_UAV_Desc(UINT Count);
	void Add_Samplers_Desc(UINT Count);

	void Initialize(Graphics* pGraphics);

	// Start pointer to cbv, srv, uav heap.
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUStartPtr();
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUStartPtr();

	// Start pinter to sampler's heap.
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUStartPtrForSAMPLERS();
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUStartPtrForSAMPLERS();

	void Bind(Graphics* pGraphics);

	Engine_API ~GlobalHeap();

private:
	std::array<ID3D12DescriptorHeap*, 2> pHeaps;
	UINT CBV_SHR_UAV_Desc_count = 0;
	UINT SAMPLERS_Desc_count = 0;
	bool Initialized = false;
};

class HeapDescriptorArray : public Bindable
{
	class RootParameter
	{
		friend HeapDescriptorArray;
		class Range
		{
			friend HeapDescriptorArray;
		public:
			Range(D3D12_DESCRIPTOR_RANGE_TYPE Type, UINT Nums);
			CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT Index);

		private:
			D3D12_DESCRIPTOR_RANGE_TYPE Type;
			UINT NumDescriptors;
			CD3DX12_CPU_DESCRIPTOR_HANDLE CPU_Offset;
			UINT Size = 0u;
		};
	public:
		RootParameter(UINT Index);
		virtual void Bind(ID3D12GraphicsCommandList* pCommandList);

	protected:
		UINT Index = 0u;
		CD3DX12_GPU_DESCRIPTOR_HANDLE GPU_OffsetFromStart;
		UINT Size = 0u;
		std::vector<Range> Ranges;
	};

	//class Constant : public RootParameter
	//{
	//public:
	//	Constant(UINT Index, CD3DX12_GPU_DESCRIPTOR_HANDLE Offset)
	//		:
	//		RootParameter(Index, Offset)
	//	{}
	//	void Bind(ID3D12GraphicsCommandList* pCommandList) override;
	//};
	//class Range : public RootParameter
	//{
	//public:
	//	Range(UINT Index, CD3DX12_GPU_DESCRIPTOR_HANDLE Offset)
	//		:
	//		RootParameter(Index, Offset)
	//	{}
	//	void Bind(ID3D12GraphicsCommandList* pCommandList) override;
	//};

	class DescriptorTable : public RootParameter
	{
	public:
		DescriptorTable(UINT Index)
			:
			RootParameter(Index)
		{}
		void Bind(ID3D12GraphicsCommandList* pCommandList) override
		{
			pCommandList->SetGraphicsRootDescriptorTable(Index, GPU_OffsetFromStart);
		}
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
	void Initialize(RS_Layout& Lay);
	void InitializePointers(Graphics* pGraphics, CD3DX12_CPU_DESCRIPTOR_HANDLE& pCPU, CD3DX12_GPU_DESCRIPTOR_HANDLE& pGPU, 
		CD3DX12_CPU_DESCRIPTOR_HANDLE& pCPUForSamplers, CD3DX12_GPU_DESCRIPTOR_HANDLE& pGPUForSamplers);
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT RootParam, UINT Range, UINT RangeIndex);
	~HeapDescriptorArray() override;
private:
	std::vector<RootParameter*> Parameters;
};
#endif