#include "Framework.h"
#include "Tower.h"
#include "Objects/Game/Balloon.h"
#include "Objects/Game/GameInstance/GameInstance.h"

Tower::Tower(wstring textureFile, TowerType type) : Button(textureFile), type(type)
{
    // 타워의 OnClick함수를 버튼의 이벤트에 등록합니다.
	SetEvent(bind(&Tower::OnClick, this));
}

Tower::~Tower()
{
}

void Tower::Update()
{
    Button::Update();
}

void Tower::Render()
{
    Quad::Render();
}

void Tower::AddEnemy(Balloon* enemy)
{
    // 전달받은 풍선이 적 배열에 존재하지 않는다면 추가합니다.
    if (find(enemies.begin(), enemies.end(), enemy) != enemies.end())
    {
        return;
    }

    enemies.push_back(enemy);
}

void Tower::RemoveEnemy(Balloon* enemy)
{
    // 전달받은 풍선이 적 배열에 존재한다면 삭제합니다.
    auto it = find(enemies.begin(), enemies.end(), enemy);
    if (it != enemies.end())
    {
        enemies.erase(it);
    }
}

void Tower::UpgradeL(int money)
{
    // 업그레이드가 불가능한 상황인지 체크합니다.
    if (towerLevel_L == 4) return;
    if (!isLeftUpgradable && towerLevel_L >= 2) return;

    // 업그레이드 가능하다고 설정합니다.
    towerLevel_L++;
    isLUpdatable = true;
    isUpdatable = true;

    // 해당 금액만큼 전체 금액에서 제외하고 타워의 전체 가격을 상승시킵니다.
    GameInstance::Get()->DecreaseMoney(money);
    totalMoney += money;

    // L 업그레이드가 2 이상이 된다면 오른쪽 업그레이드 가능 여부를 false로 설정합니다.
    if (towerLevel_L > 2)
    {
        isRightUpgradable = false;
    }
}

void Tower::UpgradeR(int money)
{
    // 업그레이드가 불가능한 상황인지 체크합니다.
    if (towerLevel_R == 4) return;
    if (!isRightUpgradable && towerLevel_R >= 2) return;

    // 업그레이드 가능하다고 설정합니다.
    towerLevel_R++;
    isRUpdatable = true;
    isUpdatable = true;

    // 해당 금액만큼 전체 금액에서 제외하고 타워의 전체 가격을 상승시킵니다.
    GameInstance::Get()->DecreaseMoney(money);
    totalMoney += money;

    // R 업그레이드가 2 이상이 된다면 왼쪽 업그레이드 가능 여부를 false로 설정합니다.
    if (towerLevel_R > 2)
    {
        isLeftUpgradable = false;
    }
}

void Tower::OnClick()
{
    // 타워UI 클릭 시 타워의 타입에 따라 돈을 차감하고 타워를 생성합니다.
    switch (type)
    {
    case Dart:
        if (GameInstance::Get()->GetMoney() >= 170)
            GameInstance::Get()->SetMoney(GameInstance::Get()->GetMoney() - 170);
        else 
            return;
        break;

    case Ice:
        if (GameInstance::Get()->GetMoney() >= 300)
            GameInstance::Get()->SetMoney(GameInstance::Get()->GetMoney() - 300);
        else
            return;
        break;

    case Tack:
        if (GameInstance::Get()->GetMoney() >= 360)
            GameInstance::Get()->SetMoney(GameInstance::Get()->GetMoney() - 360);
        else
            return;
        break;

    case Bomb:
        if (GameInstance::Get()->GetMoney() >= 650)
            GameInstance::Get()->SetMoney(GameInstance::Get()->GetMoney() - 650);
        else
            return;
        break;
    }

    Observer::Get()->ExcuteParamEvent("AddTower", this);
}

void Tower::UpdateColor()
{
    // 타워를 생산하기위한 돈이 모자라다면 빨간색, 충분하다면 원래 색으로 설정합니다.
    switch (type)
    {
    case Dart:
        if (GameInstance::Get()->GetMoney() < 170)
            GetColor() = { 1.0f, 0.0f, 0.0f, 1.0f };
        else
            GetColor() = { 1.0f, 1.0f, 1.0f, 1.0f };
        break;
    case Ice:
        if (GameInstance::Get()->GetMoney() < 300)
            GetColor() = { 1.0f, 0.0f, 0.0f, 1.0f };
        else
            GetColor() = { 1.0f, 1.0f, 1.0f, 1.0f };
        break;
    case Tack:
        if (GameInstance::Get()->GetMoney() < 360)
            GetColor() = { 1.0f, 0.0f, 0.0f, 1.0f };
        else
            GetColor() = { 1.0f, 1.0f, 1.0f, 1.0f };
        break;
    case Bomb:
        if (GameInstance::Get()->GetMoney() < 650)
            GetColor() = { 1.0f, 0.0f, 0.0f, 1.0f };
        else
            GetColor() = { 1.0f, 1.0f, 1.0f, 1.0f };
        break;
    }
}
