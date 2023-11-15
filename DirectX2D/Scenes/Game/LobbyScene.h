#pragma once

class UIButton;

class LobbyScene : public Scene
{
public:
	LobbyScene();
	virtual ~LobbyScene() override;

	virtual void Update() override;
	virtual void Render() override;

private:
	// 맵을 고르기 위한 UI를 출력할 함수를 선언합니다.
	void PrintMap();
	// 맵을 고르기 위한 UI를 닫아주는 함수를 선언합니다.
	void CloseMap();

private:
	// 배경을 저장하기 위한 변수를 선언합니다.
	Quad* background;
	// 맵 선택 그림을 저장하기 위한 변수를 선언합니다.
	Quad* maps;

	// 선택 가능한 맵의 그림을 저장하기 위한 변수들을 선언합니다.
	UIButton* miniMonkeyLane;
	UIButton* miniTutorial;
	UIButton* miniLoadGame;
	UIButton* miniPatch;

	// 시작 버튼을 저장하기 위한 변수를 선언합니다.
	UIButton* start;
	// 창 닫기 버튼을 저장하기 위한 변수를 선언합니다.
	UIButton* close;

	// 사운드 조절을 위한 변수를 선언합니다.
	UIButton* sound;
};

