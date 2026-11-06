#include <memory>
#include <vector>

#include "../../../Input/InputManager.h"
#include "../../../Audio/AudioManager.h"
#include "../../SceneManager.h"
#include "../../../Collision/CollisionMath.h"
#include "../../../Application.h"
#include "../Confirm/Confirm.h"
#include "../../../Input/MouseCursor/MouseCursor.h"
#include "../../../Scene/Scenes/Title/TitleScene.h"

#include "Pause.h"

namespace
{
	// 画像サイズ
	constexpr int IMAGE_SIZE_X = 400;
	constexpr int IMAGE_SIZE_Y = 75;

	// CONTINUE
	constexpr int CONTINUE_POS_X = Application::SCREEN_SIZE_X / 2;
	constexpr int CONTINUE_POS_Y = Application::SCREEN_SIZE_Y / 2 + 50.0f;

	// TITLE
	constexpr int TITLE_POS_X = Application::SCREEN_SIZE_X / 2;
	constexpr int TITLE_POS_Y = Application::SCREEN_SIZE_Y / 2 + 170.0f;

	// EXIT
	constexpr int EXIT_POS_X = Application::SCREEN_SIZE_X / 2;
	constexpr int EXIT_POS_Y = Application::SCREEN_SIZE_Y / 2 + 290.0f;
}

Pause::Pause(void)
{
	// マウスの表示する
	MouseCursor::GetInstance()->SetMouseDraw(true);
}

Pause::~Pause(void)
{
}

void Pause::Init(void)
{
	ChangeSelect(PAUSE::NONE);
}

void Pause::Load(void)
{
	// 背景画像読み込み
	backImage_ = LoadGraph("Data/Image/Game/Pause.png");

	pauseButtons_.clear();

	// Continue画像
	pauseButtons_.push_back({
		PAUSE::CONTINUE,
		LoadGraph("Data/Image/Game/Button/ContinueOff.png"),
		LoadGraph("Data/Image/Game/Button/ContinueOn.png"),
		CONTINUE_POS_X,CONTINUE_POS_Y,
		IMAGE_SIZE_X, IMAGE_SIZE_Y });

	// Title画像
	pauseButtons_.push_back({
		PAUSE::TITLE,
		LoadGraph("Data/Image/Game/Button/TitleOff.png"),
		LoadGraph("Data/Image/Game/Button/TitleOn.png"),
		TITLE_POS_X, TITLE_POS_Y,
		IMAGE_SIZE_X, IMAGE_SIZE_Y });

	// Exit画像
	pauseButtons_.push_back({
		PAUSE::EXIT,
		LoadGraph("Data/Image/Game/Button/ExitOff.png"),
		LoadGraph("Data/Image/Game/Button/ExitOn.png"),
		EXIT_POS_X, EXIT_POS_Y,
		IMAGE_SIZE_X, IMAGE_SIZE_Y });
}

void Pause::LoadEnd(void)
{
	Init();
}

void Pause::Update(void)
{
	// 選択処理
	SelectUpgrade();

	// 選択されているメニューがない場合も、ここで終了
	if (currentPause_ == PAUSE::NONE) return;

	// クリックされていて、かつメニューが選ばれている場合
	switch (currentPause_)
	{
	case PAUSE::CONTINUE:
		UpdateContinue();
		break;

	case PAUSE::TITLE:
		UpdateTitle();
		break;

	case PAUSE::EXIT:
		UpdateExit();
		break;
	}
}

void Pause::Draw(void)
{
	// 背景描画
	DrawGraph(0, 0, backImage_, true);

	// 全てのボタンを描画
	for (const auto& button : pauseButtons_)
	{
		// 選択されているボタン
		if (button.type == currentPause_)
		{
			DrawRotaGraphF(button.x, button.y, 1.0f, 0.0f, button.graphHandleOn, true);
		}
		// 選択されていないボタン
		else
		{
			DrawRotaGraphF(button.x, button.y, 1.0f, 0.0f, button.graphHandleOff, true);
		}
			DrawBox(button.x- IMAGE_SIZE_X/2, button.y - IMAGE_SIZE_Y / 2, 
				button.x + IMAGE_SIZE_X / 2, button.y + IMAGE_SIZE_Y / 2,
				0xff0000, false);
	}

#ifdef _DEBUG

	DrawString(0, 0, "Pause", 0xffffff);
	DrawFormatString(0, 100, 0xff0000, "currentPause_ %d", currentPause_);

#endif // _DEBUG
}

