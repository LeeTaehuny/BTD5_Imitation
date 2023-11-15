#include "Framework.h"
#include "DartMonkey.h"
#include "Objects/Game/Projectile.h"
#include "Objects/Game/Balloon.h"
#include "Objects/Game/GameInstance/GameInstance.h"

DartMonkey::DartMonkey() : Tower(L"Textures/Character/dart_monkey/d_base.png", Dart)
{
	/** 생성과 함께 처리되어야 할 부분들을 처리합니다. */
	// 이벤트 매핑
	SetEvent(bind(&DartMonkey::OnClick, this));

	// 텍스처 로드 & 데이터 리셋
	LoadTexture();
	ResetData();

	// 충돌체 생성
	selfCollider = new CircleCollider(15.0f);
	selfCollider->SetParent(this);
	selfCollider->GlobalPos() = localPosition;
	
	// 공격 범위 생성
	attackRange = new CircleCollider(t_data.range);
	attackRange->SetParent(this);
	attackRange->GlobalPos() = localPosition;

	// 발사체 생성
	projectiles.resize(15);
	for (Projectile*& p : projectiles)
	{
		p = new Projectile(L"Textures/Projectile/dart_monky/dart1.png", Type::SHARP, t_data.range);
		
		p->SetActive(false);
	}
}

DartMonkey::~DartMonkey()
{
	delete selfCollider;
	delete attackRange;

	for (Projectile* projectile : projectiles)
	{
		delete projectile;
	}
	projectiles.clear();
}

void DartMonkey::Update()
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

	Tower::Update();
}

void DartMonkey::Render()
{
	/** 타워 활성화 상태에서만 렌더링을 진행합니다. */
	if (!isActive) return;

	for (Projectile* p : projectiles)
	{
		p->Render();
	}


	Quad::Render();
	if (isSelected)
	{
		attackRange->Render();
	}
}

void DartMonkey::OnClick()
{
	/** 클릭시 등록된 이벤트 중 키 값이 Select인 이벤트를 실행합니다. */
	Observer::Get()->ExcuteParamEvent("Select", this);
}

void DartMonkey::Attack()
{
	/** 공격 범위 내에 적이 존재한다면 그 중 첫 번째로 들어온 적의 방향으로 공격합니다. */
	if (enemies.size())
	{
		if (enemies[0]->GetHide()) return;

		sumTime += DELTA;
		
		// 공격에 필요한 방향을 결정합니다.
		// * 해당 방향에서 특정 각도만큼 떨어진 위치로 추가 공격을 진행하기 위해 방향을 추가해줍니다.
		Vector2 direction = (enemies[0]->Pos() - Pos()).GetNormalized();
		float angle1 = direction.Angle() + (20.0f * XM_PI / 180.0f);
		float angle2 = direction.Angle() - (20.0f * XM_PI / 180.0f);
		float angle3 = direction.Angle() + (40.0f * XM_PI / 180.0f);
		float angle4 = direction.Angle() - (40.0f * XM_PI / 180.0f);

		int cnt = 0;
		localRotation.z = direction.Angle() - 90.0f;

		if (sumTime >= t_data.delay / GameInstance::Get()->GetGlobalSpeed())
		{
			// 공격 타입에 따른 공격을 실행합니다.
			switch (attType)
			{
			case Basic:
				for (Projectile* p : projectiles)
				{
					if (!p->Active())
					{
						p->SetStartPos(Pos());
						p->SetPower(t_data.power);
						p->Fire(Pos(), direction);
						sumTime = 0.0f;
						break;
					}
				}
				break;

			case Triple:
				for (Projectile* p : projectiles)
				{
					if (!p->Active())
					{
						if (cnt == 3)
						{
							sumTime = 0.0f;
							break;
						}
						p->SetStartPos(Pos());
						p->SetPower(t_data.power);
						
						switch (cnt)
						{
						case 0:
							p->Fire(Pos(), direction);
							break;
						case 1:
							p->SubFire(Pos(), angle1);
							break;
						case 2:
							p->SubFire(Pos(), angle2);
							break;
						}

						cnt++;
					}
				}
				break;

			case Penta:
				for (Projectile* p : projectiles)
				{
					if (!p->Active())
					{
						if (cnt == 5)
						{
							sumTime = 0.0f;
							break;
						}
						p->SetStartPos(Pos());
						p->SetPower(t_data.power);

						switch (cnt)
						{
						case 0:
							p->Fire(Pos(), direction);
							break;
						case 1:
							p->SubFire(Pos(), angle1);
							break;
						case 2:
							p->SubFire(Pos(), angle2);
							break;
						case 3:
							p->SubFire(Pos(), angle3);
							break;
						case 4:
							p->SubFire(Pos(), angle4);
							break;
						}
						cnt++;
					}
				}
				break;
			}
		}
	}
}

