#pragma once
#include "Objects/Game/UI/Tower.h"

class Projectile;

class BombTower : public Tower
{
	// 공격 타입을 구분하기 위한 열거형을 선언합니다.
	enum AttackType
	{
		Basic,	// 기본
		Split,  // 분열
	};
public:
	BombTower();
	virtual ~BombTower() override;

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
	
	// 특수 공격을 수행하기 위한 함수를 선언합니다.
	void FireSubProjectile(void* enemy);

// Member variable
private:
	float sumTime = 0.0f;
	float bulletSpeed = 600.0f;
	bool bulletSlowable = false;
	bool subProjecileUpgrade = false;
	vector<Projectile*> subProjectile;

	AttackType attType = Basic;
};

