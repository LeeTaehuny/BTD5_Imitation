#include "Framework.h"
#include "InGameScene.h"

#include "Objects/Game/Balloon.h"
#include "Objects/Game/UI/Tower.h"
#include "Objects/Game/UI/UIButton.h"
#include "Objects/Game/Tower/DartMonkey.h"
#include "Objects/Game/Tower/IceTower.h"
#include "Objects/Game/Tower/BombTower.h"
#include "Objects/Game/Tower/TackShooter.h"
#include "Objects/Game/Projectile.h"
#include "Objects/Game/GameInstance/GameInstance.h"

// 생성자 함수
InGameScene::InGameScene()
{
	CreateUI();
	CreateRoundInfo();

	Observer::Get()->AddParamEvent("AddTower", bind(&InGameScene::AddTower, this, placeholders::_1));
	Observer::Get()->AddParamEvent("Select", bind(&InGameScene::SelectTower, this, placeholders::_1));
	Observer::Get()->AddParamEvent("Upgrade", bind(&InGameScene::Upgrade, this, placeholders::_1));
	Observer::Get()->AddEvent("Sell", bind(&InGameScene::SellTower, this));
	Observer::Get()->AddEvent("StartRound", bind(&InGameScene::StartRound, this));
	Observer::Get()->AddEvent("BackHome", bind(&InGameScene::EnterLobby, this));
	Observer::Get()->AddEvent("Restart", bind(&InGameScene::RestartGame, this));
	Observer::Get()->AddEvent("Option", bind(&InGameScene::OpenOption, this));
	Observer::Get()->AddEvent("CloseOption", bind(&InGameScene::CloseOption, this));

	Font::Get()->AddColor("White", 1, 1, 1);
	Font::Get()->AddStyle("PGothic", L"맑은 고딕");

	Font::Get()->SetColor("White");
	Font::Get()->SetStyle("PGothic");

}

InGameScene::~InGameScene()
{
	Delete();
}

void InGameScene::Init()
{
	// 맵 데이터 설정
	CreateMapData();
	// 풍선 생성 및 설정
	CreateBalloon();

	// 시작 상태 설정
	if (bIsNew)
	{
		bIsNew = false;
		SetState(State::NEW);
		return;
	}
	else
	{
		SetState(State::WAIT);
	}

	// 시작 머니 설정
	GameInstance::Get()->SetMoney(GameInstance::Get()->GetLastMoney());
	// 라운드 수 설정
	roundCnt = GameInstance::Get()->GetLastRoundCnt();
	// 시작 체력 설정
	GameInstance::Get()->SetHp(GameInstance::Get()->GetLastHp());
	// 시작 타워 설정
	towers = GameInstance::Get()->GetLastTowerInfo();

	// 타워 업그레이드 정보 불러오기
	for (Tower* tower : towers)
	{
		if (tower->GetType() == Dart)
		{
			DartMonkey* dt = (DartMonkey*)tower;
			dt->LastUpgradeSetting();
		}
		else if (tower->GetType() == Bomb)
		{
			BombTower* bt = (BombTower*)tower;
			bt->LastUpgradeSetting();
		}
		else if (tower->GetType() == Ice)
		{
			IceTower* it = (IceTower*)tower;
			it->LastUpgradeSetting();
		}
		else if (tower->GetType() == Tack)
		{
			TackShooter* tt = (TackShooter*)tower;
			tt->LastUpgradeSetting();
		}

		tower->Select(false);
	}
}

void InGameScene::Update()
{
	bool isPlaying = true;

	switch (state)
	{
	case State::NEW:
		// 시작 머니 초기화
		GameInstance::Get()->SetMoney(650);
		// 라운드 수 초기화
		roundCnt = 1;
		// 체력 초기화
		GameInstance::Get()->SetHp(200);
		// 저장된 타워 초기화
		if (!GameInstance::Get()->GetLastTowerInfo().empty())
		{
			for (Tower* tower : GameInstance::Get()->GetLastTowerInfo())
			{
				delete tower;
				tower = nullptr;
			}
		}
		// 시작 타워 초기화
		if (towers.size())
		{
			for (Tower* tower : towers)
				delete tower;
			towers.clear();
		}

		SetState(State::WAIT);
		break;

	case State::WAIT:
		// 삭제 예약 풍선들 삭제
		if (deleteTowers.size())
		{
			for (Tower* deleteTower : deleteTowers)
			{
				towers.erase(find(towers.begin(), towers.end(), deleteTower));
				delete deleteTower;
			}

			deleteTowers.clear();
		}

		// 라운드 정보 백업 (돈, 라운드 수, 타워 정보, 현재 hp)
		GameInstance::Get()->SetLastMoney(GameInstance::Get()->GetMoney());
		GameInstance::Get()->SetLastRoundCnt(roundCnt);
		GameInstance::Get()->SetLastTowerInfo(towers);
		GameInstance::Get()->SetLastHp(GameInstance::Get()->GetCurHp());

		// 타워 업그레이드 정보 백업
		for (Tower* tower : towers)
		{
			tower->SetLastUpgradeLevel_L(tower->GetUpgradeLevel_L());
			tower->SetLastUpgradeLevel_R(tower->GetUpgradeLevel_R());
		}
		break;

	case State::PLAY:
		// 활성화된 풍선이 존재하는지 체크
		for (Balloon* balloon : balloons)
		{
			if (balloon->Active())
			{
				isPlaying = false;
			}
		}

		// 활성화된 풍선이 존재하지 않고, 모든 풍선이 스폰된 상태라면 라운드 카운트를 증가시키고 준비 상태로 이동
		if (spawnCnt == maxSpawnCnt && isPlaying)
		{
			roundCnt++;
			spawnCnt = 0;

			Start->SetDubleSpeed(false);
			Start->SetTexture(L"Textures/UI/start.png");
			GameInstance::Get()->SetGlobalSpeed(1.0f);
			GameInstance::Get()->IncreaseMoney(100);

			if (roundCnt >= 41)
			{
				state = State::VICTORY;
				return;
			}

			state = State::WAIT;
			break;
		}

		if (spawnCnt == maxSpawnCnt)
		{
			break;
		}

		sumTime += DELTA;
		// 스폰할 풍선 수가 50이 넘어가면 스폰 주기 감소
		if (maxSpawnCnt >= 50)
		{
			spawnTime = 0.5f;
		}
		else
		{
			spawnTime = 1.0f;
		}

		if (sumTime >= spawnTime / GameInstance::Get()->GetGlobalSpeed())
		{
			// 비활성화 되어있는 풍선 중 하나를 라운드 정보와 스폰 카운트에 맞춰 스폰
			for (Balloon* balloon : balloons)
			{
				if (!balloon->Active())
				{
					balloon->SetHP(roundInfo[roundCnt][spawnCnt] - 'a' + 1);
					balloon->Init();

					balloon->SetActive(true);

					spawnCnt++;
					break;
				}
			}
			sumTime = 0.0f;
		}
		break;

	case State::STOP:
		// 설정창을 눌러 화면이 정지된 상태에서의 업데이트
		optionWindow->SetActive(true);
		close->Update();
		BGMsound->Update();
		soundEffect->Update();
		backHome->Update();
		restart->Update();
		return;
		break;

	case State::DEFEAT:
		// 체력이 0이된 상태에서의 업데이트
		defeatWindow->SetActive(true);
		backHome->Update();
		restart->Update();
		return;
		break;

	case State::VICTORY:
		// 모든 라운드를 클리어한 상태에서의 업데이트
		roundCnt = 40;
		victoryWindow->SetActive(true);
		backHome->Update();
		restart->Update();
		return;
		break;
	}

	// 체력이 0이되었는지 체크
	if (GameInstance::Get()->GetCurHp() <= 0)
	{
		GameInstance::Get()->SetHp(0);
		state = State::DEFEAT;
	}

	// 튜토리얼 맵에서 사용 가능한 테스트 함수
	if (map == MapType::Tutorial)
	{
		Test();
	}

	for (auto r : road)
		r->UpdateWorld();

	for (Tower* t_UI : towerUI)
		t_UI->Update();

	for (Tower* tower : towers)
		tower->Update();

	for (Balloon* ballon : balloons)
		ballon->Update();

	Start->Update();
	Option->Update();
	SetTower();
	Select();
	CheckRange();
}

