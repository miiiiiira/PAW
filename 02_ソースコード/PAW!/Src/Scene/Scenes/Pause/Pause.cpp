#include <memory>
#include <vector>

#include "../../../Input/InputManager.h"
#include "../../../Audio/AudioManager.h"
//#include "../../Common/FrameRenderer/FrameRenderer.h"
#include "../../SceneManager.h"
#include "../../../Collision/Collision.h"
#include "../../../Application.h"
#include "../Confirm/Confirm.h"
#include "../../../Input/MouseCursor/MouseCursor.h"

#include "Pause.h"

Pause::Pause(void)
{
	handle_ = -1;
	confirm_ = nullptr;

	// マウスの表示する
	MouseCursor::GetInstance()->SetMouseDraw(true);
}

Pause::~Pause(void)
{
}

void Pause::Init(void)
{
	ChangeSelect(PAUSE::NONE);

	confirm_ = std::make_shared<Confirm>();
}

void Pause::Load(void)
{
	handle_ = LoadGraph("Pause/pause.png");		// PAUSEの文字画像

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
	if (currentMenu_ == PAUSE::NONE) return;

	// クリックされていて、かつメニューが選ばれている場合
	switch (currentMenu_)
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
	// 背景色を半透明で表示
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 230);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawGraph(PAUSE_POS_X, PAUSE_POS_Y, handle_, true);					// PAUSEの文字を表示


	for (const auto& button : pauseButtons_)
	{
		if (button.type == currentMenu_)
		{
			//FrameRenderer::Draw(button.x,button.y);
		}

		DrawGraph(button.x, button.y, button.graphHandle, true);		// メニューの文字を表示
	}

#ifdef _DEBUG

	DrawString(0, 0, "Pause", 0xffffff);

#endif // _DEBUG
}

void Pause::Release(void)
{
	for (const auto& button : pauseButtons_)
	{
		DeleteGraph(button.graphHandle);
	}
	pauseButtons_.clear();

	DeleteGraph(handle_);
	handle_ = -1;
}

void Pause::ChangeSelect(PAUSE pause)
{
	currentMenu_ = pause;
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
	// 確認シーンへ
	confirm_->ChangeType(Confirm::TYPE::MAIN_MENU);
	SceneManager::GetInstance()->PushScene(confirm_);
}

void Pause::UpdateQuit(void)
{
	// 確認シーンへ
	confirm_->ChangeType(Confirm::TYPE::QUIT);
	SceneManager::GetInstance()->PushScene(confirm_);
}

void Pause::SelectUpgrade(void)
{
	// 前回の選択物を入れておく
	PAUSE prevSelect = currentMenu_;

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
	if (currentMenu_ != PAUSE::NONE
		&& currentMenu_ != prevSelect)
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
	auto prevMenu = currentMenu_;

	switch (currentMenu_)
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

