#pragma once

class Balloon;
class Projectile;

// 생성된 타워의 종류를 구분하기 위한 열거형을 선언합니다.
enum TowerType
{
	None,
	Dart,
	Ice,
	Tack,
	Bomb,
};

class Tower : public Button
{
	// 실제 타워가 사용하기 위한 정보를 구조체로 생성합니다.
	struct TowerData
	{
		float range = 0.0f;
		float delay = 0.0f;
		int damage = 0;
		int power = 0;
		bool isCamoDetectable = false;

		int projectile_texture = 0;
	};
public:
	Tower(wstring textureFile, TowerType type);
	virtual ~Tower() override;

	virtual void Update() override;
	virtual void Render() override;
	
// Getter & Setter
public:
	TowerType GetType() { return type; }
	Collider* GetSelfCollider() { return selfCollider; }
	Collider* GetSelfAttackRange() { return attackRange; }
	vector<Balloon*> GetEnemies() { return enemies; }
	bool GetSelect() { return isSelected; }
	vector<Projectile*> GetProjectiles() { return projectiles; }

	int GetUpgradeLevel_L() { return towerLevel_L; }
	int GetUpgradeLevel_R() { return towerLevel_R; }

	void SetLastUpgradeLevel_L(int value) { lastTowerLevel_L = value; }
	void SetLastUpgradeLevel_R(int value) { lastTowerLevel_R = value; }

	TowerData GetData() { return t_data; }

	void AddTotalMoney(int value) { totalMoney += value; }
	int GetTotalMoney() { return totalMoney; }

	void Select(bool value) { isSelected = value; }

public:
	// 적을 추가하기 위한 함수를 선언합니다.
	void AddEnemy(Balloon* enemy);
	// 적을 제거하기 위한 함수를 선언합니다.
	void RemoveEnemy(Balloon* enemy);
	// 타워의 생성 가능 여부를 색상으로 표시하기 위한 색상 업데이트 함수를 선언합니다.
	void UpdateColor();

	// 타워 업그레이드 여부를 확인하기 위한 함수를 선언합니다.
	void UpgradeL(int money);
	void UpgradeR(int money);

// Inheritance
protected:
	Collider* selfCollider = nullptr;
	Collider* attackRange = nullptr;

	vector<Balloon*> enemies;

	void OnClick();
	TowerType type;
	bool isSelected = false;

	int towerLevel_L = 0;
	int towerLevel_R = 0;

	vector<Projectile*> projectiles;
	TowerData t_data;

	bool isUpdatable = false;
	bool isLUpdatable = false;
	bool isRUpdatable = false;

	bool isLeftUpgradable = true;
	bool isRightUpgradable = true;

private:
	// 현재 타워의 최종 금액을 저장하기 위한 변수를 선언합니다.
	int totalMoney = 0;

// backup
protected:
	int lastTowerLevel_L = 0;
	int lastTowerLevel_R = 0;
};

