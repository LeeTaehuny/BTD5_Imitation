#pragma once

// 버튼의 기능을 구분하기 위한 열거형을 선언합니다.
enum class B_Type
{
	None,
	Play,
	SelectMap,
	ContinueGame,
	MiniMap_Monkey,
	MiniMap_Tutorial,
	MiniMap_Patch,
	Close,
	CloseOption,
	Home,
	Option,
	Restart,
	L_Upgrade,
	R_Upgrade,
	Sell,
	StartRound,
	BGM_Sound,
	SoundEffect,
};

// 실제 게임상에서 작동하는 UI 버튼에 대한 클래스입니다.
class UIButton : public Button
{
public:
	UIButton(wstring textureFile, B_Type type);
	virtual ~UIButton() override;

	virtual void Update() override;
	virtual void Render() override;

// Getter & Setter
public:
	B_Type GetType() { return type; }
	void SetUpgradeableL(bool value) { bIsUpgradableL = value; }
	void SetUpgradeableR(bool value) { bIsUpgradableR = value; }

	bool GetDubleSpeed() { return bIsDubleSpeed; }
	void SetDubleSpeed(bool value) { bIsDubleSpeed = value; }

private:
	// 클릭시 실행될 함수를 선언합니다.
	void OnClick();

// Member Variable
private:
	B_Type type = B_Type::None;
	bool bIsUpgradableL = false;
	bool bIsUpgradableR = false;

	bool bIsDubleSpeed = false;
};

