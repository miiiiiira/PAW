#include "../../Player/Player.h"
#include "../Rat/Rat.h"
#include "../CatRobot/CatRobot.h"
#include "../../../Audio/AudioManager.h"
#include "../../Component/Transform/Transform.h"
#include "../../Camera/Camera.h"
#include "../../../System/Fader/Fader.h"

#include "Wave4.h"

namespace
{
	constexpr VECTOR RAT_SPAWN_POS = { -160.0f, 0.0f, 1500.0f };
	constexpr VECTOR RAT2_SPAWN_POS = { -60.0f, 0.0f, 1600.0f };
	constexpr VECTOR RAT3_SPAWN_POS = { 60.0f, 0.0f, 1600.0f };
	constexpr VECTOR RAT4_SPAWN_POS = { 160.0f, 0.0f, 1560.0f };

	// イベントシーン時のアニメーション変更時間
	constexpr int EVENT_CHANGE_ANIM_TIME = 40;
	constexpr int EVENT_CHANGE_ANIM_TIME_2 = 40;
	constexpr int EVENT_CHANGE_ANIM_TIME_3 = 60;
	constexpr int EVENT_CHANGE_ANIM_TIME_4 = 60;

	// アルファ値設定
	constexpr int ALPHA_STEP = 10;	// アルファ値の増減量
}

Wave4::Wave4(void)
{
	// 画像読み込み
	everyoneAttackImage_ = LoadGraph("Data/Image/Game/everyoneAttack.png");
	goImage_ = LoadGraph("Data/Image/Game/go.png");
	circleRoundImage_ = LoadGraph("Data/Image/Game/circleRound.png");
	hungryRoundImage_ = LoadGraph("Data/Image/Game/hungry.png");
}

Wave4::~Wave4(void)
{
}

void Wave4::Draw(void)
{
	if (state_ == WaveBase::WAVE_STATE::EVENT)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha_);
		// みんなでかかれー！画像
		DrawBillboard3D(
			{ rats_[1]->GetTransform()->pos_.x - 50.0f ,
			rats_[1]->GetTransform()->pos_.y + 170.0f ,
			rats_[1]->GetTransform()->pos_.z - 10.0f },
			0.7f, 0.7f,
			140.0f,
			0.0f,
			everyoneAttackImage_,
			true);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha2_);
		// いくぞー！画像
		DrawBillboard3D(
			{ rats_[0]->GetTransform()->pos_.x - 50.0f ,
			rats_[0]->GetTransform()->pos_.y + 140.0f ,
			rats_[0]->GetTransform()->pos_.z - 10.0f },
			0.7f, 0.7f,
			100.0f,
			0.0f,
			goImage_,
			true);

		// かこめかこめー！画像
		DrawBillboard3D(
			{ rats_[3]->GetTransform()->pos_.x + 50.0f ,
			rats_[3]->GetTransform()->pos_.y + 140.0f ,
			rats_[3]->GetTransform()->pos_.z - 10.0f },
			0.7f, 0.7f,
			100.0f,
			0.0f,
			circleRoundImage_,
			true);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha3_);
		// おなかすいたー画像
		DrawBillboard3D(
			{ rats_[2]->GetTransform()->pos_.x + 10.0f,
			rats_[2]->GetTransform()->pos_.y + 170.0f ,
			rats_[2]->GetTransform()->pos_.z - 10.0f },
			0.7f, 0.7f,
			100.0f,
			0.0f,
			hungryRoundImage_,
			true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// 基底クラスの描画処理
	WaveBase::Draw();
}

void Wave4::OnStart(void)
{
	player_->ResetPlayer();

	rats_[0]->ResetRat(RAT_SPAWN_POS, RAT_ANIMATION::SUPPORT2, EVENT_CHANGE_ANIM_TIME);
	rats_[1]->ResetRat(RAT2_SPAWN_POS, RAT_ANIMATION::ORDER, EVENT_CHANGE_ANIM_TIME_2, false);
	rats_[2]->ResetRat(RAT3_SPAWN_POS);
	rats_[3]->ResetRat(RAT4_SPAWN_POS, RAT_ANIMATION::SUPPORT, EVENT_CHANGE_ANIM_TIME_4);
}

void Wave4::OnCatRobot(void)
{
}

void Wave4::OnEvent(void)
{
	// 基底クラスのイベント処理
	WaveBase::OnEvent();

	if (alpha3_ > 0)
	{
		alpha3_ += ALPHA_STEP;

		if (alpha3_ > ALPHA_MAX)
		{
			alpha3_ = ALPHA_MAX;
		}
	}

	if (alpha2_ > 0)
	{
		alpha2_ += ALPHA_STEP;

		if (alpha2_ >= 128 &&
			alpha3_ <= 0)
		{
			alpha3_++;
		}

		if (alpha2_ > ALPHA_MAX)
		{
			alpha2_ = ALPHA_MAX;
		}
	}

	if (alpha_ > 0)
	{
		alpha_ += ALPHA_STEP;
		if (alpha_ > ALPHA_MAX)
		{
			alpha_ = ALPHA_MAX;

			if (alpha2_ <= 0)
			{
				alpha2_++;
			}
		}
	}
	else
	{
		// ネズミがイベント待機状態確認
		bool isAllIdle = true;
		for (int i = 0; i < 4; i++)
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

void Wave4::OnClear(void)
{
}

bool Wave4::CheckEventClear(void)
{
	// 全ネズミがイベント移動中かイベント待機中かを確認
	bool allClear = true;
	for (int i = 0; i < 4; i++)
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

bool Wave4::CheckCatRobotClear(void)
{
	if (catRobot_->GetNowHP() <= 0)
	{
		return true;
	}

	return false;
}

void Wave4::ChangeInit(void)
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
		rats_[2]->SetEventEndPos(RAT3_SPAWN_POS);
		rats_[3]->SetEventEndPos(RAT4_SPAWN_POS);

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
