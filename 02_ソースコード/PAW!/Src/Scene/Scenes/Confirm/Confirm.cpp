#include <vector>

#include "../../../Input/InputManager.h"
#include "../../../Audio/AudioManager.h"
#include "../../SceneManager.h"
#include "../../../Collision/CollisionMath.h"
#include "../MainMenu/MainMenu.h"

#include "Confirm.h"

namespace
{
	// 画像サイズ（YES, NO）
	constexpr int IMAGE_SIZE_X = 150;
	constexpr int IMAGE_SIZE_Y = 70;

	// YES
	constexpr int YES_POS_X = (Application::SCREEN_SIZE_X - IMAGE_SIZE_X) / 2 - 150;
	constexpr int YES_POS_Y = (Application::SCREEN_SIZE_Y - IMAGE_SIZE_Y) / 2 + 150;

	// NO
	constexpr int NO_POS_X = (Application::SCREEN_SIZE_X - IMAGE_SIZE_X) / 2 + 150;
	constexpr int NO_POS_Y = (Application::SCREEN_SIZE_Y - IMAGE_SIZE_Y) / 2 + 150;

	// 猫画像を変更するタイミング
	constexpr int CAT_COUNT = 120;
	constexpr int CHANGE_CAT_EYE_COUNT = 10;
}

Confirm::Confirm(void)
{
	for (int i = 0; i < static_cast<int>(CAT_TYPE::MAX); ++i)
	{
		catImg_[i] = -1;
	}
}

Confirm::~Confirm(void)
{
}

void Confirm::Init(void)
{
	// 現在の選択肢初期化
	ChangeSelect(SELECT::NONE);

	// 猫の種類初期化
	ChangeCat(CAT_TYPE::OPEN_EYES);
}

void Confirm::Load(void)
{
	// 画像のロード
	switch (SceneManager::GetInstance()->GetNowSceneTag())
	{
	case SCENE_TAG::TITLE:
	case SCENE_TAG::MAINMANU:
		// ゲーム終了時の確認画面
		confirmImg_ = LoadGraph("Data/Image/Confirm/ConfirmExit.png");
		break;

	default:
		break;
	}

	// 猫画像
	catImg_[static_cast<int>(CAT_TYPE::OPEN_EYES)] =
		LoadGraph("Data/Image/Confirm/ConfirmCatOpen.png");
	catImg_[static_cast<int>(CAT_TYPE::CLOSING_EYES)] =
		LoadGraph("Data/Image/Confirm/ConfirmCatClosing.png");
	catImg_[static_cast<int>(CAT_TYPE::CLOSE_EYE)] =
		LoadGraph("Data/Image/Confirm/ConfirmCatClose.png");

	// 配列をクリアにしてから、画像を追加
	selectButtons_.clear();
	// YES画像
	selectButtons_.push_back({
		SELECT::YES,
		LoadGraph("Confirm/YesOff.png"),
		LoadGraph("Confirm/YesOn.png"),
		YES_POS_X,
		YES_POS_Y,
		IMAGE_SIZE_X,
		IMAGE_SIZE_Y });

	// NO画像
	selectButtons_.push_back({
		SELECT::NO,
		LoadGraph("Confirm/NoOff.png"),
		LoadGraph("Confirm/NoOn.png"),
		NO_POS_X,
		NO_POS_Y,
		IMAGE_SIZE_X,
		IMAGE_SIZE_Y });
}

void Confirm::LoadEnd(void)
{
	Init();
}

void Confirm::Update(void)
{
	// 猫更新処理
	UpdateCat();

	// 選択処理
	UpdateSelect();

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
	// 確認の画面を描画
	DrawGraph(0, 0, confirmImg_, true);

	// 猫を描画
	DrawGraph(0, 0, catImg_[static_cast<int>(currentCat_)], true);

	// ボタン描画
	for (const auto& button : selectButtons_)
	{
		// 選択されているボタン
		if (button.type == currentSelect_)
		{
			DrawRotaGraphF(button.x, button.y, 1.0f, 0.0f, button.graphHandleOn, true);
		}
		// 選択されていないボタン
		else
		{
			DrawRotaGraphF(button.x, button.y, 1.0f, 0.0f, button.graphHandleOff, true);
		}
	}
}

