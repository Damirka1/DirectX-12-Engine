#pragma once
#ifndef DRAWABLE_HEADER
#define DRAWABLE_HEADER
#include <vector>
#include <memory>
#include "Bindable.h"
#include "..\..\ResourceManager.h"
#include "BindablesHeader.h"


class Drawable
{
	friend class ResourceManager;
	friend class FrameCommander;
	friend class DrawableArray;
protected:
	Drawable() noexcept = default;
	Drawable(ResourceManager* pRM) noexcept;
	void Init(ResourceManager* pRM) noexcept;

	void AddBindable(std::shared_ptr<Bindable> Bindable) noexcept;

	void Bind(Graphics* pGraphics);

	void SetVertexAndIndexBuffers(std::shared_ptr<VertexBuffer> pVB, std::shared_ptr<IndexBuffer> pIB) noexcept;

protected:
	bool UI_element = false;

	DirectX::XMMATRIX Transformation;
	DirectX::XMFLOAT3 Pos;

	struct
	{
		const DirectX::XMMATRIX* View;
		const DirectX::XMMATRIX* Projection;
	} pCamera;

private:
	std::vector<std::shared_ptr<Bindable>> Bindables;

	std::shared_ptr<VertexBuffer> pVertexBuffer;
	std::shared_ptr<IndexBuffer> pIndexBuffer;

	std::vector<std::shared_ptr<Bindable>> InitializeHeap_list;
	std::vector<std::shared_ptr<Bindable>> InitializeCommon_list;
	// Index of Descriptor heap.
	char DescHeapIndex = -1;
	
	std::string PSO_Key, RS_Key;
};

#endif