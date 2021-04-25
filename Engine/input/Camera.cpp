#include "..\Headers\Input\Camera.h"
#include "..\Headers\Window.h"
#include "..\Headers\Graphics.h"
#include "..\Headers\MathUtilities.h"
#include <algorithm>


Camera::Camera(std::pair<short, short> res, float FovInDegrees, float Near, float Far)
	:
	Camera(res, ProjectionType::Perspective, 0.01f, {0.0f, 0.0f,0.0f}, 0, 0, FovInDegrees, Near, Far)
{
}

Camera::Camera(std::pair<short, short> res, ProjectionType Type, float Sensitivity, DirectX::XMFLOAT3 Pos, float Pitch, float Yaw, float FovInDegrees, float Near, float Far)
	:
	Pos(Pos),
	Sensitivity(Sensitivity),
	Pitch(Pitch),
	Yaw(Yaw),
	Speed(1.0f),
	Near(Near),
	Far(Far),
	Fov(FovInDegrees),
	Type(Type)
{
	if (Type == ProjectionType::Perspective)
		Projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(FovInDegrees), (float)res.first / (float)res.second, Near, Far);
	else if (Type == ProjectionType::Orthographic)
		Projection = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, (float)res.first, 0.0f, (float)res.second, 0.0f, 1.0f);

	UpdateView();
}

DirectX::XMMATRIX& Camera::GetView() noexcept
{
	return View;
}

DirectX::XMMATRIX& Camera::GetProjection() noexcept
{
	return Projection;
}

DirectX::XMFLOAT3 Camera::GetPos() noexcept
{
	return Pos;
}

void Camera::Translate(DirectX::XMFLOAT3 Translation) noexcept
{
	DirectX::XMStoreFloat3(&Translation, DirectX::XMVector3Transform(
		DirectX::XMLoadFloat3(&Translation),
		DirectX::XMMatrixRotationRollPitchYaw(Pitch, Yaw, 0.0f) *
		DirectX::XMMatrixScaling(Speed, Speed, Speed)
	));
	Pos = {
		Pos.x + Translation.x,
		Pos.y + Translation.y,
		Pos.z + Translation.z
	};
	UpdateView();
}

void Camera::Rotate(float dx, float dy) noexcept
{
	Yaw = wrap_angle(Yaw + dx * Sensitivity);
	Pitch = std::clamp(Pitch + dy * Sensitivity, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);

	UpdateView();
}

void Camera::SetPos(const DirectX::XMFLOAT3& Pos) noexcept
{
	this->Pos = Pos;
	UpdateView();
}

void Camera::SetRotation(float Pitch, float Yaw) noexcept
{
	this->Pitch = Pitch;
	this->Yaw = Yaw;
	UpdateView();
}

void Camera::SetSpeed(float Speed) noexcept
{
	this->Speed = Speed;
}

void Camera::SetSensitivity(float Value) noexcept
{
	Sensitivity = Value;
}

void Camera::UpdateView()
{
	const DirectX::XMVECTOR forwardBaseVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	// Apply the camera rotations to a base vector
	const auto lookVector = DirectX::XMVector3Transform(forwardBaseVector,
		DirectX::XMMatrixRotationRollPitchYaw(Pitch, Yaw, 0.0f)
	);
	// Generate camera transform (applied to all objects to arrange them relative
	// to camera position/orientation in world) from cam position and direction
	// camera "top" always faces towards +Y (cannot do a barrel roll)
	const auto camPosition = DirectX::XMLoadFloat3(&Pos);
	const auto camTarget = DirectX::XMVectorAdd(camPosition, lookVector);
	View = DirectX::XMMatrixLookAtLH(camPosition, camTarget, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}