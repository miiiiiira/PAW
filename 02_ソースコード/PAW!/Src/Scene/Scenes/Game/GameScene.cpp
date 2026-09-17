#include <EffekseerForDXLib.h>
#include <algorithm>

#include "../../../Application.h"
#include "../../../Input/InputManager.h"
#include "../../../Audio/AudioManager.h"
#include "../../SceneManager.h"
#include "../GameClear/GameClear.h"
#include "../GameOver/GameOver.h"
#include "../Pause/Pause.h"
#include "../../../Collision/Collision.h"
#include "../../../Input/MouseCursor/MouseCursor.h"
#include "../../../Math/Math.h"

#include "GameScene.h"

GameScene::GameScene(void)
{
	// マウスの表示を消す
	//MouseCursor::GetInstance()->SetMouseDraw(false);
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	// BGM再生
	//AudioManager::GetInstance()->PlayBGM(SoundID::BGM_GAME_1);
}

void GameScene::Load(void)
{
	// サウンド読みこみ
	//AudioManager::GetInstance()->LoadSceneSound(LoadScene::GAME);
}

void GameScene::LoadEnd(void)
{
	Init();
}

void GameScene::Update(void)
{
#ifdef _DEBUG
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_C))
	{
		// ステージクリアにする
		SceneManager::GetInstance()->TrueGameClear();
	}

	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_O))
	{
		// ゲームオーバーにする
		SceneManager::GetInstance()->TrueGameOver();
	}

#endif // _DEBUG

	if (InputManager::GetInstance()->PauseButton())
	{
		// ポーズ画面を開いたサウンド
		//AudioManager::GetInstance()->PlaySE(SoundID::SYS_PAUSE_ON);
		// ポーズモードへ
		SceneManager::GetInstance()->PushScene(std::make_shared<Pause>());
		return;
	}

	if (SceneManager::GetInstance()->GetIsClear())
	{
		// ステージ情報などを初期化する
		SceneManager::GetInstance()->ResetGame();
		// ゲームクリアシーンへ
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<GameClear>(),CLEAR);
		return;
	}

	if (SceneManager::GetInstance()->GetIsOver())
	{
		// ステージ情報などを初期化する
		SceneManager::GetInstance()->ResetGame();
		// ゲームオーバーシーンへ
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<GameOver>(),OVER);
		return;
	}

}

void GameScene::Draw(void)
{
#ifdef _DEBUG

	DrawString(0, 0, "Game", 0xffffff);

#endif // _DEBUG
}

void GameScene::Release(void)
{
	// 音の解放
	//AudioManager::GetInstance()->DeleteSceneSound(LoadScene::GAME);
}

