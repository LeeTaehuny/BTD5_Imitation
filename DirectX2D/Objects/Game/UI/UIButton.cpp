#include "Framework.h"
#include "UIButton.h"
#include "Scenes/Game/InGameScene.h"
#include "Objects/Game/GameInstance/GameInstance.h"

UIButton::UIButton(wstring textureFile, B_Type type) : Button(textureFile), type(type)
{
	// OnClick함수를 버튼의 이벤트에 등록합니다.
	SetEvent(bind(&UIButton::OnClick, this));
}

UIButton::~UIButton()
{
}

void UIButton::Update()
{
	if (type == B_Type::BGM_Sound && GameInstance::Get()->GetBGMPlayable())
	{
		SetTexture(L"Textures/UI/soundOn.png");
	}
	else if (type == B_Type::BGM_Sound && !GameInstance::Get()->GetBGMPlayable())
	{
		SetTexture(L"Textures/UI/soundOff.png");
	}

	Button::Update();
}

void UIButton::Render()
{
	Quad::Render();
}

void UIButton::OnClick()
{
	// 버튼의 타입에 따라 다른 이벤트를 발생시켜줍니다.
	switch (type)
	{
	case B_Type::Play:
		Observer::Get()->ExcuteEvent("Play");
		break;
	case B_Type::SelectMap:
		Observer::Get()->ExcuteEvent("SelectMap");
		break;
	case B_Type::ContinueGame:
		SCENE->SettingScene("InGameScene", GameInstance::Get()->GetLastMapId(), false);
		SCENE->ChangeScene("InGameScene");
		break;
	case B_Type::MiniMap_Monkey:
		SCENE->SettingScene("InGameScene", 1, true);
		SCENE->ChangeScene("InGameScene");
		break;
	case B_Type::MiniMap_Tutorial:
		SCENE->SettingScene("InGameScene", 2, true);
		SCENE->ChangeScene("InGameScene");
		break;
	case B_Type::MiniMap_Patch:
		SCENE->SettingScene("InGameScene", 3, true);
		SCENE->ChangeScene("InGameScene");
		break;
	case B_Type::Close:
		Observer::Get()->ExcuteEvent("Close");
		break;
	case B_Type::CloseOption:
		Observer::Get()->ExcuteEvent("CloseOption");
		break;
	case B_Type::Home:
		Observer::Get()->ExcuteEvent("BackHome");
		break;
	case B_Type::Option:
		Observer::Get()->ExcuteEvent("Option");
		break;
	case B_Type::Restart:
		Observer::Get()->ExcuteEvent("Restart");
		break;
	case B_Type::L_Upgrade:
		if (bIsUpgradableL)
			Observer::Get()->ExcuteParamEvent("Upgrade", this);
		break;
	case B_Type::R_Upgrade:
		if (bIsUpgradableR)
			Observer::Get()->ExcuteParamEvent("Upgrade", this);
		break;
	case B_Type::Sell:
		Observer::Get()->ExcuteEvent("Sell");
		break;
	case B_Type::StartRound:
		Observer::Get()->ExcuteEvent("StartRound");
		break;
	case B_Type::BGM_Sound:
		if (GameInstance::Get()->GetBGMPlayable())
		{
			GameInstance::Get()->SetBGMPlayable(false);
			SetTexture(L"Textures/UI/soundOff.png");
		}
		else
		{
			GameInstance::Get()->SetBGMPlayable(true);
			SetTexture(L"Textures/UI/soundOn.png");
		}
		break;
	case B_Type::SoundEffect:
		if (GameInstance::Get()->GetSoundEffectPlayable())
		{
			GameInstance::Get()->SetSoundEffectPlayable(false);
			SetTexture(L"Textures/UI/soundOff.png");
		}
		else
		{
			GameInstance::Get()->SetSoundEffectPlayable(true);
			SetTexture(L"Textures/UI/soundOn.png");
		}
		break;
	}

}
