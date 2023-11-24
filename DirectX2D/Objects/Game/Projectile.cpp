#include "Framework.h"
#include "Projectile.h"
#include "Balloon.h"
#include "Objects/Game/GameInstance/GameInstance.h"

Projectile::Projectile(wstring file, Type type, float range)
	: Quad(file), range(range), type(type)
{
	// 충돌체 생성
	collider = new CircleCollider(texture->GetSize().y / 2);
	collider->SetParent(this);

	// 폭발타입 공격에 사용될 폭발 충돌체 생성
	explosionCollider = new CircleCollider(texture->GetSize().y * 2);
	explosionCollider->SetParent(this);
	explosionCollider->SetActive(false);

	// 폭발타입 공격에 사용될 이펙트 추가
	EffectManager::Get()->Add("bomb", 20, L"Textures/Effect/bomb.png", 4, 2);
}

Projectile::~Projectile()
{
	delete collider;
	collider = nullptr;
	delete explosionCollider;
	explosionCollider = nullptr;
}

void Projectile::Update()
{
	// 이펙트 업데이트
	EffectManager::Get()->Update();

	// 활성화된 경우에만 업데이트를 진행합니다.
	if (!isActive) return;

	// 발사체의 공격 구분
	// * 방향벡터, 각도
	if (bIsSubProjectile)
	{
		localPosition += Vector2(cos(angle), sin(angle)) * speed * DELTA * GameInstance::Get()->GetGlobalSpeed();
	}
	else
	{
		localPosition += direction * speed * DELTA * GameInstance::Get()->GetGlobalSpeed();
	}

	// 사정거리를 벗어난 총알은 비활성화합니다.
	if ((startPos - localPosition).Length() > range)
	{
		localPosition = startPos;
		isActive = false;
	}

	Attack();
	
	UpdateWorld();
	collider->UpdateWorld();
	explosionCollider->UpdateWorld();
}

void Projectile::Render()
{
	// 이펙트를 화면에 렌더링합니다.
	EffectManager::Get()->Render();

	// 활성화된 경우에만 렌더링합니다.
	if (!isActive) return;
	Quad::Render();
}

void Projectile::Fire(Vector2 pos, Vector2 direction)
{
	isActive = true;
	hit.clear();
	localPosition = pos;
	this->direction = direction;

	localRotation.z = direction.Angle();
}

void Projectile::Fire(Vector2 pos, Vector2 direction, Balloon* balloon)
{
	isActive = true;
	bIsSubProjectile = false;
	angle = 0.0f;
	hit.clear();
	hit.push_back(balloon);
	localPosition = pos;
	this->direction = direction;

	localRotation.z = direction.Angle();
}

void Projectile::SubFire(Vector2 pos, float angle)
{
	isActive = true;
	bIsSubProjectile = true;
	this->angle = angle;
	hit.clear();
	localPosition = pos;

	localRotation.z = angle;
}

void Projectile::AddHit(Balloon* b)
{
	hit.push_back(b);
}

void Projectile::Attack()
{
	// 풍선의 타입에 따라 공격을 수행합니다.
	vector<Balloon*> balloons = GameInstance::Get()->GetBalloons();

	switch (type)
	{
	case Type::SHARP:
		for (Balloon* balloon : balloons)
		{
			if (!balloon->Active()) continue;

			if (find(hit.begin(), hit.end(), balloon) != hit.end()) continue;

			if (balloon->GetCollider()->IsCircleCollision((CircleCollider*)collider))
			{
				// 타입 제한
				if (!(balloon->GetType() == Balloon_Type::SHARP_RESIST))
				{
					if (GameInstance::Get()->GetSoundEffectPlayable())
					{
						Audio::Get()->Play("DartHit", 0.5f);
					}
					balloon->takeDamage(damage, this);
				}
				else if (balloon->GetType() == Balloon_Type::SHARP_RESIST)
				{
					if (GameInstance::Get()->GetSoundEffectPlayable())
					{
						Audio::Get()->Play("lead_Guard", 0.5f);
					}
				}
				hit.push_back(balloon);
				power--;

				if (power <= 0)
				{
					localPosition = startPos;
					isActive = false;
				}
				break;
			}
		}
		break;

	case Type::EVERY:
		for (Balloon* balloon : balloons)
		{
			if (!balloon->Active()) continue;

			if (find(hit.begin(), hit.end(), balloon) != hit.end()) continue;

			if (balloon->GetCollider()->IsCircleCollision((CircleCollider*)collider))
			{
				if (GameInstance::Get()->GetSoundEffectPlayable())
				{
					Audio::Get()->Play("DartHit", 0.5f);
				}
				balloon->takeDamage(damage, this);

				hit.push_back(balloon);
				power--;

				if (power <= 0)
				{
					localPosition = startPos;
					isActive = false;
				}
				break;
			}
		}
		break;

	case Type::EXPLOSION:
		bool once = true;

		for (Balloon* balloon : balloons)
		{
			if (!balloon->Active()) continue;

			if (find(hit.begin(), hit.end(), balloon) != hit.end()) continue;

			if (balloon->GetCollider()->IsCircleCollision((CircleCollider*)collider))
			{
				explosion = true;

				if (bIsSplit && once)
				{
					Observer::Get()->ExcuteParamEvent("Split", balloon);
					once = false;
				}

				EffectManager::Get()->Play("bomb", balloon->Pos());

				if (GameInstance::Get()->GetSoundEffectPlayable())
				{
					Audio::Get()->Play("BombHit", 0.5f);
				}
			}
		}

		if (explosion)
		{
			explosionCollider->SetActive(true);
			for (Balloon* balloon : balloons)
			{
				if (balloon->GetCollider()->IsCircleCollision((CircleCollider*)explosionCollider))
				{
					hit.push_back(balloon);
				}
			}

			for (Balloon* balloon : hit)
			{
				if (isSlow && !(balloon->GetType() == Balloon_Type::ICE_RESIST) && !(balloon->GetType() == Balloon_Type::DUBLE_RESIST || balloon->GetType() == Balloon_Type::EXPLOSION_RESIST))
				{
					balloon->SetStopTime(1.0f);
					balloon->SetSlowRate(50.0f);
					balloon->SetState(SLOW);
				}

				// 타입 제한
				if (!(balloon->GetType() == Balloon_Type::DUBLE_RESIST || balloon->GetType() == Balloon_Type::EXPLOSION_RESIST))
				{
					balloon->takeDamage(damage, this);
				}
			}

			hit.clear();
			explosion = false;

			localPosition = startPos;
			isActive = false;
		}
		break;
	}
}


