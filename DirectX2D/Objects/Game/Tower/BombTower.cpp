#include "Framework.h"
#include "BombTower.h"
#include "Objects/Game/Projectile.h"
#include "Objects/Game/Balloon.h"
#include "Objects/Game/GameInstance/GameInstance.h"

BombTower::BombTower() : Tower(L"Textures/Character/bomb_tower/b_base.png", Bomb)
{
	/** 생성과 함께 처리되어야 할 부분들을 처리합니다. */
	// 이벤트 매핑
	SetEvent(bind(&BombTower::OnClick, this));

	// 텍스처 로드 & 데이터 리셋
	LoadTexture();
	ResetData();

	// 충돌체 생성
	selfCollider = new CircleCollider(15.0f);
	selfCollider->SetParent(this);

	// 공격 범위 생성
	attackRange = new CircleCollider(80.0f);
	attackRange->SetParent(this);

	// 발사체 생성
	projectiles.resize(5);
	for (Projectile*& p : projectiles)
	{
		p = new Projectile(L"Textures/Projectile/bomb_tower/bomb1.png", Type::EXPLOSION, t_data.range);

		p->SetActive(false);
	}

	// 특수 공격에 사용될 발사체 생성
	subProjectile.resize(16);
	for (Projectile*& p : subProjectile)
	{
		p = new Projectile(L"Textures/Projectile/tack_shooter/tack1.png", Type::SHARP, 150.0f);

		p->SetActive(false);
	}

	// 이벤트 등록
	Observer::Get()->AddParamEvent("Split", bind(&BombTower::FireSubProjectile, this, placeholders::_1));
}

BombTower::~BombTower()
{
	if (selfCollider)
	{
		delete selfCollider;
		selfCollider = nullptr;
	}

	if (attackRange)
	{
		delete attackRange;
		attackRange = nullptr;
	}

	for (Projectile* projectile : projectiles)
	{
		if (projectile)
		{
			delete projectile;
			projectile = nullptr;
		}
	}
	projectiles.clear();

	for (Projectile* projectile : subProjectile)
	{
		if (projectile)
		{
			delete projectile;
			projectile = nullptr;
		}
	}
	subProjectile.clear();
}

void BombTower::Update()
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

	if (attType == Split)
	{
		for (Projectile* p : subProjectile)
		{
			p->Update();
		}
	}

	Tower::Update();
}

void BombTower::Render()
{
	/** 타워 활성화 상태에서만 렌더링을 진행합니다. */
	if (!isActive) return;

	for (Projectile* p : projectiles)
	{
		p->Render();
	}

	if (attType == Split)
	{
		for (Projectile* p : subProjectile)
		{
			p->Render();
		}
	}

	Quad::Render();

	if (isSelected)
	{
		attackRange->Render();
	}
}

void BombTower::OnClick()
{
	/** 클릭시 등록된 이벤트 중 키 값이 Select인 이벤트를 실행합니다. */
	Observer::Get()->ExcuteParamEvent("Select", this);
}

void BombTower::Attack()
{
	/** 공격 범위 내에 적이 존재한다면 그중 첫 번째로 들어온 적의 방향으로 공격합니다. */
	if (enemies.size())
	{
		sumTime += DELTA;

		Vector2 direction = (enemies[0]->Pos() - Pos()).GetNormalized();
		localRotation.z = direction.Angle() + 80.0f;
		
		if (sumTime >= t_data.delay / GameInstance::Get()->GetGlobalSpeed())
		{
			for (Projectile* p : projectiles)
			{
				if (!p->Active())
				{
					p->SetStartPos(Pos());
					p->Fire(Pos(), direction);
					sumTime = 0.0f;
					break;
				}
			}
		}
	}
}

void BombTower::LoadTexture()
{
	/** 미리 사용할 텍스처들을 텍스처 맵에 추가합니다. */
	Texture::Add(L"Textures/Character/bomb_tower/up1.png");
	Texture::Add(L"Textures/Character/bomb_tower/l2.png");
	Texture::Add(L"Textures/Character/bomb_tower/l3.png");
	Texture::Add(L"Textures/Character/bomb_tower/l4.png");
	Texture::Add(L"Textures/Character/bomb_tower/r2.png");
	Texture::Add(L"Textures/Character/bomb_tower/r3.png");
	Texture::Add(L"Textures/Character/bomb_tower/r4.png");
}

void BombTower::ResetData()
{
	/** 타워의 데이터를 초기화합니다. */
	t_data.range = 80.0f;
	t_data.delay = 1.25f;
	t_data.damage = 1;
	t_data.power = 1;
	t_data.isCamoDetectable = false;
	t_data.projectile_texture = 0;
}

