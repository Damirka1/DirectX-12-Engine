#pragma once
#ifndef DRAWABLE_HEADER
#define DRAWABLE_HEADER
#include <vector>
#include <memory>
#include "Bindable.h"

class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class ResourceManager;
class Texture2D;


class Drawable
{
	friend class ResourceManager;
	friend class FrameCommander;
protected:
	void AddBindable(std::shared_ptr<Bindable> Bindable);
	void SetVertexbuffer(std::shared_ptr<VertexBuffer> pVB);
	void SetIndexBuffer(std::shared_ptr<IndexBuffer> pIB);

	void Bind(Graphics* pGraphics);

	void Draw(Graphics* pGraphics);
	void DrawIndexed(Graphics* pGraphics);

private:
	std::vector<std::shared_ptr<Bindable>> Bindables;
	std::shared_ptr<VertexBuffer> pVertexBuffer;
	std::shared_ptr<IndexBuffer> pIndexBuffer;

	std::vector<std::shared_ptr<Bindable>> InitializeHeap_list;
	std::vector<std::shared_ptr<Bindable>> InitializeCommon_list;
	// Index of Descriptor heap.
	char DescHeapIndex = -1;
};

#endif