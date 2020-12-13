#pragma once
#ifndef CAMERA_HEADER
#define CAMERA_HEADER
#include "..\Header.h"
#include <DirectXMath.h>
typedef unsigned int UINT;


class UICamera
{
public:
	UICamera() = delete;
	UICamera(UICamera&) = delete;

	Engine_API UICamera(UINT Width, UINT Height)
	{
		Projection = DirectX::XMMatrixOrthographicLH(static_cast<float>(Width), static_cast<float>(Height), 0.01f, 1.0f);
	}
	Engine_API DirectX::XMMATRIX& GetProjection()
	{
		return Projection;
	}

private:
	DirectX::XMMATRIX Projection;
};


#endif