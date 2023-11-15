#pragma once

// 모든 버튼 UI의 기초가 되는 클래스입니다.
class Button : public Quad
{
protected:
    // 버튼의 현재 상태를 구분하기 위한 열거형을 선언합니다.
    enum State
    {
        NONE, DOWN, OVER
    };

    // 버튼에 상태에 따라 적용할 색상들을 저장합니다.
    const Float4 NONE_COLOR = { 1.0f, 1.0f, 1.0f, 1.0f };
    const Float4 DOWN_COLOR = { 0.5f, 0.5f, 0.5f, 1.0f };
    const Float4 OVER_COLOR = { 0.9f, 0.9f, 0.9f, 1.0f };
    
public:
    Button(wstring textureFile);
    Button(Vector2 size);
    virtual ~Button() override;

    virtual void Update();
    virtual void Render() override;

// Getter & Setter
public:
    void SetEvent(Event event) { this->event = event; }
    void SetParamEvent(ParamEvent event) { paramEvent = event; }
    void SetObject(void* object) { this->object = object; }

    RectCollider* GetCollider() { return collider; }

// Member Variable
private:
    RectCollider* collider;

    State state = NONE;
    
    bool isDownCheck = false;

    Event event = nullptr;
    ParamEvent paramEvent = nullptr;

    void* object = nullptr;
};