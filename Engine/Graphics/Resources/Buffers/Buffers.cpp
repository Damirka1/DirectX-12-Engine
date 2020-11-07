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

ConstantBuffer::ConstantBuffer(Graphics* pGraphics, void* pData, UINT DataSize, UINT ParameterIndex)
	:
	RootParameterIndex(ParameterIndex)
{

	ID3D12Device8* pDevice = pGraphics->GetDevice();

	// Describe and create a constant buffer view (CBV) descriptor heap.
	// Flags indicate that this descriptor heap can be bound to the pipeline 
	// and that descriptors contained in it can be referenced by a root table.
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	Error_Check(
		pDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&pHeap))
	);

	UINT size = (DataSize + (D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1)) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);

	// Create buffer
	Error_Check(
		pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pBuffer))
	);

	// Describe and create a constant buffer view.
	BufferView.BufferLocation = pBuffer->GetGPUVirtualAddress();
	BufferView.SizeInBytes = size;
	pGraphics->GetDevice()->CreateConstantBufferView(&BufferView, pHeap->GetCPUDescriptorHandleForHeapStart());

	Update(pData, DataSize);
}

void ConstantBuffer::Bind(Graphics* pGraphics)
{
	ID3D12DescriptorHeap* pHeaps[] = { pHeap };

	pGraphics->GetCommandList()->SetDescriptorHeaps(_countof(pHeaps), pHeaps);

	pGraphics->GetCommandList()->SetGraphicsRootDescriptorTable(RootParameterIndex, pHeap->GetGPUDescriptorHandleForHeapStart());
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

std::pair<ID3D12DescriptorHeap*, UINT> ConstantBuffer::GetHeap()
{
	return { pHeap, RootParameterIndex };
}

ConstantBuffer::~ConstantBuffer()
{
	pHeap->Release();
	pBuffer->Release();
	pHeap = nullptr;
	pBuffer = nullptr;
}

HeapDescriptorArray::HeapDescriptorArray()
{
}

void HeapDescriptorArray::AddDescriptor(Graphics* pGrahpics, Type t, UINT RootParameterIndex)
{
	D3D12_DESCRIPTOR_HEAP_DESC Desc = {};
	Desc.NumDescriptors = 1;
	Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	switch (t)
	{
	case HeapDescriptorArray::Type::CBV:
	case HeapDescriptorArray::Type::SRV:
	case HeapDescriptorArray::Type::UAV:
	Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		break;
	case HeapDescriptorArray::Type::DSV:
	Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		break;
	case HeapDescriptorArray::Type::RTV:
	Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		break;
	case HeapDescriptorArray::Type::SAMPLER:
	Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		break;
	default:
		throw std::exception("Unknown descriptor type");
		break;
	}

	ID3D12DescriptorHeap* pHeap;

	Error_Check(
		pGrahpics->GetDevice()->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&pHeap))
	);

	AddDescriptor(pHeap, RootParameterIndex);
}

void HeapDescriptorArray::AddDescriptor(ID3D12DescriptorHeap* pHeap, UINT RootParameterIndex)
{
	pHeaps.push_back(pHeap);
	Indexes.push_back(RootParameterIndex);
}

void HeapDescriptorArray::Bind(Graphics* pGraphics)
{
	pGraphics->GetCommandList()->SetDescriptorHeaps(static_cast<UINT>(pHeaps.size()), pHeaps.data());
	for (size_t i = 0; i < pHeaps.size(); i++)
		pGraphics->GetCommandList()->SetGraphicsRootDescriptorTable(Indexes[i], pHeaps[i]->GetGPUDescriptorHandleForHeapStart());
}

HeapDescriptorArray::~HeapDescriptorArray()
{
	for (auto& p : pHeaps)
		p->Release();
	pHeaps.clear();
	Indexes.clear();
}
