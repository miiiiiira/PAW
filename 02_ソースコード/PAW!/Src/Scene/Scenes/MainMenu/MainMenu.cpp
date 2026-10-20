#include <memory>
#include <vector>

#include "MainMenu.h"

#include "../../../Input/InputManager.h"
#include "../../../Audio/AudioManager.h"
//#include "../../Common/FrameRenderer/FrameRenderer.h"
#include "../../SceneManager.h"
#include "../../../Collision/CollisionMath.h"
#include "../../../Application.h"
#include "../Confirm/Confirm.h"
#include "../Game/GameScene.h"
#include "../Title/TitleScene.h"

namespace
{
	// 画像サイズ
	static constexpr int IMAGE_SIZE_X = 401;
	static constexpr int IMAGE_SIZE_Y = 75;

	// GAME_START
	static constexpr float GAME_START_POS_X = Application::SCREEN_SIZE_X / 2;
	static constexpr float GAME_START_POS_Y = Application::SCREEN_SIZE_Y / 2 + 100.0f;

	// TITLE
	static constexpr float TITLE_POS_X = Application::SCREEN_SIZE_X / 2;
	static constexpr float TITLE_POS_Y = Application::SCREEN_SIZE_Y / 2 + 200.0f;

	// GAME_END
	static constexpr float GAME_END_POS_X = Application::SCREEN_SIZE_X / 2;
	static constexpr float GAME_END_POS_Y = Application::SCREEN_SIZE_Y / 2 + 300.0f;
}

MainMenu::MainMenu(void)
{
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

	menuButtons_.clear();

	// GameStart画像設定
	menuButtons_.push_back({ 
		MENU::GAME_START,
		LoadGraph("Data/Image/MainMenu/Button/GameStartOff.png"),
		LoadGraph("Data/Image/MainMenu/Button/GameStartOn.png"),
		GAME_START_POS_X, GAME_START_POS_Y,
		IMAGE_SIZE_X, IMAGE_SIZE_Y 
		});

	// Title画像設定
	menuButtons_.push_back({
		MENU::TITLE,
		LoadGraph("Data/Image/MainMenu/Button/TitleOff.png"),
		LoadGraph("Data/Image/MainMenu/Button/TitleOn.png"),
		TITLE_POS_X, TITLE_POS_Y,
		IMAGE_SIZE_X, IMAGE_SIZE_Y
		});

	// GameEnd画像設定
	menuButtons_.push_back({
		MENU::EXIT,
		LoadGraph("Data/Image/MainMenu/Button/ExitOff.png"),
		LoadGraph("Data/Image/MainMenu/Button/ExitOn.png"),
		GAME_END_POS_X, GAME_END_POS_Y,
		IMAGE_SIZE_X, IMAGE_SIZE_Y
		});

}

void MainMenu::LoadEnd(void)
{
	Init();
}

void MainMenu::Update(void)
{
	// メニューからタイトルに戻すボタン
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_ESCAPE))
	{
		SceneManager::GetInstance()->PopScene();
		return;
	}

	// 選択処理
	SelectUpgrade();

	// メニューが選択されていない場合、処理を行わない
	if (currentMenu_ == MENU::NONE) return;

	switch (currentMenu_)
	{
	case MENU::GAME_START:
		UpdateGameStart();
		break;

	case MENU::TITLE:
		UpdateTitle();
		break;

	case MENU::EXIT:
		UpdateExit();
		break;
	}

	// ボタン押下のSEを流す
	//AudioManager::GetInstance()->PlaySE(SoundID::SYS_BUTTON_1);
}

void MainMenu::Draw(void)
{
	// 全てのボタンを描画
	for (const auto& button : menuButtons_)
	{
		// 選択されているボタン
		if (button.type == currentMenu_)
		{
			DrawRotaGraphF(button.x, button.y, 1.0f, 0.0f, button.graphHandleOn, true);
		}
		// 選択されていないボタン
		else
		{
			DrawRotaGraphF(button.x, button.y, 1.0f, 0.0f, button.graphHandleOff, true);
		}
	}

#ifdef _DEBUG

	DrawString(0, 0, "MainMenu", 0xffffff);
	DrawFormatString(0, 100, 0xff0000, "currentMenu_ %d", currentMenu_);

#endif // _DEBUG
}

void MainMenu::Release(void)
{
	for (const auto& button : menuButtons_)
	{
		DeleteGraph(button.graphHandleOff);
		DeleteGraph(button.graphHandleOn);
	}
	menuButtons_.clear();

	//AudioManager::GetInstance()->DeleteSceneSound(LoadScene::MAIN_MENU);
}

void MainMenu::ChangeSelect(MENU menu)
{
	currentMenu_ = menu;
}

void MainMenu::UpdateGameStart(void)
{
	// 決定ボタンを押が押されたら
	if (InputManager::GetInstance()->DecideButton())
	{
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<GameScene>(), SCENE_TAG::GAME, true);
		//SceneManager::GetInstance()->PushScene(confirm_);
	}
}

void MainMenu::UpdateTitle(void)
{
	// 決定ボタンを押が押されたら
	if (InputManager::GetInstance()->DecideButton())
	{
		SceneManager::GetInstance()->PopScene();
		//SceneManager::GetInstance()->PushScene(confirm_);
	}
}

void MainMenu::UpdateExit(void)
{
	// 決定ボタンを押が押されたら
	if (InputManager::GetInstance()->DecideButton())
	{
		SceneManager::GetInstance()->GameEnd();
	}
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
	MENU nextMenu = MENU::NONE;
	
	// 衝突判定
	for (const auto& button : menuButtons_)
	{
		// ボタンと当たっているか
		if (Collision::HitMouseImg2Box(
			{button.x - static_cast<float>(button.sizeX / 2),
			button.y - static_cast<float>(button.sizeY / 2) },
			static_cast<float>(button.sizeX), 
			static_cast<float>(button.sizeY)))
		{
			// 当たっている場所をアクティブなボタンに変更
			nextMenu = button.type;
			break;
		}
	}

	ChangeSelect(nextMenu);
}

void MainMenu::PadSelect(void)
{
	// 現在選択しているメニュー
	int current = static_cast<int>(currentMenu_);

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
	MENU next = static_cast<MENU>(current);

	// 下限設定
	if (next <= MENU::NONE)
	{
		next = MENU::GAME_START;
	}

	// 上限設定
	if (next >= MENU::MAX)
	{
		next = MENU::EXIT;
	}

	// メニューを切り替える
	ChangeSelect(next);
}
