#pragma once

#include <DirectXMath.h>

struct Transform
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMMATRIX PosMatrix;
	DirectX::XMFLOAT3 Rotation;
	DirectX::XMVECTOR RotQuat;
};

struct DxTransform
{
	DirectX::XMMATRIX Pos;
	DirectX::XMMATRIX View;
	DirectX::XMMATRIX Proj;
	DirectX::XMMATRIX PosViewProj;
	DirectX::XMFLOAT3 ViewPos;
};