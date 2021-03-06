#include "..\Headers\Graphics.h"
#include "..\Headers\Graphics\Error_Check.h"
#include <algorithm>

Graphics::Graphics(HWND pWindow, short w, short h)
    :
    ViewPort(0.0f, 0.0f, static_cast<float>(w), static_cast<float>(h)),
    ScissorRect(0, 0, static_cast<LONG>(w), static_cast<LONG>(h)),
    ListToRelease(new std::vector<ID3D12Resource*>)
{
    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    // Enable the debug layer.
    {
        ID3D12Debug* debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();

            // Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
        debugController->Release();
        debugController = nullptr;
    }
#endif

    // Create Factory.
    IDXGIFactory7* pFactory = nullptr;
    Error_Check(
        CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&pFactory))
    );


    // Enumerate all gpu's.
    {
        struct DeviceInfo
        {
            DeviceInfo(IDXGIAdapter4* a, DXGI_ADAPTER_DESC3 d)
                :
                Adapter(a),
                Desc(d)
            {}

            IDXGIAdapter4* Adapter;
            DXGI_ADAPTER_DESC3 Desc;
        };
        std::vector<DeviceInfo> Devices;

        IDXGIAdapter4* Adapter = nullptr;

        for (char i = 0; pFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_UNSPECIFIED, IID_PPV_ARGS(&Adapter)) != DXGI_ERROR_NOT_FOUND; i++)
        {
            DXGI_ADAPTER_DESC3 desc;
            Adapter->GetDesc3(&desc);
            Devices.emplace_back(Adapter, desc);
        }

        // Get adapter with maximum video memory.
        auto it = std::max_element(Devices.begin(), Devices.end(), 
            [&](DeviceInfo f, DeviceInfo l)
            {
                return f.Desc.DedicatedVideoMemory < l.Desc.DedicatedVideoMemory;       // This need to test. May not work.
            });

        // Create device.
        Error_Check(
            D3D12CreateDevice(it->Adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice))
        );
        
        // This will be used to print info.
        DeviceDesc = it->Desc;


        // Delete data.
        for (char i = 0; i < Devices.size(); i++)
        {
            Devices[i].Adapter->Release();
        }
        Devices.clear();
    }

    

    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    Error_Check(
        pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pCommandQueue))
    );

    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = w;
    swapChainDesc.Height = h;
    swapChainDesc.Format = ViewFormat;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    IDXGISwapChain1* pSC;
    Error_Check(
        pFactory->CreateSwapChainForHwnd(pCommandQueue, pWindow, &swapChainDesc, nullptr, nullptr, &pSC)
    );
    Error_Check(
        pSC->QueryInterface(&pSwapChain)
        );
    pSC->Release();
    pSC = nullptr;


    Error_Check(
        pFactory->MakeWindowAssociation(pWindow, DXGI_MWA_NO_ALT_ENTER)
    );

    FrameIndex = pSwapChain->GetCurrentBackBufferIndex();
    
    // Create descriptor heaps.
    {
        // Describe and create a render target view descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
        rtvDesc.NumDescriptors = FrameCount;
        rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        Error_Check(
            pDevice->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&pRTV_Heap))
        );

        RTV_Size = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);


        // Create a depth stencil descriptor heap so we can get a pointer to the depth stencil buffer.
        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        Error_Check(
            pDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&pdsDescriptorHeap))
        );

        // Create a depth stencil buffer.
        D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
        depthOptimizedClearValue.Format = dsvFormat;
        depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
        depthOptimizedClearValue.DepthStencil.Stencil = 0;

        Error_Check(
            pDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Tex2D(dsvFormat, w, h, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthOptimizedClearValue,
            IID_PPV_ARGS(&pDepthStencilBuffer))
        );
        pdsDescriptorHeap->SetName(L"Depth/Stencil Resource Heap");


        D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
        depthStencilDesc.Format = dsvFormat;
        depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

        pDevice->CreateDepthStencilView(pDepthStencilBuffer, &depthStencilDesc, pdsDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    }


    // Create frame resources.
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(pRTV_Heap->GetCPUDescriptorHandleForHeapStart());

        // Create a RTV and a command allocator for each frame.
        for (UINT i = 0; i < FrameCount; i++)
        {
            Error_Check(
                pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pRenderTargets[i]))
            );
            pDevice->CreateRenderTargetView(pRenderTargets[i], nullptr, rtvHandle);
            rtvHandle.Offset(1, RTV_Size);

            Error_Check(
                pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCommandAllocators[i]))
            );
        }
    }


    pFactory->Release();
    pFactory = nullptr;

    // Create the command list.
    Error_Check(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocators[FrameIndex], nullptr, IID_PPV_ARGS(&pCommandList)));


    // Create synchronization objects and wait until assets have been uploaded to the GPU.
    {
        Error_Check(
            pDevice->CreateFence(FenceValues[FrameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence))
        );
        FenceValues[FrameIndex]++;

        // Create an event handle to use for frame synchronization.
        pFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (pFenceEvent == nullptr)
        {
            Error_Check(
                HRESULT_FROM_WIN32(GetLastError())
            );
        }

        // Wait for the command list to execute; we are reusing the same command 
        // list in our main loop but for now, we just want to wait for setup to 
        // complete before continuing.
        WaitForGpu();
    }
}

