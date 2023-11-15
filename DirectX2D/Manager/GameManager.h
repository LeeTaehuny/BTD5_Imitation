#pragma once

// 전체 게임 로직이 수행되는 클래스
class GameManager
{
public:
    GameManager();
    ~GameManager();

    void Update();
    void Render();
    
private:
    void Create();
    void Delete();

private:
    bool playing = false;
};