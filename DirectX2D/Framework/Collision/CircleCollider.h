#pragma once

class CircleCollider : public Collider
{
private:
    const UINT VERTEX_COUNT = 64;

public:
    CircleCollider(float radius = 1.0f);
    ~CircleCollider() = default;
        
    virtual bool IsPointCollision(Vector2 point) override;
    virtual bool IsRectCollision(RectCollider* rect, Vector2* overlap) override;
    virtual bool IsCircleCollision(CircleCollider* circle) override;

    float Radius() { return radius * max(GlobalScale().x, GlobalScale().y); }

    void SetRadius(float radius) { this->radius = radius; }
private:
    float radius;
};