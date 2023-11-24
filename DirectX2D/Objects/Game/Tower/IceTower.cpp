#include "Framework.h"
#include "IceTower.h"
#include "Objects/Game/Projectile.h"
#include "Objects/Game/Balloon.h"
#include "Objects/Game/GameInstance/GameInstance.h"

IceTower::IceTower() : Tower(L"Textures/Character/ice_tower/i_base.png", Ice)
{
	/** 생성과 함께 처리되어야 할 부분들을 처리합니다. */
	// 이벤트 매핑
	SetEvent(bind(&IceTower::OnClick, this));

	// 텍스처 로드 & 데이터 리셋
	LoadTexture();
	ResetData();

	// 충돌체 생성
	selfCollider = new CircleCollider(15.0f);
	selfCollider->SetParent(this);

	// 공격 범위 생성
	attackRange = new CircleCollider(t_data.range);
	attackRange->SetParent(this);

	// 발사체 생성
	projectiles.resize(10);
	for (Projectile*& p : projectiles)
	{
		p = new Projectile(L"Textures/Projectile/tack_shooter/tack1.png", Type::SHARP, t_data.range);

		p->SetActive(false);
	}

	// 공격시 출력될 이미지를 설정합니다.
	iceCircle = new Quad(L"Textures/Projectile/ice_tower/ice1.png");
	iceCircle->SetActive(false);
	iceCircle->SetParent(this);
}

IceTower::~IceTower()
{
	delete selfCollider;
	delete attackRange;

	for (Projectile* projectile : projectiles)
	{
		delete projectile;
	}
	projectiles.clear();

	delete iceCircle;
}

void IceTower::Update()
{
	/** 타워 활성화 상태에서만 업데이트를 진행합니다. */
	if (!isActive) return;

	selfCollider->UpdateWorld();
	attackRange->UpdateWorld();

	Attack();

	if (isUpdatable)
	{
		Upgrade();
	}

	for (Projectile* p : projectiles)
	{
		p->Update();
	}

	iceCircle->UpdateWorld();
	Tower::Update();
}

void IceTower::Render()
{
	/** 타워 활성화 상태에서만 렌더링을 진행합니다. */
	if (!isActive) return;

	for (Projectile* p : projectiles)
	{
		if (!p->Active()) continue;
		p->Render();
	}

	if (iceCircle->Active())
		iceCircle->Render();

	Quad::Render();
	if (isSelected)
	{
		attackRange->Render();
	}
}

void IceTower::OnClick()
{
	/** 클릭시 등록된 이벤트 중 키 값이 Select인 이벤트를 실행합니다. */
	Observer::Get()->ExcuteParamEvent("Select", this);
}

