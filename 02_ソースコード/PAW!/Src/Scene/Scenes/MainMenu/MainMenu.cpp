#include <memory>
#include <vector>

#include "MainMenu.h"

#include "../../../Input/InputManager.h"
#include "../../../Audio/AudioManager.h"
//#include "../../Common/FrameRenderer/FrameRenderer.h"
#include "../../SceneManager.h"
#include "../../../Collision/Collision.h"
#include "../../../Application.h"
#include "../Confirm/Confirm.h"
#include "../Game/GameScene.h"
#include "../Title/TitleScene.h"

MainMenu::MainMenu(void)
{
	handle_ = -1;
	confirm_ = nullptr;
}

MainMenu::~MainMenu(void)
{
}

void MainMenu::Init(void)
{
	ChangeSelect(MENU::NONE);

	confirm_ = std::make_shared<Confirm>();

	//AudioManager::GetInstance()->PlayBGM(SoundID::BGM_MAINMENU);
}

void MainMenu::Load(void)
{
	// メインメニューサウンド読みこみ
	//AudioManager::GetInstance()->LoadSceneSound(LoadScene::MAIN_MENU);

	handle_ = LoadGraph("Common/Title.png");

	menuButtons_.clear();

	// PLAY画像
	menuButtons_.push_back({ MENU::PLAY, LoadGraph("MainMenu/play.png"),
								PLAY_POS_X, PLAY_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });
	// QUIT画像
	menuButtons_.push_back({ MENU::QUIT, LoadGraph("Common/quit.png"),
						QUIT_POS_X, QUIT_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });

}

void MainMenu::LoadEnd(void)
{
	Init();
}

void MainMenu::Update(void)
{
#ifdef _DEBUG

	// メニューからタイトルに戻すボタン
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_SPACE))
	{
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<GameScene>(), GAME, true);
		return;
	}

#endif // _DEBUG

	// メニューからタイトルに戻すボタン
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_ESCAPE))
	{
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<TitleScene>(),TITLE, true);
		return;
	}

	// 選択処理
	SelectUpgrade();

	// 決定ボタンを押されていなかったら、処理を行わない
	if (!InputManager::GetInstance()->DecideButton()) return;

	// メニューが選択されていない場合、処理を行わない
	if (currentMenu_ == MENU::NONE) return;

	switch (currentMenu_)
	{
	case MENU::PLAY:
		UpdatePlay();
		break;

	case MENU::QUIT:
		UpdateQuit();
		break;
	}

	// ボタン押下のSEを流す
	//AudioManager::GetInstance()->PlaySE(SoundID::SYS_BUTTON_1);

}

void MainMenu::Draw(void)
{
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	DrawGraph(TITLE_POS_X, TITLE_POS_Y, handle_, true);

	for (const auto& button : menuButtons_)
	{
		if (button.type == currentMenu_)
		{
			//FrameRenderer::Draw(button.x, button.y);
		}
		DrawGraph(button.x, button.y, button.graphHandle, true);		// メニューボタンの画像
	}

#ifdef _DEBUG

	DrawString(0, 0, "MainMenu", 0xffffff);

#endif // _DEBUG
}

void MainMenu::Release(void)
{
	for (const auto& button : menuButtons_)
	{
		DeleteGraph(button.graphHandle);
	}
	menuButtons_.clear();

	DeleteGraph(handle_);
	handle_ = -1;

	//AudioManager::GetInstance()->DeleteSceneSound(LoadScene::MAIN_MENU);
}

void MainMenu::ChangeSelect(MENU menu)
{
	currentMenu_ = menu;
}


void MainMenu::UpdatePlay(void)
{
	SceneManager::GetInstance()->PushScene(confirm_);
}

void MainMenu::UpdateQuit(void)
{
	confirm_->ChangeType(Confirm::TYPE::QUIT);
	SceneManager::GetInstance()->PushScene(confirm_);
}

void MainMenu::SelectUpgrade(void)
{
	// 前回の選択物を入れておく
	MENU prevMenu = currentMenu_;

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
	if (currentMenu_ != MENU::NONE
		&& currentMenu_ != prevMenu)
	{
		// ボタンに乗ったサウンドを出す
		//AudioManager::GetInstance()->PlaySE(SoundID::SYS_SELECT_ON);
	}
}

void MainMenu::MouseSelect(void)
{
	MENU nextSelect = MENU::NONE;

	// 衝突判定
	for (const auto& button : menuButtons_)
	{
		if (Collision::HitMouseImg2Box({ static_cast<float>(button.x), static_cast<float>(button.y) },
			static_cast<float>(button.sizeX), static_cast<float>(button.sizeY)))
		{
			nextSelect = button.type;
			break;
		}
	}

	ChangeSelect(nextSelect);
}

void MainMenu::PadSelect(void)
{
	auto prevMenu = currentMenu_;

	switch (currentMenu_)
	{
	case MainMenu::MENU::NONE:

		ChangeSelect(MENU::PLAY);

		break;
	case MainMenu::MENU::PLAY:

		if (InputManager::GetInstance()->SelectDownPad())
		{
			ChangeSelect(MENU::QUIT);
		}

		break;
	case MainMenu::MENU::QUIT:

		if (InputManager::GetInstance()->SelectUpPad())
		{
			ChangeSelect(MENU::PLAY);
		}

		break;
	default:
		break;
	}
}
