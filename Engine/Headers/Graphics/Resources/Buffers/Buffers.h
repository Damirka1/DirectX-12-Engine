#pragma once
#ifndef BUFFERS_HEADER
#define BUFFERS_HEADER
#include <d3d12.h>
#include "..\..\Error_Check.h"
#include "..\..\d3dx12.h"
#include "..\Bindable.h"
#include "..\Resource.h"

class Buffer
{
protected:
	Buffer(Buffer&) = delete;
	Buffer() noexcept = default;
	Buffer(Graphics* pGraphics, void* pData, UINT DataSize, D3D12_RESOURCE_STATES state);
	void Initialize(Graphics* pGraphics, void* pData, UINT DataSize, D3D12_RESOURCE_STATES state);
	~Buffer();

	ID3D12Resource* pBuffer = nullptr;
	ID3D12Resource* pCopyBuffer = nullptr;
};

class VertexBuffer : public Buffer, public Resource
{
	friend class Drawable;
	friend class DrawableArray;
public:
	VertexBuffer(const void* pData, UINT Stride, UINT DataSize, UINT VetexCount, UINT Slot = 0);
	void Bind(Graphics* pGraphics) override;
	void Initialize(Graphics* pGraphics) override;
	~VertexBuffer() override;
	unsigned int GetVertexCount() noexcept;
	std::string GetKey() noexcept;

private:
	UINT Slot;
	void* pData;
	UINT Stride, DataSize, VertexCount;
	D3D12_VERTEX_BUFFER_VIEW VertexView;
	std::string key;
};

class IndexBuffer : public Buffer, public Resource
{
	friend class Drawable;
	friend class DrawableArray;
public:
	IndexBuffer(std::vector<unsigned int>* Indecies) noexcept;
	void Bind(Graphics* pGraphics) override;
	void Initialize(Graphics* pGraphics) override;
	~IndexBuffer() override;
	unsigned int GetIndeciesCount() noexcept;
	std::string GetKey() noexcept;

private:
	D3D12_INDEX_BUFFER_VIEW IndexView;
	std::vector<unsigned int> Indecies;
	std::string key;
	unsigned int IndeciesCount;
};

class ConstantBuffer : public Resource
{
public:
	ConstantBuffer(const void* pData, unsigned int DataSize);
	void Bind(Graphics* pGraphics) override;
	void Initialize(Graphics* pGraphics) override;
	void Update(const void* pData, UINT DataSize);
	~ConstantBuffer() override;

private:
	ID3D12Resource* pBuffer = nullptr;
	D3D12_CONSTANT_BUFFER_VIEW_DESC BufferView;
	const void* pData;
	UINT DataSize;
};

#endif
