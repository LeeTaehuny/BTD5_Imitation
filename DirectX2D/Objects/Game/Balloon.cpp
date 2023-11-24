#include "Framework.h"
#include "Balloon.h"
#include "Objects/Game/GameInstance/GameInstance.h"
#include "Objects/Game/Projectile.h"

Balloon::Balloon(int _hp) : hp(hp), Quad(Vector2(35.0f, 42.0f))
{
	/** 생성과 함께 실행할 로직 */
	// 텍스처 로드
	LoadTexture();

	// 충돌체 생성
	collider = new CircleCollider(10.0f);
	collider->SetParent(this);
}

Balloon::~Balloon()
{
	delete collider;
}

void Balloon::Init()
{
	// 초기화 함수입니다.
	UpdateTexture();
	if (hp == 17) shield = 10;
	Pos() = route[0];
	routeCnt = 0;
	speed = maxSpeed;
	stopTime = 0.0f;
	state = NONE;
	SetMeltDown(false);
	SetIsSlow(false);
	SetSlowRate(0.0f);
}

void Balloon::SecondInit()
{
	// 풍선이 새로 생성되면 실행할 초기화 함수입니다.
	UpdateTexture();
	if (state == NONE)
		speed = maxSpeed;
}

void Balloon::Update()
{
	// 풍선이 활성화되어 있는 경우에만 업데이트를 진행합니다.
	if (!isActive) return;

	// 풍선이 목적지(종료지점)에 도착하면 풍선의 체력에 따라 플레이어의 체력을 감소시켜줍니다.
	if (hp <= 0 || (Pos() - route[route.size() - 1]).Length() < 1.0f)
	{
		if (hp <= 5)
		{
			GameInstance::Get()->DecreaseHp(hp);
		}
		else if (hp == 7 || hp == 9)
		{
			GameInstance::Get()->DecreaseHp(11);
		}
		else if (hp == 11 || hp == 13)
		{
			GameInstance::Get()->DecreaseHp(23);
		}
		else if (hp == 15)
		{
			GameInstance::Get()->DecreaseHp(45);
		}
		else if (hp == 17)
		{
			GameInstance::Get()->DecreaseHp(100);
		}
		else if (hp >= 19 && hp <= 22)
		{
			GameInstance::Get()->DecreaseHp(hp - 18);
		}
		else if (hp >= 24 && hp <= 26)
		{
			GameInstance::Get()->DecreaseHp(hp - 23);
		}

		isActive = false;
	}

	// 맵 타입이 원숭이도로라면 특정 위치에서 풍선이 잠시 안보이도록 설정합니다.
	if (mapType == Map::MonkeyLane && (routeCnt == 6 || routeCnt == 17))
	{
		hide = true;
	}
	else
	{
		hide = false;
	}

	UpdateTexture();

	if (bIsMeltdown)
	{
		meltTime += DELTA;

		if (meltTime >= 2.0f / GameInstance::Get()->GetGlobalSpeed())
		{
			bIsMeltdown = false;
			meltTime = 0.0f;
		}
	}

	// 풍선의 상태에 따라 구분합니다.
	// * NONE : 일반적인 이동
	// * SLOW : 속도 감소
	// * STOP : 특정 시간 멈춤
	switch (state)
	{
	case NONE:
		Move();
		break;

	case SLOW:
		if (bIsSlow)
		{
			speed *= (100.0f - slowRate) / 100.0f;
			state = NONE;
			break;
		}
		speed *= (100.0f - slowRate) / 100.0f;
		Move();

		state = NONE;
		bIsSlow = true;
		break;

	case STOP:
		speed = 0;
		Move();

		sTime += DELTA;

		if (sTime >= stopTime / GameInstance::Get()->GetGlobalSpeed())
		{
			sTime = 0.0f;
			speed = maxSpeed;

			if (bIsSlow)
			{
				state = SLOW;
			}
			else
			{
				state = NONE;
			}

			bIsMeltdown = true;
		}
		break;
	}

	// 풍선 타입이 재생성인 경우 2초마다 체력을 재생합니다.
	if (type == Balloon_Type::REGENERATE)
	{
		sumTime += DELTA;

		if (sumTime >= 2.0f)
		{
			if (hp < 22)
				hp++;

			sumTime = 0.0f;
		}
	}
	
	collider->UpdateWorld();
	GameObject::UpdateWorld();
}

void Balloon::Render()
{
	// 풍선이 활성화 되어있다면 렌더링합니다.
	if (!isActive) return;
	// 풍선이 hide라면 렌더링을 중지합니다.
	if (hide) return;

	Quad::Render();
}

