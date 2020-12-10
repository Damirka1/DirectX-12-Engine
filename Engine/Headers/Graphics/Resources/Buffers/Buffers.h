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
	Buffer(Buffer&) = delete;
	Buffer() = default;
	Buffer(Graphics* pGraphics, void* pData, UINT DataSize, D3D12_RESOURCE_STATES state);
	void Initialize(Graphics* pGraphics, void* pData, UINT DataSize, D3D12_RESOURCE_STATES state);
	~Buffer();

	ID3D12Resource* pBuffer = nullptr;
	ID3D12Resource* pCopyBuffer = nullptr;
};



class VertexBuffer : public Buffer, public Bindable
{
	friend class Drawable;
	friend class DrawableArray;
public:
	VertexBuffer(void* pData, UINT Stride, UINT DataSize, UINT Slot = 0);
	void Bind(Graphics* pGraphics) override;
	void Initialize(Graphics* pGraphics) override;
	~VertexBuffer() override;
	unsigned int GetVertexCount();
	std::string GetKey();

private:
	D3D12_VERTEX_BUFFER_VIEW VertexView;
	UINT Slot;
	unsigned int VertexCount;
	void* pData;
	UINT Stride, DataSize;
	std::string key;
};


class IndexBuffer : public Buffer, public Bindable
{
	friend class Drawable;
	friend class DrawableArray;
public:
	IndexBuffer(std::vector<unsigned int> Indecies);
	void Bind(Graphics* pGraphics) override;
	void Initialize(Graphics* pGraphics) override;
	~IndexBuffer() override;
	unsigned int GetIndeciesCount();
	std::string GetKey();

private:
	D3D12_INDEX_BUFFER_VIEW IndexView;
	std::vector<unsigned int> Indecies;
	unsigned int IndeciesCount;
	std::string key;
};

class ConstantBuffer : public Bindable
{
public:
	ConstantBuffer(void* pData, unsigned int DataSize);
	void Bind(Graphics* pGraphics) override;
	void Initialize(Graphics* pGraphics, D3D12_CPU_DESCRIPTOR_HANDLE& pHandle) override;
	void Update(void* pData, UINT DataSize);
	~ConstantBuffer();

private:
	ID3D12Resource* pBuffer = nullptr;
	D3D12_CONSTANT_BUFFER_VIEW_DESC BufferView;
	void* pData;
	UINT DataSize;
};

#endif
