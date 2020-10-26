#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "Header.h"
#include "Graphics\d3dx12.h"
#pragma comment(lib, "D3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3d12sdklayers.h>
#include <vector>

class Graphics
{
public:
	Engine_API Graphics(HWND pWindow, short Width = 800, short Height = 600);
	Engine_API ~Graphics();
	Engine_API void Initialize();

	Engine_API void Setup(float R, float G, float B, float A = 1.0f);
	Engine_API void Execute();

	ID3D12Device8* GetDevice();
	ID3D12GraphicsCommandList6* GetCommandList();

	void AddToRelease(ID3D12Resource*& pResource);
	DXGI_FORMAT GetFormat();

private:
	void WaitForGpu();
	void MoveToNextFrame();

private:
	static const UINT FrameCount = 2;

	CD3DX12_VIEWPORT ViewPort;
	CD3DX12_RECT ScissorRect;

	IDXGISwapChain4* pSwapChain = nullptr;
	ID3D12Device8* pDevice = nullptr;
	ID3D12GraphicsCommandList6* pCommandList = nullptr;
	ID3D12CommandQueue* pCommandQueue = nullptr;
	ID3D12Resource* pRenderTargets[FrameCount];
	ID3D12CommandAllocator* pCommandAllocators[FrameCount];
	ID3D12DescriptorHeap* pRTV_Heap = nullptr;
	UINT RTV_Size = 0;

	DXGI_FORMAT ViewFormat = DXGI_FORMAT_R8G8B8A8_UNORM;


	UINT FrameIndex = 0u;
	HANDLE pFenceEvent = nullptr;
	ID3D12Fence* pFence = nullptr;
	UINT64 FenceValues[FrameCount];
	std::vector<ID3D12Resource*>* ListToRelease = nullptr;
};







#endif