void Confirm::Release(void)
{
	// 画像を解放
	for (const auto& button : selectButtons_)
	{
		DeleteGraph(button.graphHandleOff);
		DeleteGraph(button.graphHandleOn);
	}
	selectButtons_.clear();

	DeleteGraph(confirmImg_);
	confirmImg_ = -1;

	for (int i = 0; i < static_cast<int>(CAT_TYPE::MAX); ++i)
	{
		DeleteGraph(catImg_[i]);
		catImg_[i] = -1;
	}
}

void Confirm::ChangeCat(CAT_TYPE type)
{
	currentCat_ = type;

	switch (currentCat_)
	{
	case Confirm::CAT_TYPE::OPEN_EYES:
		// 猫カウント初期化
		changeCatCount = 0;
		isOpenEye = true;
		break;
	case Confirm::CAT_TYPE::CLOSING_EYES:
		break;
	case Confirm::CAT_TYPE::CLOSE_EYE:
		isOpenEye = false;
		break;
	case Confirm::CAT_TYPE::MAX:
		break;
	default:
		break;
	}
}

void Confirm::UpdateYes(void)
{
	// 決定ボタンを押されたら
	if (InputManager::GetInstance()->DecideButton())
	{
		// 画像のロード
		switch (SceneManager::GetInstance()->GetNowSceneTag())
		{
		case SCENE_TAG::TITLE:
		case SCENE_TAG::MAINMANU:
			Application::GetInstance()->SetEnd(true);
			break;

		default:
			break;
		}
	}
}

void Confirm::UpdateNo(void)
{
	// 決定ボタンを押されたら
	if (InputManager::GetInstance()->DecideButton())
	{
		// 確認シーンを閉じる
		SceneManager::GetInstance()->PopScene();
	}
}

void Confirm::UpdateSelect(void)
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
	// 現在選択しているメニュー
	int current = static_cast<int>(currentSelect_);

	// 上が押されたら
	if (InputManager::GetInstance()->SelectRightPad())
	{
		// メニューを上に移動
		current--;
	}
	// 下が押されたら
	else if (InputManager::GetInstance()->SelectLeftPad())
	{
		// メニューを下に移動
		current++;
	}

	// 移動先をキャスト
	SELECT next = static_cast<SELECT>(current);

	// 下限設定
	if (next <= SELECT::NONE)
	{
		next = SELECT::YES;
	}

	// 上限設定
	if (next >= SELECT::MAX)
	{
		next = SELECT::YES;
	}

	// メニューを切り替える
	ChangeSelect(next);
}

void Confirm::UpdateCat(void)
{
	// カウンターを進める
	changeCatCount++;

	// 変更タイミングに到達していなければ処理を行わない
	if (changeCatCount < CAT_COUNT)return;

	// 一定フレームごとに瞬きさせる
	if ((changeCatCount - CAT_COUNT) % CHANGE_CAT_EYE_COUNT != 0)return;

	// 現在選択しているメニュー
	int current = static_cast<int>(currentCat_);

	// 目を開けていたら
	if (isOpenEye)
	{
		current++;
	}
	// 目を閉じていたら
	else
	{
		current--;
	}

	// 移動先をキャスト
	CAT_TYPE next = static_cast<CAT_TYPE>(current);

	// 下限設定
	if (next <= CAT_TYPE::OPEN_EYES)
	{
		next = CAT_TYPE::OPEN_EYES;
	}

	// 上限設定
	if (next >= CAT_TYPE::CLOSE_EYE)
	{
		next = CAT_TYPE::CLOSE_EYE;
	}

	// 猫画像を切り替える
	ChangeCat(next);
}