void InGameScene::Render()
{
	background->Render();

	for (Tower* t_UI : towerUI)
	{
		t_UI->UpdateColor();
		t_UI->Render();
	}

	for (Tower* tower : towers)
		tower->Render();

	for (Balloon* balloon : balloons)
		balloon->Render();

	if (tempTower)
	{
		tempTower->Render();
	}

	// 선택한 타워에 연관된 UI 출력
	if (selectTower)
	{
		UIButtonRenderBegin();
		UpgradeL->Render();
		UpgradeR->Render();
		Sell->Render();
		wstring gold = L"+$" + to_wstring((int)(selectTower->GetTotalMoney() * 0.8f));
		Font::Get()->RenderText(gold, { 825, 60 });
	}

	Start->Render();
	Option->Render();

	// 설정창과 관련된 UI 출력
	if (optionWindow->Active())
	{
		optionWindow->Render();
		close->Render();
		backHome->Render();
		restart->Render();
		BGMsound->Render();
		soundEffect->Render();
	}
	RenderText();
}

void InGameScene::Delete()
{
	// 타워 제거
	if (towers.size())
	{
		for (Tower* tower : towers)
			delete tower;
		towers.clear();
	}

	// 풍선 제거
	if (balloons.size())
	{
		GameInstance::Get()->Delete();
		balloons.clear();
	}

	// UI 제거
	if (towerUI.size())
	{
		for (Tower* t_UI : towerUI)
			delete t_UI;
		towerUI.clear();
	}

	if (UpgradeL) 
	{
		delete UpgradeL;
		UpgradeL = nullptr;
	}
	if (UpgradeR)
	{
		delete UpgradeR;
		UpgradeR = nullptr;
	}
	if (Sell)
	{
		delete Sell;
		Sell = nullptr;
	}
	// 배경 제거
	if (background)
	{
		delete background;
		background = nullptr;
	}

	// 길 충돌체 제거
	if (road.size())
	{
		for (Collider* r : road)
			delete r;
		road.clear();
	}

	// 경로 제거
	route.clear();

	// 패배 & 승리 창 제거
	if (defeatWindow)
	{
		delete defeatWindow;
		defeatWindow = nullptr;
	}

	if (victoryWindow)
	{
		delete victoryWindow;
		victoryWindow = nullptr;
	}

	if (backHome)
	{
		delete backHome;
		backHome = nullptr;
	}

	if (restart)
	{
		delete restart;
		restart = nullptr;
	}
}

