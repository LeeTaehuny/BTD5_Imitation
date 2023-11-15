#pragma once

class Balloon;
class Tower;

// 실제 게임 중 전역으로 사용하기 위한 정보를 모아둔 매니저 클래스입니다.
class GameInstance : public Singleton<GameInstance>
{
private:
    friend class Singleton;

    GameInstance();
    ~GameInstance();

// Getter & Setter
public:
    vector<Balloon*> GetBalloons() { return balloons; }
    bool GetBackup() { return bIsBackup; }
    int GetLastMapId() { return lastMapId; }
    int GetCurHp() { return curHp; }
    int GetLastHp() { return lastHp; }
    int GetMoney() { return curMoney; }
    float GetGlobalSpeed() { return globalSpeed; }
    int GetLastMoney() { return lastMoney; }
    int GetLastRoundCnt() { return lastRoundCnt; }
    vector<Tower*> GetLastTowerInfo() { return lastTowerInfo; }
    bool GetBGMPlayable() { return bIsBGMPlayable; }
    bool GetSoundEffectPlayable() { return bIsSoundEffectPlayable; }

    void SetBackup(bool value) { bIsBackup = value; }
    void SetLastMapId(int value) { lastMapId = value; }
    void SetHp(int value) { curHp = value; }
    void DecreaseHp(int value) { curHp -= value; }
    void IncreaseMoney(int value) { curMoney += value; }
    void DecreaseMoney(int value) { curMoney -= value; }
    void SetMoney(int value) { curMoney = value; }
    void SetGlobalSpeed(float value) { globalSpeed = value; }
    void SetLastMoney(int value) { lastMoney = value; }
    void SetLastRoundCnt(int value) { lastRoundCnt = value; }
    void SetLastTowerInfo(vector<Tower*> towerInfo) { lastTowerInfo = towerInfo; }
    void SetLastHp(int value) { lastHp = value; }
    void SetBGMPlayable(bool value) { bIsBGMPlayable = value; }
    void SetSoundEffectPlayable(bool value) { bIsSoundEffectPlayable = value; }

// Game Data
private:
    // 풍선을 저장하기 위한 벡터 컨테이너를 선언합니다.
    vector<Balloon*> balloons;
    // 게인 진행 중 사용할 체력을 저장하기 위한 변수를 선언합니다.
    int curHp = 0;
    // 게임 진행 중 사용할 게임 머니를 저장하기 위한 변수를 선언합니다.
    int curMoney = 0;
    // 게임 진행 속도를 조절하기 위한 변수를 선언합니다.
    float globalSpeed = 1.0f;

// BackUp
private:
    // 백업 여부 판별용 정보
    bool bIsBackup = false;
    // 임시 종료시 저장될 맵 정보
    int lastMapId = 0;
    // 임시 종료시 저장될 체력 정보
    int lastHp = 200;
    // 임시 종료시 저장될 게임 머니
    int lastMoney = 650;
    // 임시 종료시 저장될 게임 라운드 정보
    int lastRoundCnt = 1;
    // 임시 종료시 저장될 타워 정보
    vector<Tower*> lastTowerInfo;

// Sound
private:
    // BGM On/Off 정보
    bool bIsBGMPlayable = true;
    // SoundEffect On/Off 정보
    bool bIsSoundEffectPlayable = true;
};

