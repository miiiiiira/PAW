#include "../../Object.h"
#include "../../Component/Graphics/Render3D.h"
#include "../../Component/Animation/Animation.h"
#include "../../Camera/CameraUtility.h"
#include "../../../Math/Math.h"

#include "../../../Input/InputManager.h"

#include "Rat.h"

namespace
{
	// 移動設定
	constexpr float MOVE_SPEED = 5.0f;			// 移動速度
	constexpr float ATTACK_MOVE_SPEED = 12.0f;	// 攻撃時の移動速度
	constexpr float MIN_DISTANCE = 150.0f;		// プレイヤーとの距離をとる長さ
	constexpr float MAX_DISTANCE = 200.0f;		// プレイヤーとの距離をつめる長さ

	// 方向設定
	constexpr float ANGLE_COEFFICIENT = 0.2f;	// 補間の係数

	// 体力設定
	constexpr int DEFAULT_HP = 5;	// 初期HP

	// 重力設定
	constexpr float JUMP_POW = 17.0f;	// ジャンプ力
	constexpr float GRAVITY = -1.98f;	// 重力加速度
	constexpr float MAX_FALL = -10.0f;	// 最大落下速度

	// ダメージ設定
	constexpr float HIT_REACT_FRICTION = 0.5f;	// ダメージ時のリアクション時の摩擦

	// アニメーション設定
	constexpr float ANIM_SPEED = 0.8f;

	// リミット設定
	constexpr int INVINCIBLE_TIME = 120;		// 無敵時間
}

Rat::Rat(void)
{
	// プレイヤー状態
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
	// 座標の設定
	transform_ = owner_->AddComponent<Transform>();
	transform_->pos_ = { 0.0f,0.0f,200.0f };
	transform_->angle_ = {};
	transform_->scale_ = { 2.0f,2.0f,2.0f };

	// 3Dモデル設定
	render3D_ = owner_->AddComponent<Render3D>();
	render3D_->Init();
	render3D_->SetModel("Data/Model/Enemy/Enemy.mv1");

	// アニメーション設定
	animation_ = owner_->AddComponent<Animation>();
	animation_->Init();
	// アニメーション登録
	for (int i = 0; i < static_cast<int>(RAT_ANIMATION::MAX); i++)
	{
		animation_->AddInFbx(
			static_cast<int>(i),
			ANIM_SPEED,
			static_cast<int>(i));
	}

	// HPの初期化
	info_.hp_ = DEFAULT_HP;

	// ネズミの状態初期化
	ChangeState(RAT_STATE_IDLE);
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
		info_.invincibleTime_ = INVINCIBLE_TIME;

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
	// ネズミの移動速度を設定
	rat.info_.moveSpeed_ = 0.0f;

	// 待機アニメーション再生
	rat.animation_->Play(static_cast<int>(RAT_ANIMATION::IDLE), true);
}

void Rat::InitMove(Rat& rat)
{
	// ネズミの移動速度を設定
	rat.info_.moveSpeed_ = MOVE_SPEED;
	// 待機アニメーション再生
	rat.animation_->Play(static_cast<int>(RAT_ANIMATION::MOVE), true);
}

void Rat::InitRetreat(Rat& rat)
{
	// ネズミの移動速度を設定
	rat.info_.moveSpeed_ = MOVE_SPEED;
	// 待機アニメーション再生
	rat.animation_->Play(static_cast<int>(RAT_ANIMATION::MOVE), true);
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
	// ネズミからプレイヤーへの方向を算出
	rat.GetDirectionToPlayer();

	// モデルがプレイヤーに向くようにする
	rat.GetAngleToPlayer();

	float distance = rat.GetDistanceToPlayer();

	// プレイヤーと距離が取れたら
	if (distance < MIN_DISTANCE)
	{
		// 後退中へ
		rat.ChangeState(RAT_STATE_RETREAT);
	}
	else if (distance > MAX_DISTANCE)
	{
		// 移動中へ
		rat.ChangeState(RAT_STATE_MOVE);
	}
#ifdef _DEBUG
	else
	{
		if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_N))
		{
			// 攻撃中へ
			rat.ChangeState(RAT_STATE_ATTACK);
		}
	}
#endif // _DEBUG
}

void Rat::UpdateMove(Rat& rat)
{
	// モデルがプレイヤーに向くようにする
	rat.GetAngleToPlayer();

	// ネズミからプレイヤーへの方向を算出
	rat.GetDirectionToPlayer();

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
	// モデルがプレイヤーに向くようにする
	rat.GetAngleToPlayer();

	// プレイヤーから離れる方向を算出
	rat.GetDirectionToPlayer(true);

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
}

void Rat::InitAttackAfter(Rat& rat)
{
	// モデルがプレイヤーに向くようにする
	rat.GetAngleToPlayer();
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
	// 攻撃終了へ
	rat.ChangeAttackState(RAT_ATTACK_END);
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
