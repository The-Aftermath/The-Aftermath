#pragma once
#include "../Graphics/Graphics.h"
#include <DirectXMath.h>
namespace TheAftermath {
	class Camera {
	public:
		Camera() {
			DirectX::XMVECTOR position;
			position.m128_f32[0] = 0.f;
			position.m128_f32[1] = 0.f;
			position.m128_f32[2] = 0.f;
			position.m128_f32[3] = 1.f;
			DirectX::XMVECTOR focus;
			focus.m128_f32[0] = 0.f;
			focus.m128_f32[1] = 0.f;
			focus.m128_f32[2] = 1.f;
			focus.m128_f32[3] = 1.f;
			DirectX::XMVECTOR up;
			up.m128_f32[0] = 0.f;
			up.m128_f32[1] = 1.f;
			up.m128_f32[2] = 0.f;
			up.m128_f32[3] = 1.f;

			_v = DirectX::XMMatrixLookAtLH(position, focus, up);
			_p = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, 1.f, 0.1f, 1000.f);
		}
		Camera(
			float PositionX, float PositionY, float PositionZ,
			float FocusX, float FocusY, float FocusZ,
			float UpX, float UpY, float UpZ,
			float FovAngleY,float AspectRatio,float NearZ,float FarZ
		) {
			DirectX::XMVECTOR position;
			position.m128_f32[0] = PositionX;
			position.m128_f32[1] = PositionY;
			position.m128_f32[2] = PositionZ;
			position.m128_f32[3] = 1.f;
			DirectX::XMVECTOR focus;
			focus.m128_f32[0] = FocusX;
			focus.m128_f32[1] = FocusY;
			focus.m128_f32[2] = FocusZ;
			focus.m128_f32[3] = 1.f;
			DirectX::XMVECTOR up;
			up.m128_f32[0] = UpX;
			up.m128_f32[1] = UpY;
			up.m128_f32[2] = UpZ;
			up.m128_f32[3] = 1.f;

			_v = DirectX::XMMatrixLookAtLH(position, focus, up);
			_p = DirectX::XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ);
		}

		DirectX::XMFLOAT4X4 GetVP() const {
			auto vp =  _v * _p;
			auto vpT = DirectX::XMMatrixTranspose(vp);
			DirectX::XMFLOAT4X4 _temp;
			DirectX::XMStoreFloat4x4(&_temp, vpT);
			return _temp;
		}
		Camera(const Camera&) = default;
		Camera(Camera&&) noexcept = default;
		Camera& operator=(const Camera&) = default;
		Camera& operator=(Camera&&) noexcept = default;


	private:
		DirectX::XMMATRIX _v;
		DirectX::XMMATRIX _p;

	};
}