void BombTower::Upgrade()
{
	/** 업그레이드가 가능할 시 타워의 레벨에 따른 업그레이드를 진행합니다. */
	if (isLUpdatable)
	{
		switch (towerLevel_L)
		{
		case 0:
			SetTexture(L"Textures/Character/bomb_tower/b_base.png");
			t_data.projectile_texture = 0;
			subProjecileUpgrade = false;
			bulletSlowable = false;
			attType = Basic;
			break;

		case 1:
			if (towerLevel_L >= towerLevel_R)
				SetTexture(L"Textures/Character/bomb_tower/up1.png");
			t_data.range += 25;
			break;

		case 2:
			if (towerLevel_L > towerLevel_R)
				SetTexture(L"Textures/Character/bomb_tower/l2.png");
			attType = Split;
			break;

		case 3:
			if (towerLevel_L > towerLevel_R)
			{
				SetTexture(L"Textures/Character/bomb_tower/l3.png");
				t_data.projectile_texture = 0;
			}
			subProjecileUpgrade = true;
			t_data.range += 10;
			break;

		case 4:
			if (towerLevel_L > towerLevel_R)
			{
				SetTexture(L"Textures/Character/bomb_tower/l4.png");
			}
			attType = Basic;
			subProjecileUpgrade = false;
			bulletSlowable = true;
			break;
		}

		isLUpdatable = false;
	}

	if (isRUpdatable)
	{
		switch (towerLevel_R)
		{
		case 0:
			SetTexture(L"Textures/Character/bomb_tower/b_base.png");
			t_data.projectile_texture = 0;
			break;

		case 1:
			if (towerLevel_R >= towerLevel_L)
				SetTexture(L"Textures/Character/bomb_tower/up1.png");
			t_data.range += 10;
			t_data.damage += 1;
			break;

		case 2:
			if (towerLevel_R >= towerLevel_L)
			{
				SetTexture(L"Textures/Character/bomb_tower/r2.png");
				t_data.projectile_texture = 1;
			}
			t_data.range += 15;
			t_data.damage += 1;
			break;

		case 3:
			if (towerLevel_R > towerLevel_L)
			{
				SetTexture(L"Textures/Character/bomb_tower/r3.png");
				t_data.projectile_texture = 2;
			}
			t_data.damage += 1;
			bulletSpeed += 200.0f;
			t_data.delay = 0.9f;
			break;

		case 4:
			if (towerLevel_R > towerLevel_L)
			{
				SetTexture(L"Textures/Character/bomb_tower/r4.png");
				t_data.projectile_texture = 3;
			}
			t_data.delay = 0.6f;
			t_data.isCamoDetectable = true;
			break;
		}

		isRUpdatable = false;
	}

	UpdateData();
}

void BombTower::UpdateData()
{
	// 업데이트된 데이터를 바탕으로 실제 타워 정보를 업데이트합니다.
	for (Projectile* p : projectiles)
	{
		p->SetDamage(t_data.damage);
		p->SetPower(t_data.power);
		p->SetRange(t_data.range);
		p->SetSlow(bulletSlowable);

		if (attType == Split)
		{
			p->SetSplit(true);
		}
		else
		{
			p->SetSplit(false);
		}

		switch (t_data.projectile_texture)
		{
		case 0:
			p->SetTexture(L"Textures/Projectile/bomb_tower/bomb1.png");
			break;

		case 1:
			p->SetTexture(L"Textures/Projectile/bomb_tower/bomb2.png");
			break;

		case 2:
			p->SetTexture(L"Textures/Projectile/bomb_tower/bomb3.png");
			break;

		case 3:
			p->SetTexture(L"Textures/Projectile/bomb_tower/bomb4.png");
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

	if (attType == Split)
	{
		for (Projectile* sub : subProjectile)
		{
			sub->SetPower(100);
			sub->SetDamage(1);
		}
	}

	isUpdatable = false;
}

void BombTower::FireSubProjectile(void* balloon)
{
	/** 여러 갈래로 파편을 발사하기 위한 함수 */
	// 처음으로 충돌한 적의 정보를 토대로 해당 위치에서 4 ~ 8방향 파편 발사
	Balloon* enemy = (Balloon*)balloon;
	int cnt = 0;

	for (Projectile* sub : subProjectile)
	{
		if (!sub->Active())
		{
			if (subProjecileUpgrade)
			{
				if (cnt == 8)
				{
					break;
				}
			}
			else
			{
				if (cnt == 4)
				{
					break;
				}
			}

			sub->SetStartPos(enemy->Pos());

			switch (cnt)
			{
			case 0:
				sub->Fire(enemy->Pos(), Vector2(0, 1));
				sub->AddHit(enemy);
				break;
			case 1:
				sub->Fire(enemy->Pos(), Vector2(1, 0));
				sub->AddHit(enemy);
				break;
			case 2:
				sub->Fire(enemy->Pos(), Vector2(0, -1));
				sub->AddHit(enemy);
				break;
			case 3:
				sub->Fire(enemy->Pos(), Vector2(-1, 0));
				sub->AddHit(enemy);
				break;
			case 4:
				sub->Fire(enemy->Pos(), Vector2(1, 1).GetNormalized());
				sub->AddHit(enemy);
				break;
			case 5:
				sub->Fire(enemy->Pos(), Vector2(1, -1).GetNormalized());
				sub->AddHit(enemy);
				break;
			case 6:
				sub->Fire(enemy->Pos(), Vector2(-1, 1).GetNormalized());
				sub->AddHit(enemy);
				break;
			case 7:
				sub->Fire(enemy->Pos(), Vector2(-1, -1).GetNormalized());
				sub->AddHit(enemy);
				break;
			}

			cnt++;
		}
	}
}

void BombTower::LastUpgradeSetting()
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
