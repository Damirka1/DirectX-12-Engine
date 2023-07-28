#include "..\..\..\Headers\Graphics\Resources\Buffers\Buffers.h"

Buffer::Buffer(Graphics* pGraphics, void* pData, UINT DataSize, D3D12_RESOURCE_STATES state)
{
	Initialize(pGraphics, pData, DataSize, state);
}

void Buffer::Initialize(Graphics* pGraphics, void* pData, UINT DataSize, D3D12_RESOURCE_STATES state)
{
	ID3D12Device9* pDevice = pGraphics->GetDevice();
	ID3D12GraphicsCommandList6* pCommandList = pGraphics->GetCommandList();

	// Create read only buffer.
	Error_Check(
		pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(DataSize),
			D3D12_RESOURCE_STATE_COMMON,
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

VertexBuffer::VertexBuffer(const void* pData, UINT Stride, UINT DataSize, UINT VertexCount, UINT Slot)
	:
	Slot(Slot),
	DataSize(DataSize),
	Stride(Stride),
	VertexCount(VertexCount)
{
	Name = "VertexBuffer";
	if (pData)
	{
		// Copy data.
		this->pData = malloc(DataSize);
		if (!this->pData)
			throw std::exception("Not enough memory");
		memcpy(this->pData, pData, DataSize);
	}
	else
		throw std::exception("Invalid Vertex data pointer");
}

void VertexBuffer::Bind(Graphics* pGraphics)
{
	pGraphics->GetCommandList()->IASetVertexBuffers(Slot, 1, &VertexView);
}

void VertexBuffer::Initialize(Graphics* pGraphics)
{
	if (!Initialized)
	{
		Buffer::Initialize(pGraphics, pData, DataSize, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

		VertexView.BufferLocation = pBuffer->GetGPUVirtualAddress();
		VertexView.StrideInBytes = Stride;
		VertexView.SizeInBytes = DataSize;

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

unsigned int VertexBuffer::GetVertexCount() noexcept
{
	return VertexCount;
}

std::string VertexBuffer::GetKey() noexcept
{
	return key;
}

IndexBuffer::IndexBuffer(std::vector<unsigned int>* Indecies) noexcept
	:
	IndeciesCount(static_cast<unsigned int>(Indecies->size())),
	Indecies(*Indecies)
{
	Name = "IndexBuffer";
}

void IndexBuffer::Bind(Graphics* pGraphics)
{
	pGraphics->GetCommandList()->IASetIndexBuffer(&IndexView);
}

void IndexBuffer::Initialize(Graphics* pGraphics)
{
	if (!Initialized)
	{
		Buffer::Initialize(pGraphics, Indecies.data(), static_cast<UINT>(Indecies.size() * sizeof(unsigned int)), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

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

unsigned int IndexBuffer::GetIndeciesCount() noexcept
{
	return IndeciesCount;
}

std::string IndexBuffer::GetKey() noexcept
{
	return key;
}

ConstantBuffer::ConstantBuffer(const void* pData, UINT DataSize)
	:
	pData(pData),
	DataSize(DataSize)
{
	if (!pData)
		throw std::exception("Null pointer in constant buffer");
	Name = "ConstantBuffer";
}

void ConstantBuffer::Bind(Graphics* pGraphics)
{
	pGraphics->GetCommandList()->SetGraphicsRootDescriptorTable(Index, pDescriptor->GetGpuHandle());
}

void ConstantBuffer::Initialize(Graphics* pGraphics)
{
	ID3D12Device9* pDevice = pGraphics->GetDevice();
	BufferSize = (DataSize + 255) & ~255;

	if (!Initialized)
	{
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

		Update(pData, DataSize);
		Initialized = true;
	}

	// Describe and create a constant buffer view.
	BufferView.BufferLocation = pBuffer->GetGPUVirtualAddress();
	BufferView.SizeInBytes = BufferSize;

	pDevice->CreateConstantBufferView(&BufferView, pDescriptor->GetCpuHandle());
}

void ConstantBuffer::Update(const void* pData, UINT DataSize)
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