void InGameScene::CreateMapData()
{
	// 맵 생성 전 기존에 저장된 맵이 있다면 삭제
	if (background != nullptr)
	{
		delete background;
		background = nullptr;
	}

	if (route.size())
	{
		route.clear();
	}

	if (road.size())
	{
		for (Collider* r : road)
		{
			delete r;
		}
		road.clear();
	}

	switch (map)
	{
	case MapType::MonkeyLane:
		// 배경
		{
			background = new Quad(L"Textures/Maps/MonkyLane.png");
			background->Pos() = { CENTER_X,CENTER_Y };
			background->UpdateWorld();
		}

		// 맵 정보 (경로)
		{
			route.push_back(Vector2(0, 337));
			route.push_back(Vector2(162, 337));
			route.push_back(Vector2(162, 410));
			route.push_back(Vector2(271, 410));
			route.push_back(Vector2(271, 160));
			route.push_back(Vector2(425, 160));
			route.push_back(Vector2(425, 190)); // 안보이는 구간 시작 6
			route.push_back(Vector2(425, 340)); // 안보이는 구간 종료 7
			route.push_back(Vector2(425, 485));
			route.push_back(Vector2(162, 485));
			route.push_back(Vector2(162, 560));
			route.push_back(Vector2(515, 560));
			route.push_back(Vector2(515, 485));
			route.push_back(Vector2(605, 485));
			route.push_back(Vector2(605, 410));
			route.push_back(Vector2(515, 410));
			route.push_back(Vector2(515, 265));
			route.push_back(Vector2(350, 265)); // 안보이는 구간 시작 17
			route.push_back(Vector2(195, 265)); // 안보이는 구간 종료 18
			route.push_back(Vector2(160, 265));
			route.push_back(Vector2(160, 125));
		}
		
		// 맵 정보 (길 충돌 체크)
		{
			road.push_back(new RectCollider(Vector2(180.0f, 25.0f)));
			road[road.size() - 1]->Pos() = Vector2(90.0f, 337.5f);
			road.push_back(new RectCollider(Vector2(30.0f, 105.0f)));
			road[road.size() - 1]->Pos() = Vector2(165.0f, 377.5f);
			road.push_back(new RectCollider(Vector2(140.0f, 30.0f)));
			road[road.size() - 1]->Pos() = Vector2(220.0f, 415.0f);
			road.push_back(new RectCollider(Vector2(35.0f, 280.0f)));
			road[road.size() - 1]->Pos() = Vector2(272.5f, 290.0f);
			road.push_back(new RectCollider(Vector2(195.0f, 30.0f)));
			road[road.size() - 1]->Pos() = Vector2(352.5f, 165.0f);
			road.push_back(new RectCollider(Vector2(45.0f, 15.0f)));
			road[road.size() - 1]->Pos() = Vector2(427.5f, 187.5f);
			road.push_back(new RectCollider(Vector2(45.0f, 165.0f)));
			road[road.size() - 1]->Pos() = Vector2(427.5f, 417.5f);
			road.push_back(new RectCollider(Vector2(305.0f, 25.0f)));
			road[road.size() - 1]->Pos() = Vector2(297.5f, 487.5f);
			road.push_back(new RectCollider(Vector2(30.0f, 100.0f)));
			road[road.size() - 1]->Pos() = Vector2(160.0f, 525.0f);
			road.push_back(new RectCollider(Vector2(385.0f, 25.0f)));
			road[road.size() - 1]->Pos() = Vector2(337.5f, 562.5f);
			road.push_back(new RectCollider(Vector2(35.0f, 100.0f)));
			road[road.size() - 1]->Pos() = Vector2(512.5f, 525.0f);
			road.push_back(new RectCollider(Vector2(130.0f, 30.0f)));
			road[road.size() - 1]->Pos() = Vector2(560.0f, 490.0f);
			road.push_back(new RectCollider(Vector2(30.0f, 110.0f)));
			road[road.size() - 1]->Pos() = Vector2(610.0f, 450.0f);
			road.push_back(new RectCollider(Vector2(130.0f, 25.0f)));
			road[road.size() - 1]->Pos() = Vector2(560.0f, 407.5f);
			road.push_back(new RectCollider(Vector2(35.0f, 175.0f)));
			road[road.size() - 1]->Pos() = Vector2(512.5f, 332.5f);
			road.push_back(new RectCollider(Vector2(185.0f, 40.0f)));
			road[road.size() - 1]->Pos() = Vector2(437.5f, 265.0f);
			road.push_back(new RectCollider(Vector2(60.0f, 35.0f)));
			road[road.size() - 1]->Pos() = Vector2(170.0f, 262.5f);
			road.push_back(new RectCollider(Vector2(40.0f, 155.0f)));
			road[road.size() - 1]->Pos() = Vector2(160.0f, 202.5f);

			road.push_back(new RectCollider(Vector2(80.0f, 115.0f)));
			road[road.size() - 1]->Pos() = Vector2(40.0f, 457.5f);
			road.push_back(new RectCollider(Vector2(65.0f, 85.0f)));
			road[road.size() - 1]->Pos() = Vector2(32.5f, 232.5f);
			road.push_back(new RectCollider(Vector2(70.0f, 110.0f)));
			road[road.size() - 1]->Pos() = Vector2(685.0f, 415.0f);
			road.push_back(new RectCollider(Vector2(105.0f, 115.0f)));
			road[road.size() - 1]->Pos() = Vector2(667.5f, 287.5f);
		}

		break;

	case MapType::Tutorial:
		// 배경
		{
			background = new Quad(L"Textures/Maps/TestMap.png");
			background->Pos() = { CENTER_X,CENTER_Y };
			background->UpdateWorld();
		}

		// 맵 정보(경로)
		{
			route.push_back(Vector2(0.0f, 337.0f));
			route.push_back(Vector2(720.0f, 337.0f));
		}

		// 맵 정보(길 충돌 체크)
		{
			road.push_back(new RectCollider(Vector2(720.0f, 25.0f)));
			road[road.size() - 1]->Pos() = Vector2(360.0f, 337.5f);
		}

		break;

	case MapType::Patch:
		// 배경
		{	
		background = new Quad(L"Textures/Maps/Patch.png");
		background->Pos() = { CENTER_X,CENTER_Y };
		background->UpdateWorld();
		}

		// 맵 정보(경로)
		{
			route.push_back(Vector2(720.0f, 515.0f));
			route.push_back(Vector2(120.0f, 515.0f));
			route.push_back(Vector2(35.0f, 495.0f));
			route.push_back(Vector2(35.0f, 420.0f));
			route.push_back(Vector2(645.0f, 400.0f));
			route.push_back(Vector2(690.0f, 380.0f));
			route.push_back(Vector2(690.0f, 310.0f));
			route.push_back(Vector2(645.0f, 285.0f));
			route.push_back(Vector2(80.0f, 280.0f));
			route.push_back(Vector2(50.0f, 270.0f));
			route.push_back(Vector2(40.0f, 240.0f));
			route.push_back(Vector2(50.0f, 200.0f));
			route.push_back(Vector2(540.0f, 200.0f));
			route.push_back(Vector2(720.0f, 190.0f));
		}

		// 맵 정보(길 충돌 체크)
		{
			road.push_back(new RectCollider(Vector2(710.0f, 35.0f)));
			road[road.size() - 1]->Pos() = Vector2(365.0f, 512.5f);
			road.push_back(new RectCollider(Vector2(40.0f, 150.0f)));
			road[road.size() - 1]->Pos() = Vector2(30.0f, 455.0f);
			road.push_back(new RectCollider(Vector2(380.0f, 60.0f)));
			road[road.size() - 1]->Pos() = Vector2(200.0f, 410.0f);
			road.push_back(new RectCollider(Vector2(160.0f, 45.0f)));
			road[road.size() - 1]->Pos() = Vector2(470.0f, 417.5f);
			road.push_back(new RectCollider(Vector2(125.0f, 45.0f)));
			road[road.size() - 1]->Pos() = Vector2(612.5f, 402.5f);
			road.push_back(new RectCollider(Vector2(35.0f, 110.0f)));
			road[road.size() - 1]->Pos() = Vector2(692.5f, 355.0f);
			road.push_back(new RectCollider(Vector2(640.0f, 30.0f)));
			road[road.size() - 1]->Pos() = Vector2(390.0f, 285.0f);
			road.push_back(new RectCollider(Vector2(30.0f, 30.0f)));
			road[road.size() - 1]->Pos() = Vector2(55.0f, 285.0f);
			road.push_back(new RectCollider(Vector2(50.0f, 95.0f)));
			road[road.size() - 1]->Pos() = Vector2(45.0f, 222.5f);
			road.push_back(new RectCollider(Vector2(650.0f, 40.0f)));
			road[road.size() - 1]->Pos() = Vector2(395.0f, 195.0f);

			road.push_back(new RectCollider(Vector2(75.0f, 70.0f)));
			road[road.size() - 1]->Pos() = Vector2(37.5f, 565.0f);
			road.push_back(new RectCollider(Vector2(105.0f, 70.0f)));
			road[road.size() - 1]->Pos() = Vector2(342.5f, 345.0f);
		}

		break;
	}
}

void InGameScene::CreateUI()
{
	// 사용할 UI 생성 및 초기화
	Tower* tower = new Tower(L"Textures/Character/dart_monkey/d_base.png", Dart);
	tower->Pos() = Vector2(765.0f, 390.0f);
	towerUI.push_back(tower);

	tower = new Tower(L"Textures/Character/ice_tower/i_base.png", Ice);
	tower->Pos() = Vector2(855.0f, 390.0f);
	towerUI.push_back(tower);

	tower = new Tower(L"Textures/Character/bomb_tower/b_base.png", Bomb);
	tower->Pos() = Vector2(765.0f, 270.0f);
	towerUI.push_back(tower);

	tower = new Tower(L"Textures/Character/tack_shooter/t_base.png", Tack);
	tower->Pos() = Vector2(855.0f, 270.0f);
	towerUI.push_back(tower);

	UpgradeL = new UIButton(L"Textures/UI/Upgrade/upgrade_base.png", B_Type::L_Upgrade);
	UpgradeL->Pos() = Vector2(181.0f, 63.5f);

	UpgradeR = new UIButton(L"Textures/UI/Upgrade/upgrade_base.png", B_Type::R_Upgrade);
	UpgradeR->Pos() = Vector2(543.0f, 63.5f);

	Sell = new UIButton(L"Textures/UI/Sell.png", B_Type::Sell);
	Sell->Pos() = Vector2(810.0f, 90.0f);

	Start = new UIButton(L"Textures/UI/start.png", B_Type::StartRound);
	Start->Pos() = Vector2(690.0f, 575.0f);

	Option = new UIButton(L"Textures/UI/setting.png", B_Type::Option);
	Option->Pos() = Vector2(690.0f, 525.0f);
	
	optionWindow = new Quad(L"Textures/UI/option.png");
	optionWindow->Pos() = Vector2(CENTER_X, CENTER_Y);
	optionWindow->UpdateWorld();
	optionWindow->SetActive(false);

	defeatWindow = new Quad(L"Textures/UI/defeat.png");
	defeatWindow->Pos() = Vector2(CENTER_X, CENTER_Y);
	defeatWindow->UpdateWorld();
	defeatWindow->SetActive(false);

	victoryWindow = new Quad(L"Textures/UI/victory.png");
	victoryWindow->Pos() = Vector2(CENTER_X, CENTER_Y + 30.0f);
	victoryWindow->UpdateWorld();
	victoryWindow->SetActive(false);

	backHome = new UIButton(L"Textures/UI/backHome.png", B_Type::Home);
	backHome->Pos() = Vector2(345.0f, 185.0f);

	restart = new UIButton(L"Textures/UI/restart.png", B_Type::Restart);
	restart->Pos() = Vector2(545.0f, 185.0f);

	close = new UIButton(L"Textures/UI/close.png", B_Type::CloseOption);
	close->Pos() = Vector2(610.0f, 425.0f);

	BGMsound = new UIButton(L"Textures/UI/soundOn.png", B_Type::BGM_Sound);
	BGMsound->Pos() = Vector2(540.0f, 355.0f);

	soundEffect = new UIButton(L"Textures/UI/soundOn.png", B_Type::SoundEffect);
	soundEffect->Pos() = Vector2(540.0f, 280.0f);
}

