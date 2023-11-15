#pragma once

class Balloon;

// 발사체의 타입을 구분하기 위한 열거형을 선언합니다.
enum class Type
{
	NONE,
	SHARP,		// 날카로운 타입
	EXPLOSION,	// 폭발 타입
	EVERY,		// 모든 타입 공격 가능
};

class Projectile : public Quad
{
public:
	Projectile(wstring file, Type type, float range);
	virtual ~Projectile() override;

	void Update();
	virtual void Render() override;

	void Fire(Vector2 pos, Vector2 direction);
	void Fire(Vector2 pos, Vector2 direction, Balloon* balloon);
	void SubFire(Vector2 pos, float angle);

// Getter & Setter
public:
	void SetRange(float range) { this->range = range; }
	void SetPower(int power) { this->power = power; }
	void SetDamage(int damage) { this->damage = damage; }
	void SetType(Type type) { this->type = type; }
	Collider* GetCollider() { return collider; }
	void SetStartPos(Vector2 pos) { startPos = pos; }
	void SetSlow(bool value) { isSlow = value; }
	void SetSub(bool value) { bIsSubProjectile = value; }
	void Setangle(float value) { angle = value; }
	void SetSplit(bool value) { bIsSplit = value; }

	void AddHit(Balloon* b);
	Type GetType() { return type; }

private:
	// 데미지를 주기 위한 함수를 선언합니다.
	void Attack();

// Member Variable
private:
	Type type = Type::NONE;
	
	// damage : 피해량
	// power  : 관통력
	int damage = 1;
	int power = 1;
	
	float range = 0.0f;
	float speed = 600.0f;
	Vector2 direction;
	Vector2 startPos;

	vector<Balloon*> hit;

	Collider* collider;
	Collider* explosionCollider;
	bool explosion = false;
	bool isSlow = false;

	bool bIsSubProjectile = false;
	bool bIsSplit = false;
	float angle = 0.0f;
};