void IceTower::Attack()
{
	/** 공격 범위 내에 적이 존재한다면 공격을 시작합니다. */
	if (enemies.size())
	{
		sumTime += DELTA;

		switch (attType)
		{
		case AttackType::Basic:
			if (sumTime >= t_data.delay / GameInstance::Get()->GetGlobalSpeed())
			{
				iceCircle->SetActive(true);
				sumTime = 0.0f;
				for (Balloon* enemy : enemies)
				{
					// 저항이 있는 풍선은 무시합니다.
					if (enemy->GetType() == Balloon_Type::ICE_RESIST || enemy->GetType() == Balloon_Type::DUBLE_RESIST) continue;
					// 이미 멈춰있거나, 한 번이라도 멈춘 풍선은 무시합니다.
					if (enemy->GetState() == STOP) continue;
					if (enemy->GetMeltDown() == true) continue;

					// 멈출 시간 및 데미지를 설정합니다.
					enemy->SetStopTime(freezingTime);
					enemy->SetMeltDown(true);

					// 예외처리용 적 활성화 체크
					if (enemy->Active())
						enemy->SetState(STOP);
					enemy->takeDamage(t_data.damage);
				}

			}
			break;

		case AttackType::Duble:
			if (sumTime >= t_data.delay / GameInstance::Get()->GetGlobalSpeed())
			{
				iceCircle->SetActive(true);
				sumTime = 0.0f;
				for (Balloon* enemy : enemies)
				{
					// 저항이 있는 풍선은 무시합니다.
					if (enemy->GetType() == Balloon_Type::ICE_RESIST || enemy->GetType() == Balloon_Type::DUBLE_RESIST) continue;
					// 이미 멈춰있거나, 한 번이라도 멈춘 풍선은 무시합니다.
					if (enemy->GetState() == STOP) continue;
					if (enemy->GetMeltDown() == true) continue;

					// 느려질 값과 멈출 시간, 데미지를 설정합니다.
					enemy->SetStopTime(freezingTime);
					enemy->SetSlowRate(freezingRate);
					enemy->SetMeltDown(true);
					enemy->SetIsSlow(true);
					// 예외처리용 적 활성화 체크
					if (enemy->Active())
						enemy->SetState(STOP);
					enemy->takeDamage(t_data.damage);


				}

			}
			break;

		case AttackType::Explore:
			if (sumTime >= t_data.delay / GameInstance::Get()->GetGlobalSpeed())
			{
				iceCircle->SetActive(true);
				sumTime = 0.0f;
				for (Balloon* enemy : enemies)
				{
					// 저항이 있는 풍선은 무시합니다.
					if (enemy->GetType() == Balloon_Type::ICE_RESIST || enemy->GetType() == Balloon_Type::DUBLE_RESIST) continue;
					// 이미 멈춰있거나, 한 번이라도 멈춘 풍선은 무시합니다.
					if (enemy->GetState() == STOP) continue;
					if (enemy->GetMeltDown() == true) continue;

					// 멈출 시간, 데미지를 설정합니다.
					enemy->SetStopTime(freezingTime);
					enemy->SetMeltDown(true);
					// 예외처리용 적 활성화 체크
					if (enemy->Active())
						enemy->SetState(STOP);

					enemy->takeDamage(t_data.damage);


					int cnt = 0;

					// 멈춘 적을 기준으로 4방향으로 다트를 발사합니다.
					for (Projectile* p : projectiles)
					{
						if (!p->Active())
						{
							if (cnt == 4)
							{
								break;
							}
							p->SetStartPos(enemy->Pos());
							p->SetPower(t_data.power);

							switch (cnt)
							{
							case 0:
								p->Fire(enemy->Pos(), Vector2(0, 1), enemy);
								p->AddHit(enemy);
								break;
							case 1:
								p->Fire(enemy->Pos(), Vector2(1, 0), enemy);
								p->AddHit(enemy);
								break;
							case 2:
								p->Fire(enemy->Pos(), Vector2(0, -1), enemy);
								p->AddHit(enemy);
								break;
							case 3:
								p->Fire(enemy->Pos(), Vector2(-1, 0), enemy);
								p->AddHit(enemy);
								break;
							}

							cnt++;
						}
					}
				}

			}
			break;

		case AttackType::Ignore_Resist:
			if (sumTime >= t_data.delay / GameInstance::Get()->GetGlobalSpeed())
			{
				iceCircle->SetActive(true);
				sumTime = 0.0f;
				for (Balloon* enemy : enemies)
				{
					// 저항은 생각하지 않고 멈춰있거나, 멈춘 적이 있다면 무시합니다.
					if (enemy->GetState() == STOP) continue;
					if (enemy->GetMeltDown() == true) continue;

					// 멈출 시간과 데미지를 설정합니다.
					enemy->SetStopTime(freezingTime);

					// 예외처리용 적 활성화 체크
					if (enemy->Active())
						enemy->SetState(STOP);

					enemy->takeDamage(t_data.damage);


				}
			}
			break;
		}
	}
	else
	{
		// 활성화했던 공격 이미지를 보이지 않도록 설정합니다.
		iceCircle->SetActive(false);
	}
}

void IceTower::LoadTexture()
{
	/** 미리 사용할 텍스처들을 텍스처 맵에 추가합니다. */
	Texture::Add(L"Textures/Character/ice_tower/up1.png");
	Texture::Add(L"Textures/Character/ice_tower/up2.png");
	Texture::Add(L"Textures/Character/ice_tower/l3.png");
	Texture::Add(L"Textures/Character/ice_tower/l4.png");
	Texture::Add(L"Textures/Character/ice_tower/r3.png");
	Texture::Add(L"Textures/Character/ice_tower/r4.png");
}

void IceTower::ResetData()
{
	/** 타워의 데이터를 초기화합니다. */
	t_data.range = 60.0f;
	t_data.delay = 0.3f;
	t_data.damage = 0;
	t_data.power = 1;
	t_data.isCamoDetectable = false;
	t_data.projectile_texture = 0;
	freezingRate = 0.0f;
	freezingTime = 1.0f;
}

