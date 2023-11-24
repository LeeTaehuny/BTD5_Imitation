#pragma once

class Projectile;

// 풍선의 타입(속성)을 구분하기 위한 열거형을 선언합니다.
enum class Balloon_Type
{
    COMMON,             // 일반
    EXPLOSION_RESIST,   // 폭발 저항
    ICE_RESIST,         // 얼음 저항
    DUBLE_RESIST,       // 폭발 및 얼음 저항
    SHARP_RESIST,       // 날카로움 저항
    CAMO,               // 은신
    REGENERATE,         // 재생성
};

// 풍선의 상태를 구분하기 위한 열거형을 선언합니다.
enum Balloon_State
{
    NONE,
    SLOW,
    STOP,
};

// 맵 종류에 따라 잠시 안보여야 하는 구간이 존재할 수도 있습니다.
// * 맵을 구분하기 위한 열거형을 선언합니다.
enum class Map
{
    None,
    MonkeyLane,
    Tutorial,
    Patch,
};

class Balloon : public Quad
{
public:
    Balloon(int _hp);
    virtual ~Balloon() override;

    // 초기화 함수
    void Init();
    void SecondInit();

    void Update();
    virtual void Render() override;

// Getter & Setter
public:
    void SetShield(int _shield) { shield = _shield; }
    void SetHP(int _hp) { hp = _hp; }
    void SetSpeed(float _speed) { speed = _speed; }
    void SetRouteCnt(int cnt) { routeCnt = cnt; }
    int GetRouteCnt() { return routeCnt; }
    int GetShield() { return shield; }
    int GetHP() { return hp; }
    float GetMaxSpeed() { return maxSpeed; }
    float GetSpeed() { return speed; }
    bool GetHide() { return hide; }

    bool GetMeltDown() { return bIsMeltdown; }
    void SetMeltDown(bool value) { bIsMeltdown = value; }

    Balloon_State GetState() { return state; }
    void SetState(Balloon_State state) { this->state = state; }

    //void SetSlowTime(float time) { slowTime = time; }
    void SetSlowRate(float rate) { slowRate = rate; }
    void SetStopTime(float time) { stopTime = time; }
    void SetIsSlow(bool value) { bIsSlow = value; }
    
    void SetRoute(const vector<Vector2>& _route) { route = _route; }
    Collider* GetCollider() { return collider; }

    Balloon_Type GetType() { return type; }

    void SetMapType(Map type) { mapType = type; }

public:
    // 텍스처를 업데이트하기 위한 함수를 선언합니다.
    void UpdateTexture();
    // 풍선에 데미지를 적용하기 위한 함수를 선언합니다.
    void takeDamage(int damage, Projectile* p = nullptr);

private:
    // 사용할 텍스처를 미리 로드해두기 위한 함수를 선언합니다.
    void LoadTexture();
    // 풍선을 이동시키기 위한 함수를 선언합니다.
    void Move();

// Member Variable
private:
    int hp = 0;
    int shield = 0;
    float maxSpeed = 20.0f;
    float speed = 20.0f;
    float sumTime = 0.0f;
    bool hide = false;

    Vector2 startPos;
    Vector2 endPos;
    int routeCnt = 0;
    vector<Vector2> route;

    Collider* collider;

    Balloon_Type type;
    Vector2 direction;

    Map mapType = Map::None;

    float slowRate = 0.0f;
    float sTime = 0.0f;
    float stopTime = 0.0f;
    float meltTime = 0.0f;

    bool bIsSlow = false;
    bool bIsMeltdown = false;
    Balloon_State state = NONE;

    Balloon_State temp = NONE;

private:
    // 새로운 풍선을 생성하기 위한 함수들을 선언합니다.
    void CreatePinkBalloon(Projectile* p, float d);
    void CreateBlackBalloon(Projectile* p, float d);
    void CreateWhiteBalloon(Projectile* p, float d);
    void CreateRanbowBalloon(Projectile* p, float d);
};

