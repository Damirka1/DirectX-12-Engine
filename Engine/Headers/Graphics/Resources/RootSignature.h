#pragma once
#ifndef ROOTSIGNATURE_HEADER
#define ROOTSIGNATURE_HEADER
#include "Bindable.h"

class HeapDescriptorArray;

class RS_Layout
{
	friend class RootSignature;
	friend HeapDescriptorArray;
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


class RootSignature : public Bindable
{
	friend class PipelineStateObject;
public:
	RootSignature(RS_Layout& Lay);
	void Initialize(Graphics* pGraphics);
	void Bind(Graphics* pGraphics) override;
	~RootSignature() override;

private:
	ID3D12RootSignature* pRootSignature = nullptr;
	RS_Layout Lay;
};
#endif