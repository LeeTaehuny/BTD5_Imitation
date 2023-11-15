#include "Framework.h"

#include "Scenes/Game/StartScene.h"
#include "Scenes/Game/LobbyScene.h"
#include "Scenes/Game/InGameScene.h"
#include "Objects/Game/GameInstance/GameInstance.h"

GameManager::GameManager()
{
    Create();

    SCENE->Register("StartScene", new StartScene());
    SCENE->Register("LobbyScene", new LobbyScene());
    SCENE->Register("InGameScene", new InGameScene());

    SCENE->ChangeScene("StartScene");


    GameInstance::Get();

    // BGM 등록
    Audio::Get()->Add("BGM", "Sounds/BGM.wav", true);
    Audio::Get()->Add("BombHit", "Sounds/bomb_Hit.wav"); 
    Audio::Get()->Add("DartHit", "Sounds/dart_Hit.wav");
    Audio::Get()->Add("lead_Guard", "Sounds/lead_Guard.wav");

    Audio::Get()->Play("BGM", 0.2f);
    playing = true;
}

GameManager::~GameManager()
{
    SCENE->Delete();

    Delete();
}

void GameManager::Update()
{
    CAMERA->Update();

    Keyboard::Get()->Update();
    Timer::Get()->Update();

    SCENE->Update();

    if (GameInstance::Get()->GetBGMPlayable())
    {
        if (!playing)
        {
            Audio::Get()->Play("BGM", 0.5f);
            playing = true;
        }
    }
    else
    {
        if (playing)
        {
            Audio::Get()->Stop("BGM");
            playing = false;
        }
    }
}

void GameManager::Render()
{
    Device::Get()->Clear();

    Font::Get()->GetDC()->BeginDraw();

    SCENE->Render();

    //ImGui_ImplDX11_NewFrame();
    //ImGui_ImplWin32_NewFrame();
    //ImGui::NewFrame();

    //CAMERA->RenderUI();

    //ImGui::Render();
    //ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    Font::Get()->GetDC()->EndDraw();

    Device::Get()->Present();
}

void GameManager::Create()
{
    Keyboard::Get();
    Timer::Get();
    Device::Get();
    Environment::Get();
    Observer::Get();

    //ImGui::CreateContext();
    //ImGui::StyleColorsDark();

    //ImGui_ImplWin32_Init(hWnd);
    //ImGui_ImplDX11_Init(DEVICE, DC);
}

void GameManager::Delete()
{
    Keyboard::Delete();
    Timer::Delete();
    Device::Delete();
    Shader::Delete();
    Texture::Delete();
    Observer::Delete();
    Font::Delete();
    Audio::Delete();
    EffectManager::Delete();

    //ImGui_ImplDX11_Shutdown();
    //ImGui_ImplWin32_Shutdown();

    //ImGui::DestroyContext();
}
 