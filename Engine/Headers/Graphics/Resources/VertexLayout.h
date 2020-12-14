#pragma once
#ifndef VERTEXLAYOUT_HEADER
#define VERTEXLAYOUT_HEADER
#include <d3d12.h>
#include <string>
#include <vector>

class VertexLayout
{
	struct Element
	{
		Element(std::string Name, DXGI_FORMAT f, unsigned int SemanticIndex, unsigned int InputSlot, D3D12_INPUT_CLASSIFICATION IC) noexcept;

		std::string Name;
		unsigned int SemanticIndex;
		DXGI_FORMAT Format;
		unsigned int Size;
		unsigned int InputSlot;
		D3D12_INPUT_CLASSIFICATION InputClassification;
	};
public:
	VertexLayout() noexcept;
	void AddElement(std::string Name, DXGI_FORMAT f, unsigned int SemanticIndex = 0u, unsigned int InputSlot = 0u, D3D12_INPUT_CLASSIFICATION IC = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA) noexcept;
	std::string GetCode() noexcept;
	std::vector<D3D12_INPUT_ELEMENT_DESC> GetDesc() noexcept;

private:
	std::vector<Element> elements;
};

#endif