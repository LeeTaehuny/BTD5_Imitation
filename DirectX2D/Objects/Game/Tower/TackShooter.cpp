#include "Framework.h"
#include "TackShooter.h"
#include "Objects/Game/Projectile.h"
#include "Objects/Game/Balloon.h"
#include "Objects/Game/GameInstance/GameInstance.h"

TackShooter::TackShooter() : Tower(L"Textures/Character/tack_shooter/t_base.png", Tack)
{
	/** 생성과 함께 처리되어야 할 부분들을 처리합니다. */
	// 이벤트 매핑
	SetEvent(bind(&TackShooter::OnClick, this));

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
	projectiles.resize(32);
	for (Projectile*& p : projectiles)
	{
		p = new Projectile(L"Textures/Projectile/tack_shooter/tack1.png", Type::SHARP, t_data.range);

		p->SetActive(false);
	}

	// 공격시 출력될 이미지를 설정합니다.
	ringOgFire = new Quad(L"Textures/Projectile/tack_shooter/tack3.png");
	ringOgFire->SetActive(false);
	ringOgFire->SetParent(this);
}

TackShooter::~TackShooter()
{
	delete selfCollider;
	delete attackRange;
	delete ringOgFire;

	for (Projectile* projectile : projectiles)
	{
		delete projectile;
	}
	projectiles.clear();
}

void TackShooter::Update()
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
	ringOgFire->UpdateWorld();
	Tower::Update();
}

void TackShooter::Render()
{
	/** 타워 활성화 상태에서만 렌더링을 진행합니다. */
	if (!isActive) return;

	for (Projectile* p : projectiles)
	{
		p->Render();
	}

	if (ringOgFire->Active())
		ringOgFire->Render();

	Quad::Render();
	if (isSelected)
	{
		attackRange->Render();
	}
}

void TackShooter::OnClick()
{
	/** 클릭시 등록된 이벤트 중 키 값이 Select인 이벤트를 실행합니다. */
	Observer::Get()->ExcuteParamEvent("Select", this);
}

