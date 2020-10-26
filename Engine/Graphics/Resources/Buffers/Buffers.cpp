#include "..\..\..\Headers\Graphics\Resources\Buffers\Buffers.h"

Buffer::Buffer(Graphics* pGraphics, void* pData, UINT DataSize, D3D12_RESOURCE_STATES state)
{

	ID3D12Device8* pDevice = pGraphics->GetDevice();
	ID3D12GraphicsCommandList6* pCommandList = pGraphics->GetCommandList();

	// Create read only buffer.
	Error_Check(
		pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(DataSize),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&pBuffer)
		)
	);

	// Create upload buffer.
	Error_Check(
		pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(DataSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&pCopyBuffer)
		)
	);

	// Copy data to upload buffer.
	UINT8* ptr = nullptr;
	pCopyBuffer->Map(0, nullptr, reinterpret_cast<void**>(&ptr));

	memcpy(ptr, pData, DataSize);

	pCopyBuffer->Unmap(0, nullptr);

	// Copy from upload buffer to read buffer.
	pCommandList->CopyBufferRegion(pBuffer, 0, pCopyBuffer, 0, DataSize);
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pBuffer, D3D12_RESOURCE_STATE_COPY_DEST, state));


	pGraphics->AddToRelease(pCopyBuffer);
}

Buffer::~Buffer()
{
	pBuffer->Release();
	pBuffer = nullptr;
}

VertexBuffer::VertexBuffer(Graphics* pGraphics, void* pData, UINT Stride, UINT DataSize, UINT Slot)
	:
	Buffer(pGraphics, pData, DataSize, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER),
	Slot(Slot)
{
	VertexView.BufferLocation = pBuffer->GetGPUVirtualAddress();
	VertexView.StrideInBytes = Stride;
	VertexView.SizeInBytes = DataSize;
	VertexCount = DataSize / Stride;
}

void VertexBuffer::Bind(Graphics* pGraphics)
{
	pGraphics->GetCommandList()->IASetVertexBuffers(Slot, 1, &VertexView);
}

VertexBuffer::~VertexBuffer()
{
}

unsigned int VertexBuffer::GetVertexCount()
{
	return VertexCount;
}

IndexBuffer::IndexBuffer(Graphics* pGraphics, std::vector<unsigned int> Indecies)
	:
	Buffer(pGraphics, Indecies.data(), static_cast<UINT>(Indecies.size() * sizeof(unsigned int)), D3D12_RESOURCE_STATE_INDEX_BUFFER),
	Indecies(static_cast<unsigned int>(Indecies.size()))
{
	IndexView.BufferLocation = pBuffer->GetGPUVirtualAddress();
	IndexView.SizeInBytes = static_cast<UINT>(Indecies.size() * sizeof(unsigned int));
	IndexView.Format = DXGI_FORMAT_R32_UINT;
}

void IndexBuffer::Bind(Graphics* pGraphics)
{
	pGraphics->GetCommandList()->IASetIndexBuffer(&IndexView);
}

IndexBuffer::~IndexBuffer()
{
}

unsigned int IndexBuffer::GetIndeciesCount()
{
	return Indecies;
}
