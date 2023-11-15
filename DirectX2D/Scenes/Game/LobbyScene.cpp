#include "Framework.h"
#include "LobbyScene.h"
#include "Objects/Game/UI/UIButton.h"
#include "Objects/Game/GameInstance/GameInstance.h"

LobbyScene::LobbyScene()
{
	background = new Quad(L"Textures/Maps/Lobby.png");
	background->Pos() = { CENTER_X,CENTER_Y };
	background->UpdateWorld();

	maps = new Quad(L"Textures/UI/Maps.png");
	maps->Pos() = { CENTER_X,CENTER_Y };
	maps->UpdateWorld();
	maps->SetActive(false);

	start = new UIButton(L"Textures/UI/startButton.png", B_Type::SelectMap);
	start->Pos() = Vector2(450.0f, 240.0f);

	close = new UIButton(L"Textures/UI/close.png", B_Type::Close);
	close->SetParent(maps);
	close->Pos() = Vector2(305.0f, 255.0f);

	miniMonkeyLane = new UIButton(L"Textures/UI/minimap/mini_monkeylane.png", B_Type::MiniMap_Monkey);
	miniMonkeyLane->SetParent(maps);
	miniMonkeyLane->Pos() = Vector2(-150.0f, 0.0f);

	miniTutorial = new UIButton(L"Textures/UI/minimap/mini_tutorial.png", B_Type::MiniMap_Tutorial);
	miniTutorial->SetParent(maps);
	miniTutorial->Pos() = Vector2(150.0f, 0.0f);

	miniPatch = new UIButton(L"Textures/UI/minimap/mini_patch.png", B_Type::MiniMap_Patch);
	miniPatch->SetParent(maps);
	miniPatch->Pos() = Vector2(-150.0f, -180.0f);

	miniLoadGame = new UIButton(L"Textures/UI/minimap/mini_loadgame.png", B_Type::ContinueGame);
	miniLoadGame->SetParent(maps);
	miniLoadGame->Pos() = Vector2(150.0f, -180.0f);

	sound = new UIButton(L"Textures/UI/soundOn.png", B_Type::BGM_Sound);
	sound->Pos() = Vector2(850.0f, 560.0f);

	Observer::Get()->AddEvent("SelectMap", bind(&LobbyScene::PrintMap, this));
	Observer::Get()->AddEvent("Close", bind(&LobbyScene::CloseMap, this));
}

LobbyScene::~LobbyScene()
{
	delete background;
	delete start;
	delete miniMonkeyLane;
	delete miniTutorial;
	delete miniPatch;
	delete miniLoadGame;
	delete sound;
}

void LobbyScene::Update()
{
	start->Update();
	sound->Update();
	if (maps->Active())
	{
		close->Update();
		miniMonkeyLane->Update();
		miniTutorial->Update();
		miniPatch->Update();

		if (GameInstance::Get()->GetBackup())
			miniLoadGame->Update();
	}
}

void LobbyScene::Render()
{
	background->Render();
	start->Render();
	sound->Render();
	if (maps->Active())
	{
		maps->Render();
		close->Render();
		miniMonkeyLane->Render();
		miniTutorial->Render();
		miniPatch->Render();
		if (GameInstance::Get()->GetBackup())
			miniLoadGame->Render();
	}
}

void LobbyScene::PrintMap()
{
	maps->SetActive(true);
}

void LobbyScene::CloseMap()
{
	maps->SetActive(false);
}
