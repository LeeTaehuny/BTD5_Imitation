# [DirectX11] BTD5_Imitation(BTD5 모작)
## 개요
- 개발 기간 : 3주 (23.10.23 ~ 23.11.10)
- 개발 인원 : 1인
- 개발 목적 : DirectX11의 렌더링 파이프라인 이해 및 실습
- 개발 환경    
&nbsp;&nbsp;&nbsp;&nbsp;* 사용 언어 : C++    
&nbsp;&nbsp;&nbsp;&nbsp;* Tool : Visual Studio 2022    
&nbsp;&nbsp;&nbsp;&nbsp;* WinAPI 환경에서의 DirectX11 API 사용

## 목표
- 2주 내외의 짧은 기간동안 BTD5 중 특정 기능들을 구현해보기
<img src="https://github.com/LeeTaehuny/BTD5_Imitation/assets/105622632/c4237a1e-e1ce-4a2c-9895-ac6fef0a8200" width="400" height="400"/>
<img src="https://github.com/LeeTaehuny/BTD5_Imitation/assets/105622632/45cb874c-ac90-48fa-9a20-143c6c997a98" width="400" height="400"/>

## 설계
> **User Flow Diagram**
      
<img src="https://github.com/LeeTaehuny/BTD5_Imitation/assets/105622632/01886721-df9b-4a8f-ac65-2736ea558342" width="400" height="400"/>

## 구현
> **Balloon**

- 각각의 풍선은 타입과 상태를 구분해 동작하도록 설정하였습니다.
- 타입은 일반, 폭발 저항, 얼음 저항, 날카로운 공격 저항, 재생성, 은신이 있습니다.
- 상태는 일반, 느려짐, 멈춤 상태가 있습니다.
- 각 풍선은 체력에 따라 텍스처를 설정하고, 데미지를 받을 때마다 텍스처를 업데이트하도록 구현하였습니다.
- 풍선은 맵의 길(route)를 따라 진행합니다.
    * 타일맵 형식으로 제작하고 싶었으나 맵의 사이즈가 정확히 나눠지지 않아서 특정 포인트를 찍고 포인트를 기준으로 이동하도록 구현했습니다.
    * 특정 포인트와의 거리가 특정 길이 이하로 떨어지면 다음 포인트로 이동하도록 구현했습니다.
    * 특정 맵(MonkeyLane)의 일부 구간(지하)에서는 hide 변수를 켜서 이동은 하지만 렌더링되지 않도록 설정했습니다.
    * 마지막 포인트에 도착하면 풍선의 데미지(남은 체력)만큼 체력에서 제외하고 풍선은 비활성화됩니다.
    