void InGameScene::CreateBalloon()
{
	// 해당 씬에서 사용할 풍선들 초기화
	balloons = GameInstance::Get()->GetBalloons();

	for (Balloon* balloon : balloons)
	{
		balloon->SetRoute(route);
		balloon->SetMapType((Map)map);
	}
}

void InGameScene::CreateRoundInfo()
{
	// 아스키 코드표에 따른 연산
	roundInfo[1] = "aaaaaaaaaaaaaaaaaaaa";
	roundInfo[2] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	roundInfo[3] = "aaaaaaaaaaaaaaaaaaaabbbbb";
	roundInfo[4] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbb";
	roundInfo[5] = "aaaaabbbbbbbbbbbbbbbbbbbbbbbbb";
	roundInfo[6] = "aaaaaaaaaaaaaaabbbbbbbbbbbbbbbcccc";
	roundInfo[7] = "aaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbccccc";
	roundInfo[8] = "aaaaaaaaaabbbbbbbbbbbbbbbbbbbbcccccccccccccc";
	roundInfo[9] = "cccccccccccccccccccccccccccccc";
	roundInfo[10] = "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
	roundInfo[11] = "aaaaaaaaaabbbbbbbbbbccccccccccccdd";
	roundInfo[12] = "bbbbbbbbbbbbbbbccccccccccddddd";
	roundInfo[13] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaacccccccccccccccccccccccdddd";
	roundInfo[14] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbccccccccccddddddddd";
	roundInfo[15] = "aaaaaaaaaaaaaaaaaaaaccccccccccccdddddeee";
	roundInfo[16] = "ccccccccccccccccccccddddddddeeee";
	roundInfo[17] = "vvvvvvvv";
	roundInfo[18] = "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc";
	roundInfo[19] = "ccccccccccddddvvvvveeeeeee";
	roundInfo[20] = "gggggg";
	roundInfo[21] = "eeeeeeeeeeeeee";
	roundInfo[22] = "iiiiiiii";
	roundInfo[23] = "gggggggiiii";
	roundInfo[24] = "x";
	roundInfo[25] = "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv";
	roundInfo[26] = "eeeeeeeeeeeeeeeeeeeeeeemmmm";
	roundInfo[27] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccccccccddddddddddddddddddddddddddddddddddddddddddddd";
	roundInfo[28] = "kkkk";
	roundInfo[29] = "dddddddddddddddddddddddddvvvvvvvvvvvv";
	roundInfo[30] = "kkkkkkkkk";
	roundInfo[31] = "mmmmmmmmvvvvvvvvvv";
	roundInfo[32] = "gggggggggggggggggggggggggiiiiiiiiiiiiiiiiiiiiiiiiiiiikkkkkkkk";
	roundInfo[33] = "yyyyyyyyyyyyyyyyyyyy";
	roundInfo[34] = "ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddmmmmm";
	roundInfo[35] = "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeiiiiiiiiiiiiiiiiiiiiiiiiooooo";
	roundInfo[36] = "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee";
	roundInfo[37] = "ggggggggggggggggggggiiiiiiiiiiiiiiiiiiiizzzzzkkkkkkkkkkkkkkkmmmmmmmmmm";
	roundInfo[38] = "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvviiiiiiiiiiiiiiiiikkkkkkkkkkkkkkmmmmmmmmmmoooo";
	roundInfo[39] = "ggggggggggiiiiiiiiiikkkkkkkkkkkkkkkkkkkmmmmmmmmmmmmmmmmmmmmoooooooooooooooooo";
	roundInfo[40] = "ooooooooooqqqq";
}

void InGameScene::AddTower(void* tower)
{
	// TowerUI 클릭 시 임시 타워 생성 & 돈 차감
	Tower* t = (Tower*)tower;

	if (t->GetType() == Dart)
	{
		tempTower = new DartMonkey();
		tempTower->AddTotalMoney(170);
	}
	else if (t->GetType() == Ice)
	{
		tempTower = new IceTower();
		tempTower->AddTotalMoney(300);
	}
	else if (t->GetType() == Bomb)
	{
		tempTower = new BombTower();
		tempTower->AddTotalMoney(650);
	}
	else if (t->GetType() == Tack)
	{
		tempTower = new TackShooter();
		tempTower->AddTotalMoney(360);
	}

	if (selectTower)
	{
		selectTower->Select(false);
	}

	selectTower = tempTower;
	selectTower->Select(true);
}

void InGameScene::SelectTower(void* tower)
{
	// 맵에 존재하는 타워 선택 시 Select타워 설정
	Tower* t = (Tower*)tower;

	if (selectTower)
	{
		selectTower->Select(false);
	}

	selectTower = t;
	selectTower->Select(true);
}

void InGameScene::SetTower()
{
	// 타워를 설치(설치 가능 여부 판별)
	if (tempTower == nullptr) return;

	bool isInstallable = true;

	for (Tower* tower : towers)
	{
		if (tower->GetCollider()->Active() && tower->GetSelfCollider()->IsCircleCollision((CircleCollider*)tempTower->GetSelfCollider()))
		{
			isInstallable = false;
		}
	}

	for (auto r : road)
	{
		if (r->IsCollision(tempTower->GetSelfCollider()))
		{
			isInstallable = false;
		}
	}

	if ((mousePos.x >= 700.0f || mousePos.x < 20.0f) || (mousePos.y < 145 || mousePos.y > 580))
	{
		isInstallable = false;
	}

	if (KEY_PRESS(VK_LBUTTON))
	{
		tempTower->Pos() = mousePos;
		tempTower->Update();
	}

	if (!isInstallable)
	{
		tempTower->GetColor() = { 1.0f, 0.0f, 0.0f, 1.0f };

		if (KEY_UP(VK_LBUTTON))
		{
			if ((mousePos.x >= 700.0f || mousePos.x < 20.0f) || (mousePos.y < 145 || mousePos.y > 580))
			{
				// 설치를 취소한 경우 해당 타워의 금액만큼 게임머니 추가
				switch (tempTower->GetType())
				{
				case Dart:
					GameInstance::Get()->IncreaseMoney(170);
					break;
				case Ice:
					GameInstance::Get()->IncreaseMoney(300);
					break;
				case Tack:
					GameInstance::Get()->IncreaseMoney(360);
					break;
				case Bomb:
					GameInstance::Get()->IncreaseMoney(650);
					break;
				}

				tempTower->Select(false);
				delete tempTower;
				tempTower = nullptr;
				selectTower = nullptr;
			}
		}
		return;
	}

	// 타워 설치 시 towers 배열에 추가
	if (KEY_UP(VK_LBUTTON))
	{
		towers.push_back(tempTower);
		tempTower = nullptr;
	}
}

