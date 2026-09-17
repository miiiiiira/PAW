#include "GameClear.h"

#include <DxLib.h>

#include "../../../Input/InputManager.h"
#include "../../../Audio/AudioManager.h"
#include "../../SceneManager.h"
#include "../Title/TitleScene.h"
//#include "../../Common/MouseCursor/MouseCursor.h"

GameClear::GameClear(void)
{
	handle_ = -1;
	// マウスの表示する
	//MouseCursor::GetInstance()->SetMouseDraw(true);
}

GameClear::~GameClear(void)
{
}

void GameClear::Init(void)
{
}

void GameClear::Load(void)
{
	handle_ = LoadGraph("Data/Image/GameClear/EndRoll.png");

	//AudioManager::GetInstance()->LoadSceneSound(LoadScene::GAME_CLEAR);
}

void GameClear::LoadEnd(void)
{
	Init();
}

void GameClear::Update(void)
{
	// 何かのボタンを押されたら、タイトルに遷移
	if (InputManager::GetInstance()->PushAnyButton())
	{
		// ゲームシーンへ
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<TitleScene>(), TITLE);
		return;
	}

}

void GameClear::Draw(void)
{
#ifdef _DEBUG

	DrawString(0, 0, "GameClear", 0xffffff);

#endif // _DEBUG
}

void GameClear::Release(void)
{
	DeleteGraph(handle_);

	//AudioManager::GetInstance()->DeleteSceneSound(LoadScene::GAME_CLEAR);
}
