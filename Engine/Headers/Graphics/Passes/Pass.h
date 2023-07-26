#pragma once

class ResourceManager;
class Graphics;
class DrawableMesh;

class Pass
{
public:
	Pass(Graphics* pGraphics);
	Pass(Pass&) = delete;

	ResourceManager* GetResourceManager();

	~Pass();

	virtual void Bind();

	virtual void Execute() = 0;

	virtual void AddMesh(DrawableMesh*) = 0;

protected:
	Graphics* pGraphics;
	ResourceManager* pRM;
};