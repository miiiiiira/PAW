#include "../../Object.h"

#include "Rat.h"

namespace
{
	// 移動設定
	constexpr float MOVE_SPEED = 10.0f;	// 移動速度

	// 重力設定
	constexpr float JUMP_POW = 25.0f;	// ジャンプ力
	constexpr float GRAVITY = -1.98f;	// 重力加速度
	constexpr float MAX_FALL = -15.0f;	// 最大落下速度

	// ダメージ設定
	constexpr float HIT_REACT_FRICTION = 0.5f;	// ダメージ時のリアクション時の摩擦
}

Rat::Rat(void)
{
	// プレイヤー状態
	// 初期化関数
	stateCtrl_.initStateTable_[RAT_STATE_IDLE] = InitIdle;
	stateCtrl_.initStateTable_[RAT_STATE_MOVE] = InitMove;
	stateCtrl_.initStateTable_[RAT_STATE_ATTACK] = InitAttack;
	stateCtrl_.initStateTable_[RAT_STATE_HIT] = InitHit;
	stateCtrl_.initStateTable_[RAT_STATE_DEAD] = InitDead;
	stateCtrl_.initStateTable_[RAT_STATE_END] = InitEnd;
	// 更新関数
	stateCtrl_.updateStateTable_[RAT_STATE_IDLE] = UpdateIdle;
	stateCtrl_.updateStateTable_[RAT_STATE_MOVE] = UpdateMove;
	stateCtrl_.updateStateTable_[RAT_STATE_ATTACK] = UpdateAttack;
	stateCtrl_.updateStateTable_[RAT_STATE_HIT] = UpdateHit;
	stateCtrl_.updateStateTable_[RAT_STATE_DEAD] = UpdateDead;
	stateCtrl_.updateStateTable_[RAT_STATE_END] = UpdateEnd;

	// プレイヤー攻撃状態
	// 初期化関数
	stateCtrl_.initAttackTable_[RAT_ATTACK_START] = InitAttackStart;
	stateCtrl_.initAttackTable_[RAT_ATTACKING] = InitAttacking;
	stateCtrl_.initAttackTable_[RAT_ATTACK_AFTER] = InitAttackAfter;
	stateCtrl_.initAttackTable_[RAT_ATTACK_END] = InitAttackEnd;
	// 更新関数
	stateCtrl_.updateAttackTable_[RAT_ATTACK_START] = UpdateAttackStart;
	stateCtrl_.updateAttackTable_[RAT_ATTACKING] = UpdateAttacking;
	stateCtrl_.updateAttackTable_[RAT_ATTACK_AFTER] = UpdateAttackAfter;
	stateCtrl_.updateAttackTable_[RAT_ATTACK_END] = UpdateAttackEnd;

	// プレイヤー被ダメージ状態
	// 初期化関数
	stateCtrl_.initHitTable_[RAT_HIT_START] = InitHitStart;
	stateCtrl_.initHitTable_[RAT_HIT_STUN] = InitHitStun;
	stateCtrl_.initHitTable_[RAT_HIT_END] = InitHitEnd;
	// 更新関数
	stateCtrl_.updateHitTable_[RAT_HIT_START] = UpdateHitStart;
	stateCtrl_.updateHitTable_[RAT_HIT_STUN] = UpdateHitStun;
	stateCtrl_.updateHitTable_[RAT_HIT_END] = UpdateHitEnd;
}

void Rat::Load(void)
{
}

void Rat::Init(void)
{
}

void Rat::Update(void)
{
	// 終了していたら処理を行わない
	if (stateCtrl_.state_ == RAT_STATE_END)
	{
		return;
	}

	// 状態別更新処理
	UpdateState();

	// 重力処理
	ApplyGravity();

	// 無敵時間を更新
	UpdateInvincible();
}

void Rat::Draw2D(void)
{
}

void Rat::Draw3D(void)
{
}

void Rat::SetDamage(int damage)
{
	// 無敵時間があればダメージを与えない
	if (info_.invincibleTime_ > 0)return;

	// HPにダメージを与える
	info_.hp_ -= damage;

	// HPが0以下になったら
	if (info_.hp_ <= 0)
	{
		// 0初期化しておく
		info_.hp_ = 0;

		// 死亡ステートへ
		ChangeState(RAT_STATE_DEAD);
	}
	else
	{
		// 無敵時間を設ける
		//info_.invincibleTime_ = INVINCIBLE_TIME;

		// ダメージを受けたステートへ
		ChangeState(RAT_STATE_HIT);
	}
}

void Rat::SetHitReact(VECTOR moveDir, float moveSpeed, float jumpPow)
{
	// 指定された移動向きを設定
	info_.moveDir_ = moveDir;

	// 指定された移動速度を設定
	info_.moveSpeed_ = moveSpeed;

	// 指定されたジャンプ力を設定
	info_.velocityY_ = jumpPow;

	// ダメージを受けたステートへ
	ChangeState(RAT_STATE_HIT);
}

void Rat::UpdateState(void)
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

void Rat::InitIdle(Rat& rat)
{
}

void Rat::InitMove(Rat& rat)
{
	// ねずみの移動速度を設定
	rat.info_.moveSpeed_ = MOVE_SPEED;
}

