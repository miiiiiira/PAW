#include "../../Object.h"
#include "../../Component/Graphics/Render3D.h"
#include "../../Component/Animation/Animation.h"
#include "../../Camera/CameraUtility.h"
#include "../../../Collision/3DCollider/CapsuleCollider.h"
#include "../../../Math/Math.h"

#include "Rat.h"

namespace
{
	// 移動設定
	constexpr float MOVE_SPEED = 5.0f;			// 移動速度
	constexpr float MIN_MOVE_SPEED = 3.0f;		// 最低移動速度
	constexpr float ATTACK_MOVE_SPEED = 12.0f;	// 攻撃時の移動速度
	constexpr float APPROACH_MOVE_SPEED = 4.0f;	// ちょっと近づく時の移動速度
	constexpr float FORWARD_OFFSET = -0.3f;		// 前方向のオフセット

	// プレイヤーとの距離設定
	constexpr float MIN_DISTANCE = 170.0f;		// プレイヤーとの最小距離
	constexpr float MAX_DISTANCE = 200.0f;		// プレイヤーとの最大距離
	constexpr float MID_DISTANCE = (MIN_DISTANCE + MAX_DISTANCE) / 2.0f; // 最小距離と最大距離の中間の距離

	// 方向設定
	constexpr float ANGLE_COEFFICIENT = 0.2f;	// 補間の係数

	// 体力設定
	constexpr int DEFAULT_HP = 3;	// 初期HP

	// 重力設定
	constexpr float JUMP_POW = 17.0f;	// ジャンプ力
	constexpr float GRAVITY = -1.98f;	// 重力加速度
	constexpr float MAX_FALL = -10.0f;	// 最大落下速度

	// ダメージ設定
	constexpr float HIT_REACT_FRICTION = 0.5f;	// ダメージ時のリアクション時の摩擦

	// アニメーション設定
	constexpr float ANIM_SPEED_ATTACK = 0.8f;
	constexpr float ANIM_SPEED_ATTACK_START = 0.2f;
	constexpr float ANIM_SPEED_DEAD = 0.4f;
	constexpr float ANIM_SPEED_IDLE = 0.8f;
	constexpr float ANIM_SPEED_MOVE = 0.8f;
	constexpr float ANIM_SPEED_MOVE_CLOSER = 0.8f;

	// リミット設定
	constexpr int INVINCIBLE_TIME = 10;		// 無敵時間
	constexpr int WAIT_TIME = 120;			// 待機時間
	constexpr int SWAY_NUM_MAX = 4;			// 揺れの最大回数
	constexpr int RANDOM_RIMIT = 1000;		// ランダム用の最大係数
}

Rat::Rat(void)
{
	// ネズミ状態
	// 初期化関数
	stateCtrl_.initStateTable_[RAT_STATE_IDLE] = InitIdle;
	stateCtrl_.initStateTable_[RAT_STATE_MOVE] = InitMove;
	stateCtrl_.initStateTable_[RAT_STATE_RETREAT] = InitRetreat;
	stateCtrl_.initStateTable_[RAT_STATE_ATTACK] = InitAttack;
	stateCtrl_.initStateTable_[RAT_STATE_HIT] = InitHit;
	stateCtrl_.initStateTable_[RAT_STATE_DEAD] = InitDead;
	stateCtrl_.initStateTable_[RAT_STATE_END] = InitEnd;
	// 更新関数
	stateCtrl_.updateStateTable_[RAT_STATE_IDLE] = UpdateIdle;
	stateCtrl_.updateStateTable_[RAT_STATE_MOVE] = UpdateMove;
	stateCtrl_.updateStateTable_[RAT_STATE_RETREAT] = UpdateRetreat;
	stateCtrl_.updateStateTable_[RAT_STATE_ATTACK] = UpdateAttack;
	stateCtrl_.updateStateTable_[RAT_STATE_HIT] = UpdateHit;
	stateCtrl_.updateStateTable_[RAT_STATE_DEAD] = UpdateDead;
	stateCtrl_.updateStateTable_[RAT_STATE_END] = UpdateEnd;

	// ネズミ待機状態
	// 初期化関数
	stateCtrl_.initIdleTable_[RAT_IDLE_WAIT] = InitIdleWait;
	stateCtrl_.initIdleTable_[RAT_IDLE_SWAY] = InitIdleSway;
	stateCtrl_.initIdleTable_[RAT_IDLE_APPROACH] = InitIdleApproach;
	// 更新関数
	stateCtrl_.updateIdleTable_[RAT_IDLE_WAIT] = UpdateIdleWait;
	stateCtrl_.updateIdleTable_[RAT_IDLE_SWAY] = UpdateIdleSway;
	stateCtrl_.updateIdleTable_[RAT_IDLE_APPROACH] = UpdateIdleApproach;

	// ネズミ攻撃状態
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

	// ネズミ被ダメージ状態
	// 初期化関数
	stateCtrl_.initHitTable_[RAT_HIT_START] = InitHitStart;
	stateCtrl_.initHitTable_[RAT_HIT_STUN] = InitHitStun;
	stateCtrl_.initHitTable_[RAT_HIT_END] = InitHitEnd;
	// 更新関数
	stateCtrl_.updateHitTable_[RAT_HIT_START] = UpdateHitStart;
	stateCtrl_.updateHitTable_[RAT_HIT_STUN] = UpdateHitStun;
	stateCtrl_.updateHitTable_[RAT_HIT_END] = UpdateHitEnd;
}