void DartMonkey::LoadTexture()
{
	/** 미리 사용할 텍스처들을 텍스처 맵에 추가합니다. */
	Texture::Add(L"Textures/Character/dart_monkey/up1.png");
	Texture::Add(L"Textures/Character/dart_monkey/up2.png");
	Texture::Add(L"Textures/Character/dart_monkey/l3.png");
	Texture::Add(L"Textures/Character/dart_monkey/l4.png");
	Texture::Add(L"Textures/Character/dart_monkey/r3.png");
	Texture::Add(L"Textures/Character/dart_monkey/r4.png");
}

void DartMonkey::ResetData()
{
	/** 타워의 데이터를 초기화합니다. */
	t_data.range = 100.0f;
	t_data.delay = 0.9f;
	t_data.damage = 1;
	t_data.power = 1;
	t_data.isCamoDetectable = false;
	t_data.projectile_texture = 0;
}

void DartMonkey::Upgrade()
{
	/** 업그레이드가 가능할 시 타워의 레벨에 따른 업그레이드를 진행합니다. */
	if (isLUpdatable)
	{
		switch (towerLevel_L)
		{
		case 0:
			SetTexture(L"Textures/Character/dart_monkey/d_base.png");
			t_data.projectile_texture = 0;
			attType = Basic;
			break;

		case 1:
			if (towerLevel_L >= towerLevel_R)
				SetTexture(L"Textures/Character/dart_monkey/up1.png");
			t_data.range += 25;
			break;

		case 2:
			if (towerLevel_L >= towerLevel_R)
				SetTexture(L"Textures/Character/dart_monkey/up2.png");
			t_data.range += 25;
			t_data.isCamoDetectable = true;
			break;

		case 3:
			if (towerLevel_L > towerLevel_R)
			{
				SetTexture(L"Textures/Character/dart_monkey/l3.png");
				t_data.projectile_texture = 2;
			}
			t_data.power = 18;
			t_data.delay = 1.54f;
			break;

		case 4:
			if (towerLevel_L > towerLevel_R)
			{
				SetTexture(L"Textures/Character/dart_monkey/l4.png");
				t_data.projectile_texture = 3;
			}
			t_data.power = 100;
			t_data.range += 25;
			t_data.delay = 1.4f;
			break;
		}

		isLUpdatable = false;
	}

	if (isRUpdatable)
	{
		switch (towerLevel_R)
		{
		case 0:
			SetTexture(L"Textures/Character/dart_monkey/d_base.png");
			t_data.projectile_texture = 0;
			attType = Basic;
			break;

		case 1:
			if (towerLevel_R >= towerLevel_L)
				SetTexture(L"Textures/Character/dart_monkey/up1.png");
			t_data.damage += 1;
			break;

		case 2:
			if (towerLevel_R >= towerLevel_L)
			{
				SetTexture(L"Textures/Character/dart_monkey/up2.png");
				t_data.projectile_texture = 1;
			}
			t_data.damage += 1;
			break;

		case 3:
			if (towerLevel_R > towerLevel_L)
			{
				SetTexture(L"Textures/Character/dart_monkey/r3.png");
				t_data.projectile_texture = 1;
			}
			t_data.power += 1;
			break;

		case 4:
			if (towerLevel_R > towerLevel_L)
			{
				SetTexture(L"Textures/Character/dart_monkey/r4.png");
				t_data.projectile_texture = 1;
			}
			break;
		}

		isRUpdatable = false;
	}
	
	UpdateData();
}

void DartMonkey::UpdateData()
{
	// 업데이트된 데이터를 바탕으로 실제 타워 정보를 업데이트합니다.
	for (Projectile* p : projectiles)
	{
		p->SetDamage(t_data.damage);
		p->SetPower(t_data.power);
		p->SetRange(t_data.range);

		switch (t_data.projectile_texture)
		{
		case 0:
			p->SetTexture(L"Textures/Projectile/dart_monky/dart1.png");
			break;
		case 1:
			p->SetTexture(L"Textures/Projectile/dart_monky/dart4.png");
			break;

		case 2:
			p->SetTexture(L"Textures/Projectile/dart_monky/dart2.png");
			p->SetType(Type::EVERY);
			break;

		case 3:
			p->SetTexture(L"Textures/Projectile/dart_monky/dart3.png");
			break;
		}
	}

	attackRange = new CircleCollider(t_data.range);
	attackRange->SetParent(this);
	attackRange->GlobalPos() = localPosition;

	Collider* temp = new CircleCollider(t_data.range);
	temp->SetParent(this);
	temp->GlobalPos() = localPosition;
	delete attackRange;
	attackRange = temp;

	isUpdatable = false;

	if (towerLevel_R == 3)
		attType = Triple;
	else if (towerLevel_R == 4)
		attType = Penta;
	else
		attType = Basic;
}

void DartMonkey::LastUpgradeSetting()
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