void InGameScene::CheckRange()
{
	for (Tower* tower : towers)
	{
		for (Balloon* balloon : balloons)
		{
			// 타워의 공격 범위에 들어온 풍선을 해당 타워의 적 배열에 추가
			if (tower->GetSelfAttackRange()->IsCircleCollision((CircleCollider*)balloon->GetCollider()))
			{
				if (balloon->GetType() == Balloon_Type::CAMO && tower->GetData().isCamoDetectable == false) continue;

				tower->AddEnemy(balloon);
			}
		}

		vector<Balloon*> enemies = tower->GetEnemies();

		for (Balloon* balloon : enemies)
		{
			// 타워의 적 배열에 존재하는 풍선 중 해당 풍선이 비활성화 되어있다면 풍선을 적 배열에서 제거
			if (!balloon->Active())
			{
				tower->RemoveEnemy(balloon);
				continue;
			}

			// 타워의 적 배열에 존재하는 풍선 중 해당 타워의 공격 범위를 벗어난 풍선을 적 배열에서 제거
			if (!tower->GetSelfAttackRange()->IsCircleCollision((CircleCollider*)balloon->GetCollider()))
			{
				tower->RemoveEnemy(balloon);
			}
		}
	}
}

void InGameScene::Upgrade(void* button)
{
	// 업그레이드 버튼이 클릭된 경우 업그레이드 머니 설정 & 업그레이드 함수 호출
	UIButton* ui = (UIButton*)button;

	int upgradeMoney = 0;

	if (ui->GetType() == B_Type::L_Upgrade)
	{
		int level = selectTower->GetUpgradeLevel_L();

		switch (selectTower->GetType())
		{
		case Dart:
			switch (level)
			{
			case 0:
				upgradeMoney = 75;
				break;

			case 1:
				upgradeMoney = 100;
				break;

			case 2:
				upgradeMoney = 425;
				break;

			case 3:
				upgradeMoney = 1275;
				break;
			}
			break;
		case Ice:
			switch (level)
			{
			case 0:
				upgradeMoney = 190;
				break;

			case 1:
				upgradeMoney = 340;
				break;

			case 2:
				upgradeMoney = 1050;
				break;

			case 3:
				upgradeMoney = 2400;
				break;
			}
			break;
		case Tack:
			switch (level)
			{
			case 0:
				upgradeMoney = 210;
				break;

			case 1:
				upgradeMoney = 300;
				break;

			case 2:
				upgradeMoney = 500;
				break;

			case 3:
				upgradeMoney = 2500;
				break;
			}
			break;
		case Bomb:
			switch (level)
			{
			case 0:
				upgradeMoney = 170;
				break;

			case 1:
				upgradeMoney = 255;
				break;

			case 2:
				upgradeMoney = 680;
				break;

			case 3:
				upgradeMoney = 3400;
				break;
			}
			break;
		}

		// 업그레이드 가능한지 체크
		if (GameInstance::Get()->GetMoney() - upgradeMoney >= 0)
		{
			selectTower->UpgradeL(upgradeMoney);
		}
	}
	else if (ui->GetType() == B_Type::R_Upgrade)
	{
		int level = selectTower->GetUpgradeLevel_R();

		switch (selectTower->GetType())
		{
		case Dart:
			switch (level)
			{
			case 0:
				upgradeMoney = 120;
				break;

			case 1:
				upgradeMoney = 145;
				break;

			case 2:
				upgradeMoney = 280;
				break;

			case 3:
				upgradeMoney = 1300;
				break;
			}
			break;
		case Ice:
			switch (level)
			{
			case 0:
				upgradeMoney = 85;
				break;

			case 1:
				upgradeMoney = 295;
				break;

			case 2:
				upgradeMoney = 1100;
				break;

			case 3:
				upgradeMoney = 1700;
				break;
			}
			break;
		case Tack:
			switch (level)
			{
			case 0:
				upgradeMoney = 100;
				break;

			case 1:
				upgradeMoney = 230;
				break;

			case 2:
				upgradeMoney = 680;
				break;

			case 3:
				upgradeMoney = 2500;
				break;
			}
			break;
		case Bomb:
			switch (level)
			{
			case 0:
				upgradeMoney = 340;
				break;

			case 1:
				upgradeMoney = 340;
				break;

			case 2:
				upgradeMoney = 765;
				break;

			case 3:
				upgradeMoney = 1650;
				break;
			}
			break;
		}

		// 업그레이드 가능한지 체크
		if (GameInstance::Get()->GetMoney() - upgradeMoney >= 0)
		{
			selectTower->UpgradeR(upgradeMoney);
		}
	}
}

