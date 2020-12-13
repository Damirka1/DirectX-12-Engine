#include "..\..\..\Headers\Graphics\Resources\Buffers\Buffers.h"

Buffer::Buffer(Graphics* pGraphics, void* pData, UINT DataSize, D3D12_RESOURCE_STATES state)
{
	Initialize(pGraphics, pData, DataSize, state);
}

void Buffer::Initialize(Graphics* pGraphics, void* pData, UINT DataSize, D3D12_RESOURCE_STATES state)
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

VertexBuffer::VertexBuffer(void* pData, UINT Stride, UINT DataSize, UINT Slot)
	:
	Slot(Slot),
	DataSize(DataSize),
	Stride(Stride)
{
	// Copy data.
	this->pData = malloc(DataSize);
	if (pData)
		memcpy(this->pData, pData, DataSize);
	else
		throw std::exception();
}

void VertexBuffer::Bind(Graphics* pGraphics)
{
	pGraphics->GetCommandList()->IASetVertexBuffers(Slot, 1, &VertexView);
}

void VertexBuffer::Initialize(Graphics* pGraphics)
{
	if (!Initialized)
	{
		Buffer::Initialize(pGraphics, pData, DataSize, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		VertexView.BufferLocation = pBuffer->GetGPUVirtualAddress();
		VertexView.StrideInBytes = Stride;
		VertexView.SizeInBytes = DataSize;
		VertexCount = DataSize / Stride;

		key = "{location: " + std::to_string(VertexView.BufferLocation);
		key += "; size: " + std::to_string(VertexView.SizeInBytes);
		key += "; stride: " + std::to_string(VertexView.StrideInBytes);
		key += "; count: " + std::to_string(VertexCount) + '}';

		Initialized = true;
	}
}

VertexBuffer::~VertexBuffer()
{
	free(pData);
}

unsigned int VertexBuffer::GetVertexCount()
{
	return VertexCount;
}

std::string VertexBuffer::GetKey()
{
	return key;
}

IndexBuffer::IndexBuffer(std::vector<unsigned int> Indecies)
	:
	IndeciesCount(static_cast<unsigned int>(Indecies.size())),
	Indecies(Indecies)
{
}

void IndexBuffer::Bind(Graphics* pGraphics)
{
	pGraphics->GetCommandList()->IASetIndexBuffer(&IndexView);
}

void IndexBuffer::Initialize(Graphics* pGraphics)
{
	if (!Initialized)
	{
		Buffer::Initialize(pGraphics, Indecies.data(), static_cast<UINT>(Indecies.size() * sizeof(unsigned int)), D3D12_RESOURCE_STATE_INDEX_BUFFER);

		IndexView.BufferLocation = pBuffer->GetGPUVirtualAddress();
		IndexView.SizeInBytes = static_cast<UINT>(Indecies.size() * sizeof(unsigned int));
		IndexView.Format = DXGI_FORMAT_R32_UINT;

		key =  "{location: " + std::to_string(IndexView.BufferLocation);
		key += "; size: " + std::to_string(IndexView.SizeInBytes);
		key += "; format: " + std::to_string(IndexView.Format) + '}';

		Initialized = true;
	}
}

IndexBuffer::~IndexBuffer()
{
}

unsigned int IndexBuffer::GetIndeciesCount()
{
	return IndeciesCount;
}

std::string IndexBuffer::GetKey()
{
	return key;
}

ConstantBuffer::ConstantBuffer(void* pData, UINT DataSize)
	:
	pData(pData),
	DataSize(DataSize)
{
}

void ConstantBuffer::Bind(Graphics* pGraphics)
{
}

void ConstantBuffer::Initialize(Graphics* pGraphics, D3D12_CPU_DESCRIPTOR_HANDLE& pHandle)
{
	if (!Initialized)
	{
		ID3D12Device8* pDevice = pGraphics->GetDevice();

		UINT BufferSize = (DataSize + 255) & ~255;

		// Create buffer
		Error_Check(
			pDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(BufferSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&pBuffer))
		);

		// Describe and create a constant buffer view.
		BufferView.BufferLocation = pBuffer->GetGPUVirtualAddress();
		BufferView.SizeInBytes = BufferSize;
		pDevice->CreateConstantBufferView(&BufferView, pHandle);

		Update(pData, DataSize);

		Initialized = true;
	}
}

void ConstantBuffer::Update(void* pData, UINT DataSize)
{
	// Map and initialize the constant buffer.
	CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
	void* pConstBuffer;
	Error_Check(
		pBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pConstBuffer))
	);

	// Copy.
	memcpy(pConstBuffer, pData, DataSize);

	// Close buffer.
	pBuffer->Unmap(NULL, nullptr);
}


ConstantBuffer::~ConstantBuffer()
{
	pBuffer->Release();
	pBuffer = nullptr;
}