void IceTower::Upgrade()
{
	/** 업그레이드가 가능할 시 타워의 레벨에 따른 업그레이드를 진행합니다. */
	if (isLUpdatable)
	{
		switch (towerLevel_L)
		{
		case 0:
			SetTexture(L"Textures/Character/ice_tower/i_base.png");
			t_data.projectile_texture = 0;
			break;

		case 1:
			if (towerLevel_L >= towerLevel_R)
				SetTexture(L"Textures/Character/ice_tower/up1.png");
			t_data.range += 15.0f;
			freezingTime += 0.57f;
			break;

		case 2:
			if (towerLevel_L >= towerLevel_R)
				SetTexture(L"Textures/Character/ice_tower/up2.png");
			t_data.damage += 1;
			break;

		case 3:
			if (towerLevel_L > towerLevel_R)
				SetTexture(L"Textures/Character/ice_tower/l3.png");
			attType = AttackType::Duble;
			t_data.range += 30.0f;
			freezingRate += 25.0f;
			break;

		case 4:
			if (towerLevel_L > towerLevel_R)
			{
				SetTexture(L"Textures/Character/ice_tower/l4.png");
			}
			attType = AttackType::Ignore_Resist;
			t_data.damage += 2;
			break;
		}

		isLUpdatable = false;
	}

	if (isRUpdatable)
	{
		switch (towerLevel_R)
		{
		case 0:
			SetTexture(L"Textures/Character/ice_tower/i_base.png");
			t_data.projectile_texture = 0;
			break;

		case 1:
			if (towerLevel_R >= towerLevel_L)
				SetTexture(L"Textures/Character/ice_tower/up1.png");
			if (towerLevel_L != 4)
				attType = AttackType::Duble;
			freezingRate += 25.0f;
			break;

		case 2:
			if (towerLevel_R >= towerLevel_L)
			{
				SetTexture(L"Textures/Character/ice_tower/up2.png");
			}
			t_data.damage += 1;
			break;

		case 3:
			if (towerLevel_R > towerLevel_L)
			{
				SetTexture(L"Textures/Character/ice_tower/r3.png");
				t_data.projectile_texture = 1;
			}
			attType = AttackType::Explore;
			t_data.power = 100;
			break;

		case 4:
			if (towerLevel_R > towerLevel_L)
			{
				SetTexture(L"Textures/Character/ice_tower/r4.png");
			}
			attType = AttackType::Ignore_Resist;
			freezingTime = 4.0f;
			break;
		}

		isRUpdatable = false;
	}

	UpdateData();
}

void IceTower::UpdateData()
{
	// 업데이트된 데이터를 바탕으로 실제 타워 정보를 업데이트합니다.
	attackRange = new CircleCollider(t_data.range);
	attackRange->SetParent(this);
	attackRange->GlobalPos() = localPosition;

	Collider* temp = new CircleCollider(t_data.range);
	temp->SetParent(this);
	temp->GlobalPos() = localPosition;
	delete attackRange;
	attackRange = temp;

	isUpdatable = false;
}

void IceTower::LastUpgradeSetting()
{
	// 기본 정보 초기화
	ResetData();

	// 업그레이드 레벨 초기화
	towerLevel_L = 0;
	towerLevel_R = 0;

	// 업그레이드 실시
	if (lastTowerLevel_L > lastTowerLevel_R)
	{
		while (towerLevel_R < lastTowerLevel_R + 1)
		{
			isRUpdatable = true;
			Upgrade();
			towerLevel_R++;
		}
		towerLevel_R--;

		while (towerLevel_L < lastTowerLevel_L + 1)
		{
			isLUpdatable = true;
			Upgrade();
			towerLevel_L++;
		}
		towerLevel_L--;
	}
	else
	{
		while (towerLevel_L < lastTowerLevel_L + 1)
		{
			isLUpdatable = true;
			Upgrade();
			towerLevel_L++;
		}
		towerLevel_L--;

		while (towerLevel_R < lastTowerLevel_R + 1)
		{
			isRUpdatable = true;
			Upgrade();
			towerLevel_R++;
		}
		towerLevel_R--;
	}

	isRightUpgradable = true;
	isLeftUpgradable = true;
}