#pragma once
#ifndef ROOTSIGNATURE_HEADER
#define ROOTSIGNATURE_HEADER
#include "Bindable.h"


class RS_Layout
{
	friend class RootSignature;
	struct DescriptorTable
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

		DescriptorTable(D3D12_SHADER_VISIBILITY V);
		DescriptorTable& AddRange(D3D12_DESCRIPTOR_RANGE_TYPE T, UINT numDescriptors, UINT ShaderRegister = 0u, UINT RegisterSpace = 0u, D3D12_DESCRIPTOR_RANGE_FLAGS Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
		Range& operator[](UINT index);
		std::string GetCode();

		std::vector<Range> Ranges;
		D3D12_SHADER_VISIBILITY Visibility;
	};

public:

	RS_Layout(D3D12_ROOT_SIGNATURE_FLAGS F = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
											 D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS		  |
											 D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS     |
											 D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS   |
											 D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS);
	DescriptorTable& AddDescriptorTable(D3D12_SHADER_VISIBILITY V);
	DescriptorTable& operator[](UINT index);
	std::string GetCode();

private:
	std::vector<DescriptorTable> DescriptorTables;
	D3D12_ROOT_SIGNATURE_FLAGS Flags;
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

private:
	ID3D12RootSignature* pRootSignature;
	std::string Code;
};
#endif