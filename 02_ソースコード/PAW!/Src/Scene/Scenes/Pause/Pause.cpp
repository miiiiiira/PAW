#include <memory>
#include <vector>

#include "../../../Input/InputManager.h"
#include "../../../Audio/AudioManager.h"
//#include "../../Common/FrameRenderer/FrameRenderer.h"
#include "../../SceneManager.h"
#include "../../../Collision/CollisionMath.h"
#include "../../../Application.h"
#include "../Confirm/Confirm.h"
#include "../../../Input/MouseCursor/MouseCursor.h"

#include "Pause.h"

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

	// CONTINUE画像
	pauseButtons_.push_back({ PAUSE::CONTINUE, LoadGraph("Pause/continue.png"),
								CONTINUE_POS_X, CONTINUE_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });
	// MAIN MENU画像
	pauseButtons_.push_back({ PAUSE::MAINMENU,  LoadGraph("Pause/mainMenu.png"),
								MAINMENU_POS_X, MAINMENU_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });
	// QUIT画像
	pauseButtons_.push_back({ PAUSE::QUIT, LoadGraph("Common/quit.png"),
								QUIT_POS_X, QUIT_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });

}

void Pause::LoadEnd(void)
{
	Init();
}

void Pause::Update(void)
{
	// Escape押したら
	if (InputManager::GetInstance()->PauseEndButton())
	{
		// ポーズモード終了しゲームシーンへ戻る
		UpdateContinue();
	}

	// 選択処理
	SelectUpgrade();

	// マウスを左クリックされていなかったら、ここで終了
	if (!InputManager::GetInstance()->DecideButton()) return;

	// 選択されているメニューがない場合も、ここで終了
	if (currentPause_ == PAUSE::NONE) return;

	// クリックされていて、かつメニューが選ばれている場合
	switch (currentPause_)
	{
	case PAUSE::CONTINUE:
		UpdateContinue();
		break;

	case PAUSE::MAINMENU:
		UpdateMainMenu();
		break;

	case PAUSE::QUIT:
		UpdateQuit();
		break;
	}

	// ボタン押下のSEを流す
	//AudioManager::GetInstance()->PlaySE(SoundID::SYS_BUTTON_1);
}

void Pause::Draw(void)
{
	// 背景描画
	DrawGraph(0, 0, backImage_, true);


	for (const auto& button : pauseButtons_)
	{
		if (button.type == currentPause_)
		{
			//FrameRenderer::Draw(button.x,button.y);
		}

		DrawGraph(button.x, button.y, button.graphHandle, true);		// メニューの文字を表示
	}
}

void Pause::Release(void)
{
	for (const auto& button : pauseButtons_)
	{
		DeleteGraph(button.graphHandle);
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
	// マウスカーソルを画面中央に戻す
	SetMousePoint(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2);

	// マウスの表示を消す
	MouseCursor::GetInstance()->SetMouseDraw(false);

	// ゲームシーンへ戻る(ポーズモードを終了する)
	//AudioManager::GetInstance()->PlaySE(SoundID::SYS_PAUSE_OFF);

	// ゲームシーンへ
	SceneManager::GetInstance()->PopScene();
}

void Pause::UpdateMainMenu(void)
{
}

void Pause::UpdateQuit(void)
{
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
		//AudioManager::GetInstance()->PlaySE(SoundID::SYS_SELECT_ON);
	}
}

void Pause::MouseSelect(void)
{
	PAUSE nextSelect = PAUSE::NONE;

	// 衝突判定
	for (const auto& button : pauseButtons_)
	{
		if (Collision::HitMouseImg2Box({ static_cast<float>(button.x), static_cast<float>(button.y) },
			static_cast<float>(button.sizeX), static_cast<float>(button.sizeY)))
		{
			nextSelect = button.type;
			break;
		}
	}

	// 最後に一回だけ状態を更新する
	ChangeSelect(nextSelect);
}

void Pause::PadSelect(void)
{
	auto prevMenu = currentPause_;

	switch (currentPause_)
	{
	case PAUSE::NONE:
		ChangeSelect(PAUSE::CONTINUE);
		break;

	case PAUSE::CONTINUE:
		if (InputManager::GetInstance()->SelectDownPad())
		{
			ChangeSelect(PAUSE::MAINMENU);
		}
		break;

	case PAUSE::MAINMENU:
		if (InputManager::GetInstance()->SelectUpPad())
		{
			ChangeSelect(PAUSE::CONTINUE);
		}

		if (InputManager::GetInstance()->SelectDownPad())
		{
			ChangeSelect(PAUSE::QUIT);
		}
		break;

	case PAUSE::QUIT:
		if (InputManager::GetInstance()->SelectUpPad())
		{
			ChangeSelect(PAUSE::MAINMENU);
		}
		break;

	default:
		break;
	}
}

