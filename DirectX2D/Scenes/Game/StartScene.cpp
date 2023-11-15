#include "Framework.h"
#include "StartScene.h"
#include "Objects/Game/UI/UIButton.h"

StartScene::StartScene()
{
	background = new Quad(L"Textures/Maps/Loading.png");
	background->Pos() = { CENTER_X,CENTER_Y };
	background->UpdateWorld();

	play = new UIButton(L"Textures/UI/play.png", B_Type::Play);
	play->Pos() = Vector2(430.0f, 110.0f);

	Observer::Get()->AddEvent("Play", bind(&StartScene::EnterLobby, this));
}

StartScene::~StartScene()
{
	delete background;
	delete play;
}

void StartScene::Update()
{
	play->Update();
}

void StartScene::Render()
{
	background->Render();
	play->Render();
}

void StartScene::EnterLobby()
{
	SCENE->ChangeScene("LobbyScene");
}
