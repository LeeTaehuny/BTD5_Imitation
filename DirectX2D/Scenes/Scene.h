#pragma once

class Scene
{
public:
    virtual ~Scene() {}

    //필수
    virtual void Update() = 0;    
    virtual void Render() = 0;

    //옵션
    virtual void Init() {}

protected:
    // 몬스터 이동 경로
    vector<Collider*> road;
    vector<Vector2> route;
};