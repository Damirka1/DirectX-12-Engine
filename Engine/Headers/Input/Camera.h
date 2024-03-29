#pragma once
#ifndef CAMERA_HEADER
#define CAMERA_HEADER
#include "..\Header.h"
#include <DirectXMath.h>
#include <utility>
typedef unsigned int UINT;

class Camera
{
public:

	struct CamView
	{
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
		DirectX::XMFLOAT3 Pos;
	};

	static Camera GetStaticCamera()
	{
		static Camera cam({ 800, 600 });
		return cam;
	}

	static CamView GetStaticCamView()
	{
		Camera cam = Camera::GetStaticCamera();
		CamView v;
		v.View = cam.GetView();
		v.Projection = cam.GetProjection();
		return v;
	}

	enum class ProjectionType
	{
		Perspective,
		Orthographic
	};
	
	Engine_API Camera(std::pair<short, short> res, float FovInDegrees, float Near = 0.01f, float Far = 200.0f);
	Engine_API Camera(std::pair<short, short> res, ProjectionType Type = ProjectionType::Perspective, float Sensitivity = 0.01f, DirectX::XMFLOAT3 Pos = { 0.0f, 0.0f, 0.0f }, float Pitch = 0, float Yaw = 0,
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

	

private:
	void UpdateView();

	// Projection.
	DirectX::XMMATRIX Projection;
	ProjectionType Type;
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