void Balloon::LoadTexture()
{
	// 사용될 텍스처들을 미리 텍스처 맵에 추가합니다.
	Texture::Add(L"Textures/Ballons/red.png");
	Texture::Add(L"Textures/Ballons/blue.png");
	Texture::Add(L"Textures/Ballons/green.png");
	Texture::Add(L"Textures/Ballons/yellow.png");
	Texture::Add(L"Textures/Ballons/pink.png");
	Texture::Add(L"Textures/Ballons/black.png");
	Texture::Add(L"Textures/Ballons/white.png");
	Texture::Add(L"Textures/Ballons/zebra.png");
	Texture::Add(L"Textures/Ballons/lead.png");
	Texture::Add(L"Textures/Ballons/rainbow.png");
	Texture::Add(L"Textures/Ballons/ceramic.png");

	Texture::Add(L"Textures/Ballons/regrow_red.png");
	Texture::Add(L"Textures/Ballons/regrow_blue.png");
	Texture::Add(L"Textures/Ballons/regrow_green.png");
	Texture::Add(L"Textures/Ballons/regrow_yellow.png");

	Texture::Add(L"Textures/Ballons/camo_green.png");
	Texture::Add(L"Textures/Ballons/camo_yellow.png");
	Texture::Add(L"Textures/Ballons/camo_white.png");
}

void Balloon::UpdateTexture()
{
	/** 체력에 따라 텍스처를 업데이트합니다. */
	// COMMON
	// 0 풍선 삭제
	// 1 빨강 30
	// 2 파랑 40
	// 3 초록 50
	// 4 노랑 60
	// 5 분홍 70
	
	// EXPLOSION_RESIST
	// 6 풍선 삭제
	// 7 검정 50 - 분홍x2
	
	// ICE_RESIST
	// 8 풍선 삭제
	// 9 흰색 60 - 분홍x2

	// SHARP_RESIST
	// 10 풍선 삭제
	// 11 납 30 - 검정x2

	// DUBLE_RESIST
	// 12 풍선 삭제
	// 13 얼룩말 50 - 검정1 흰색1

	// COMMON
	// 14 풍선 삭제
	// 15 무지개 55 - 검정x2 흰색x2
	// 16 풍선 삭제
	// 17 세라믹 65 - 무지개x2, 체력 10

	// REGENERATE
	// 18 풍선 삭제
	// 19 재생성 빨강 30
	// 20 재생성 파랑 40
	// 21 재생성 초록 50
	// 22 재생성 노랑 60

	// CAMO
	// 23 풍선 삭제
	// 24 은신 초록 50
	// 25 은신 노랑 60
	// 26 은신 하양 60

	switch (hp)
	{
	// COMMON
	case 1: // 빨강
		SetTexture(L"Textures/Ballons/red.png");
		type = Balloon_Type::COMMON;
		maxSpeed = 30.0f;
		break;
	case 2: // 파랑
		SetTexture(L"Textures/Ballons/blue.png");
		type = Balloon_Type::COMMON;
		maxSpeed = 40.0f;
		break;
	case 3: // 초록
		SetTexture(L"Textures/Ballons/green.png");
		type = Balloon_Type::COMMON;
		maxSpeed = 50.0f;
		break;
	case 4: // 노랑
		SetTexture(L"Textures/Ballons/yellow.png");
		type = Balloon_Type::COMMON;
		maxSpeed = 60.0f;
		break;
	case 5: // 분홍
		SetTexture(L"Textures/Ballons/pink.png");
		type = Balloon_Type::COMMON;
		maxSpeed = 70.0f;
		break;

	case 15: // 무지개
		SetTexture(L"Textures/Ballons/rainbow.png");
		type = Balloon_Type::COMMON;
		maxSpeed = 55.0f;
		break;
	case 17: // 세라믹
		SetTexture(L"Textures/Ballons/ceramic.png");
		type = Balloon_Type::COMMON;
		maxSpeed = 65.0f;
		break;

	// EXPLOSION_RESIST
	case 7: // 검정
		SetTexture(L"Textures/Ballons/black.png");
		type = Balloon_Type::EXPLOSION_RESIST;
		maxSpeed = 50.0f;
		break;

	// ICE_RESIST
	case 9: // 흰색
		SetTexture(L"Textures/Ballons/white.png");
		type = Balloon_Type::ICE_RESIST;
		maxSpeed = 60.0f;
		break;

	// SHARP_RESIST
	case 11: // 납
		SetTexture(L"Textures/Ballons/lead.png");
		type = Balloon_Type::SHARP_RESIST;
		maxSpeed = 30.0f;
		break;

	// DUBLE_RESIST
	case 13: // 얼룩말
		SetTexture(L"Textures/Ballons/zebra.png");
		type = Balloon_Type::DUBLE_RESIST;
		maxSpeed = 50.0f;
		break;

	// REGENERATE
	case 19: // 재생성 빨강
		SetTexture(L"Textures/Ballons/regrow_red.png");
		type = Balloon_Type::REGENERATE;
		maxSpeed = 30.0f;
		break;
	case 20: // 재생성 파랑
		SetTexture(L"Textures/Ballons/regrow_blue.png");
		type = Balloon_Type::REGENERATE;
		maxSpeed = 40.0f;
		break;
	case 21: // 재생성 초록
		SetTexture(L"Textures/Ballons/regrow_green.png");
		type = Balloon_Type::REGENERATE;
		maxSpeed = 50.0f;
		break;
	case 22: // 재생성 노랑
		SetTexture(L"Textures/Ballons/regrow_yellow.png");
		type = Balloon_Type::REGENERATE;
		maxSpeed = 60.0f;
		break;

	// CAMO
	case 24: // 은신 초록
		SetTexture(L"Textures/Ballons/camo_green.png");
		type = Balloon_Type::CAMO;
		maxSpeed = 50.0f;
		break;
	case 25: // 은신 노랑
		SetTexture(L"Textures/Ballons/camo_yellow.png");
		type = Balloon_Type::CAMO;
		maxSpeed = 60.0f;
		break;
	case 26: // 은신 하양
		SetTexture(L"Textures/Ballons/camo_white.png");
		type = Balloon_Type::CAMO;
		maxSpeed = 60.0f;
		break;
	}
}

