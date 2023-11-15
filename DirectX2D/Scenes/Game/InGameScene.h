#pragma once

class Balloon;
class Tower;
class UIButton;

// 인게임 맵과 맵 정보를 구분하기 위한 열거형
enum class MapType
{
	NONE,
	MonkeyLane,
	Tutorial,
	Patch,
};

class InGameScene : public Scene
{
	// 인게임에서의 상태를 구분하기 위한 열거형
	enum class State
	{
		NEW,      // 새로운 게임 시작 상태
		WAIT,	  // 라운드 대기 상태
		PLAY,	  // 게임 플레이 상태
		STOP,     // 게임 정지 상태
		DEFEAT,   // 게임 패배
		VICTORY,  // 게임 승리
	};
public:
	InGameScene();
	virtual ~InGameScene() override;

	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;

	void SetState(State state) { this->state = state; }
	void SetMapType(MapType map) { this->map = map; }
	void SetNew(bool value) { bIsNew = value; }
	void Delete();

// 내부 함수들
private:
	// 맵 & UI 생성
	void CreateMapData();
	void CreateUI();

	// 풍선 생성
	void CreateBalloon();

	// 라운드 정보 생성
	void CreateRoundInfo();

	// 타워 추가
	void AddTower(void* tower);
	// 타워 선택
	void SelectTower(void* tower);
	void Select();
	// 타워 목록에 추가
	void SetTower();
	// 타워의 범위 체크
	void CheckRange();
	
	// 업그레이드
	void Upgrade(void* button);
	void UIButtonRenderBegin();

	// 판매
	void SellTower();

	// 라운드 시작 & 배속
	void StartRound();

	// 텍스트 출력
	void RenderText();

	// 로비로 돌아가기
	void EnterLobby();
	// 다시 시작하기
	void RestartGame();
	// 옵션 창 열기
	void OpenOption();
	// 옵션 창 닫기
	void CloseOption();

	// 게임 테스트용 함수
	void Test();

// 인게임 정보
private:
	// 현재 씬의 상태
	State state = State::WAIT;
	// 임시 씬 상태
	State temp;

	// 설치된 타워 & 풍선
	vector<Tower*> towers;
	vector<Balloon*> balloons;

	// 삭제할 타워
	vector<Tower*> deleteTowers;

	// 새로하기 여부
	bool bIsNew = false;

// 시스템 정보
private:
	// 라운드 정보
	int roundCnt = 1;
	string roundInfo[41];

	// 누적 시간
	float sumTime = 0.0f;

	// 스폰한 풍선 수
	int spawnCnt = 0;
	// 스폰 가능한 최대 수
	int maxSpawnCnt = 0;
	// 스폰 타이머
	float spawnTime = 1.0f;

// UI 관련 정보
private:
	// 타워 UI
	vector<Tower*> towerUI;
	
	// Upgrage 버튼
	UIButton* UpgradeL;
	UIButton* UpgradeR;

	// Start 버튼
	UIButton* Start;

	// Sell 버튼
	UIButton* Sell;

	// 선택 & 임시 타워
	Tower* tempTower;
	Tower* selectTower;

	// 옵션
	UIButton* Option;
	UIButton* close;
	Quad* optionWindow;

	// 패배 & 승리창
	Quad* defeatWindow;
	Quad* victoryWindow;
	UIButton* backHome;
	UIButton* restart;

	// 사운드 조절을 위한 변수를 선언합니다.
	UIButton* BGMsound;
	UIButton* soundEffect;

// 맵 관련 정보
private:
	// 맵의 종류를 저장하기 위한 변수를 선언합니다.
	// - 테스트 원숭이맵
	MapType map = MapType::MonkeyLane;

	// 맵의 배경를 저장하기 위한 변수를 선언합니다.
	Quad* background;
	// 길에 해당하는 충돌체들을 저장하기 위한 벡터 컨테이너를 선언합니다.
	vector<Collider*> road;
	// 몬스터 이동 경로를 저장하기위한 벡터 컨테이너를 선언합니다.
	vector<Vector2> route;
};

