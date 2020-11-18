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
	Buffer(Graphics* pGraphics, void* pData, UINT DataSize, D3D12_RESOURCE_STATES state);
	~Buffer();

	ID3D12Resource* pBuffer = nullptr;
	ID3D12Resource* pCopyBuffer = nullptr;
};



class VertexBuffer : public Buffer, public Bindable
{
	friend class Drawable;
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
	friend class Drawable;
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
	ConstantBuffer(Graphics* pGraphics, void* pData, UINT DataSize, D3D12_CPU_DESCRIPTOR_HANDLE& pHandle);
	void Bind(Graphics* pGraphics) override;
	void Update(void* pData, UINT DataSize);
	~ConstantBuffer();

private:
	ID3D12Resource* pBuffer = nullptr;
	D3D12_CONSTANT_BUFFER_VIEW_DESC BufferView;
};

class Texture2D : public Bindable
{
public:
	Texture2D(Graphics* pGraphics, void* pData, UINT DataSize, D3D12_RESOURCE_DESC* pDesc, CD3DX12_CPU_DESCRIPTOR_HANDLE* pHandle);
	void Bind(Graphics* pGraphics) override;
	~Texture2D() override;

private:
	ID3D12Resource* pBuffer = nullptr;
	ID3D12Resource* pCopyBuffer = nullptr;
};

class Sampler : public Bindable
{
public:
	Sampler(Graphics* pGraphics, D3D12_SAMPLER_DESC pDesc, CD3DX12_CPU_DESCRIPTOR_HANDLE* pHandle);
	void Bind(Graphics* pGraphics) override;
	~Sampler() override;
private:
};


#endif
