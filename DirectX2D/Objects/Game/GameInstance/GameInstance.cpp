#include "Framework.h"
#include "GameInstance.h"
#include "Objects/Game/Balloon.h"
#include "Objects/Game/UI/Tower.h"

GameInstance::GameInstance()
{
	// 풍선 객체 생성
    balloons.resize(300);

    for (Balloon*& ballon : balloons)
    {
        ballon = new Balloon(15);
        ballon->SetActive(false);
    }
}

GameInstance::~GameInstance()
{
    for (Balloon* balloon : balloons)
        delete balloon;
}