void InGameScene::UIButtonRenderBegin()
{
	// 현재 타워의 업그레이드 레벨을 받아오기
	int level_L = selectTower->GetUpgradeLevel_L();
	int level_R = selectTower->GetUpgradeLevel_R();

	if (level_L >= 3 && level_R == 2)
	{
		level_R = 5;
	}
	else if (level_R >= 3 && level_L == 2)
	{
		level_L = 5;
	}

	// 업그레이드 레벨에 따른 업그레이드 UI 갱신
	// * 업그레이드 금액이 부족하면 색상 변화
	if (selectTower->GetType() == Dart)
	{
		switch (level_L)
		{
		case 0:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/dart_monkey/l1.png");
			if (GameInstance::Get()->GetMoney() < 75)
			{
				UpgradeL->SetUpgradeableL(false);
				UpgradeL->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeL->SetUpgradeableL(true);
				UpgradeL->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 1:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/dart_monkey/l2.png");
			if (GameInstance::Get()->GetMoney() < 100)
			{
				UpgradeL->SetUpgradeableL(false);
				UpgradeL->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeL->SetUpgradeableL(true);
				UpgradeL->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 2:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/dart_monkey/l3.png");
			if (GameInstance::Get()->GetMoney() < 425)
			{
				UpgradeL->SetUpgradeableL(false);
				UpgradeL->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeL->SetUpgradeableL(true);
				UpgradeL->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 3:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/dart_monkey/l4.png");
			if (GameInstance::Get()->GetMoney() < 1275)
			{
				UpgradeL->SetUpgradeableL(false);
				UpgradeL->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeL->SetUpgradeableL(true);
				UpgradeL->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 4:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/upgrade_max.png");
			break;
		case 5:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/upgrade_end.png");
			break;
		}

		switch (level_R)
		{
		case 0:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/dart_monkey/r1.png");
			if (GameInstance::Get()->GetMoney() < 120)
			{
				UpgradeR->SetUpgradeableR(false);
				UpgradeR->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeR->SetUpgradeableR(true);
				UpgradeR->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 1:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/dart_monkey/r2.png");
			if (GameInstance::Get()->GetMoney() < 145)
			{
				UpgradeR->SetUpgradeableR(false);
				UpgradeR->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeR->SetUpgradeableR(true);
				UpgradeR->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 2:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/dart_monkey/r3.png");
			if (GameInstance::Get()->GetMoney() < 280)
			{
				UpgradeR->SetUpgradeableR(false);
				UpgradeR->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeR->SetUpgradeableR(true);
				UpgradeR->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 3:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/dart_monkey/r4.png");
			if (GameInstance::Get()->GetMoney() < 1300)
			{
				UpgradeR->SetUpgradeableR(false);
				UpgradeR->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeR->SetUpgradeableR(true);
				UpgradeR->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 4:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/upgrade_max.png");
			break;
		case 5:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/upgrade_end.png");
			break;
		}
	}

	if (selectTower->GetType() == Tack)
	{
		switch (level_L)
		{
		case 0:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/tack_shooter/l1.png");
			if (GameInstance::Get()->GetMoney() < 210)
			{
				UpgradeL->SetUpgradeableL(false);
				UpgradeL->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeL->SetUpgradeableL(true);
				UpgradeL->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 1:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/tack_shooter/l2.png");
			if (GameInstance::Get()->GetMoney() < 300)
			{
				UpgradeL->SetUpgradeableL(false);
				UpgradeL->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeL->SetUpgradeableL(true);
				UpgradeL->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 2:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/tack_shooter/l3.png");
			if (GameInstance::Get()->GetMoney() < 500)
			{
				UpgradeL->SetUpgradeableL(false);
				UpgradeL->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeL->SetUpgradeableL(true);
				UpgradeL->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 3:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/tack_shooter/l4.png");
			if (GameInstance::Get()->GetMoney() < 2500)
			{
				UpgradeL->SetUpgradeableL(false);
				UpgradeL->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeL->SetUpgradeableL(true);
				UpgradeL->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 4:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/upgrade_max.png");
			break;
		case 5:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/upgrade_end.png");
			break;
		}

		switch (level_R)
		{
		case 0:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/tack_shooter/r1.png");
			if (GameInstance::Get()->GetMoney() < 100)
			{
				UpgradeR->SetUpgradeableR(false);
				UpgradeR->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeR->SetUpgradeableR(true);
				UpgradeR->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 1:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/tack_shooter/r2.png");
			if (GameInstance::Get()->GetMoney() < 230)
			{
				UpgradeR->SetUpgradeableR(false);
				UpgradeR->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeR->SetUpgradeableR(true);
				UpgradeR->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 2:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/tack_shooter/r3.png");
			if (GameInstance::Get()->GetMoney() < 680)
			{
				UpgradeR->SetUpgradeableR(false);
				UpgradeR->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeR->SetUpgradeableR(true);
				UpgradeR->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 3:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/tack_shooter/r4.png");
			if (GameInstance::Get()->GetMoney() < 2500)
			{
				UpgradeR->SetUpgradeableR(false);
				UpgradeR->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeR->SetUpgradeableR(true);
				UpgradeR->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 4:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/upgrade_max.png");
			break;
		case 5:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/upgrade_end.png");
			break;
		}
	}

	if (selectTower->GetType() == Bomb)
	{
		switch (level_L)
		{
		case 0:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/bomb_tower/l1.png");
			if (GameInstance::Get()->GetMoney() < 170)
			{
				UpgradeL->SetUpgradeableL(false);
				UpgradeL->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeL->SetUpgradeableL(true);
				UpgradeL->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 1:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/bomb_tower/l2.png");
			if (GameInstance::Get()->GetMoney() < 255)
			{
				UpgradeL->SetUpgradeableL(false);
				UpgradeL->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeL->SetUpgradeableL(true);
				UpgradeL->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 2:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/bomb_tower/l3.png");
			if (GameInstance::Get()->GetMoney() < 680)
			{
				UpgradeL->SetUpgradeableL(false);
				UpgradeL->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeL->SetUpgradeableL(true);
				UpgradeL->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 3:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/bomb_tower/l4.png");
			if (GameInstance::Get()->GetMoney() < 3400)
			{
				UpgradeL->SetUpgradeableL(false);
				UpgradeL->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeL->SetUpgradeableL(true);
				UpgradeL->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 4:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/upgrade_max.png");
			break;
		case 5:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/upgrade_end.png");
			break;
		}

		switch (level_R)
		{
		case 0:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/bomb_tower/r1.png");
			if (GameInstance::Get()->GetMoney() < 340)
			{
				UpgradeR->SetUpgradeableR(false);
				UpgradeR->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeR->SetUpgradeableR(true);
				UpgradeR->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 1:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/bomb_tower/r2.png");
			if (GameInstance::Get()->GetMoney() < 340)
			{
				UpgradeR->SetUpgradeableR(false);
				UpgradeR->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeR->SetUpgradeableR(true);
				UpgradeR->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 2:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/bomb_tower/r3.png");
			if (GameInstance::Get()->GetMoney() < 765)
			{
				UpgradeR->SetUpgradeableR(false);
				UpgradeR->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeR->SetUpgradeableR(true);
				UpgradeR->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 3:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/bomb_tower/r4.png");
			if (GameInstance::Get()->GetMoney() < 1650)
			{
				UpgradeR->SetUpgradeableR(false);
				UpgradeR->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeR->SetUpgradeableR(true);
				UpgradeR->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 4:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/upgrade_max.png");
			break;
		case 5:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/upgrade_end.png");
			break;
		}
	}

	if (selectTower->GetType() == Ice)
	{
		switch (level_L)
		{
		case 0:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/ice_tower/l1.png");
			if (GameInstance::Get()->GetMoney() < 190)
			{
				UpgradeL->SetUpgradeableL(false);
				UpgradeL->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeL->SetUpgradeableL(true);
				UpgradeL->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 1:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/ice_tower/l2.png");
			if (GameInstance::Get()->GetMoney() < 340)
			{
				UpgradeL->SetUpgradeableL(false);
				UpgradeL->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeL->SetUpgradeableL(true);
				UpgradeL->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 2:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/ice_tower/l3.png");
			if (GameInstance::Get()->GetMoney() < 1050)
			{
				UpgradeL->SetUpgradeableL(false);
				UpgradeL->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeL->SetUpgradeableL(true);
				UpgradeL->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 3:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/ice_tower/l4.png");
			if (GameInstance::Get()->GetMoney() < 2400)
			{
				UpgradeL->SetUpgradeableL(false);
				UpgradeL->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeL->SetUpgradeableL(true);
				UpgradeL->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 4:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/upgrade_max.png");
			break;
		case 5:
			UpgradeL->SetTexture(L"Textures/UI/Upgrade/upgrade_end.png");
			break;
		}

		switch (level_R)
		{
		case 0:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/ice_tower/r1.png");
			if (GameInstance::Get()->GetMoney() < 85)
			{
				UpgradeR->SetUpgradeableR(false);
				UpgradeR->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeR->SetUpgradeableR(true);
				UpgradeR->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 1:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/ice_tower/r2.png");
			if (GameInstance::Get()->GetMoney() < 295)
			{
				UpgradeR->SetUpgradeableR(false);
				UpgradeR->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeR->SetUpgradeableR(true);
				UpgradeR->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 2:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/ice_tower/r3.png");
			if (GameInstance::Get()->GetMoney() < 1100)
			{
				UpgradeR->SetUpgradeableR(false);
				UpgradeR->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeR->SetUpgradeableR(true);
				UpgradeR->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 3:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/ice_tower/r4.png");
			if (GameInstance::Get()->GetMoney() < 1700)
			{
				UpgradeR->SetUpgradeableR(false);
				UpgradeR->GetColor() = { 1, 1, 1, 0.5f };
			}
			else
			{
				UpgradeR->SetUpgradeableR(true);
				UpgradeR->GetColor() = { 1, 1, 1, 1 };
			}
			break;
		case 4:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/upgrade_max.png");
			break;
		case 5:
			UpgradeR->SetTexture(L"Textures/UI/Upgrade/upgrade_end.png");
			break;
		}
	}
}

void InGameScene::SellTower()
{
	if (selectTower)
	{
		// 삭제할 타워들에 선택된 타워를 추가 & 비활성화
		deleteTowers.push_back(selectTower);
		selectTower->GetCollider()->SetActive(false);
		selectTower->SetActive(false);

		// 선택한 타워의 총 금액의 80%를 게임머니에 추가
		GameInstance::Get()->IncreaseMoney((int)(selectTower->GetTotalMoney() * 0.8f));

		// 선택 정보 초기화
		selectTower = nullptr;
	}
}

void InGameScene::StartRound()
{
	// Start 버튼을 인게임 상태에 따라 다르게 동작하도록 설정
	if (state == State::WAIT)
	{
		// 최대 스폰 가능 수를 저장
		maxSpawnCnt = roundInfo[roundCnt].size();

		// 상태 변경
		state = State::PLAY;
	}
	else if (state == State::PLAY)
	{
		if (Start->GetDubleSpeed())
		{
			// 게임 전체 속도를 1배로 설정
			Start->SetDubleSpeed(false);
			Start->SetTexture(L"Textures/UI/start.png");
			GameInstance::Get()->SetGlobalSpeed(1.0f);
		}
		else
		{
			// 게임 전체 속도를 2배로 설정
			Start->SetDubleSpeed(true);
			Start->SetTexture(L"Textures/UI/dubleSpeed.png");
			GameInstance::Get()->SetGlobalSpeed(2.0f);
		}
	}
}

void InGameScene::Select()
{
	// 타워 클릭시 해당 타워를 선택
	if (selectTower && KEY_DOWN(VK_LBUTTON))
	{
		bool unSelect = true;
		for (Tower* tower : towers)
		{
			if (tower->GetCollider()->IsPointCollision(mousePos))
			{
				unSelect = false;
				break;
			}
		}

		if (UpgradeL->GetCollider()->IsPointCollision(mousePos) || UpgradeR->GetCollider()->IsPointCollision(mousePos) || Sell->GetCollider()->IsPointCollision(mousePos))
		{
			unSelect = false;
		}

		if (unSelect && !tempTower)
		{
			selectTower->Select(false);
			selectTower = nullptr;
		}
	}

	// 선택된 타워가 있을 경우에만 업그레이드 & 판매 버튼 업데이트
	if (selectTower)
	{
		UpgradeL->Update();
		UpgradeR->Update();
		Sell->Update();
	}
}

void InGameScene::RenderText()
{
	// 텍스트 렌더링 정보 모음
	if (state == State::DEFEAT)
	{
		defeatWindow->Render();
		backHome->Render();
		restart->Render();
		wstring round = L"Round : " + to_wstring(roundCnt);
		Font::Get()->RenderText(round, { 500, 290 });
	}
	else if (state == State::VICTORY)
	{
		victoryWindow->Render();
		backHome->Render();
		restart->Render();
		Font::Get()->RenderText(L"YOU WIN!", { 485, 300 });
	}

	wstring round = to_wstring(roundCnt) + L" 라운드";
	Font::Get()->RenderText(round, { WIN_WIDTH - 80, WIN_HEIGHT - 24 });
	float pading = 0.0f;
	if (GameInstance::Get()->GetMoney() >= 10000)
	{
		pading = 20.0f;
	}
	else if (GameInstance::Get()->GetMoney() >= 1000)
	{
		pading = 30.0f;
	}
	else if (GameInstance::Get()->GetMoney() >= 100)
	{
		pading = 40.0f;
	}
	else if (GameInstance::Get()->GetMoney() >= 10)
	{
		pading = 50.0f;
	}
	else if (GameInstance::Get()->GetMoney() >= 0)
	{
		pading = 60.0f;
	}
	wstring gold = L"Gold : $" + to_wstring(GameInstance::Get()->GetMoney());
	Font::Get()->RenderText(gold, { WIN_WIDTH - pading, WIN_HEIGHT - 72 });
	wstring hp = L"HP : " + to_wstring(GameInstance::Get()->GetCurHp());
	Font::Get()->RenderText(hp, { WIN_WIDTH - 70, WIN_HEIGHT - 120 });
}

void InGameScene::EnterLobby()
{
	// 라운드 정보 초기화 (풍선, 시작버튼 상태)
	for (Balloon* balloon : balloons)
	{
		if (balloon->Active())
		{
			balloon->SetActive(false);
		}
		balloon->Init();
	}
	spawnCnt = 0;
	Start->SetDubleSpeed(false);
	Start->SetTexture(L"Textures/UI/start.png");

	if (state == State::DEFEAT || state == State::VICTORY)
	{
		// 라운드 정보 초기화 (돈, 라운드 수, 타워 정보, 현재 hp)
		GameInstance::Get()->SetLastMoney(650);
		GameInstance::Get()->SetLastRoundCnt(1);
		if (towers.size())
		{
			for (Tower* tower : towers)
				delete tower;
			towers.clear();
		}
		GameInstance::Get()->SetLastTowerInfo(towers);
		GameInstance::Get()->SetLastHp(200);

		// 백업 정보 초기화
		GameInstance::Get()->SetBackup(false);
	}
	else if (state == State::STOP)
	{
		// 옵션창 종료 & 맵 정보 저장
		optionWindow->SetActive(false);
		GameInstance::Get()->SetLastMapId((int)(map));

		// 백업 정보와 비교해 백업되지 않은 타워 제거
		vector<Tower*> tempTowers = GameInstance::Get()->GetLastTowerInfo();
		for (Tower* tower : towers)
		{
			if (find(tempTowers.begin(), tempTowers.end(), tower) == tempTowers.end())
			{
				delete tower;
				tower = nullptr;
			}
		}
		towers.clear();
		
		// 삭제 예정으로 모아뒀던 타워들 화면에 보이게 설정 및 삭제 리스트 초기화
		for (Tower* tower : deleteTowers)
		{
			if (tower != nullptr)
				tower->SetActive(true);
		}
		deleteTowers.clear();

		// 백업 정보 활성화
		GameInstance::Get()->SetBackup(true);
	}

	// 선택된 타워 제거
	if (selectTower != nullptr)
	{
		selectTower->Select(false);
		selectTower = nullptr;
	}

	// 로비 씬으로 이동
	SCENE->ChangeScene("LobbyScene");
}

void InGameScene::RestartGame()
{
	// 라운드 정보 초기화 (풍선, 돈, 라운드 수, 타워 정보, 현재 hp)
	for (Balloon* balloon : balloons)
	{
		if (balloon->Active())
		{
			balloon->SetActive(false);
		}
		balloon->Init();
	}

	// 선택된 타워 제거
	if (selectTower != nullptr)
	{
		selectTower->Select(false);
		selectTower = nullptr;
	}

	spawnCnt = 0;
	Start->SetDubleSpeed(false);
	Start->SetTexture(L"Textures/UI/start.png");
	GameInstance::Get()->SetLastMoney(650);
	GameInstance::Get()->SetLastRoundCnt(1);
	if (towers.size())
	{
		for (Tower* tower : towers)
			delete tower;
		towers.clear();
	}
	deleteTowers.clear();
	GameInstance::Get()->SetLastTowerInfo(towers);
	GameInstance::Get()->SetLastHp(200);

	// 시작 머니 설정
	GameInstance::Get()->SetMoney(GameInstance::Get()->GetLastMoney());
	// 라운드 수 설정
	roundCnt = GameInstance::Get()->GetLastRoundCnt();
	// 시작 체력 설정
	GameInstance::Get()->SetHp(GameInstance::Get()->GetLastHp());
	// 시작 타워 설정
	if (towers.size())
	{
		for (Tower* tower : towers)
			delete tower;
		towers.clear();
	}
	towers = GameInstance::Get()->GetLastTowerInfo();

	// 씬 상태 설정
	SetState(State::WAIT);

	if (optionWindow->Active())
	{
		optionWindow->SetActive(false);
	}

	// 백업 정보 초기화
	GameInstance::Get()->SetBackup(false);
}

void InGameScene::OpenOption()
{
	// 설정 버튼 클릭 시 현재 상태를 임시로 저장하고 현재 상태를 STOP로 지정
	temp = state;
	SetState(State::STOP);
}

void InGameScene::CloseOption()
{
	// 설정창을 닫고 현재 상태를 임시로 저장된 상태로 덮어씌우기
	optionWindow->SetActive(false);
	SetState(temp);
}

void InGameScene::Test()
{
	// SpaceBar : 게임머니 +10000
	if (KEY_DOWN(VK_SPACE))
	{
		GameInstance::Get()->IncreaseMoney(10000);
	}
	// W : 체력 -100
	if (KEY_DOWN('W'))
	{
		GameInstance::Get()->DecreaseHp(100);
	}
	// F : 라운드 +1
	if (KEY_DOWN('F'))
	{
		if (roundCnt < 40)
			roundCnt++;
	}

	// 풍선 테스트
	if (KEY_DOWN('A'))
	{
		for (Balloon* balloon : balloons)
		{
			if (!balloon->Active())
			{
				balloon->SetHP(1);
				balloon->Init();
				balloon->SetActive(true);
				break;
			}
		}
	}
	if (KEY_DOWN('B'))
	{
		for (Balloon* balloon : balloons)
		{
			if (!balloon->Active())
			{
				balloon->SetHP(2);
				balloon->Init();
				balloon->SetActive(true);
				break;
			}
		}
	}
	if (KEY_DOWN('C'))
	{
		for (Balloon* balloon : balloons)
		{
			if (!balloon->Active())
			{
				balloon->SetHP(3);
				balloon->Init();
				balloon->SetActive(true);
				break;
			}
		}
	}
	if (KEY_DOWN('D'))
	{
		for (Balloon* balloon : balloons)
		{
			if (!balloon->Active())
			{
				balloon->SetHP(4);
				balloon->Init();
				balloon->SetActive(true);
				break;
			}
		}
	}
	if (KEY_DOWN('E'))
	{
		for (Balloon* balloon : balloons)
		{
			if (!balloon->Active())
			{
				balloon->SetHP(5);
				balloon->Init();
				balloon->SetActive(true);
				break;
			}
		}
	}
	if (KEY_DOWN('G'))
	{
		for (Balloon* balloon : balloons)
		{
			if (!balloon->Active())
			{
				balloon->SetHP(7);
				balloon->Init();
				balloon->SetActive(true);
				break;
			}
		}
	}
	if (KEY_DOWN('I'))
	{
		for (Balloon* balloon : balloons)
		{
			if (!balloon->Active())
			{
				balloon->SetHP(9);
				balloon->Init();
				balloon->SetActive(true);
				break;
			}
		}
	}
	if (KEY_DOWN('K'))
	{
		for (Balloon* balloon : balloons)
		{
			if (!balloon->Active())
			{
				balloon->SetHP(11);
				balloon->Init();
				balloon->SetActive(true);
				break;
			}
		}
	}
	if (KEY_DOWN('M'))
	{
		for (Balloon* balloon : balloons)
		{
			if (!balloon->Active())
			{
				balloon->SetHP(13);
				balloon->Init();
				balloon->SetActive(true);
				break;
			}
		}
	}
	if (KEY_DOWN('O'))
	{
		for (Balloon* balloon : balloons)
		{
			if (!balloon->Active())
			{
				balloon->SetHP(15);
				balloon->Init();
				balloon->SetActive(true);
				break;
			}
		}
	}
	if (KEY_DOWN('Q'))
	{
		for (Balloon* balloon : balloons)
		{
			if (!balloon->Active())
			{
				balloon->SetHP(17);
				balloon->Init();
				balloon->SetActive(true);
				break;
			}
		}
	}
	if (KEY_DOWN('S'))
	{
		for (Balloon* balloon : balloons)
		{
			if (!balloon->Active())
			{
				balloon->SetHP(19);
				balloon->Init();
				balloon->SetActive(true);
				break;
			}
		}
	}
	if (KEY_DOWN('T'))
	{
		for (Balloon* balloon : balloons)
		{
			if (!balloon->Active())
			{
				balloon->SetHP(20);
				balloon->Init();
				balloon->SetActive(true);
				break;
			}
		}
	}
	if (KEY_DOWN('U'))
	{
		for (Balloon* balloon : balloons)
		{
			if (!balloon->Active())
			{
				balloon->SetHP(21);
				balloon->Init();
				balloon->SetActive(true);
				break;
			}
		}
	}
	if (KEY_DOWN('V'))
	{
		for (Balloon* balloon : balloons)
		{
			if (!balloon->Active())
			{
				balloon->SetHP(22);
				balloon->Init();
				balloon->SetActive(true);
				break;
			}
		}
	}
	if (KEY_DOWN('X'))
	{
		for (Balloon* balloon : balloons)
		{
			if (!balloon->Active())
			{
				balloon->SetHP(24);
				balloon->Init();
				balloon->SetActive(true);
				break;
			}
		}
	}
	if (KEY_DOWN('Y'))
	{
		for (Balloon* balloon : balloons)
		{
			if (!balloon->Active())
			{
				balloon->SetHP(25);
				balloon->Init();
				balloon->SetActive(true);
				break;
			}
		}
	}
	if (KEY_DOWN('Z'))
	{
		for (Balloon* balloon : balloons)
		{
			if (!balloon->Active())
			{
				balloon->SetHP(26);
				balloon->Init();
				balloon->SetActive(true);
				break;
			}
		}
	}
}
