#pragma once
#include "Objects/Game/UI/Tower.h"

class IceTower : public Tower
{
	// 공격 타입을 구분하기 위한 열거형을 선언합니다.
	enum AttackType
	{
		Basic,
		Duble,
		Explore,
		Ignore_Resist,
	};
public:
	IceTower();
	virtual ~IceTower() override;

	virtual void Update() override;
	virtual void Render() override;

	// 세이브된 마지막 업그레이드 정보로 다시 세팅하기 위한 함수를 선언합니다.
	void LastUpgradeSetting();

private:
	// Button의 클릭 이벤트와 매핑할 함수를 선언합니다.
	void OnClick();
	// 공격 함수를 선언합니다.
	void Attack();
	// 텍스처를 미리 로드해두기 위한 함수를 선언합니다.
	void LoadTexture();
	// 타워의 데이터를 초기화하기 위한 함수를 선언합니다.
	void ResetData();
	// 업그레이드를 위한 함수를 선언합니다.
	void Upgrade();
	// 업그레이드 내용에 따라 타워의 데이터를 업데이트하기 위한 함수를 선언합니다.
	void UpdateData();

// Member variable
private:
	AttackType attType = Basic;

	float sumTime = 0.0f;
	Quad* iceCircle;
	float freezingTime = 1.0f;
	float freezingRate = 0.0f;
};

