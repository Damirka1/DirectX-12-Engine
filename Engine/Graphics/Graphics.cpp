#include "..\Headers\Graphics.h"
#include "..\Headers\Graphics\Error_Check.h"

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

    IDXGIFactory7* pFactory = nullptr;
    Error_Check(
        CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&pFactory))
    );

    Error_Check(
        D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice))
    );

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

    delete ListToRelease;
    ListToRelease = nullptr;
    
    pCommandList = nullptr;
    pFenceEvent = nullptr;
	pSwapChain = nullptr;
	pDevice = nullptr;
	pCommandQueue = nullptr;
	pFence = nullptr;
    pRTV_Heap = nullptr;
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

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(pRTV_Heap->GetCPUDescriptorHandleForHeapStart(), FrameIndex, RTV_Size);
    pCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

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
        pSwapChain->Present(1, 0)
    );

    MoveToNextFrame();
}

ID3D12Device8* Graphics::GetDevice()
{
    return pDevice;
}

ID3D12GraphicsCommandList6* Graphics::GetCommandList()
{
    return pCommandList;
}

void Graphics::AddToRelease(ID3D12Resource*& pResource)
{
    ListToRelease->push_back(pResource);
}

DXGI_FORMAT Graphics::GetFormat()
{
    return ViewFormat;
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
