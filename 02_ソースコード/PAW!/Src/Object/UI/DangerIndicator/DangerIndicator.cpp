#include <DxLib.h>
#include "../../../Application.h"
#include "../../Camera/CameraUtility.h"
#include "../../Enemy/CatRobot/CatRobot.h"
#include "../../Enemy/CatRobot/Attack/PlateAttack/PlateAttack.h"
#include "../../Enemy/CatRobot/Attack/CutleryAttack/CutleryAttack.h"

#include "DangerIndicator.h"

namespace
{
	// チカチカさせる間隔
	constexpr int BLINK_INTERVAL = 25;

	// 分割数縦横
	constexpr int DIV_NUM_Y = 3;

	// 描画する時間
	constexpr int DRAW_COUNT = 100;
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
	if (drawCounter_ > 0)
	{
		drawCounter_--;

		if (drawCounter_ <= 0)
		{
			arrowType_ = ARROW_MAX;
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
	if (arrowType_ != ARROW_TYPE::ARROW_MAX)
	{
		// 危険知らせる矢印を描画
		DrawGraph(0, 0, handles_[static_cast<int>(arrowType_)], true);
	}
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
	//if (isOnce_)
	//{
	//	for (const auto& plate : catRobot_.GetPlateAttack())
	//	{
	//		if (CheckCameraViewClip(plate->GetTransform()->pos_))continue;

	//		// カメラの視界に入っていれば初期化
	//		arrowType_ = ARROW_MAX;
	//		drawCounter_ = DRAW_COUNT;
	//		isOnce_ = false;
	//	}
	//	return;
	//}

	for (const auto& plate : catRobot_.GetPlateAttack())
	{
		// カメラの視界に入っていれば処理を行わない
		if (!CheckCameraViewClip(plate->GetTransform()->pos_)) continue;

		if (plate->GetState() == PLATE_ATTACK_STATE_END) continue;

		// 描画する矢印の種類を設定
		SelectArrowType(plate->GetTransform()->pos_);
		drawCounter_ = DRAW_COUNT;
		isOnce_ = true;
		break;
	}
}

void DangerIndicator::CheckCutleryAttack(void)
{
	if (isOnce_)
	{
		for (const auto& cutlery : catRobot_.GetCutleryAttack())
		{
			if (CheckCameraViewClip(cutlery->GetTransform()->pos_))continue;

			// カメラの視界に入っていれば初期化
			arrowType_ = ARROW_MAX;
			drawCounter_ = DRAW_COUNT;
		}
		return;
	}

	for (const auto& cutlery : catRobot_.GetCutleryAttack())
	{
		// 攻撃準備中でなければ処理を行わない
		if (cutlery->GetState() != CUTLERY_ATTACK_STATE::CUTLERY_ATTACK_STATE_IDLE)break;

		// カメラの視界に入っていなければ
		if (CheckCameraViewClip(cutlery->GetTransform()->pos_))
		{
			// 描画する矢印の種類を設定
			SelectArrowType(cutlery->GetTransform()->pos_);
			drawCounter_ = DRAW_COUNT;
			isOnce_ = true;
			break;
		}
	}
}

void DangerIndicator::SelectArrowType(VECTOR pos)
{
	VECTOR vec = VSub(pos, CameraUtility::GetCameraPos());

	MATRIX mat = CameraUtility::GetCameraMatrix();

	float side = VDot(vec, { mat.m[0][0], mat.m[0][1], mat.m[0][2] });    // + なら右側、- なら左側
	float forward = VDot(vec, { mat.m[2][0], mat.m[2][1], mat.m[2][2] });  // + なら前方、- なら背後

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

	if (forward < 0)
	{
		// 下向きの矢印
		arrowType_ = ARROW_2;
	}
}
