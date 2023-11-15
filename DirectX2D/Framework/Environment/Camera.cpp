#include "Framework.h"

Camera::Camera()
{
    //공간 버퍼
    viewBuffer = new MatrixBuffer();
    viewBuffer->SetPS(1);

    tag = "Camera";
}

Camera::~Camera()
{
    delete viewBuffer;
}

void Camera::Update()
{
    if (target != nullptr) FollowMode();
    else FreeMode();

    // 뷰 세팅 
    SetView();
}

void Camera::SetView()
{
    UpdateWorld();

    view = XMMatrixInverse(nullptr, world);

    viewBuffer->Set(view);
    viewBuffer->SetVS(1);
}

void Camera::FollowMode()
{
}

void Camera::FreeMode()
{
}

