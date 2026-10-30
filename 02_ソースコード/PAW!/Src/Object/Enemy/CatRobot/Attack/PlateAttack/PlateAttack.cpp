#include "../../../../Object.h"
#include "../../../../../Collision/3DCollider/CylinderCollider.h"

#include "PlateAttack.h"

namespace
{
	// 移動速度
	constexpr float MOVE_SPEED = 30.0f;
}

PlateAttack::PlateAttack(void)
{
	// 攻撃状態
	// 初期化関数
	stateCtrl_.initStateTable_[PLATE_ATTACK_STATE_IDLE] = InitIdle;
	stateCtrl_.initStateTable_[PLATE_ATTACK_STATE_MOVE] = InitMove;
	stateCtrl_.initStateTable_[PLATE_ATTACK_STATE_END] = InitEnd;
	// 更新関数
	stateCtrl_.updateStateTable_[PLATE_ATTACK_STATE_IDLE] = UpdateIdle;
	stateCtrl_.updateStateTable_[PLATE_ATTACK_STATE_MOVE] = UpdateMove;
	stateCtrl_.updateStateTable_[PLATE_ATTACK_STATE_END] = UpdateEnd;
}

void PlateAttack::Init(void)
{
	AddComponets();

	// 移動速度初期化
	info_.moveSpeed_ = MOVE_SPEED;

	// 終了状態へ
	ChangeState(PLATE_ATTACK_STATE_END);
}

void PlateAttack::Update(void)
{
	// 状態別更新処理
	UpdateState();
}

void PlateAttack::StartAttack(VECTOR pos, VECTOR moveDir, float moveDistance, int idleCount)
{
	// 指定の座標を設定
	transform_->pos_ = pos;

	// 向きを設定
	info_.moveDir_ = moveDir;

	// 移動距離を設定
	info_.moveDistance_ = moveDistance;

	// 目的地を設定
	info_.goalPos_ =
		VAdd(transform_->pos_,
			VScale(info_.moveDir_, moveDistance));

	// 待機時間を設定
	info_.idleCount_ = idleCount;

	// 待機状態へ
	ChangeState(PLATE_ATTACK_STATE_IDLE);
}

void PlateAttack::UpdateState(void)
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

void PlateAttack::InitIdle(PlateAttack& plateAttack)
{
}

void PlateAttack::InitMove(PlateAttack& plateAttack)
{
	// タグを攻撃へ
	plateAttack.cylinderCollider_->SetCollisionTag(CollisionTag::ENEMY_ATTACK);
}

void PlateAttack::InitEnd(PlateAttack& plateAttack)
{
	// タグをNONEへ
	plateAttack.cylinderCollider_->SetCollisionTag(CollisionTag::NONE);

	// 座標初期化
	plateAttack.transform_->pos_ = {};
}

void PlateAttack::UpdateIdle(PlateAttack& plateAttack)
{
	plateAttack.info_.idleCount_--;

	// カウントが0になったら
	if (plateAttack.info_.idleCount_ <= 0)
	{
		// カウントを0に初期化
		plateAttack.info_.idleCount_ = 0;

		// 移動状態へ
		plateAttack.ChangeState(PLATE_ATTACK_STATE_MOVE);
	}
}

void PlateAttack::UpdateMove(PlateAttack& plateAttack)
{
	// 移動
	plateAttack.Move();

	// 距離を調べる
	VECTOR vec = VSub(plateAttack.transform_->pos_, plateAttack.info_.goalPos_);
	float distance = vec.x * vec.x + vec.z * vec.z;

	// 目標距離を超えていたら
	if (distance >= plateAttack.info_.moveDistance_ * plateAttack.info_.moveDistance_)
	{
		// 終了状態へ
		plateAttack.ChangeState(PLATE_ATTACK_STATE_END);
	}
}

void PlateAttack::UpdateEnd(PlateAttack& attack2)
{
}

void PlateAttack::ChangeState(PLATE_ATTACK_STATE state)
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

void PlateAttack::AddComponets(void)
{
	// 座標の設定
	transform_ = owner_->AddComponent<Transform>();
	transform_->pos_ = {};
	transform_->angle_ = {};
	transform_->scale_ = { 4.0f,4.0f,4.0f };

	// 当たり判定の設定(円柱)
	cylinderCollider_ = owner_->AddComponent<CylinderCollider>();
	cylinderCollider_->SetTransform(transform_);
	// 円柱の設定
	cylinderCollider_->SetCenterOffset({});
	cylinderCollider_->SetRadius(PLATE_RADIUS);
	cylinderCollider_->SetHeight(PLATE_HEIGHT);
	// タグの設定
	cylinderCollider_->SetCollisionTag(CollisionTag::NONE);
	cylinderCollider_->SetInverseMass(0.0f);
}

void PlateAttack::Move(void)
{
	// 移動速度が0以下であれば処理を行わない
	if (info_.moveSpeed_ <= 0.0f)return;

	// 方向×スピードで移動量を作って、座標に足して移動
	transform_->pos_ =
		VAdd(transform_->pos_,
			VScale(info_.moveDir_, info_.moveSpeed_));
}

void PlateAttack::OnCollision(Collider* other)
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
