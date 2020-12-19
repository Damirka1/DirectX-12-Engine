#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "Header.h"
#include "Graphics\d3dx12.h"
#pragma comment(lib, "D3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "uuid.lib")

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3d12sdklayers.h>
#include <vector>

class Camera;

class Graphics
{
	friend class Window;
public:
	Engine_API Graphics(HWND pWindow, short Width = 800, short Height = 600);
	Engine_API ~Graphics();
	Engine_API void Initialize();

	Engine_API void Setup(float R, float G, float B, float A = 1.0f);
	Engine_API void Execute();
	Engine_API std::wstring GetInfo() noexcept;

	ID3D12Device8* GetDevice() noexcept;
	ID3D12GraphicsCommandList6* GetCommandList() noexcept;

	void AddToRelease(ID3D12Resource*& pResource) noexcept;
	DXGI_FORMAT GetRTVFormat() noexcept;
	DXGI_FORMAT GetDSVFormat() noexcept;

	std::pair<short, short> GetResolution() noexcept;

private:
	void WaitForGpu();
	void MoveToNextFrame();


private:
	static const UINT FrameCount = 2;

	// Window size.
	CD3DX12_VIEWPORT ViewPort;
	CD3DX12_RECT ScissorRect;

	// Graphics basic units.
	IDXGISwapChain4* pSwapChain = nullptr;
	ID3D12Device8* pDevice = nullptr;
	ID3D12GraphicsCommandList6* pCommandList = nullptr;
	ID3D12CommandQueue* pCommandQueue = nullptr;
	ID3D12Resource* pRenderTargets[FrameCount];
	ID3D12CommandAllocator* pCommandAllocators[FrameCount];
	ID3D12DescriptorHeap* pRTV_Heap = nullptr;
	UINT RTV_Size = 0;

	// DepthStencil buffers.
	ID3D12Resource* pDepthStencilBuffer;
	ID3D12DescriptorHeap* pdsDescriptorHeap;
	DXGI_FORMAT dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Additional info.
	DXGI_FORMAT ViewFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_ADAPTER_DESC3 DeviceDesc;

	// Synchronization info.
	UINT FrameIndex = 0u;
	HANDLE pFenceEvent = nullptr;
	ID3D12Fence* pFence = nullptr;
	UINT64 FenceValues[FrameCount];
	std::vector<ID3D12Resource*>* ListToRelease = nullptr;
};

#endif