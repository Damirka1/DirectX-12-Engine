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
protected:
	void AddBindable(std::shared_ptr<Bindable> Bindable);
	void SetVertexbuffer(std::shared_ptr<VertexBuffer> pVB);
	void SetIndexBuffer(std::shared_ptr<IndexBuffer> pIB);
	std::shared_ptr<ConstantBuffer> CreateConstBuffer(Graphics* pGraphics, void* pData, unsigned int DataSize, unsigned int RootParameterIndex);
	std::shared_ptr<Texture2D> CreateTexture2D(Graphics* pGraphics, std::string Path, unsigned int RootParameterIndex);


	void Bind(Graphics* pGraphics);

	void Draw(Graphics* pGraphics);
	void DrawIndexed(Graphics* pGraphics);

private:
	void CheckArray();


	std::vector<std::shared_ptr<Bindable>> Bindables;
	std::shared_ptr<VertexBuffer> pVertexBuffer;
	std::shared_ptr<IndexBuffer> pIndexBuffer;
	int HeapArrayIndex = -1;
};

#endif