void TackShooter::Attack()
{
	/** 공격 범위 내에 적이 존재한다면 공격을 시작합니다. */
	if (enemies.size())
	{
		sumTime += DELTA;

		// 공격 타입이 RingOfFire인 경우
		if (attType == AttackType::RingOfFire)
		{
			// 공격 이미지를 눈에 보이도록 설정하고 범위 내 모든 풍선에 데미지를 줍니다.
			ringOgFire->SetActive(true);
			if (sumTime >= t_data.delay / GameInstance::Get()->GetGlobalSpeed())
			{
				for (Balloon* enemy : enemies)
				{
					enemy->takeDamage(t_data.damage);
				}
				sumTime = 0.0f;
			}
			return;
		}

		// 공격에 필요한 방향을 결정합니다.
		// * 기본 8방향
		// * 더블 16방향
		Vector2 rightDir = (Pos() + Vector2(10.0f, 0.0f) - Pos()).GetNormalized();
		Vector2 leftDir = (Pos() + Vector2(-10.0f, 0.0f) - Pos()).GetNormalized();
		Vector2 upDir = (Pos() + Vector2(0.0f, 10.0f) - Pos()).GetNormalized();
		Vector2 downDir = (Pos() + Vector2(0.0f, -10.0f) - Pos()).GetNormalized();
		Vector2 rightupDir = (Pos() + Vector2(10.0f, 10.0f) - Pos()).GetNormalized();
		Vector2 rightdownDir = (Pos() + Vector2(10.0f, -10.0f) - Pos()).GetNormalized();
		Vector2 leftupDir = (Pos() + Vector2(-10.0f, 10.0f) - Pos()).GetNormalized();
		Vector2 leftdownDir = (Pos() + Vector2(-10.0f, -10.0f) - Pos()).GetNormalized();

		int cnt = 0;

		if (sumTime >= t_data.delay / GameInstance::Get()->GetGlobalSpeed())
		{
			switch (attType)
			{
			case Basic:
				for (Projectile* p : projectiles)
				{
					if (!p->Active())
					{
						if (cnt == 8)
						{
							sumTime = 0.0f;
							break;
						}
						p->SetStartPos(Pos());
						p->SetPower(t_data.power);

						switch (cnt)
						{
						case 0:
							p->Fire(Pos(), rightDir);
							break;
						case 1:
							p->Fire(Pos(), leftDir);
							break;
						case 2:
							p->Fire(Pos(), upDir);
							break;
						case 3:
							p->Fire(Pos(), downDir);
							break;
						case 4:
							p->Fire(Pos(), rightupDir);
							break;
						case 5:
							p->Fire(Pos(), rightdownDir);
							break;
						case 6:
							p->Fire(Pos(), leftupDir);
							break;
						case 7:
							p->Fire(Pos(), leftdownDir);
							break;
						}
						cnt++;
					}
				}
				break;

			case Duble:
				Vector2 rightDir1 = (Pos() + Vector2(10.0f, 5.0f) - Pos()).GetNormalized();
				Vector2 rightDir2 = (Pos() + Vector2(10.0f, -5.0f) - Pos()).GetNormalized();
				Vector2 leftDir1 = (Pos() + Vector2(-10.0f, 5.0f) - Pos()).GetNormalized();
				Vector2 leftDir2 = (Pos() + Vector2(-10.0f, -5.0f) - Pos()).GetNormalized();
				Vector2 upDir1 = (Pos() + Vector2(5.0f, 10.0f) - Pos()).GetNormalized();
				Vector2 upDir2 = (Pos() + Vector2(-5.0f, 10.0f) - Pos()).GetNormalized();
				Vector2 downDir1 = (Pos() + Vector2(5.0f, -10.0f) - Pos()).GetNormalized();
				Vector2 downDir2 = (Pos() + Vector2(-5.0f, -10.0f) - Pos()).GetNormalized();

				for (Projectile* p : projectiles)
				{
					if (!p->Active())
					{
						if (cnt == 16)
						{
							sumTime = 0.0f;
							break;
						}
						p->SetStartPos(Pos());
						p->SetPower(t_data.power);

						switch (cnt)
						{
						case 0:
							p->Fire(Pos(), rightDir);
							break;
						case 1:
							p->Fire(Pos(), leftDir);
							break;
						case 2:
							p->Fire(Pos(), upDir);
							break;
						case 3:
							p->Fire(Pos(), downDir);
							break;
						case 4:
							p->Fire(Pos(), rightupDir);
							break;
						case 5:
							p->Fire(Pos(), rightdownDir);
							break;
						case 6:
							p->Fire(Pos(), leftupDir);
							break;
						case 7:
							p->Fire(Pos(), leftdownDir);
							break;
						case 8:
							p->Fire(Pos(), leftDir1);
							break;
						case 9:
							p->Fire(Pos(), leftDir2);
							break;
						case 10:
							p->Fire(Pos(), rightDir1);
							break;
						case 11:
							p->Fire(Pos(), rightDir2);
							break;
						case 12:
							p->Fire(Pos(), upDir1);
							break;
						case 13:
							p->Fire(Pos(), upDir2);
							break;
						case 14:
							p->Fire(Pos(), downDir1);
							break;
						case 15:
							p->Fire(Pos(), downDir2);
							break;
						}
						cnt++;
					}
				}
				break;
			}
		}
	}
	else
	{
		ringOgFire->SetActive(false);
	}
}

void TackShooter::LoadTexture()
{
	/** 미리 사용할 텍스처들을 텍스처 맵에 추가합니다. */
	Texture::Add(L"Textures/Character/tack_shooter/up1.png");
	Texture::Add(L"Textures/Character/tack_shooter/up2.png");
	Texture::Add(L"Textures/Character/tack_shooter/l3.png");
	Texture::Add(L"Textures/Character/tack_shooter/l4.png");
	Texture::Add(L"Textures/Character/tack_shooter/r3.png");
	Texture::Add(L"Textures/Character/tack_shooter/r4.png");
}

