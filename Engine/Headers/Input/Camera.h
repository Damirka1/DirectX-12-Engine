#pragma once
#ifndef CAMERA_HEADER
#define CAMERA_HEADER
#include "..\Header.h"
#include <DirectXMath.h>
typedef unsigned int UINT;

class Window;

class Camera
{
	friend class Graphics;
public:
	Engine_API Camera(float FovInDegrees, float Near = 0.01f, float Far = 200.0f);
	Engine_API Camera(float Sensitivity = 0.01f, DirectX::XMFLOAT3 Pos = { 0.0f, 0.0f, 0.0f }, float Pitch = 0, float Yaw = 0,
					  float FovInDegrees = 60.0f, float Near = 0.01f, float Far = 200.0f);

	Engine_API Camera(Window* pWindow, float FovInDegrees, float Near = 0.01f, float Far = 200.0f);
	Engine_API Camera(Window* pWindow, float Sensitivity = 0.01f, DirectX::XMFLOAT3 Pos = { 0.0f, 0.0f, 0.0f }, float Pitch = 0, float Yaw = 0,
					  float FovInDegrees = 60.0f, float Near = 0.01f, float Far = 200.0f);

	Engine_API DirectX::XMMATRIX& GetView() noexcept;
	Engine_API DirectX::XMMATRIX& GetProjection() noexcept;
	Engine_API DirectX::XMFLOAT3 GetPos() noexcept;

	Engine_API void Translate(DirectX::XMFLOAT3 Translation) noexcept;
	Engine_API void Rotate(float dx, float dy) noexcept;
	Engine_API void SetPos(const DirectX::XMFLOAT3& Pos) noexcept;
	Engine_API void SetRotation(float Pitch, float Yaw) noexcept;
	Engine_API void SetSpeed(float Speed) noexcept;
	Engine_API void SetSensitivity(float Value) noexcept;
	
	Engine_API void SetCameraTo(Window* pWindow);


private:
	void UpdateView();

	// Projection.
	DirectX::XMMATRIX Projection;
	float Fov;
	float Near, Far;

	// View.
	DirectX::XMMATRIX View;
	DirectX::XMFLOAT3 Pos;
	float Pitch;
	float Yaw;
	float Speed;
	float Sensitivity;
};


#endif