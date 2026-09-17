#include <DxLib.h>

#include "../../../Input/InputManager.h"
#include "../../../Audio/AudioManager.h"
//#include "../../Common/FrameRenderer/FrameRenderer.h"
#include "../../SceneManager.h"
#include "../../../Collision/Collision.h"
#include "../Game/GameScene.h"
#include "../Title/TitleScene.h"
#include "../../../Input/MouseCursor/MouseCursor.h"

#include "GameOver.h"

GameOver::GameOver(void)
{
	// マウスの表示する
	//MouseCursor::GetInstance()->SetMouseDraw(true);
}

GameOver::~GameOver(void)
{
}

void GameOver::Init(void)
{
	// 現在の選択タイプを初期化
	currentType_ = NONE;
}

void GameOver::Load(void)
{
	// 画像を読み込み
	// ゲームオーバー画像
	handle_ = LoadGraph("Data/Image/GameOver/GameOver01.png");
	// RETRY画像
	buttons_.push_back({ TYPE::RETRY, LoadGraph("Data/Image/GameOver/Retry.png"),
								RETRY_POS_X, RETRY_POS_Y, RETRY_SIZE_X, RETRY_SIZE_Y });
	// RETURN_TITLE画像
	buttons_.push_back({ TYPE::RETURN_TITLE, LoadGraph("Data/Image/GameOver/ReturnTitle.png"),
							RETURN_TITLE_POS_X, RETURN_TITLE_POS_Y, RETURN_TITLE_SIZE_X, RETURN_TITLE_SIZE_Y });

	// サウンドを読み込み
	//AudioManager::GetInstance()->LoadSceneSound(LoadScene::GAME_OVER);
}

void GameOver::LoadEnd(void)
{
	// 初期化
	Init();
}

void GameOver::Update(void)
{
#ifdef _DEBUG

	// メニューからタイトルに戻すボタン
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_SPACE))
	{
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<TitleScene>(), TITLE);
		return;
	}

#endif // _DEBUG

	// 選択処理
	SelectUpdate();

	// マウスを左クリックしなかったら、処理を行わない
	if (!InputManager::GetInstance()->DecideButton()) return;

	// 種類が選択されていない場合、処理を行わない
	if (currentType_ == TYPE::NONE) return;

	// ボタン押下のSEを流す
	//AudioManager::GetInstance()->PlaySE(SoundID::SYS_BUTTON_1);

	switch (currentType_)
	{
	case GameOver::RETRY:

		// ゲームシーンへ
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<GameScene>(), GAME);
		return;

		break;
	case GameOver::RETURN_TITLE:

		// タイトルシーンへ
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<TitleScene>(), TITLE);
		return;

		break;
	default:
		break;
	}
}

void GameOver::Draw(void)
{
#ifdef _DEBUG

	DrawString(0, 0, "GameOver", 0xffffff);

#endif // _DEBUG
}

void GameOver::Release(void)
{
	// 画像の解放
	DeleteGraph(handle_);

	for (const auto& button : buttons_)
	{
		DeleteGraph(button.graphHandle);
	}
	buttons_.clear();

	// サウンドの解放
	//AudioManager::GetInstance()->DeleteSceneSound(LoadScene::GAME_OVER);
}

void GameOver::SelectUpdate(void)
{
	// 前回の選択物を入れておく
	TYPE prevType = currentType_;

	if (InputManager::GetInstance()->GetActiveDevice() == InputManager::ActiveDevice::KEY_MOUSE)
	{
		// マウス選択
		MouseSelect();
	}
	else
	{
		// パッド選択
		PadSelect();
	}

	// 中身がNONじゃないかつ、選択物が変わっていたら
	if (currentType_ != TYPE::NONE
		&& currentType_ != prevType)
	{
		// ボタンに乗ったサウンドを出す
		//AudioManager::GetInstance()->PlaySE(SoundID::SYS_SELECT_ON);
	}
}

void GameOver::MouseSelect(void)
{
	TYPE nextType = TYPE::NONE;

	// 衝突判定
	for (const auto& button : buttons_)
	{
		// 当たっていたら
		if (Collision::HitMouseImg2Box({ static_cast<float>(button.x), static_cast<float>(button.y) },
			static_cast<float>(button.sizeX), static_cast<float>(button.sizeY)))
		{
			// 選択している種類へ変更
			nextType = button.type;
			break;
		}
	}

	// 選択している種類を反映
	currentType_ = nextType;
}

void GameOver::PadSelect(void)
{
	switch (currentType_)
	{
	case GameOver::RETRY:

		// 下ボタンを押されたら
		if (InputManager::GetInstance()->SelectDownPad())
		{
			// タイトルを選択
			currentType_ = RETURN_TITLE;
		}

		break;
	case GameOver::RETURN_TITLE:

		// 上ボタンを押されたら
		if (InputManager::GetInstance()->SelectUpPad())
		{
			// リトライを選択
			currentType_ = RETRY;
		}

		break;
	case GameOver::NONE:

		// リトライを選択
		currentType_ = RETRY;

		break;
	default:
		break;
	}
}
