#pragma once

class Camera : public Transform
{
public:
    Camera();
    ~Camera();

    void Update();
    void SetView();

    void SetTarget(Transform* target) { this->target = target; }

private:
    // ī�޶��� �þ� ���� ����
    void FollowMode(); // Follow-Cam : ī�޶� �������� �־ ������ ��ġ�� ��� ����
    void FreeMode();   // Free-Cam   : ī�޶� ������ ���� �Է¹��� ��� ��ġ ����

    //������ ���� ����
    MatrixBuffer* viewBuffer;
    Matrix view;

    float speed = 100.0f;

    // �ȷο� ķ ����� �� ������ �� �� �ִ� ǥ��
    Transform* target = nullptr; 
};

