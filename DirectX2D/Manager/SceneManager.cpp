#include "Framework.h"
#include "Scenes/Game/InGameScene.h"

SceneManager::~SceneManager()
{
    for (pair<string, Scene*> scene : scenes)
    {
        delete scene.second;
    }
}

void SceneManager::Update()
{
    if (curScene == nullptr) return;

    curScene->Update();
}

void SceneManager::Render()
{
    if (curScene == nullptr) return;

    curScene->Render();
}

void SceneManager::Register(string key, Scene* scene)
{
    if (scenes.count(key) > 0) return;

    scenes[key] = scene;
}

void SceneManager::ChangeScene(string key)
{
    if (scenes.count(key) == 0) return;
    if (scenes[key] == curScene) return;

    curScene = scenes[key];
}

void SceneManager::SettingScene(string key, int type, bool newGame)
{
    if (scenes.count(key) == 0) return;
    
    // 새로 변경할 씬을 임시로 받아옵니다.
    InGameScene* gameScene = (InGameScene*)scenes[key];

    if (gameScene)
    {
        // 새로 변경할 씬의 맵 타입을 지정합니다.
        gameScene->SetMapType((MapType)type);

        // 새로 변경할 씬의 newGame 여부를 지정합니다.
        if (newGame)
        {
            gameScene->SetNew(true);
        }
        else
        {
            gameScene->SetNew(false);
        }
        // 새로 변경할 씬을 초기화합니다.
        gameScene->Init();
    }
}