void Balloon::Move()
{
	// 저장된 길 정보를 바탕으로 이동합니다.
	if (routeCnt >= route.size() - 1)
	{
		return;
	}

	startPos = route[routeCnt];
	endPos = route[routeCnt + 1];
	Vector2 dir = endPos - startPos;
	float length = dir.Length();

	direction = dir.GetNormalized();
	Pos() += direction * DELTA * speed * GameInstance::Get()->GetGlobalSpeed();

	if (length <= (Pos() - startPos).Length())
	{
		routeCnt++;
	}
}

void Balloon::takeDamage(int damage, Projectile* p)
{
	// 풍선에 데미지를 적용하는 함수입니다.
	int temp = hp;
	if (hp <= 5)
	{
		hp -= damage;

		if (hp <= 0)
		{
			hp = 0;
			GameInstance::Get()->IncreaseMoney(temp);
			state = NONE;
			SetMeltDown(false);
			SetIsSlow(false);
			SetSlowRate(0.0f);
			SetActive(false);
		}
		else
		{
			GameInstance::Get()->IncreaseMoney(damage);
		}
			
	}
	else if (hp == 7)
	{
		hp -= damage;

		if (hp < 7)
		{
			hp = 0;
			GameInstance::Get()->IncreaseMoney(1);
			state = NONE;
			SetMeltDown(false);
			SetIsSlow(false);
			SetSlowRate(0.0f);
			SetActive(false);
			CreatePinkBalloon(p, 1.0f);
			CreatePinkBalloon(p, 4.0f);
		}
	}
	else if (hp == 9)
	{
		hp -= damage;

		if (hp < 9)
		{
			hp = 0;
			GameInstance::Get()->IncreaseMoney(1);
			state = NONE;
			SetMeltDown(false);
			SetIsSlow(false);
			SetSlowRate(0.0f);
			SetActive(false);
			CreatePinkBalloon(p, 1.0f);
			CreatePinkBalloon(p, 4.0f);
		}
	}
	else if (hp == 11)
	{
		hp -= damage;

		if (hp < 11)
		{
			hp = 0;
			GameInstance::Get()->IncreaseMoney(1);
			state = NONE;
			SetMeltDown(false);
			SetIsSlow(false);
			SetSlowRate(0.0f);
			SetActive(false);
			CreateBlackBalloon(p, 1.0f);
			CreateBlackBalloon(p, 4.0f);
		}
	}
	else if (hp == 13)
	{
		hp -= damage;

		if (hp < 13)
		{
			hp = 0;
			GameInstance::Get()->IncreaseMoney(1);
			state = NONE;
			SetMeltDown(false);
			SetIsSlow(false);
			SetSlowRate(0.0f);
			SetActive(false);
			CreateBlackBalloon(p, 1.0f);
			CreateWhiteBalloon(p, 4.0f);
		}
	}
	else if (hp == 15)
	{
		hp -= damage;

		if (hp < 15)
		{
			hp = 0;
			GameInstance::Get()->IncreaseMoney(1);
			state = NONE;
			SetMeltDown(false);
			SetIsSlow(false);
			SetSlowRate(0.0f);
			SetActive(false);
			CreateBlackBalloon(p, 1.0f);
			CreateBlackBalloon(p, 2.0f);
			CreateWhiteBalloon(p, 4.0f);
			CreateWhiteBalloon(p, 6.0f);
		}
	}
	else if (hp == 17)
	{
		shield -= damage;

		if (shield < 0)
		{
			hp += shield;
		}

		if (hp < 17)
		{
			hp = 0;
			GameInstance::Get()->IncreaseMoney(10);
			state = NONE;
			SetMeltDown(false);
			SetIsSlow(false);
			SetSlowRate(0.0f);
			SetActive(false);
			CreateRanbowBalloon(p, 1.0f);
			CreateRanbowBalloon(p, 4.0f);
		}
	}
	else if (hp >= 19 && hp <= 22)
	{
		hp -= damage;

		if (hp < 19)
		{
			hp = 0;
			GameInstance::Get()->IncreaseMoney(temp - 18);
			state = NONE;
			SetMeltDown(false);
			SetIsSlow(false);
			SetSlowRate(0.0f);
			SetActive(false);
		}
		else
		{
			GameInstance::Get()->IncreaseMoney(damage);
		}
	}
	else if (hp >= 24 && hp <= 26)
	{
		hp -= damage;

		if (hp < 24)
		{
			hp = 0;
			GameInstance::Get()->IncreaseMoney(temp - 23);
			state = NONE;
			SetMeltDown(false);
			SetIsSlow(false);
			SetSlowRate(0.0f);
			SetActive(false);
		}
		else
		{
			GameInstance::Get()->IncreaseMoney(damage);
		}
	}
}

