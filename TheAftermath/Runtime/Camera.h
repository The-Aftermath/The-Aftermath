#pragma once
#include <DirectXMath.h>
#include <cmath>
namespace TheAftermath {
    class Camera
    {
    public:
        Camera();

        void SetPerspectiveMatrix(float FovAngleY, float AspectRatio, float NearZ, float FarZ) {
            mFovAngleY = FovAngleY;
            mAspectRatio = AspectRatio;
            mNearZ = NearZ;
            mFarZ = FarZ;
            mProj = DirectX::XMMatrixPerspectiveFovLH(mFovAngleY, mAspectRatio, mNearZ, mFarZ);
        }

        void Strafe(float distance) {
            using namespace DirectX;
            mPos += (mRight * distance);
            mViewDirty = true;
        }
        void Walk(float distance) {
            using namespace DirectX;
            mPos += (mDir * distance);
            mViewDirty = true;
        }

        void Pitch(float angle) {
            using namespace DirectX;
            auto R = XMMatrixRotationAxis(mRight, angle);
            mUp = XMVector3TransformNormal(mUp, R);
            mDir = XMVector3TransformNormal(mDir, R);
            mViewDirty = true;

            XMFLOAT4 up;
            XMStoreFloat4(&up, mUp);
            if (up.y <= 0.05f) {
                up.y = 0.05f;
                mUp = XMLoadFloat4(&up);
                mDir = XMVector3Cross(mRight, mUp);
            }
        }
        void Yaw(float angle) {
            using namespace DirectX;
            XMMATRIX R = XMMatrixRotationY(angle);//旋转轴为Y轴，得到旋转矩阵
            mRight = XMVector3TransformNormal(mRight, R);//矩阵变换后的up向量
            mDir = XMVector3TransformNormal(mDir, R);//矩阵变换后的look向量
            mViewDirty = true;
        }

        void UpdateViewMatrix();

        float* GetNative() {
            return mMatrixNative;
        }
    private:
        float mFovAngleY;
        float mAspectRatio;
        float mNearZ;
        float mFarZ;

        DirectX::XMMATRIX mProj;

        DirectX::XMVECTOR mPos;
        DirectX::XMVECTOR mDir;
        DirectX::XMVECTOR mUp;
        DirectX::XMVECTOR mRight;

        DirectX::XMMATRIX mView;

        bool mViewDirty = false;
        float mMatrixNative[32];
    };
}