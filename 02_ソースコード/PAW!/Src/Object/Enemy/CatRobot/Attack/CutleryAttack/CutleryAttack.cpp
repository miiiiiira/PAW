#include "../../../../Object.h"
#include "../../../../Component/Graphics/Render3D.h"
#include "../../../../../Collision/3DCollider/CapsuleCollider.h"
#include "../../../../../Audio/AudioManager.h"

#include "CutleryAttack.h"

namespace
{
	// 移動速度
	constexpr float MOVE_SPEED = 30.0f;

	// 当たり判定の半径
	constexpr float COLLIDER_RADIUS = 30.0f;
	constexpr float START_OFFSET_XZ = 1600.0f;
	constexpr float END_OFFSET_XZ = -1600.0f;
}

CutleryAttack::CutleryAttack(void)
{
	// 攻撃状態
	// 初期化関数
	stateCtrl_.initStateTable_[CUTLERY_ATTACK_STATE_IDLE] = InitIdle;
	stateCtrl_.initStateTable_[CUTLERY_ATTACK_STATE_MOVE] = InitMove;
	stateCtrl_.initStateTable_[CUTLERY_ATTACK_STATE_END] = InitEnd;
	// 更新関数
	stateCtrl_.updateStateTable_[CUTLERY_ATTACK_STATE_IDLE] = UpdateIdle;
	stateCtrl_.updateStateTable_[CUTLERY_ATTACK_STATE_MOVE] = UpdateMove;
	stateCtrl_.updateStateTable_[CUTLERY_ATTACK_STATE_END] = UpdateEnd;
}

void CutleryAttack::Init(void)
{
	AddComponets();

	// 移動速度初期化
	info_.moveSpeed_ = MOVE_SPEED;

	// 終了状態へ
	ChangeState(CUTLERY_ATTACK_STATE_END);
}

void CutleryAttack::Update(void)
{
	// 状態別更新処理
	UpdateState();
}

void CutleryAttack::StartAttack(VECTOR pos, VECTOR moveDir, float moveDistance, int idleCount)
{
	// 指定の座標を設定
	transform_->pos_ = pos;

	// 向きを設定
	info_.moveDir_ = moveDir;

	// 当たり判定の形状を設定
	if (std::abs(info_.moveDir_.x) > 0)
	{
		// X軸方向に移動する場合、Z軸方向にカプセルを設定
		capsuleCollider_->SetStartOffset(
			{ 0.0f,transform_->pos_.y,START_OFFSET_XZ });
		capsuleCollider_->SetEndOffset(
			{ 0.0f,transform_->pos_.y,END_OFFSET_XZ });
	}
	else if (std::abs(info_.moveDir_.z) > 0)
	{
		// Z軸方向に移動する場合、X軸方向にカプセルを設定
		capsuleCollider_->SetStartOffset(
			{ START_OFFSET_XZ ,transform_->pos_.y ,0.0f });
		capsuleCollider_->SetEndOffset(
			{ END_OFFSET_XZ ,transform_->pos_.y ,0.0f });
	}

	// 移動距離を設定
	info_.moveDistance_ = moveDistance;

	// 目的地を設定
	info_.goalPos_ =
		VAdd(transform_->pos_,
			VScale(info_.moveDir_, moveDistance));

	// 待機時間を設定
	info_.idleCount_ = idleCount;

	// 待機状態へ
	ChangeState(CUTLERY_ATTACK_STATE_IDLE);
}

void CutleryAttack::UpdateState(void)
{
	// Transformがなければ処理しない
	if (!transform_) return;

	// nullチェック
	if (stateCtrl_.updateStateTable_[stateCtrl_.state_])
	{
		// 状態別更新
		stateCtrl_.updateStateTable_[stateCtrl_.state_](*this);
	}
}

void CutleryAttack::InitIdle(CutleryAttack& cutleryAttack)
{
	// 描画しない
	cutleryAttack.render3D_->SetIsDraw(true);
}