void Pause::Release(void)
{
	for (const auto& button : pauseButtons_)
	{
		DeleteGraph(button.graphHandleOff);
		DeleteGraph(button.graphHandleOn);
	}
	pauseButtons_.clear();

	DeleteGraph(backImage_);
	backImage_ = -1;
}

void Pause::ChangeSelect(PAUSE pause)
{
	currentPause_ = pause;
}

void Pause::UpdateContinue(void)
{
	// 決定ボタンかEscapeを押されたら
	if (InputManager::GetInstance()->DecideButton() ||
		InputManager::GetInstance()->PauseEndButton())
	{
		// ゲームシーンへ戻る(ポーズモードを終了する)
		AudioManager::GetInstance()->PlaySE(SoundID::SYS_BUTTON);

		// マウスカーソルを画面中央に戻す
		SetMousePoint(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2);

		// マウスの表示を消す
		MouseCursor::GetInstance()->SetMouseDraw(false);

		// ゲームシーンへ
		SceneManager::GetInstance()->PopScene(SCENE_TAG::GAME);
	}
}

void Pause::UpdateTitle(void)
{
	// 決定ボタンを押されたら
	if (InputManager::GetInstance()->DecideButton())
	{
		// ボタン押下のSEを流す
		AudioManager::GetInstance()->PlaySE(SoundID::SYS_BUTTON);

		// ゲームシーンへ飛ばす
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<TitleScene>(), SCENE_TAG::TITLE, true);
	}
}

void Pause::UpdateExit(void)
{
	// 決定ボタンを押されたら
	if (InputManager::GetInstance()->DecideButton())
	{
		// ボタン押下のSEを流す
		AudioManager::GetInstance()->PlaySE(SoundID::SYS_BUTTON);

		// 確認画面を積む
		SceneManager::GetInstance()->PushScene(std::make_shared<Confirm>());
	}
}

void Pause::SelectUpgrade(void)
{
	// 前回の選択物を入れておく
	PAUSE prevSelect = currentPause_;

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
	if (currentPause_ != PAUSE::NONE
		&& currentPause_ != prevSelect)
	{
		// ボタンに乗ったサウンドを出す
		AudioManager::GetInstance()->PlaySE(SoundID::SYS_SELECT_ON);
	}
}

void Pause::MouseSelect(void)
{
	PAUSE nextSelect = PAUSE::NONE;

	// 衝突判定
	for (const auto& button : pauseButtons_)
	{
		// ボタンと当たっているか
		if (Collision::HitMouseImg2Box(
			{ button.x - static_cast<float>(button.sizeX / 2),
			button.y - static_cast<float>(button.sizeY / 2) },
			static_cast<float>(button.sizeX),
			static_cast<float>(button.sizeY)))
		{
			// 当たっている場所をアクティブなボタンに変更
			nextSelect = button.type;
			break;
		}
	}

	// 最後に一回だけ状態を更新する
	ChangeSelect(nextSelect);
}

void Pause::PadSelect(void)
{
	// 現在選択しているメニュー
	int current = static_cast<int>(currentPause_);

	// 上が押されたら
	if (InputManager::GetInstance()->SelectUpPad())
	{
		// メニューを上に移動
		current--;
	}
	// 下が押されたら
	else if (InputManager::GetInstance()->SelectDownPad())
	{
		// メニューを下に移動
		current++;
	}

	// 移動先をキャスト
	PAUSE next = static_cast<PAUSE>(current);

	// 下限設定
	if (next <= PAUSE::NONE)
	{
		next = PAUSE::CONTINUE;
	}

	// 上限設定
	if (next >= PAUSE::MAX)
	{
		next = PAUSE::EXIT;
	}

	// メニューを切り替える
	ChangeSelect(next);
}