Graphics::~Graphics()
{
    // Ensure that the GPU is no longer referencing resources that are about to be
    // cleaned up by the destructor.
    WaitForGpu();

    CloseHandle(pFenceEvent);
    for (short i = 0; i < FrameCount; i++)
    {
        pCommandAllocators[i]->Release();
        pRenderTargets[i]->Release();
    }
	pSwapChain->Release();
	pDevice->Release();
    pCommandList->Release();
	pCommandQueue->Release();
	pFence->Release();
    pRTV_Heap->Release();
    pDepthStencilBuffer->Release();
    pdsDescriptorHeap->Release();

    delete ListToRelease;
    ListToRelease = nullptr;
    
    pCommandList = nullptr;
    pFenceEvent = nullptr;
	pSwapChain = nullptr;
	pDevice = nullptr;
	pCommandQueue = nullptr;
	pFence = nullptr;
    pRTV_Heap = nullptr;
    pDepthStencilBuffer = nullptr;
    pdsDescriptorHeap = nullptr;
}

void Graphics::Initialize()
{
    // Close command list.
    pCommandList->Close();

    // Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { pCommandList };
    pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    WaitForGpu();

    for (size_t i = 0; i < ListToRelease->size(); i++)
    {
        (*ListToRelease)[i]->Release();
        (*ListToRelease)[i] = nullptr;
    }
    ListToRelease->clear();


}

void Graphics::Setup(float R, float G, float B, float A)
{
    // Command list allocators can only be reset when the associated 
    // command lists have finished execution on the GPU; apps should use 
    // fences to determine GPU execution progress.
    Error_Check(
        pCommandAllocators[FrameIndex]->Reset()
    );

    // However, when ExecuteCommandList() is called on a particular command 
    // list, that command list can then be reset at any time and must be before 
    // re-recording.
    Error_Check(
        pCommandList->Reset(pCommandAllocators[FrameIndex], nullptr)
    );

    // Set necessary state.
    pCommandList->RSSetViewports(1, &ViewPort);
    pCommandList->RSSetScissorRects(1, &ScissorRect);

    // Indicate that the back buffer will be used as a render target.
    pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pRenderTargets[FrameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    // Get a handle to the render target buffer
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(pRTV_Heap->GetCPUDescriptorHandleForHeapStart(), FrameIndex, RTV_Size);
    // Get a handle to the depth/stencil buffer
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(pdsDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    // Bind render target and depth stencil buffers.
    pCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
    pCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // Record commands.
    const float clearColor[] = { R, G, B, A };
    pCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void Graphics::Execute()
{
    // Indicate that the back buffer will now be used to present.
    pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pRenderTargets[FrameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

    // Close command list.
    pCommandList->Close();

    // Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { pCommandList };
    pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Present the frame.
    Error_Check(
        pSwapChain->Present(VSync, 0)
    );

    MoveToNextFrame();
}

ID3D12Device8* Graphics::GetDevice() noexcept
{
    return pDevice;
}

ID3D12GraphicsCommandList6* Graphics::GetCommandList() noexcept
{
    return pCommandList;
}

void Graphics::AddToRelease(ID3D12Resource*& pResource) noexcept
{
    ListToRelease->push_back(pResource);
}

DXGI_FORMAT Graphics::GetRTVFormat() noexcept
{
    return ViewFormat;
}

DXGI_FORMAT Graphics::GetDSVFormat() noexcept
{
    return dsvFormat;
}

std::pair<short, short> Graphics::GetResolution() noexcept
{
    return std::make_pair<short, short>(static_cast<short>(ViewPort.Width), static_cast<short>(ViewPort.Height));
}

std::wstring Graphics::GetInfo() noexcept
{
    return L"[Device]: " + std::wstring(static_cast<wchar_t*>(DeviceDesc.Description)) + L'\n'
        +  L"[Video Memory]: " + std::to_wstring(DeviceDesc.DedicatedVideoMemory / 1048576) + L" Mb\n"
        +  L"[Shared Memory]: " + std::to_wstring(DeviceDesc.SharedSystemMemory / 1048576) + L" Mb\n";
}

void Graphics::WaitForGpu()
{
    // Schedule a Signal command in the queue.
    Error_Check(
        pCommandQueue->Signal(pFence, FenceValues[FrameIndex])
    );

    // Wait until the fence has been processed.
    Error_Check(
        pFence->SetEventOnCompletion(FenceValues[FrameIndex], pFenceEvent)
    );
    WaitForSingleObjectEx(pFenceEvent, INFINITE, FALSE);


    // Increment the fence value for the current frame.
    FenceValues[FrameIndex]++;
}

void Graphics::MoveToNextFrame()
{
    // Schedule a Signal command in the queue.
    const UINT64 currentFenceValue = FenceValues[FrameIndex];
    Error_Check(
        pCommandQueue->Signal(pFence, currentFenceValue)
    );

    // Update the frame index.
    FrameIndex = pSwapChain->GetCurrentBackBufferIndex();

    // If the next frame is not ready to be rendered yet, wait until it is ready.
    if (pFence->GetCompletedValue() < FenceValues[FrameIndex])
    {
        Error_Check(
            pFence->SetEventOnCompletion(FenceValues[FrameIndex], pFenceEvent)
        );
        WaitForSingleObjectEx(pFenceEvent, INFINITE, FALSE);
    }

    // Set the fence value for the next frame.
    FenceValues[FrameIndex] = currentFenceValue + 1;
}
