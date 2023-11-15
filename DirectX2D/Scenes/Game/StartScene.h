#pragma once

class UIButton;

class StartScene : public Scene
{
public:
	StartScene();
	virtual ~StartScene() override;

	virtual void Update() override;
	virtual void Render() override;

private:
	// 로비 씬으로 이동하기 위한 함수를 선언합니다.
	void EnterLobby();

private:
	// 배경을 저장하기 위한 변수를 선언합니다.
	Quad* background;
	// 시작 버튼을 저장하기 위한 변수를 선언합니다.
	UIButton* play;
};

