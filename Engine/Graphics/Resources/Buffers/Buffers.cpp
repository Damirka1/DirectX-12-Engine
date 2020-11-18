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

ConstantBuffer::ConstantBuffer(Graphics* pGraphics, void* pData, UINT DataSize, D3D12_CPU_DESCRIPTOR_HANDLE& pHandle)
{
	ID3D12Device8* pDevice = pGraphics->GetDevice();

	// Create buffer
	Error_Check(
		pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((DataSize + (1024 * 64 - 1)) & ~(1024 * 64 - 1)), // Heap must be 64Kb alligned.
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pBuffer))
	);

	// Describe and create a constant buffer view.
	BufferView.BufferLocation = pBuffer->GetGPUVirtualAddress();
	BufferView.SizeInBytes = (DataSize + 255) & ~255;
	pDevice->CreateConstantBufferView(&BufferView, pHandle);

	Update(pData, DataSize);
}

void ConstantBuffer::Bind(Graphics* pGraphics)
{
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


Texture2D::Texture2D(Graphics* pGraphics, void* pData, UINT DataSize, D3D12_RESOURCE_DESC* pDesc, CD3DX12_CPU_DESCRIPTOR_HANDLE* pHandle)
{
	ID3D12Device8* pDevice = pGraphics->GetDevice();
	ID3D12GraphicsCommandList6* pCommandList = pGraphics->GetCommandList();

	Error_Check(pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		pDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&pBuffer)));


	Error_Check(pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(GetRequiredIntermediateSize(pBuffer, 0, 1)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pCopyBuffer)));

	UINT64 pixels = DataSize / (pDesc->Width * pDesc->Height);

	D3D12_SUBRESOURCE_DATA textureData = {};
	textureData.pData = pData;
	textureData.RowPitch = pDesc->Width * pixels;
	textureData.SlicePitch = textureData.RowPitch * pDesc->Height;

	UpdateSubresources(pCommandList, pBuffer, pCopyBuffer, 0, 0, 1, &textureData);
	pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	pGraphics->AddToRelease(pCopyBuffer);

	// Describe and create a SRV for the texture.
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = pDesc->Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	pDevice->CreateShaderResourceView(pBuffer, &srvDesc, *pHandle);
}

void Texture2D::Bind(Graphics* pGraphics)
{
	// Do nothing. Bind with Descriptors tables.
}

Texture2D::~Texture2D()
{
	pBuffer->Release();
	pBuffer = nullptr;
}

Sampler::Sampler(Graphics* pGraphics, D3D12_SAMPLER_DESC pDesc, CD3DX12_CPU_DESCRIPTOR_HANDLE* pHandle)
{
	pGraphics->GetDevice()->CreateSampler(&pDesc, *pHandle);
}

void Sampler::Bind(Graphics* pGraphics)
{
	// Do nothing. Bind with Descriptors tables.
}

Sampler::~Sampler()
{
}
