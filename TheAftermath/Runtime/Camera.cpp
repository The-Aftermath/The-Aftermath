#include "Camera.h"
#include <immintrin.h>
using namespace DirectX;

namespace TheAftermath {

    Camera::Camera() {
        DirectX::XMFLOAT3 pos(0, 0, 0);
        mPos = DirectX::XMLoadFloat3(&pos);
        DirectX::XMFLOAT3 dir(0, 0, 1);
        mDir = DirectX::XMLoadFloat3(&dir);
        DirectX::XMFLOAT3 up(0, 1, 0);
        mUp = DirectX::XMLoadFloat3(&up);
        mRight = DirectX::XMVector3Cross(mUp, mDir);
        mView = DirectX::XMMatrixLookToLH(mPos, mDir, mUp);

        SetPerspectiveMatrix(DirectX::XM_PIDIV4, 16.f / 9.f, 1.f, 1000.f);

        auto vT = DirectX::XMMatrixTranspose(mView);
        auto pT = DirectX::XMMatrixTranspose(mProj);

        _mm_storeu_ps(&mMatrixNative[0], vT.r[0]);
        _mm_storeu_ps(&mMatrixNative[4], vT.r[1]);
        _mm_storeu_ps(&mMatrixNative[8], vT.r[2]);
        _mm_storeu_ps(&mMatrixNative[12], vT.r[3]);
        _mm_storeu_ps(&mMatrixNative[16], pT.r[0]);
        _mm_storeu_ps(&mMatrixNative[20], pT.r[1]);
        _mm_storeu_ps(&mMatrixNative[24], pT.r[2]);
        _mm_storeu_ps(&mMatrixNative[28], pT.r[3]);
    }
    void Camera::UpdateViewMatrix() {
        if (mViewDirty) {
            mView = DirectX::XMMatrixLookToLH(mPos, mDir, mUp);
            mViewDirty = false;

            auto vT = DirectX::XMMatrixTranspose(mView);
            auto pT = DirectX::XMMatrixTranspose(mProj);

            _mm_storeu_ps(&mMatrixNative[0], vT.r[0]);
            _mm_storeu_ps(&mMatrixNative[4], vT.r[1]);
            _mm_storeu_ps(&mMatrixNative[8], vT.r[2]);
            _mm_storeu_ps(&mMatrixNative[12], vT.r[3]);
            _mm_storeu_ps(&mMatrixNative[16], pT.r[0]);
            _mm_storeu_ps(&mMatrixNative[20], pT.r[1]);
            _mm_storeu_ps(&mMatrixNative[24], pT.r[2]);
            _mm_storeu_ps(&mMatrixNative[28], pT.r[3]);
        }
    }

}