void Balloon::CreatePinkBalloon(Projectile* p, float d)
{
	vector<Balloon*> balloons = GameInstance::Get()->GetBalloons();
	for (int i = balloons.size() - 1; i >= 0; i--)
	{
		if (!balloons[i]->Active())
		{
			balloons[i]->Pos() = Pos() + (direction * d);
			balloons[i]->SetRouteCnt(routeCnt);
			balloons[i]->SetHP(5);
			if (p != nullptr && !(p->GetType() == Type::EXPLOSION))
				p->AddHit(balloons[i]);
			balloons[i]->SecondInit();
			balloons[i]->SetActive(true);
			UpdateTexture();
			break;
		}
	}
}

void Balloon::CreateBlackBalloon(Projectile* p, float d)
{
	vector<Balloon*> balloons = GameInstance::Get()->GetBalloons();
	for (int i = balloons.size() - 1; i >= 0; i--)
	{
		if (!balloons[i]->Active())
		{
			balloons[i]->Pos() = Pos() + (direction * d);
			balloons[i]->SetRouteCnt(routeCnt);
			balloons[i]->SetHP(7);
			if (p != nullptr && !(p->GetType() == Type::EXPLOSION))
				p->AddHit(balloons[i]);
			balloons[i]->SecondInit();
			balloons[i]->SetActive(true);
			UpdateTexture();
			break;
		}
	}
}

void Balloon::CreateWhiteBalloon(Projectile* p, float d)
{
	vector<Balloon*> balloons = GameInstance::Get()->GetBalloons();
	for (int i = balloons.size() - 1; i >= 0; i--)
	{
		if (!balloons[i]->Active())
		{
			balloons[i]->Pos() = Pos() + (direction * d);
			balloons[i]->SetRouteCnt(routeCnt);
			balloons[i]->SetHP(9);
			if (p != nullptr && !(p->GetType() == Type::EXPLOSION))
				p->AddHit(balloons[i]);
			balloons[i]->SecondInit();
			balloons[i]->SetActive(true);
			UpdateTexture();
			break;
		}
	}
}

void Balloon::CreateRanbowBalloon(Projectile* p, float d)
{
	vector<Balloon*> balloons = GameInstance::Get()->GetBalloons();
	for (int i = balloons.size() - 1; i >= 0; i--)
	{
		if (!balloons[i]->Active())
		{
			balloons[i]->Pos() = Pos() + (direction * d);
			balloons[i]->SetRouteCnt(routeCnt);
			balloons[i]->SetHP(15);
			if (p != nullptr && !(p->GetType() == Type::EXPLOSION))
				p->AddHit(balloons[i]);
			balloons[i]->SecondInit();
			balloons[i]->SetActive(true);
			UpdateTexture();
			break;
		}
	}
}