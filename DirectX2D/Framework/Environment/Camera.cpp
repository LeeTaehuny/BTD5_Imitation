#include "Framework.h"

Camera::Camera()
{
    //���� ����
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

    // �� ���� 
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

