#pragma once

class ResourceManager;
class Graphics;
class RTXResources;
class DrawableMesh;
class Camera;

class RTXPass
{
public:

	RTXPass(Graphics* pGraphics);

	~RTXPass();

	void Initialize(Camera* pCamera);

	void AddMesh(DrawableMesh* mesh, unsigned int hitGroup = 0);

	void Execute();

private:
	Graphics* pGraphics;

	ResourceManager* pRM;
	RTXResources* pRTX;
};