void Rat::Init(void)
{
	// コンポーネントの追加
	AddComponets();

	// HPの初期化
	info_.hp_ = DEFAULT_HP;

	// ネズミの状態初期化
	ChangeState(RAT_STATE_IDLE);
}

void Rat::Update(void)
{
	// 終了していたら処理を行わない
	if (stateCtrl_.state_ == RAT_STATE_END)	return;

	// 状態別更新処理
	UpdateState();

	// 重力処理
	ApplyGravity();

	// 無敵時間を更新
	UpdateInvincible();
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
	// 待機状態を初期化
	rat.ChangeIdleState(RAT_IDLE_WAIT);
}

void Rat::InitMove(Rat& rat)
{
	// ネズミの移動速度を設定
	rat.info_.moveSpeed_ = MOVE_SPEED;
	// 待機アニメーション再生
	rat.animation_->Play(static_cast<int>(RAT_ANIMATION::MOVE));
}

void Rat::InitRetreat(Rat& rat)
{
	// ネズミの移動速度を設定
	rat.info_.moveSpeed_ = MOVE_SPEED;
	// 待機アニメーション再生
	rat.animation_->Play(static_cast<int>(RAT_ANIMATION::MOVE));
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
	// 死亡アニメーション再生
	rat.animation_->Play(static_cast<int>(RAT_ANIMATION::DEAD), false);
}

void Rat::InitEnd(Rat& rat)
{
	rat.render3D_->SetIsDraw(false);
}

void Rat::UpdateIdle(Rat& rat)
{
	// モデルがプレイヤーに向くようにする
	rat.GetAngleToPlayer();

	// nullチェック
	if (rat.stateCtrl_.updateIdleTable_[rat.stateCtrl_.idleState_])
	{
		// 状態別更新
		rat.stateCtrl_.updateIdleTable_[rat.stateCtrl_.idleState_](rat);
	}

	// 距離に応じて移動状態を更新する
	rat.UpdateMovementState();
}

void Rat::UpdateMove(Rat& rat)
{
	// ネズミからプレイヤーへの方向を算出
	rat.GetDirectionToPlayer();

	// モデルがプレイヤーに向くようにする
	rat.GetAngleToPlayer();

	// プレイヤーとの距離を詰めれたら
	if (rat.GetDistanceToPlayer() <= MAX_DISTANCE)
	{
		// 待機中へ
		rat.ChangeState(RAT_STATE_IDLE);
		return;
	}

	// 移動処理
	rat.Move();
}

void Rat::UpdateRetreat(Rat& rat)
{
	// プレイヤーから離れる方向を算出
	rat.GetDirectionToPlayer(true);

	// モデルがプレイヤーに向くようにする
	rat.GetAngleToPlayer();

	// プレイヤーと距離が取れたら
	if (rat.GetDistanceToPlayer() >= MIN_DISTANCE)
	{
		// 待機中へ
		rat.ChangeState(RAT_STATE_IDLE);
		return;
	}

	// 移動処理
	rat.Move();
}

void Rat::UpdateAttack(Rat& rat)
{
	// nullチェック
	if (rat.stateCtrl_.updateAttackTable_[rat.stateCtrl_.attackState_])
	{
		// 状態別更新
		rat.stateCtrl_.updateAttackTable_[rat.stateCtrl_.attackState_](rat);
	}
}

