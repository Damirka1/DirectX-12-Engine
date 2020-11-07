#pragma once
#ifndef BUFFERS_HEADER
#define BUFFERS_HEADER
#include <d3d12.h>
#include "..\..\Error_Check.h"
#include "..\..\d3dx12.h"
#include "..\Bindable.h"

class Buffer
{
protected:
	Buffer(Graphics* pGraphics, void* pData, UINT DataSize, D3D12_RESOURCE_STATES state);
	~Buffer();

	ID3D12Resource* pBuffer = nullptr;
	ID3D12Resource* pCopyBuffer = nullptr;
};



class VertexBuffer : public Buffer, public Bindable
{
public:
	VertexBuffer(Graphics* pGraphics, void* pData, UINT Stride, UINT DataSize, UINT Slot = 0);
	void Bind(Graphics* pGraphics) override;
	~VertexBuffer() override;
	unsigned int GetVertexCount();

private:
	D3D12_VERTEX_BUFFER_VIEW VertexView;
	UINT Slot;
	unsigned int VertexCount;
};


class IndexBuffer : public Buffer, public Bindable
{
public:
	IndexBuffer(Graphics* pGraphics, std::vector<unsigned int> Indecies);
	void Bind(Graphics* pGraphics) override;
	~IndexBuffer() override;
	unsigned int GetIndeciesCount();

private:
	D3D12_INDEX_BUFFER_VIEW IndexView;
	unsigned int Indecies;
};

class ConstantBuffer : public Bindable
{
public:
	ConstantBuffer(Graphics* pGraphics, void* pData, UINT DataSize, UINT ParameterIndex);
	void Bind(Graphics* pGraphics) override;
	void Update(void* pData, UINT DataSize);
	std::pair<ID3D12DescriptorHeap*, UINT> GetHeap();
	~ConstantBuffer();

private:
	ID3D12Resource* pBuffer = nullptr;
	ID3D12DescriptorHeap* pHeap;
	D3D12_CONSTANT_BUFFER_VIEW_DESC BufferView;
	UINT RootParameterIndex = 0;
};


class HeapDescriptorArray : public Bindable
{
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
	void AddDescriptor(Graphics* pGrahpics, Type t, UINT RootParameterIndex);
	void AddDescriptor(ID3D12DescriptorHeap* pHeap, UINT RootParameterIndex);
	void Bind(Graphics* pGraphics) override;
	~HeapDescriptorArray() override;
private:
	std::vector<ID3D12DescriptorHeap*> pHeaps;
	std::vector<UINT> Indexes;
};

#endif
