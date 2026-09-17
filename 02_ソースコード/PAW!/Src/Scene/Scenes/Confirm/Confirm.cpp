#include <vector>

#include "Confirm.h"

#include "../../../Input/InputManager.h"
#include "../../../Audio/AudioManager.h"
#include "../../SceneManager.h"
#include "../../../Collision/Collision.h"
#include "../MainMenu/MainMenu.h"
//#include "../../Common/FrameRenderer/FrameRenderer.h"

Confirm::Confirm(void)
{
	confirmImg_ = -1;
}

Confirm::~Confirm(void)
{
}

void Confirm::Init(void)
{
	ChangeSelect(SELECT::NONE);
}

void Confirm::Load(void)
{

	switch (confirmType_)
	{
	case TYPE::QUIT:
		confirmImg_ = LoadGraph("Confirm/ConfirmQuit.png");		// QUITの時の確認画面
		break;
	case TYPE::MAIN_MENU:
		confirmImg_ = LoadGraph("Confirm/ConfirmMainMenu.png");	// MAIN MENUの時の確認画面
		break;
	break;
	}


	// 配列をクリアにしてから、画像を追加
	selectButtons_.clear();
	// YES画像
	selectButtons_.push_back({ SELECT::YES, LoadGraph("Confirm/Yes.png"),
								YES_POS_X, YES_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });
	// NO画像
	selectButtons_.push_back({ SELECT::NO,  LoadGraph("Confirm/No.png"),
							NO_POS_X, NO_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });
}

void Confirm::LoadEnd(void)
{
	Init();
}

void Confirm::Update(void)
{
#ifdef _DEBUG

	// メニューからタイトルに戻すボタン
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_SPACE))
	{
		UpdateYes();
		return;
	}

#endif // _DEBUG


	// 選択処理
	SelectUpgrade();

	// マウスを左クリックされなかったら、処理しない
	if (!InputManager::GetInstance()->DecideButton()) return;
	// どの選択肢も選ばれていない場合は処理しない
	if (currentSelect_ == SELECT::NONE) return;

	switch (currentSelect_)
	{
	case Confirm::SELECT::YES:
		UpdateYes();
		break;
	case Confirm::SELECT::NO:
		UpdateNo();
		break;
	}

	// ボタン押下のSEを流す
	//AudioManager::GetInstance()->PlaySE(SoundID::SYS_BUTTON_1);
}

void Confirm::Draw(void)
{
	DrawGraph(CONFIRM_POS_X, CONFIRM_POS_Y, confirmImg_, true);				// 確認の文字を表示

	for (const auto& button : selectButtons_)
	{
		if (button.type == currentSelect_)
		{
			/*FrameRenderer::Draw(button.x,
				button.y,
				button.sizeX,
				button.sizeY,
				FRAME_OFFSET);*/
		}
		DrawGraph(button.x, button.y, button.graphHandle, true);
	}

#ifdef _DEBUG

	DrawString(0, 0, "Confirm", 0xffffff);

#endif // _DEBUG
}

void Confirm::Release(void)
{
	for (const auto& button : selectButtons_)
	{
		DeleteGraph(button.graphHandle);
	}
	selectButtons_.clear();

	DeleteGraph(confirmImg_);
	confirmImg_ = -1;

}

void Confirm::ChangeType(TYPE type)
{
	confirmType_ = type;
}

void Confirm::ChangeSelect(SELECT select)
{
	currentSelect_ = select;
}

void Confirm::UpdateYes(void)
{
	switch (confirmType_)
	{
	case TYPE::QUIT:
		Application::GetInstance()->SetEnd(true);
		break;
	case TYPE::MAIN_MENU:
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<MainMenu>(),MAINMANU, true);
		// ゲームを終了したため初期化する
		SceneManager::GetInstance()->ResetGame();
		break;
	default:
		break;
	}
}

void Confirm::UpdateNo(void)
{
	// 確認シーンを閉じる
	SceneManager::GetInstance()->PopScene();
}

void Confirm::SelectUpgrade(void)
{
	// 前回の選択物を入れておく
	SELECT prevSelect = currentSelect_;

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
	if (currentSelect_ != SELECT::NONE
		&& currentSelect_ != prevSelect)
	{
		// ボタンに乗ったサウンドを出す
		//AudioManager::GetInstance()->PlaySE(SoundID::SYS_SELECT_ON);
	}
}

void Confirm::MouseSelect(void)
{
	// 衝突判定
	SELECT nextSelect = SELECT::NONE;

	for (const auto& button : selectButtons_)
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

void Confirm::PadSelect(void)
{
	switch (currentSelect_)
	{
	case Confirm::SELECT::NONE:

		// NOボタンへ
		ChangeSelect(SELECT::NO);

		break;
	case Confirm::SELECT::YES:

		// 右押されたら
		if (InputManager::GetInstance()->SelectRightPad())
		{
			// NOボタンへ
			ChangeSelect(SELECT::NO);
		}

		break;
	case Confirm::SELECT::NO:

		// 左押されたら
		if (InputManager::GetInstance()->SelectLeftPad())
		{
			// YESボタンへ
			ChangeSelect(SELECT::YES);
		}

		break;
	default:
		break;
	}
}