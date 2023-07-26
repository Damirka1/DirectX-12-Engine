#pragma once
#include <string>
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include <PxPhysicsAPI.h>

#include "../../Header.h"

#include "../../Graphics/Resources/DrawableMesh.h"

class ResourceManager;
class DrawableMesh;
class ConstantBuffer;
class Graphics;
class Camera;

class StaticMeshComponent
{
	friend class RTXResources;

	struct DxTransform
	{
		DirectX::XMMATRIX Pos;
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Proj;
		DirectX::XMMATRIX PosViewProj;
		DirectX::XMFLOAT3 ViewPos;
	};

	struct Transform
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMMATRIX PosMatrix;
		DirectX::XMFLOAT3 Rotation;
		DirectX::XMVECTOR RotQuat;
	};

public:
	Engine_API StaticMeshComponent(ResourceManager* pRM, std::string ModelPath, float scale = 1.0f);

	Engine_API void Draw(Graphics* pGraphics);

	Engine_API void Update(Camera* cam);

	Engine_API void SetPos(DirectX::XMFLOAT3 Pos);

	Engine_API void SetRotation(DirectX::XMFLOAT3 Rotation);

	Engine_API DirectX::XMFLOAT3 GetPos();
	Engine_API DirectX::XMFLOAT3 GetRotation();
	Engine_API DirectX::XMMATRIX& GetPosMatrix();
	Engine_API DirectX::XMMATRIX GetTransform();

private:
	

protected:
	std::vector<DrawableMesh> Meshes;
	std::shared_ptr<ConstantBuffer> CB;

	DxTransform DxTransform = { };
	Transform Transform = { };
};