void Rat::InitAttack(Rat& rat)
{
	// 初期化
	rat.ChangeAttackState(RAT_ATTACK_START);
}

void Rat::InitHit(Rat& rat)
{
	// 最初の状態へ初期化
	rat.ChangeHitState(RAT_HIT_START);
}

void Rat::InitDead(Rat& rat)
{
}

void Rat::InitEnd(Rat& rat)
{
}

void Rat::UpdateIdle(Rat& rat)
{
}

void Rat::UpdateMove(Rat& rat)
{
}

void Rat::UpdateAttack(Rat& rat)
{
	// nullチェック
	if (rat.stateCtrl_.updateAttackTable_[rat.stateCtrl_.attackState_])
	{
		// 状態別初期化
		rat.stateCtrl_.updateAttackTable_[rat.stateCtrl_.attackState_](rat);
	}
}

void Rat::UpdateHit(Rat& rat)
{
	// nullチェック
	if (rat.stateCtrl_.updateHitTable_[rat.stateCtrl_.hitState_])
	{
		// 状態別初期化
		rat.stateCtrl_.updateHitTable_[rat.stateCtrl_.hitState_](rat);
	}
}

void Rat::UpdateDead(Rat& rat)
{
	rat.ChangeState(RAT_STATE_END);
}

void Rat::UpdateEnd(Rat& rat)
{
}

void Rat::ChangeState(RAT_STATE state)
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

void Rat::InitAttackStart(Rat& rat)
{
}

void Rat::InitAttacking(Rat& rat)
{
}

void Rat::InitAttackAfter(Rat& rat)
{
}

void Rat::InitAttackEnd(Rat& rat)
{
}

void Rat::UpdateAttackStart(Rat& rat)
{
	// 攻撃中へ
	rat.ChangeAttackState(RAT_ATTACKING);
}

void Rat::UpdateAttacking(Rat& rat)
{
	// 攻撃後の隙状態へ
	rat.ChangeAttackState(RAT_ATTACK_AFTER);
}

void Rat::UpdateAttackAfter(Rat& rat)
{
	// 攻撃終了へ
	rat.ChangeAttackState(RAT_ATTACK_END);
}

void Rat::UpdateAttackEnd(Rat& rat)
{
	// 待機状態へ
	rat.ChangeState(RAT_STATE_IDLE);
}

void Rat::ChangeAttackState(RAT_ATTACK_STATE state)
{
	// 指定されたステートへ変更
	stateCtrl_.attackState_ = state;

	// nullチェック
	if (stateCtrl_.initAttackTable_[stateCtrl_.attackState_])
	{
		// 状態別初期化
		stateCtrl_.initAttackTable_[stateCtrl_.attackState_](*this);
	}
}

void Rat::InitHitStart(Rat& rat)
{
}

void Rat::InitHitStun(Rat& rat)
{
}

void Rat::InitHitEnd(Rat& rat)
{
}

void Rat::UpdateHitStart(Rat& rat)
{
	rat.ChangeHitState(RAT_HIT_STUN);
}

void Rat::UpdateHitStun(Rat& rat)
{
	// 移動速度が0より大きく移動している場合
	if (rat.info_.moveSpeed_ > 0.0f)
	{
		// 移動速度を減算
		rat.info_.moveSpeed_ -= HIT_REACT_FRICTION;

		// 指定された方向と移動速度を使用し座標に反映
		rat.Move();
	}
	else
	{
		rat.ChangeHitState(RAT_HIT_END);
	}
}

void Rat::UpdateHitEnd(Rat& rat)
{
	// 待機状態へ
	rat.ChangeState(RAT_STATE_IDLE);
}

void Rat::ChangeHitState(RAT_HIT_STATE state)
{
	// 指定されたステートへ変更
	stateCtrl_.hitState_ = state;

	// nullチェック
	if (stateCtrl_.initHitTable_[stateCtrl_.hitState_])
	{
		// 状態別初期化
		stateCtrl_.initHitTable_[stateCtrl_.hitState_](*this);
	}
}

void Rat::ApplyGravity(void)
{
	// ステージコライダが無ければ処理を行わない
	//if (!stageColl_) return;
	// Y座標へ反映
	transform_->pos_.y += info_.velocityY_;
	// 接地判定

	// 空中
	//if (!stageColl_->IsGround())
	if (transform_->pos_.y > 0.0f)
	{
		// 重力加算
		info_.velocityY_ += GRAVITY;

		// 最大落下速度
		if (info_.velocityY_ < MAX_FALL)
			info_.velocityY_ = MAX_FALL;
	}
	else
	{
		transform_->pos_.y = 0.0f;

		// 地面上なら少し下方向に押す
		// 0だと浮く場合があるため
		info_.velocityY_ = -0.1f;
	}
}

void Rat::UpdateInvincible(void)
{
	// 無敵時間を減らす
	if (info_.invincibleTime_ > 0)
	{
		--info_.invincibleTime_;
	}
}

void Rat::Move(void)
{
	// 移動速度が0以下であれば処理を行わない
	if (info_.moveSpeed_ <= 0.0f)return;

	// 方向×スピードで移動量を作って、座標に足して移動
	transform_->pos_ =
		VAdd(transform_->pos_,
			VScale(info_.moveDir_, info_.moveSpeed_));
}
