#include <DxLib.h>

#include "../../../Input/InputManager.h"
#include "../../../Audio/AudioManager.h"
#include "../../SceneManager.h"
#include "../../../Application.h"
#include "../Confirm/Confirm.h"
#include "../MainMenu/MainMenu.h"
//#include "../../Common/MouseCursor/MouseCursor.h"

#include "TitleScene.h"

TitleScene::TitleScene(void)
{
	alpha_ = ALPHA_MIN;
	isIncreasing_ = false; 
	isPlaySoundSE_ = false;
	confirm_ = nullptr;
	// マウスの表示する
	//MouseCursor::GetInstance()->SetMouseDraw(true);
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	alpha_ = ALPHA_MAX;
	isIncreasing_ = false;
}

void TitleScene::Load(void)
{
	//AudioManager::GetInstance()->LoadSceneSound(LoadScene::TITLE);
	backImg_ = LoadGraph("Data/Image/Common/Title.png");
	buttonImg_ = LoadGraph("Data/Image/Title/PushAnyButton.png");

	confirm_ = std::make_shared<Confirm>();
}

void TitleScene::LoadEnd(void)
{
	// 初期化処理
	Init();

	// BGMを再生
	//AudioManager::GetInstance()->PlayBGM(SoundID::BGM_TITLE);
}

void TitleScene::Update(void)
{
	// 何かのボタンを押されたら
	if (InputManager::GetInstance()->PushAnyButton())
	{
		/*confirm_->ChangeType(Confirm::TYPE::QUIT);
		SceneManager::GetInstance()->PushScene(confirm_);*/
		SceneManager::GetInstance()->PushScene(std::make_shared<MainMenu>());
		return;
	}

	// ボタンを押されたら動きを止める
	if (isPlaySoundSE_)return;

	// ボタンのアルファ値を変化させる
	if (isIncreasing_)
	{
		alpha_ += ALPHA_SPEED; // 増加速度

		// 増加が最大になったら減少に切り替える
		if(alpha_ >= ALPHA_MAX)
		{
			alpha_ = ALPHA_MAX;
			isIncreasing_ = false; 
		}
	}
	else
	{
		alpha_ -= ALPHA_SPEED; // 減少速度

		// 減少が最小になったら増加に切り替える
		if(alpha_ <= ALPHA_MIN)
		{
			alpha_ = ALPHA_MIN;
			isIncreasing_ = true;
		}
	}
}

void TitleScene::Draw(void)
{
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 - 100, 1.0, 0.0, backImg_, true);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(alpha_));
	DrawRotaGraph(BUTTON_POS_X, BUTTON_POS_Y, 1.0, 0.0, buttonImg_, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

#ifdef _DEBUG

	DrawString(0, 0, "Title", 0xffffff);

#endif // _DEBUG

}

void TitleScene::Release(void)
{
	// BGMの解放
	//AudioManager::GetInstance()->DeleteSceneSound(LoadScene::TITLE);

	// 画像の解放
	DeleteGraph(backImg_);
	DeleteGraph(buttonImg_);
}
