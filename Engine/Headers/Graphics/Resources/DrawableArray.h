#pragma once
#include <unordered_set>
#include <memory>

class Drawable;
class Graphics;
class VertexBuffer;
class IndexBuffer;

class DrawableArray
{
public:

	void AddDrawable(Drawable* pDrawable) noexcept;
	void RemoveDrawable(Drawable* pDrawable) noexcept;
	void DrawIndexed(Graphics* pGraphics);
	void SetVertexAndIndexBuffers(std::shared_ptr<VertexBuffer> pVB, std::shared_ptr<IndexBuffer> pIB) noexcept; 

private:
	std::shared_ptr<VertexBuffer> pVertexBuffer;
	std::shared_ptr<IndexBuffer> pIndexBuffer;
	std::unordered_set<Drawable*> pDrawables;

};