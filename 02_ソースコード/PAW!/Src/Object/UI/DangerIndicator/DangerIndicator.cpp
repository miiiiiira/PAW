#include <cmath>
#include <DxLib.h>
#include "../../../Application.h"
#include "../../Camera/CameraUtility.h"
#include "../../Enemy/CatRobot/CatRobot.h"
#include "../../Enemy/CatRobot/Attack/PlateAttack/PlateAttack.h"
#include "../../Enemy/CatRobot/Attack/CutleryAttack/CutleryAttack.h"

#include "DangerIndicator.h"

namespace
{
	// 分割数縦横
	constexpr int DIV_NUM_Y = 3;

	// 描画する時間
	constexpr int ALPHA_MAX = 255;

	// 後ろと判定する範囲
	const float BACK_RANGE = 120.0f * DX_PI_F / 180.0f;
}

DangerIndicator::DangerIndicator(CatRobot& catRobot)
	:catRobot_(catRobot)
{
	// 画像ハンドルの初期化
	for (int i = 0; i < static_cast<int>(ARROW_TYPE::ARROW_MAX); i++)
	{
		handles_[i] = -1;
	}
}

DangerIndicator::~DangerIndicator(void)
{
}

void DangerIndicator::Init(void)
{
	// 描画種類の初期化
	arrowType_ = ARROW_MAX;
}

void DangerIndicator::Load(void)
{
	// 1枚の画像を分割して読み込み
	LoadDivGraph(
		"Data/Image/Game/arrow.png",
		static_cast<int>(ARROW_TYPE::ARROW_MAX),
		1,
		DIV_NUM_Y,
		Application::SCREEN_SIZE_X,
		Application::SCREEN_SIZE_Y,
		handles_);
}

void DangerIndicator::Update(void)
{
	// アルファ値があれば
	if (alpha_ > 0)
	{
		// カウンタを減少させる
		alpha_ -= 3;

		if (alpha_ <= 0)
		{
			// 初期化
			alpha_ = 0;
		}
	}

	// 攻撃状態を取得
	CAT_ROBOT_ATTACK_STATE attackState = catRobot_.GetAttackState();

	// 攻撃の状態ごとに確認項目変更
	switch (attackState)
	{
	case CAT_ROBOT_ATTACK_2:
	case CAT_ROBOT_ATTACK_3:
		CheckPlateAttack();
		break;

	case CAT_ROBOT_ATTACK_4:
	case CAT_ROBOT_ATTACK_5:
		CheckCutleryAttack();
		break;

	case CAT_ROBOT_ATTACK_SELECT:
	case CAT_ROBOT_ATTACK_1:
	case CAT_ROBOT_ATTACK_END:
	case CAT_ROBOT_ATTACK_MAX:
	default:
		break;
	}
}

void DangerIndicator::Draw(void)
{
	// アルファ値を設定
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha_);

	// 危険知らせる矢印を描画
	DrawGraph(0, 0, handles_[static_cast<int>(arrowType_)], true);

	// アルファ値を元に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void DangerIndicator::Release(void)
{
	// 画像ハンドルの解放
	for (int i = 0; i < static_cast<int>(ARROW_TYPE::ARROW_MAX); i++)
	{
		DeleteGraph(handles_[i]);
	}
}

void DangerIndicator::CheckPlateAttack(void)
{
	// 表示が行われていたら
	if (arrowType_ != ARROW_TYPE::ARROW_MAX)
	{
		bool isAllEnd = true;
		for (const auto& plate : catRobot_.GetPlateAttack())
		{
			// 終了状態になっていなければ
			if (plate->GetState() != PLATE_ATTACK_STATE_END)
			{
				// フラグ折る
				isAllEnd = false;
			}
		}

		if (isAllEnd)
		{
			// すべて終了状態であれば初期化
			arrowType_ = ARROW_MAX;
			alpha_ = 0;
		}

		return;
	}

	// カメラ内に入っているか確認
	CheckCameraView();
}

void DangerIndicator::CheckCutleryAttack(void)
{
	// 表示が行われていたら
	if (arrowType_ != ARROW_TYPE::ARROW_MAX)
	{
		bool isAllEnd = true;
		for (const auto& cutlery : catRobot_.GetCutleryAttack())
		{
			// 終了状態になっていなければ
			if (cutlery->GetState() != CUTLERY_ATTACK_STATE_END)
			{
				// フラグ折る
				isAllEnd = false;
			}
		}

		if (isAllEnd)
		{
			// すべて終了状態であれば初期化
			arrowType_ = ARROW_MAX;
			alpha_ = 0;
		}

		return;
	}
	
	// カメラ内に入っているか確認
	CheckCameraView();
}

void DangerIndicator::CheckCameraView(void)
{
	VECTOR attackPos = catRobot_.GetPlateCutleryAttackPos();
	// カメラの視界に入っていれば処理を行わない
	if (!CheckCameraViewClip(attackPos)) return;

	// 描画する矢印の種類を設定
	SelectArrowType(attackPos);
	alpha_ = ALPHA_MAX;
}

void DangerIndicator::SelectArrowType(VECTOR pos)
{
	VECTOR vec = VSub(pos, CameraUtility::GetCameraPos());
	vec.y = 0.0f;

	MATRIX mat = CameraUtility::GetCameraMatrix();

	// 向きを調べる
	float side = VDot(vec, { mat.m[0][0], 0.0f, mat.m[0][2] });    // + なら右側、- なら左側
	float forward = VDot(vec, { mat.m[2][0], 0.0f, mat.m[2][2] });  // + なら前方、- なら背後

	if (side < 0)
	{
		// 左向きの矢印
		arrowType_ = ARROW_1;
	}
	else if (side > 0)
	{
		// 右向きの矢印
		arrowType_ = ARROW_3;
	}

	// 後ろ側にあると判断した場合
	if (forward < 0)
	{
		float angle = std::atan2(side, forward);

		// 後ろと判定する範囲(全体で120度 → 片側60度)
		if (std::fabs(angle) >= DX_PI_F - BACK_RANGE * 0.5f)
		{
			// 一定の角度以上であれば後ろにあると判断
			arrowType_ = ARROW_2;
		}
	}
}
