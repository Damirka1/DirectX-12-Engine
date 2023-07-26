#pragma once

class ResourceManager;
class Graphics;
class DrawableMesh;

class Pass
{
public:
	Pass(Graphics* pGraphics, ResourceManager* pRM);
	Pass(Pass&) = delete;

	~Pass() = default;

	virtual void Bind() = 0;

	virtual void Execute() = 0;

	virtual void AddMesh(DrawableMesh*) = 0;

protected:
	Graphics* pGraphics;
	ResourceManager* pRM;
};