#include "..\..\Headers\Graphics\Resources\VertexLayout.h"

VertexLayout::VertexLayout() noexcept
{
}

void VertexLayout::AddElement(std::string Name, DXGI_FORMAT f, unsigned int SemanticIndex, unsigned int InputSlot, D3D12_INPUT_CLASSIFICATION IC) noexcept
{
	elements.emplace_back(Name, f, SemanticIndex, InputSlot, IC);
}

std::string VertexLayout::GetCode() noexcept
{
	std::string code;
	for (auto& el : elements)
	{
		code += "{" + el.Name + ":" + std::to_string(el.Size) + ":" + std::to_string(el.SemanticIndex) + "}";
	}
	return code;
}

std::vector<D3D12_INPUT_ELEMENT_DESC> VertexLayout::GetDesc() noexcept
{
	std::vector<D3D12_INPUT_ELEMENT_DESC> desc;
	unsigned int Offset = 0u;
	
	for (auto& el : elements)
	{
		desc.push_back({ el.Name.c_str(), el.SemanticIndex, el.Format, el.InputSlot, Offset, el.InputClassification, 0 });
		Offset += el.Size;
	}
	return desc;
}

VertexLayout::Element::Element(std::string Name, DXGI_FORMAT f, unsigned int SemanticIndex, unsigned int InputSlot, D3D12_INPUT_CLASSIFICATION IC) noexcept
	:
	Name(Name),
	Format(f),
	SemanticIndex(SemanticIndex),
	InputSlot(InputSlot),
	InputClassification(IC)
{

	switch (f)
	{
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		Size = 16;
		break;
	case DXGI_FORMAT_R32G32B32_FLOAT:
		Size = 12;
		break;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R32G32_FLOAT:
		Size = 8;
		break;
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
		Size = 4;
		break;
	case DXGI_FORMAT_R16_FLOAT:
		Size = 2;
		break;
	case DXGI_FORMAT_UNKNOWN:
	default:
		Name = "ERROR!";
		Size = 0;
		break;
	}
}