void TackShooter::ResetData()
{
	/** 타워의 데이터를 초기화합니다. */
	t_data.range = 50.0f;
	t_data.delay = 0.9f;
	t_data.damage = 1;
	t_data.power = 1;
	t_data.isCamoDetectable = false;
	t_data.projectile_texture = 0;
}

void TackShooter::Upgrade()
{
	/** 업그레이드가 가능할 시 타워의 레벨에 따른 업그레이드를 진행합니다. */
	if (isLUpdatable)
	{
		switch (towerLevel_L)
		{
		case 0:
			SetTexture(L"Textures/Character/tack_shooter/t_base.png");
			t_data.projectile_texture = 0;
			attType = Basic;
			break;

		case 1:
			if (towerLevel_L >= towerLevel_R)
				SetTexture(L"Textures/Character/tack_shooter/up1.png");
			t_data.delay = 0.7f;
			break;

		case 2:
			if (towerLevel_L >= towerLevel_R)
				SetTexture(L"Textures/Character/tack_shooter/up2.png");
			t_data.delay = 0.5f;
			break;

		case 3:
			if (towerLevel_L > towerLevel_R)
				SetTexture(L"Textures/Character/tack_shooter/l3.png");
			break;

		case 4:
			if (towerLevel_L > towerLevel_R)
			{
				SetTexture(L"Textures/Character/tack_shooter/l4.png");
			}
			t_data.range += 10;
			t_data.delay = 0.4f;
			t_data.isCamoDetectable = true;
			break;
		}

		isLUpdatable = false;
	}

	if (isRUpdatable)
	{
		switch (towerLevel_R)
		{
		case 0:
			SetTexture(L"Textures/Character/tack_shooter/t_base.png");
			t_data.projectile_texture = 0;
			attType = Basic;
			break;

		case 1:
			if (towerLevel_R >= towerLevel_L)
				SetTexture(L"Textures/Character/tack_shooter/up1.png");
			t_data.range += 10;
			break;

		case 2:
			if (towerLevel_R >= towerLevel_L)
			{
				SetTexture(L"Textures/Character/tack_shooter/up2.png");
			}
			t_data.range += 10;
			break;

		case 3:
			if (towerLevel_R > towerLevel_L)
			{
				SetTexture(L"Textures/Character/tack_shooter/r3.png");
				t_data.projectile_texture = 1;
			}
			t_data.power = 2;
			break;

		case 4:
			if (towerLevel_R > towerLevel_L)
			{
				SetTexture(L"Textures/Character/tack_shooter/r4.png");
			}
			t_data.power = 100;
			attType = AttackType::Duble;
			break;
		}

		isRUpdatable = false;
	}

	UpdateData();
}

void TackShooter::UpdateData()
{
	// 업데이트된 데이터를 바탕으로 실제 타워 정보를 업데이트합니다.
	for (Projectile* p : projectiles)
	{
		p->SetDamage(t_data.damage);
		p->SetPower(t_data.power);
		p->SetRange(t_data.range);

		if (t_data.projectile_texture == 0)
		{
			p->SetTexture(L"Textures/Projectile/tack_shooter/tack1.png");
			p->SetType(Type::SHARP);
		} 
		else if (t_data.projectile_texture == 1)
		{
			p->SetTexture(L"Textures/Projectile/tack_shooter/tack2.png");
			p->SetType(Type::EVERY);
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

	if (towerLevel_L == 3)
		attType = Duble;
	else if (towerLevel_L == 4)
		attType = RingOfFire;
	else if (towerLevel_R == 4)
		attType = Duble;
	else
		attType = Basic;
}

void TackShooter::LastUpgradeSetting()
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