void Rat::UpdateHit(Rat& rat)
{
	// nullチェック
	if (rat.stateCtrl_.updateHitTable_[rat.stateCtrl_.hitState_])
	{
		// 状態別更新
		rat.stateCtrl_.updateHitTable_[rat.stateCtrl_.hitState_](rat);
	}
}

void Rat::UpdateDead(Rat& rat)
{
	// アニメーションが終了していたら
	if (rat.animation_->IsEnd())
	{
		// 完全死亡へ
		rat.ChangeState(RAT_STATE_END);
	}
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

bool Rat::IsCanAttack(void)
{
	bool isDistance = false;

	// プレイヤーとの距離を調べる
	float distance = GetDistanceToPlayer();

	// プレイヤーとの距離がちょうどよかったら
	if (distance < MAX_DISTANCE &&
		distance > MIN_DISTANCE)
	{
		isDistance = true;
	}

	// 待機中かつプレイヤーとの距離がちょうどよかったらtrueを返す
	return stateCtrl_.state_ == RAT_STATE_IDLE && isDistance;
}

void Rat::InitIdleWait(Rat& rat)
{
	// ネズミの移動速度を設定
	rat.info_.moveSpeed_ = 0.0f;

	// 待機時間の初期化
	rat.info_.waitTime_ = 0;

	// 待機アニメーション再生
	rat.animation_->Play(static_cast<int>(RAT_ANIMATION::IDLE));
}

void Rat::InitIdleSway(Rat& rat)
{
	// 揺れ時間初期化
	rat.info_.swayTime_ = 0;
	// 実際に揺れた回数初期化
	rat.info_.swayTotalNum_ = 0;
	// 指定されたジャンプ力を設定
	rat.info_.velocityY_ = JUMP_POW;
	// 待機時の揺れ処理用のランダム系の初期化
	rat.InitSwayRandom();
	// 移動方向を算出
	rat.GetDirectionLR(rat.info_.isRightSway_);
	// 待機アニメーション再生
	rat.animation_->Play(static_cast<int>(RAT_ANIMATION::MOVE_CLOSER));
}

void Rat::InitIdleApproach(Rat& rat)
{
	// ネズミの移動速度を設定
	rat.info_.moveSpeed_ = APPROACH_MOVE_SPEED;

	// ちょっと近づく時の距離を算出
	rat.SetApproachDistance();

	// 近づき中に設定
	rat.info_.isApproaching_ = true;

	// 待機アニメーション再生
	rat.animation_->Play(static_cast<int>(RAT_ANIMATION::MOVE));
}

void Rat::UpdateIdleWait(Rat& rat)
{
	rat.info_.waitTime_++;

	if (rat.info_.waitTime_ >= WAIT_TIME)
	{
		// 乱数を取得
		int random = GetRand(RANDOM_RIMIT);

		// 0～1にする
		random %= 2;
		// 1～2にする
		random++;

		// 待機状態の動きを変更
		rat.ChangeIdleState(static_cast<RAT_IDLE_STATE>(random));
	}
}

void Rat::UpdateIdleSway(Rat& rat)
{
	rat.info_.swayTime_++;

	// 移動方向を算出
	rat.GetDirectionLR(rat.info_.isRightSway_);

	if (rat.transform_->pos_.y > 0.0f)
	{
		// 移動させる
		rat.Move();
	}

	if (rat.info_.swayTime_ > 30)
	{
		// ジャンプ力を設定
		rat.info_.velocityY_ = JUMP_POW;
		rat.info_.swayTime_ = 0;
		rat.info_.swayTotalNum_++;
		rat.info_.isRightSway_ = !rat.info_.isRightSway_;
	}

	// 指定の回数分飛んでいたら
	if (rat.info_.swayNum_ <= rat.info_.swayTotalNum_)
	{
		// 重力初期化
		rat.info_.velocityY_ = -0.1f;

		// 待機中へ
		rat.ChangeIdleState(RAT_IDLE_WAIT);
		return;
	}
}

void Rat::UpdateIdleApproach(Rat& rat)
{
	if (rat.info_.isApproaching_)
	{
		// ネズミからプレイヤーへの方向を算出
		rat.GetDirectionToPlayer();
	}
	else
	{
		// プレイヤーから離れる方向を算出
		rat.GetDirectionToPlayer(true);
	}

	// 移動処理
	rat.Move();

	// 指定の距離まで近づいたら
	if (rat.GetDistanceToPlayer() <= rat.info_.approachDistance_)
	{
		rat.info_.isApproaching_ = false;
	}

	// プレイヤーと距離が取れたら
	if (!rat.info_.isApproaching_ &&
		rat.GetDistanceToPlayer() >= MIN_DISTANCE)
	{
		// 待機中へ
		rat.ChangeIdleState(RAT_IDLE_WAIT);
		return;
	}
}

void Rat::ChangeIdleState(RAT_IDLE_STATE state)
{
	// 指定されたステートへ変更
	stateCtrl_.idleState_ = state;

	// nullチェック
	if (stateCtrl_.initIdleTable_[stateCtrl_.idleState_])
	{
		// 状態別初期化
		stateCtrl_.initIdleTable_[stateCtrl_.idleState_](*this);
	}
}

void Rat::InitAttackStart(Rat& rat)
{
	// 攻撃始めアニメーション再生
	rat.animation_->Play(static_cast<int>(RAT_ANIMATION::ATTACK_START), false);
}

void Rat::InitAttacking(Rat& rat)
{
	// ネズミからプレイヤーへの方向を算出
	rat.GetDirectionToPlayer();

	// モデルがプレイヤーに向くようにする
	rat.GetAngleToPlayer();

	// 指定された移動速度を設定
	rat.info_.moveSpeed_ = ATTACK_MOVE_SPEED;

	// 指定されたジャンプ力を設定
	rat.info_.velocityY_ = JUMP_POW;

	// 攻撃アニメーション再生
	rat.animation_->Play(static_cast<int>(RAT_ANIMATION::ATTACK), false);

	// カプセルの種類を攻撃状態へ
	rat.capsuleCollder_->SetCollisionTag(CollisionTag::ENEMY_ATTACK);
}

void Rat::InitAttackAfter(Rat& rat)
{
	// モデルがプレイヤーに向くようにする
	rat.GetAngleToPlayer();

	// 攻撃後アニメーション再生
	rat.animation_->Play(static_cast<int>(RAT_ANIMATION::MOVE_CLOSER), false);

	// カプセルの種類を敵に戻す
	rat.capsuleCollder_->SetCollisionTag(CollisionTag::ENEMY);
}

void Rat::InitAttackEnd(Rat& rat)
{
}

void Rat::UpdateAttackStart(Rat& rat)
{
	// モデルがプレイヤーに向くようにする
	rat.GetAngleToPlayer();

	if (rat.animation_->IsEnd())
	{
		// 攻撃中へ
		rat.ChangeAttackState(RAT_ATTACKING);
	}
}

void Rat::UpdateAttacking(Rat& rat)
{
	// 移動処理
	rat.Move();

	// アニメーションが終わっているかつ、床についたら
	if (rat.animation_->IsEnd()
		&& rat.transform_->pos_.y <= 0.0f)
	{
		// 攻撃後の隙状態へ
		rat.ChangeAttackState(RAT_ATTACK_AFTER);
	}
}

void Rat::UpdateAttackAfter(Rat& rat)
{
	if (rat.animation_->IsEnd())
	{
		// 攻撃終了へ
		rat.ChangeAttackState(RAT_ATTACK_END);
	}
}

void Rat::UpdateAttackEnd(Rat& rat)
{
	// 後退中へ
	rat.ChangeState(RAT_STATE_RETREAT);
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
		// だんだん移動量を少なくする
		rat.info_.moveSpeed_ =
			Math::SubtractClampZero(rat.info_.moveSpeed_, HIT_REACT_FRICTION);

		// 指定された方向と移動速度を使用し座標に反映
		rat.Move();
	}
	else
	{
		// 地面に足が付いていなかったら処理を行わない
		if (rat.transform_->pos_.y > 0.0f)return;

		// 吹き飛びリアクション終了へ
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

void Rat::AddComponets(void)
{
	// 座標の設定
	transform_ = owner_->AddComponent<Transform>();
	transform_->angle_ = {};
	transform_->scale_ = { 2.0f,2.0f,2.0f };

	// 当たり判定の設定(カプセル)
	capsuleCollder_ = owner_->AddComponent<CapsuleCollider>();
	capsuleCollder_->Init();

	// カプセルの設定
	capsuleCollder_->AddCapsule(
		VGet(0.0f, 150.0f, 0.0f),
		VGet(0.0f, 30.0f, 0.0f),
		40.0f);

	// タグの設定
	capsuleCollder_->SetCollisionTag(CollisionTag::ENEMY);
	capsuleCollder_->SetInverseMass(1.0f);

	// 3Dモデル設定
	render3D_ = owner_->AddComponent<Render3D>();
	render3D_->Init();
	render3D_->SetModel("Data/Model/Enemy/Enemy.mv1");

	// アニメーション設定
	animation_ = owner_->AddComponent<Animation>();
	animation_->Init();
	// アニメーション登録
	animation_->AddInFbx(
		static_cast<int>(RAT_ANIMATION::ATTACK),
		ANIM_SPEED_ATTACK,
		static_cast<int>(RAT_ANIMATION::ATTACK));
	animation_->AddInFbx(
		static_cast<int>(RAT_ANIMATION::ATTACK_START),
		ANIM_SPEED_ATTACK_START,
		static_cast<int>(RAT_ANIMATION::ATTACK_START));
	animation_->AddInFbx(
		static_cast<int>(RAT_ANIMATION::DEAD),
		ANIM_SPEED_DEAD,
		static_cast<int>(RAT_ANIMATION::DEAD));
	animation_->AddInFbx(
		static_cast<int>(RAT_ANIMATION::IDLE),
		ANIM_SPEED_IDLE,
		static_cast<int>(RAT_ANIMATION::IDLE));
	animation_->AddInFbx(
		static_cast<int>(RAT_ANIMATION::MOVE),
		ANIM_SPEED_MOVE,
		static_cast<int>(RAT_ANIMATION::MOVE));
	animation_->AddInFbx(
		static_cast<int>(RAT_ANIMATION::MOVE_CLOSER),
		ANIM_SPEED_MOVE_CLOSER,
		static_cast<int>(RAT_ANIMATION::MOVE_CLOSER));
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

void Rat::UpdateMovementState(void)
{
	// 横揺れ時は距離によって動くのをやめる
	if (stateCtrl_.state_ == RAT_STATE_IDLE && stateCtrl_.idleState_ == RAT_IDLE_SWAY)return;

	// プレイヤーとの距離を調べる
	float distance = GetDistanceToPlayer();

	// プレイヤーが遠かったら
	if (distance > MAX_DISTANCE)
	{
		// 近づき処理へ
		ChangeState(RAT_STATE_MOVE);
		return;
	}

	// 待機中のちょっと近づく処理の場合処理を行わない
	if (stateCtrl_.state_ == RAT_STATE_IDLE && stateCtrl_.idleState_ == RAT_IDLE_APPROACH)return;

	// プレイヤーが近かったら
	if (distance < MIN_DISTANCE)
	{
		// 後退中へ
		ChangeState(RAT_STATE_RETREAT);
		return;
	}
}

void Rat::GetDirectionToPlayer(bool isRetreat)
{
	// 相手へのベクトルを計算
	VECTOR vec = VSub(playerTransform_->pos_, transform_->pos_);

	// ベクトルの正規化で単位ベクトル(方向)を取得する
	float length = sqrtf(vec.x * vec.x + vec.z * vec.z);

	if (length < 0.01f)
	{
		// プレイヤーと位置がほぼ同じだった場合、
		// 無理やり移動するように向きの情報を入れる
		info_.moveDir_.x = 1.0f;
		info_.moveDir_.z = 0.0f;
		return;
	}

	// XZ面の方向を算出
	info_.moveDir_.x = vec.x / length;
	info_.moveDir_.z = vec.z / length;

	// 後退方向の指示があれば
	if (isRetreat)
	{
		// 反転させる
		info_.moveDir_.x *= -1.0f;
		info_.moveDir_.z *= -1.0f;
	}
}

void Rat::GetDirectionLR(bool isRight)
{
	VECTOR dir = Math::VECTOR_ZERO;

	if (isRight)
	{
		// 右成分
		dir = VAdd(dir, { 1.0f, 0.0f, 0.0f });
	}
	else
	{
		// 左成分
		dir = VAdd(dir, { -1.0f, 0.0f, 0.0f });
	}

	// 中間の距離より距離がひらいていたら
	if (GetDistanceToPlayer() >= MID_DISTANCE)
	{
		// 少し前に詰める
		dir = VAdd(dir, { 0.0f, 0.0f, FORWARD_OFFSET });
	}

	if (!Math::EqualsVZero(dir))
	{
		// 正規化
		dir = VNorm(dir);

		MATRIX mat = MGetIdent();

		// カメラのY軸角度回転行列を出す
		mat = MMult(mat, MGetRotY(transform_->angle_.y));

		// 回転行列を使用して、ベクトルを回転させる
		info_.moveDir_ = VTransform(dir, mat);
	}
}

void Rat::GetAngleToPlayer(void)
{
	// 相手へのベクトルを計算
	VECTOR vec = VSub(playerTransform_->pos_, transform_->pos_);

	// あまりにも近ければ処理をおこなわない
	if (vec.x * vec.x + vec.z * vec.z < 0.0001f) return;

	// 目標の角度を算出
	float targetAngle = atan2f(vec.x, vec.z);

	// 今回のモデルのY軸向きが逆なので向きを反転させる
	targetAngle += DX_PI_F;

	// 角度補間をかける
	Math::SmoothAngle(
		transform_->angle_.y,
		targetAngle,
		info_.velocityRot_,
		ANGLE_COEFFICIENT);

	transform_->angle_.x = transform_->angle_.z = 0.0f;

	// モデルに向きを設定
	MV1SetRotationXYZ(render3D_->GetHandle(), transform_->angle_);
}

float Rat::GetDistanceToPlayer(void)
{
	// プレイヤーとの距離を算出
	VECTOR vec = VSub(playerTransform_->pos_, transform_->pos_);
	return VSize(vec);
}

void Rat::InitSwayRandom(void)
{
	info_.moveSpeed_ = static_cast<float>(GetRand(static_cast<int>(MOVE_SPEED)));
	// 移動速度は最低値をつけておく
	if (info_.moveSpeed_ < MIN_MOVE_SPEED)
	{
		info_.moveSpeed_ = MIN_MOVE_SPEED;
	}

	int random = GetRand(RANDOM_RIMIT);

	// 揺れ回数
	info_.swayNum_ = 3;

	// 揺れ方向
	info_.isRightSway_ = random % 2;
}

void Rat::SetApproachDistance(void)
{
	// プレイヤーと距離を算出
	float distance = GetDistanceToPlayer();

	// 近づく距離算出
	int random = GetRand(static_cast<int>(distance / 5));
	random += static_cast<int>(distance / 5);
	distance -= random;

	// 決定した距離を設定
	info_.approachDistance_ = distance;
}

void Rat::SetDamage(void)
{
	// HPにダメージを与える
	--info_.hp_;

	// HPが0以下になったら
	if (info_.hp_ <= 0)
	{
		// 0初期化しておく
		info_.hp_ = 0;

		ChangeState(RAT_STATE_DEAD);
	}
	else
	{
		// 無敵時間を設ける
		info_.invincibleTime_ = INVINCIBLE_TIME;
	}
}

void Rat::SetHitReact(VECTOR moveDir)
{
	// 死亡していたら吹き飛び処理を行わない
	if (stateCtrl_.state_ == RAT_STATE_DEAD)return;

	// 指定された移動向きを設定
	info_.moveDir_ = moveDir;

	// 指定された移動速度を設定
	info_.moveSpeed_ = 7.0f;

	// 指定されたジャンプ力を設定
	info_.velocityY_ = 10.0f;

	// ダメージを受けたステートへ
	ChangeState(RAT_STATE_HIT);
}

void Rat::OnCollision(Collider* other)
{
	// 衝突したオブジェクトをタグごとに判別
	switch (other->GetCollisionTag())
	{
	case CollisionTag::PLAYER:
		// プレイヤー本体と接触した
		if (stateCtrl_.attackState_ == RAT_ATTACKING)
		{
			// 攻撃後の隙状態へ
			ChangeAttackState(RAT_ATTACK_AFTER);
		}
		break;

	case CollisionTag::PLAYER_ATTACK:
		// プレイヤーの攻撃を受けた
		// 死亡処理に入っていたら処理を行わない
		if (stateCtrl_.state_ == RAT_STATE_DEAD ||
			stateCtrl_.state_ == RAT_STATE_END)return;

		// 無敵時間があればダメージを与えない
		if (info_.invincibleTime_ > 0)return;

		// ダメージ処理
		SetDamage();

		// 吹っ飛びリアクションをさせる
		SetHitReact(other->GetCollisionInfo().normal);
		break;

	default:
		break;
	}
}
