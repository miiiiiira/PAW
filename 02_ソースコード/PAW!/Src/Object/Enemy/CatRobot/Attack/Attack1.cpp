#include "../../../Object.h"
#include "../../../../Collision/3DCollider/SphereCollider.h"

#include "Attack1.h"

namespace
{
	// 移動速度
	constexpr float MOVE_SPEED = 20.0f;

	// 移動方向
	constexpr VECTOR MOVE_DIR = { 0.0f,-1.0f,0.0f };

	// 待機時カウント
	constexpr int IDLE_COUNT = 120;
}

Attack1::Attack1(void)
{
	// 攻撃状態
	// 初期化関数
	stateCtrl_.initStateTable_[ATTACK1_STATE_IDLE] = InitIdle;
	stateCtrl_.initStateTable_[ATTACK1_STATE_MOVE] = InitMove;
	stateCtrl_.initStateTable_[ATTACK1_STATE_END] = InitEnd;
	// 更新関数
	stateCtrl_.updateStateTable_[ATTACK1_STATE_IDLE] = UpdateIdle;
	stateCtrl_.updateStateTable_[ATTACK1_STATE_MOVE] = UpdateMove;
	stateCtrl_.updateStateTable_[ATTACK1_STATE_END] = UpdateEnd;
}

void Attack1::Init(void)
{
	AddComponets();

	// 移動速度初期化
	info_.moveSpeed_ = MOVE_SPEED;
	// 移動方向初期化
	info_.moveDir_ = MOVE_DIR;

	// 終了状態へ
	ChangeState(ATTACK1_STATE_END);
}

void Attack1::Update(void)
{
	// 状態別更新処理
	UpdateState();
}

void Attack1::StartAttack(VECTOR pos)
{
	// 指定の座標を設定
	transform_->pos_ = pos;

	// 待機状態へ
	ChangeState(ATTACK1_STATE_IDLE);
}

void Attack1::UpdateState(void)
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

void Attack1::InitIdle(Attack1& attack1)
{
	attack1.info_.idleCount_ = IDLE_COUNT;
}

void Attack1::InitMove(Attack1& attack1)
{
	// タグを攻撃へ
	attack1.sphereCollder_->SetCollisionTag(CollisionTag::ENEMY_ATTACK);
}

void Attack1::InitEnd(Attack1& attack1)
{
	// タグをNONEへ
	attack1.sphereCollder_->SetCollisionTag(CollisionTag::NONE);

	// 座標初期化
	attack1.transform_->pos_ = {};
}

void Attack1::UpdateIdle(Attack1& attack1)
{
	attack1.info_.idleCount_--;

	// カウントが0になったら
	if (attack1.info_.idleCount_ <= 0)
	{
		// カウントを0に初期化
		attack1.info_.idleCount_ = 0;

		// 移動状態へ
		attack1.ChangeState(ATTACK1_STATE_MOVE);
	}
}

void Attack1::UpdateMove(Attack1& attack1)
{
	// 移動
	attack1.Move();

	// 地面についていたら
	if (attack1.transform_->pos_.y <= 0.0f)
	{
		// 終了状態へ
		attack1.ChangeState(ATTACK1_STATE_END);
	}
}

void Attack1::UpdateEnd(Attack1& attack1)
{
}

void Attack1::ChangeState(ATTACK1_STATE state)
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

void Attack1::AddComponets(void)
{
	// 座標の設定
	transform_ = owner_->AddComponent<Transform>();
	transform_->pos_ = {};
	transform_->angle_ = {};
	transform_->scale_ = { 4.0f,4.0f,4.0f };

	// 当たり判定の設定(球)
	sphereCollder_ = owner_->AddComponent<SphereCollider>();
	sphereCollder_->SetTransform(transform_);
	// 球の設定
	sphereCollder_->SetCenterOffset({});
	sphereCollder_->SetRadius(ATTACK1_RADIUS);
	// タグの設定
	sphereCollder_->SetCollisionTag(CollisionTag::NONE);
	sphereCollder_->SetInverseMass(0.0f);
}

void Attack1::Move(void)
{
	// 移動速度が0以下であれば処理を行わない
	if (info_.moveSpeed_ <= 0.0f)return;

	// 方向×スピードで移動量を作って、座標に足して移動
	transform_->pos_ =
		VAdd(transform_->pos_,
			VScale(info_.moveDir_, info_.moveSpeed_));
}

void Attack1::OnCollision(Collider* other)
{
	// 衝突したオブジェクトをタグごとに判別
	switch (other->GetCollisionTag())
	{
	case CollisionTag::PLAYER:
		// プレイヤー本体と接触した
		// 攻撃後の隙状態へ
		ChangeState(ATTACK1_STATE_END);
		break;

	default:
		break;
	}
}
