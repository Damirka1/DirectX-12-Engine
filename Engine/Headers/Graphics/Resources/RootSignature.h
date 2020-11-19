#pragma once
#ifndef ROOTSIGNATURE_HEADER
#define ROOTSIGNATURE_HEADER
#include "Bindable.h"


class RS_Layout
{
	friend class RootSignature;
	enum class Type
	{
		Constant,
		Range,
		DescriptorTable
	};
	struct RootParameter
	{
		struct Range
		{
			Range(D3D12_DESCRIPTOR_RANGE_TYPE T, UINT N, UINT SR, UINT RS, D3D12_DESCRIPTOR_RANGE_FLAGS F);
			std::string GetCode();

			D3D12_DESCRIPTOR_RANGE_TYPE Type;
			UINT numDescriptors;
			UINT ShaderRegister;
			UINT RegisterSpace;
			D3D12_DESCRIPTOR_RANGE_FLAGS Flags;
		};

		RootParameter(D3D12_SHADER_VISIBILITY V, Type t);
		RootParameter& AddRange(D3D12_DESCRIPTOR_RANGE_TYPE T, UINT numDescriptors, UINT ShaderRegister = 0u, UINT RegisterSpace = 0u, D3D12_DESCRIPTOR_RANGE_FLAGS Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE);
		Range& operator[](UINT index);
		std::string GetCode();

		std::vector<Range> Ranges;
		D3D12_SHADER_VISIBILITY Visibility;
		Type type;
	};

public:

	RS_Layout(D3D12_ROOT_SIGNATURE_FLAGS F = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
											 D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS		  |
											 D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS     |
											 D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS   |
											 D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS);
	RootParameter& AddDescriptorTable(D3D12_SHADER_VISIBILITY V);
	RootParameter& operator[](UINT index);
	std::string GetCode();

private:
	std::vector<RootParameter> Params;
	D3D12_ROOT_SIGNATURE_FLAGS Flags;
};

class HeapDescriptorArray : public Bindable
{
	friend class RootSignature;

	class RootParameter
	{
		friend class RootSignature;
		friend HeapDescriptorArray;
		class Range
		{
			friend class RootSignature;
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
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT RootParam, UINT Range, UINT RangeIndex);
	~HeapDescriptorArray() override;
private:

	std::vector<ID3D12DescriptorHeap*> pHeaps;
	std::vector<RootParameter*> Parameters;
};


class RootSignature : public Bindable
{
	friend class PipelineStateObject;
public:
	RootSignature(Graphics* pGraphics, std::string code);
	RootSignature(Graphics* pGraphics, RS_Layout& Lay, std::string code);
	void Bind(Graphics* pGraphics) override;
	~RootSignature() override;
	std::string GetCode();
	std::shared_ptr<HeapDescriptorArray> GetHeapArray();

private:
	ID3D12RootSignature* pRootSignature;
	std::string Code;
	std::shared_ptr<HeapDescriptorArray> HeapArray;
};
#endif