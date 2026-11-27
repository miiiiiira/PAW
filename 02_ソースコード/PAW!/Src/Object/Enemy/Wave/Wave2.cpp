#include "../../Player/Player.h"
#include "../Rat/Rat.h"
#include "../CatRobot/CatRobot.h"
#include "../../../Audio/AudioManager.h"
#include "../../Component/Transform/Transform.h"
#include "../../Camera/Camera.h"
#include "../../../System/Fader/Fader.h"

#include "Wave2.h"

namespace
{
	// イベントシーン時のネズミの開始座標
	constexpr VECTOR RAT_SPAWN_POS = { -70.0f, 0.0f, 1500.0f };
	constexpr VECTOR RAT2_SPAWN_POS = { 70.0f, 0.0f, 1500.0f };

	// イベントシーン時のアニメーション変更時間
	constexpr int EVENT_CHANGE_ANIM_TIME = 40;
	constexpr int EVENT_CHANGE_ANIM_TIME_2 = 40;

	// アルファ値設定
	constexpr int ALPHA_STEP = 10;	// アルファ値の増減量
}

Wave2::Wave2(void)
{
	// 画像読み込み
	vsImage_ = LoadGraph("Data/Image/Game/2vs1.png");
}

Wave2::~Wave2(void)
{
	// 画像解放
	DeleteGraph(vsImage_);
}

void Wave2::Draw(void)
{
	if (state_ == WaveBase::WAVE_STATE::EVENT)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha_);
		// 2たい1だ画像
		DrawBillboard3D(
			{ rats_[0]->GetTransform()->pos_.x + 95.0f ,
			rats_[0]->GetTransform()->pos_.y + 170.0f ,
			rats_[0]->GetTransform()->pos_.z - 10.0f },
			0.7f, 0.7f,
			120.0f,
			0.0f,
			vsImage_,
			true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// 基底クラスの描画処理
	WaveBase::Draw();
}

void Wave2::OnStart(void)
{
	player_->ResetPlayer();
	rats_[0]->ResetRat(RAT_SPAWN_POS, RAT_ANIMATION::SUPPORT, EVENT_CHANGE_ANIM_TIME);
	rats_[1]->ResetRat(RAT2_SPAWN_POS, RAT_ANIMATION::SUPPORT2, EVENT_CHANGE_ANIM_TIME_2);
}

void Wave2::OnCatRobot(void)
{
}

void Wave2::OnEvent(void)
{
	// 基底クラスのイベント処理
	WaveBase::OnEvent();

	if (alpha_ > 0)
	{
		alpha_ += ALPHA_STEP;
		if (alpha_ > ALPHA_MAX)
		{
			alpha_ = ALPHA_MAX;
		}
	}
	else
	{
		// ネズミがイベント待機状態確認
		bool isAllIdle = true;
		for (int i = 0; i < 2; i++)
		{
			if (rats_[i]->GetState() != RAT_STATE_EVENT_IDLE)
			{
				isAllIdle = false;
			}
		}

		// ネズミがイベント待機状態になったらアルファ値を増やす
		if (isAllIdle)
		{
			alpha_++;
		}
	}
}

void Wave2::OnClear(void)
{
}

bool Wave2::CheckEventClear(void)
{
	// 全ネズミがイベント移動中かイベント待機中かを確認
	bool allClear = true;
	for (int i = 0; i < 2; i++)
	{
		if (!rats_[i]->IsEndEventMove())
		{
			allClear = false;
		}
	}

	// スキップ処理を一定以上押されるか、イベント移動が終了したら
	if (skipTimer_ >= MAX_SKIP_TIME ||
		allClear)
	{
		// BGMを鳴らす
		AudioManager::GetInstance()->PlayBGM(SoundID::BGM_GAME);
		return true;
	}

	return false;
}

bool Wave2::CheckCatRobotClear(void)
{
	if (catRobot_->GetNowHP() <= CatRobot::DEFAULT_HP - 10)
	{
		return true;
	}

	return false;
}

void Wave2::ChangeInit(void)
{
	switch (state_)
	{
	case WaveBase::WAVE_STATE::EVENT:
		// BGM再生
		AudioManager::GetInstance()->PlayBGM(SoundID::BGM_RAT_EVENT);

		// カメラのモード変更
		camera_->ChangeMode(Camera::MODE::FIXED, EVENT_CAMERA_POS, EVENT_CAMERA_ANGLE);
		break;

	case WaveBase::WAVE_STATE::RAT:
		rats_[0]->SetEventEndPos(RAT_SPAWN_POS);
		rats_[1]->SetEventEndPos(RAT2_SPAWN_POS);

		// カメラのモード変更
		camera_->ChangeMode(Camera::MODE::FOLLOW);
		break;

	case WaveBase::WAVE_STATE::CAT_ROBOT:
		break;

	case WaveBase::WAVE_STATE::CLEARED:
		Fader::GetInstance()->SetFade(Fader::STATE::FADE_OUT, Fader::TYPE::CROSS, true);
		break;

	default:
		break;
	}
}