- 풍선의 타입 및 종류
    
    ![balloons](https://github.com/seungdo1234/Game_Portfolio/assets/105622632/cb56c316-a06d-4d9d-a195-6eae00b2853a)    
       
    * Common : 일반 타입입니다. 모든 종류의 데미지를 받으며, 특별한 이벤트가 발생하지 않습니다.    
    * Explosion Resist : 폭발 저항 타입입니다. 폭발로부터 받는 피해를 무시합니다.      
    * Ice Resist : 얼음 저항 타입입니다. 상태이상(동상, 슬로우)에 걸리지 않습니다.       
    * Duble Resist : 폭발 및 얼음 저항 타입입니다. 폭발과 상태이상에 피해를 입지 않습니다.       
    * Sharp resist : 날카로운 공격 저항 타입입니다. 날카로운 공격으로부터 받는 피해를 무시합니다.        
    * Regenerate : 재생성 타입입니다. 시간이 지나면 체력을 회복합니다.        
    * Camo : 은신 타입입니다. 일반적인 타워의 공격 대상이 되지 않습니다.    
    
> **Tower**

- 타워는 클릭 이벤트를 처리하는 기능이 담긴 Button 클래스를 상속받았으며, 타워의 종류에 따라 자식 클래스로 나눠 구현하였습니다.
- 게임 머니를 사용해 타워를 업그레이드 가능합니다.
- 타워의 공통적인 부분(스탯, 발사체 등)을 Tower 클래스에서 생성해 세부 타워들이 모두 이용 가능하도록 구현했습니다.
- 업그레이드가 완료 되었을 때마다 타워의 기능(스탯, 공격 타입 등)과 텍스처를 업데이트하도록 구현했습니다.
- 업그레이드 진행 시 해당 타워의 누적 비용이 증가합니다.
- 타워를 판매하면 누적 비용의 80%를 획득할 수 있습니다.
- 타워는 설치 UI를 클릭하면 생성되며, 설치 가능한 장소에만 드래그&드롭을 통해 배치할 수 있습니다.
- 설치 가능 여부는 색상(불가능-빨강)을 통해 구분 가능하도록 구현했습니다.
- 업그레이드 루트가 2개 존재하며 한 쪽으로 업그레이드를 2이상 했다면 다른쪽은 최대 업그레이드 가능 레벨이 2가 됩니다.

- 타워의 종류 및 업그레이드 정보    
    ![dart bomb](https://github.com/LeeTaehuny/BTD5_Imitation/assets/105622632/e4fcab86-2772-4451-95f8-d1dfda3bb849)     
    ![Ice Tack](https://github.com/LeeTaehuny/BTD5_Imitation/assets/105622632/be074bdd-d774-4781-a468-355da7ff1963)

> **System**

- 여러 곳에서 사용해야 하는 정보들(풍선, 임시저장 정보, 게임 진행 속도, 사운드 조절)은 매니저 클래스(GameInstance)를 따로 만들어 관리했습니다.
- 게임 씬(InGameScene)은 여러 상태(New, Wait, Play, Stop, Defeat, Victory)를 가지고 있으며, 현재 상태에 맞게 동작합니다.    
    * New 상태 : 새로운 게임 시작(이어하기는 Wait로 바로 이동), 게임에 필요한 정보들을 초기화합니다.    
    * Wait 상태 : 삭제 예약된 타워를 삭제하고, 라운드 정보를 백업합니다.
    * Play 상태 : 라운드 정보(string)에 따른 풍선을 스폰하며, 모든 풍선이 비활성화되면 Wait 상태로 이동합니다.
    * Stop 상태 : 설정 UI를 누른 경우로, 게임이 정지된 상태입니다.
    * Defeat 상태 : 체력이 0이되어 패배한 상태입니다. 모든 정보를 초기화 합니다.
    * Victory 상태 : 모든 라운드(40)을 클리어한 상태입니다. 모든 정보를 초기화 합니다.
- 맵 선택
    * 맵은 MonkeyLane(원숭이 도로), Patch(밭), Tutorial(테스트용)이 구현되어 있습니다.
    * 맵 선택 정보를 게임 씬(InGameScene)에 전달해 풍선의 진행로 등 맵 정보를 자동으로 설정하도록 설계하였습니다.    
        <img src="https://github.com/LeeTaehuny/BTD5_Imitation/assets/105622632/66c32fa1-9ddb-4685-a72e-9fc211c40d9e" width="400" height="400"/>
- 테스트용 맵(Tutorial)에서는 여러 단축키를 통해 테스트 가능하도록 구현했습니다.
    * SpaceBar : 게임머니 +10,000
    * W : 체력 -100
    * F : 라운드 +1
    * A ~ Z(F, H, J, L, N, P, R, W 제외) : 풍선 스폰
- 이어하기 시스템
    * 맵 선택 중 이어하기 버튼이 존재하며, GameInstance에 저장된 정보가 있으면 화면에 노출됩니다.
    * 게임을 클리어 or 패배한 경우 이어하기 정보는 소멸합니다.
    * 게임 진행 중 설정을 통해 로비로 돌아오는 경우 진행 직전의 라운드(Wait) 정보를 저장합니다.
        <img src="https://github.com/LeeTaehuny/BTD5_Imitation/assets/105622632/4cd4d39d-dad1-44eb-b276-be3de072c9c5" width="600" height="200"/>
- 사운드 시스템
    * 사운드는 크게 BGM(배경), EffectSound(효과음)이 존재합니다.
    * 로비에서는 BGM을 On/Off 가능하며 게임 진행 중에 설정을 통해 BGM, EffectSound를 On/Off 가능합니다.
        <img src="https://github.com/LeeTaehuny/BTD5_Imitation/assets/105622632/9d61c738-2d25-4bf4-8119-02751a27b387" width="600" height="200"/>
- 배속 시스템
    * 시작 UI를 게임 진행 중에 1회 추가로 누르면 배속 플레이가 가능하도록 구현했습니다.
    * 해당 라운드가 끝나면 원래 속도로 돌아오도록 설계했습니다.
        <img src="https://github.com/LeeTaehuny/BTD5_Imitation/assets/105622632/fa54945d-b954-4601-949b-2605e93c64f8" width="700" height="200"/>
            
## 플레이 영상 링크
**https://www.youtube.com/watch?v=2ZdODOGuAvg**