void CutleryAttack::InitMove(CutleryAttack& cutleryAttack)
{
	// タグを攻撃へ
	cutleryAttack.capsuleCollider_->SetCollisionTag(CollisionTag::ENEMY_ATTACK);
}

void CutleryAttack::InitEnd(CutleryAttack& cutleryAttack)
{
	// タグをNONEへ
	cutleryAttack.capsuleCollider_->SetCollisionTag(CollisionTag::NONE);

	// 座標初期化
	cutleryAttack.transform_->pos_ = {};

	// 描画しない
	cutleryAttack.render3D_->SetIsDraw(false);
}

void CutleryAttack::UpdateIdle(CutleryAttack& cutleryAttack)
{
	cutleryAttack.info_.idleCount_--;

	// カウントが0になったら
	if (cutleryAttack.info_.idleCount_ <= 0)
	{
		// カウントを0に初期化
		cutleryAttack.info_.idleCount_ = 0;

		// 移動状態へ
		cutleryAttack.ChangeState(CUTLERY_ATTACK_STATE_MOVE);

		// 攻撃2
		AudioManager::GetInstance()->PlaySE(SoundID::SE_ROBOT_ATTACK2);
	}
}

void CutleryAttack::UpdateMove(CutleryAttack& cutleryAttack)
{
	// 移動
	cutleryAttack.Move();

	// 距離を調べる
	VECTOR vec = VSub(cutleryAttack.transform_->pos_, cutleryAttack.info_.goalPos_);
	float distance = vec.x * vec.x + vec.z * vec.z;

	// 目標距離を超えていたら
	if (distance >= cutleryAttack.info_.moveDistance_ * cutleryAttack.info_.moveDistance_)
	{
		// 終了状態へ
		cutleryAttack.ChangeState(CUTLERY_ATTACK_STATE_END);
	}
}

void CutleryAttack::UpdateEnd(CutleryAttack& cutleryAttack)
{
}

void CutleryAttack::ChangeState(CUTLERY_ATTACK_STATE state)
{
	// 指定されたステートへ変更
	stateCtrl_.state_ = state;

	// nullチェック
	if (stateCtrl_.initStateTable_[stateCtrl_.state_])
	{
		// 状態別初期化
		stateCtrl_.initStateTable_[stateCtrl_.state_](*this);
	}
}

void CutleryAttack::AddComponets(void)
{
	// 座標の設定
	transform_ = owner_->AddComponent<Transform>();
	transform_->pos_ = {};
	transform_->angle_ = {};
	transform_->scale_ = { 1.0f,1.0f,1.0f };

	// 3Dモデル設定
	render3D_ = owner_->AddComponent<Render3D>();
	render3D_->SetTransform(transform_);
	render3D_->SetModel("Data/Model/Enemy/NekoBot/Plate/Plate.mv1");

	// 当たり判定の設定(円柱)
	capsuleCollider_ = owner_->AddComponent<CapsuleCollider>();
	capsuleCollider_->SetTransform(transform_);
	capsuleCollider_->AddCapsule(
		{ 0.0f,transform_->pos_.y,START_OFFSET_XZ },
		{ 0.0f,transform_->pos_.y,END_OFFSET_XZ },
		COLLIDER_RADIUS);

	// タグの設定
	capsuleCollider_->SetCollisionTag(CollisionTag::NONE);
	capsuleCollider_->SetInverseMass(0.0f);
}

void CutleryAttack::Move(void)
{
	// 移動速度が0以下であれば処理を行わない
	if (info_.moveSpeed_ <= 0.0f)return;

	// 方向×スピードで移動量を作って、座標に足して移動
	transform_->pos_ =
		VAdd(transform_->pos_,
			VScale(info_.moveDir_, info_.moveSpeed_));
}

void CutleryAttack::OnCollision(Collider* other)
{
	// 衝突したオブジェクトをタグごとに判別
	switch (other->GetCollisionTag())
	{
	case CollisionTag::PLAYER:
		// プレイヤー本体と接触した
		break;

	default:
		break;
	}
}
