#include "../Object.h"
#include "../../Math/Math.h"
#include "../../Input/InputManager.h"
#include "../../Scene/SceneManager.h"
#include "../Camera/CameraUtility.h"

#include "Player.h"

namespace
{
	// リミット設定
	constexpr float DAMAGE_POS_Y = -2000.0f;	// プレイヤーがダメージを受ける座標
	constexpr int INVINCIBLE_TIME = 120;		// 無敵時間

	// 移動設定
	constexpr float MOVE_SPEED = 10.0f;			// 移動速度
	constexpr float AVOID_DISTANCE = 500.0f;	// 回避距離
	constexpr float COEFFICIENT = 0.2f;			// 線形補間の係数
	constexpr float AVOID_MIN_DISTANCE = 30.0f;	// 回避時の打ち切り距離

	// 体力設定
	constexpr int DEFAULT_HP = 5;	// 初期HP

	// 重力設定
	constexpr float JUMP_POW = 25.0f;	// ジャンプ力
	constexpr float GRAVITY = -1.98f;	// 重力加速度
	constexpr float MAX_FALL = -15.0f;	// 最大落下速度

	// ダメージ設定
	constexpr float HIT_REACT_FRICTION = 0.5f;	// ダメージ時のリアクション時の摩擦
	constexpr int SHAKE_TIME = 20;				// 揺らす時間

	// 描画設定
	constexpr int  STATUS_DRAW_POS_X = 10;		// ステータス描画を始める座標
	constexpr int  HP_DRAW_POS_Y = 50;			// HPの描画Y軸
	constexpr int STATUS_DRAW_POS_OFFSET = 10;	// オフセット
}

Player::Player(void)
{
	// テーブルに関数のポインタを格納
	
	// プレイヤー状態
	// 初期化関数
	stateCtrl_.initStateTable_[PLAYER_STATE_IDLE] = InitIdle;
	stateCtrl_.initStateTable_[PLAYER_STATE_MOVE] = InitMove;
	stateCtrl_.initStateTable_[PLAYER_STATE_ATTACK] = InitAttack;
	stateCtrl_.initStateTable_[PLAYER_STATE_AVOID] = InitAvoid;
	stateCtrl_.initStateTable_[PLAYER_STATE_JUMP] = InitJump;
	stateCtrl_.initStateTable_[PLAYER_STATE_HIT] = InitHit;
	stateCtrl_.initStateTable_[PLAYER_STATE_DEAD] = InitDead;
	stateCtrl_.initStateTable_[PLAYER_STATE_END] = InitEnd;
	// 更新関数
	stateCtrl_.updateStateTable_[PLAYER_STATE_IDLE] = UpdateIdle;
	stateCtrl_.updateStateTable_[PLAYER_STATE_MOVE] = UpdateMove;
	stateCtrl_.updateStateTable_[PLAYER_STATE_ATTACK] = UpdateAttack;
	stateCtrl_.updateStateTable_[PLAYER_STATE_AVOID] = UpdateAvoid;
	stateCtrl_.updateStateTable_[PLAYER_STATE_JUMP] = UpdateJump;
	stateCtrl_.updateStateTable_[PLAYER_STATE_HIT] = UpdateHit;
	stateCtrl_.updateStateTable_[PLAYER_STATE_DEAD] = UpdateDead;
	stateCtrl_.updateStateTable_[PLAYER_STATE_END] = UpdateEnd;

	// プレイヤー攻撃状態
	// 初期化関数
	stateCtrl_.initAttackTable_[PLAYER_ATTACK_1] = InitAttack1;
	stateCtrl_.initAttackTable_[PLAYER_ATTACK_2] = InitAttack2;
	stateCtrl_.initAttackTable_[PLAYER_ATTACK_3] = InitAttack3;
	// 更新関数
	stateCtrl_.updateAttackTable_[PLAYER_ATTACK_1] = UpdateAttack1;
	stateCtrl_.updateAttackTable_[PLAYER_ATTACK_2] = UpdateAttack2;
	stateCtrl_.updateAttackTable_[PLAYER_ATTACK_3] = UpdateAttack3;

	// プレイヤージャンプ状態
	// 初期化関数
	stateCtrl_.initJumpTable_[PLAYER_JUMP_START] = InitJumpStart;
	stateCtrl_.initJumpTable_[PLAYER_JUMPING] = InitJumping;
	stateCtrl_.initJumpTable_[PLAYER_JUMP_END] = InitJumpEnd;
	// 更新関数
	stateCtrl_.updateJumpTable_[PLAYER_JUMP_START] = UpdateJumpStart;
	stateCtrl_.updateJumpTable_[PLAYER_JUMPING] = UpdateJumping;
	stateCtrl_.updateJumpTable_[PLAYER_JUMP_END] = UpdateJumpEnd;

	// プレイヤー被ダメージ状態
	// 初期化関数
	stateCtrl_.initHitTable_[PLAYER_HIT_START] = InitHitStart;
	stateCtrl_.initHitTable_[PLAYER_HIT_STUN] = InitHitStun;
	stateCtrl_.initHitTable_[PLAYER_HIT_END] = InitHitEnd;
	// 更新関数
	stateCtrl_.updateHitTable_[PLAYER_HIT_START] = UpdateHitStart;
	stateCtrl_.updateHitTable_[PLAYER_HIT_STUN] = UpdateHitStun;
	stateCtrl_.updateHitTable_[PLAYER_HIT_END] = UpdateHitEnd;
}

void Player::Load(void)
{
}

// 初期化
void Player::Init(void)
{
	// 座標の設定
	transform_ = owner_->AddComponent<Transform>();
	transform_->pos_ = { 0.0f,0.0f,0.0f };
	transform_->angle_ = { 0.0f,0.0f,0.0f };

	// 移動速度の初期化
	info_.moveSpeed_ = MOVE_SPEED;

	// HPの初期化
	info_.hp_ = DEFAULT_HP;

	// プレイヤーの状態初期化
	ChangeState(PLAYER_STATE_IDLE);
}

// 更新
void Player::Update(void)
{
	// 終了していたら処理を行わない
	if (stateCtrl_.state_ == PLAYER_STATE_END)
	{
		return;
	}

	// 重力処理
	ApplyGravity();

	// 移動処理
	UpdateState();

	// 無敵時間を更新
	UpdateInvincible();

	// ヒットストップ更新
	UpdateHitStop();

	// 死亡座標へ到達しているか
	IsReachedDeadPos();
}

void Player::Draw2D(void)
{
	// HP描画
	DrawHP();

#ifdef _DEBUG
	// デバッグ表示
	DebugDraw();
#endif // _DEBUG
}

void Player::Draw3D(void)
{
	// プレイヤー描画
	DrawCapsule3D(GetPlayerHeadPos(), transform_->pos_, 30.0f, 10, 0x0000ff, 0x0000ff, true);
}

void Player::SetDamage(int damage)
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
		ChangeState(PLAYER_STATE_DEAD);
	}
	else
	{
		// 無敵時間を設ける
		info_.invincibleTime_ = INVINCIBLE_TIME;

		// HP描画を揺らす
		info_.hitStopCounter_ = SHAKE_TIME;

		// ダメージを受けたステートへ
		ChangeState(PLAYER_STATE_HIT);
	}
}

void Player::SetHitReact(VECTOR moveDir, float moveSpeed, float jumpPow)
{
	// 指定された移動向きを設定
	info_.moveDir_ = moveDir;

	// 指定された移動速度を設定
	info_.moveSpeed_ = moveSpeed;

	// 指定されたジャンプ力を設定
	info_.velocityY_ = jumpPow;

	// ダメージを受けたステートへ
	ChangeState(PLAYER_STATE_HIT);
}

void Player::UpdateState(void)
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

void Player::InitIdle(Player& player)
{
	// 移動速度を初期化
	player.info_.moveSpeed_ = 0.0f;
}

void Player::InitMove(Player& player)
{
	// プレイヤーの移動速度を設定
	player.info_.moveSpeed_ = MOVE_SPEED;
}

void Player::InitAttack(Player& player)
{
	// 初期化
	player.ChangeAttackState(PLAYER_ATTACK_1);
}

void Player::InitHit(Player& player)
{
	// 最初の状態へ初期化
	player.ChangeHitState(PLAYER_HIT_START);
}

void Player::InitAvoid(Player& player)
{
	// 移動していなかったら
	if (!player.InputMove())
	{
		// カメラが向いている方向を設定
		MATRIX mat = MGetRotY(CameraUtility::GetCameraAngle().y);

		// 行列から回転成分を取る
		player.info_.moveDir_.x = mat.m[2][0];
		player.info_.moveDir_.y = mat.m[2][1];
		player.info_.moveDir_.z = mat.m[2][2];
	}

	// 回避先の座標を算出
	player.info_.avoidPos_ =
		VAdd(player.transform_->pos_,
			VScale(player.info_.moveDir_, AVOID_DISTANCE));

	// 移動速度を初期化
	player.info_.moveSpeed_ = 0.0f;
}

void Player::InitJump(Player& player)
{
	player.ChangeJumpState(PLAYER_JUMP_START);

	// 移動速度を初期化
	player.info_.moveSpeed_ = 0.0f;
}

void Player::InitDead(Player& player)
{
}

void Player::InitEnd(Player& player)
{
	// ゲームオーバーフラグを立てる
	SceneManager::GetInstance()->TrueGameOver();
}

void Player::UpdateIdle(Player& player)
{
	// 攻撃ボタンが押されていたら
	if (InputManager::GetInstance()->AttackButton())
	{
		// 攻撃状態へ
 		player.ChangeState(PLAYER_STATE_ATTACK);
		return;
	}

	// 回避ボタンが押されていたら
	if (InputManager::GetInstance()->AvoidButton())
	{
		// 回避状態へ
		player.ChangeState(PLAYER_STATE_AVOID);
		return;
	}

	// ジャンプボタンを押されたかつ、ジャンプ中ではなかったら
	if (InputManager::GetInstance()->JumpButton())
	{
		// ジャンプ状態へ
		player.ChangeState(PLAYER_STATE_JUMP);
		return;
	}

	// 移動していたら
	if (player.InputMove())
	{
		// 移動状態へ
		player.ChangeState(PLAYER_STATE_MOVE);
	}
}

void Player::UpdateMove(Player& player)
{
	// 移動していたら
	if (player.InputMove())
	{
		// 指定された方向と移動速度を使用し座標に反映
		player.Move();
	}
	else
	{
		// 待機状態へ
		player.ChangeState(PLAYER_STATE_IDLE);
	}

	// 攻撃ボタンが押されていたら
	if (InputManager::GetInstance()->AttackButton())
	{
		// 攻撃状態へ
		player.ChangeState(PLAYER_STATE_ATTACK);
		return;
	}

	// 回避ボタンが押されていたら
	if (InputManager::GetInstance()->AvoidButton())
	{
		// 回避状態へ
		player.ChangeState(PLAYER_STATE_AVOID);
		return;
	}

	// ジャンプボタンを押されたかつ、ジャンプ中ではなかったら
	if (InputManager::GetInstance()->JumpButton())
	{
		// ジャンプ状態へ
		player.ChangeState(PLAYER_STATE_JUMP);
		return;
	}
}

void Player::UpdateAttack(Player& player)
{
	// nullチェック
	if (player.stateCtrl_.updateAttackTable_[player.stateCtrl_.attackState_])
	{
		// 状態別初期化
		player.stateCtrl_.updateAttackTable_[player.stateCtrl_.attackState_](player);
	}

	// 回避ボタンが押されていたら
	if (InputManager::GetInstance()->AvoidButton())
	{
		// 回避状態へ
		player.ChangeState(PLAYER_STATE_AVOID);
		return;
	}

	// ジャンプボタンを押されたかつ、ジャンプ中ではなかったら
	if (InputManager::GetInstance()->JumpButton())
	{
		// ジャンプ状態へ
		player.ChangeState(PLAYER_STATE_JUMP);
		return;
	}
}

void Player::UpdateAvoid(Player& player)
{
	// 回避先の座標へ近づける
	player.transform_->pos_ = Math::Lerp(player.transform_->pos_, player.info_.avoidPos_, COEFFICIENT);

	// 現在地から回避先までの距離を算出
	VECTOR vec = VSub(player.info_.avoidPos_, player.transform_->pos_);
	vec.y = 0.0f;

	float dis = VSize(vec);

	// 回避先の座標にほぼ近ければ
	if (dis < AVOID_MIN_DISTANCE)
	{
		// 待機状態へ
		player.ChangeState(PLAYER_STATE_IDLE);
	}
}

void Player::UpdateJump(Player& player)
{
	// 移動していたら
	if (player.InputMove())
	{
		// プレイヤーの移動速度を設定
		player.info_.moveSpeed_ = MOVE_SPEED;
		// 指定された方向と移動速度を使用し座標に反映
		player.Move();
		player.info_.moveSpeed_ = 0.0f;
	}

	// 攻撃ボタンが押されていたら
	if (InputManager::GetInstance()->AttackButton())
	{
		// 攻撃状態へ
		player.ChangeState(PLAYER_STATE_ATTACK);
		return;
	}

	// 回避ボタンが押されていたら
	if (InputManager::GetInstance()->AvoidButton())
	{
		// 回避状態へ
		player.ChangeState(PLAYER_STATE_AVOID);
		return;
	}

	// nullチェック
	if (player.stateCtrl_.updateJumpTable_[player.stateCtrl_.jumpState_])
	{
		// 状態別初期化
		player.stateCtrl_.updateJumpTable_[player.stateCtrl_.jumpState_](player);
	}
}

void Player::UpdateHit(Player& player)
{
	// nullチェック
	if (player.stateCtrl_.updateHitTable_[player.stateCtrl_.hitState_])
	{
		// 状態別初期化
		player.stateCtrl_.updateHitTable_[player.stateCtrl_.hitState_](player);
	}
}

void Player::UpdateDead(Player& player)
{
	player.ChangeState(PLAYER_STATE_END);
}

void Player::UpdateEnd(Player& player)
{
}
void Player::ChangeState(PLAYER_STATE state)
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

void Player::InitAttack1(Player& player)
{
}

void Player::InitAttack2(Player& player)
{
}

void Player::InitAttack3(Player& player)
{
}

void Player::UpdateAttack1(Player& player)
{
	// 待機状態へ
	player.ChangeState(PLAYER_STATE_IDLE);
}

void Player::UpdateAttack2(Player& player)
{
	// 待機状態へ
	player.ChangeState(PLAYER_STATE_IDLE);
}

void Player::UpdateAttack3(Player& player)
{
	// 待機状態へ
	player.ChangeState(PLAYER_STATE_IDLE);
}

void Player::ChangeAttackState(PLAYER_ATTACK_STATE state)
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

void Player::InitJumpStart(Player& player)
{
	// ジャンプ力を設定
	player.info_.velocityY_ = JUMP_POW;
}

void Player::InitJumping(Player& player)
{
}

void Player::InitJumpEnd(Player& player)
{
}

void Player::UpdateJumpStart(Player& player)
{
	player.ChangeJumpState(PLAYER_JUMPING);
}

void Player::UpdateJumping(Player& player)
{
	// 床についているため
	if (player.transform_->pos_.y <= 0.0f)
	{
		// ジャンプ終了状態へ
 		player.ChangeJumpState(PLAYER_JUMP_END);
	}
}

void Player::UpdateJumpEnd(Player& player)
{
	// 待機状態へ
	player.ChangeState(PLAYER_STATE_IDLE);
}

void Player::ChangeJumpState(PLAYER_JUMP_STATE state)
{
	// 指定されたステートへ変更
	stateCtrl_.jumpState_ = state;

	// nullチェック
	if (stateCtrl_.initJumpTable_[stateCtrl_.jumpState_])
	{
		// 状態別初期化
		stateCtrl_.initJumpTable_[stateCtrl_.jumpState_](*this);
	}
}

void Player::InitHitStart(Player& player)
{
}

void Player::InitHitStun(Player& player)
{
}

void Player::InitHitEnd(Player& player)
{

}

void Player::UpdateHitStart(Player& player)
{
	player.ChangeHitState(PLAYER_HIT_STUN);
}

void Player::UpdateHitStun(Player& player)
{
	// 移動速度が0より大きく移動している場合
	if (player.info_.moveSpeed_ > 0.0f)
	{
		// 移動速度を減算
		player.info_.moveSpeed_ -= HIT_REACT_FRICTION;

		// 指定された方向と移動速度を使用し座標に反映
		player.Move();
	}
	else
	{
		player.ChangeHitState(PLAYER_HIT_END);
	}
}

void Player::UpdateHitEnd(Player& player)
{
	// 待機状態へ
	player.ChangeState(PLAYER_STATE_IDLE);
}

void Player::ChangeHitState(PLAYER_HIT_STATE state)
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

void Player::ApplyGravity(void)
{
	// 回避中は重力処理を行わない
	if (stateCtrl_.state_ == PLAYER_STATE_AVOID)return;

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

void Player::UpdateInvincible(void)
{
	// 無敵時間を減らす
	if (info_.invincibleTime_ > 0)
	{
		--info_.invincibleTime_;
	}
}

void Player::UpdateHitStop(void)
{
	// ヒットストップ更新処理
	if (info_.hitStopCounter_ > 0) {
		info_.hitStopCounter_--;
	}
}

void Player::GetShakeOffset(int& offset)
{
	if (info_.hitStopCounter_ > 0) {
		// 振動先をカウンターから計算する----------
		// 0 or 1
		offset = (info_.hitStopCounter_ / 5) % 2;
		// 0 or 2　中心を作る
		offset *= 2;
		// -1 or 1　0を中心にする
		offset -= 1;
		// -3 or 3　振れ幅を付ける
		offset *= 5;
		// ----------------------------------------
	}
}

bool Player::InputMove(void)
{
	// 移動量
	VECTOR dir = Math::VECTOR_ZERO;

	// WASDで移動する
	if (InputManager::GetInstance()->MoveBeforeButton()) { dir = VAdd(dir, { 0.0f, 0.0f, 1.0f }); }
	if (InputManager::GetInstance()->MoveBackButton()) { dir = VAdd(dir, { 0.0f, 0.0f, -1.0f });  }
	if (InputManager::GetInstance()->MoveLeftButton()) { dir = VAdd(dir, { -1.0f, 0.0f, 0.0f }); }
	if (InputManager::GetInstance()->MoveRightButton()) { dir = VAdd(dir, { 1.0f, 0.0f, 0.0f }); }

	if (!Math::EqualsVZero(dir))
	{
		// 正規化
		dir = VNorm(dir);

		// XYZの回転行列
		// XZ平面移動にする場合は、XZの回転を考慮しないようにする
		MATRIX mat = MGetIdent();

		// カメラのY軸角度回転行列を出す
		mat = MMult(mat, MGetRotY(CameraUtility::GetCameraAngle().y));

		// 回転行列を使用して、ベクトルを回転させる
		info_.moveDir_ = VTransform(dir, mat);

		// 移動している
		return true;
	}

	// 移動していない
	return false;
}

void Player::Move(void)
{
	// 移動速度が0以下であれば処理を行わない
	if (info_.moveSpeed_ <= 0.0f)return;

	// 方向×スピードで移動量を作って、座標に足して移動
	transform_->pos_ =
		VAdd(transform_->pos_,
			VScale(info_.moveDir_, info_.moveSpeed_));
}

void Player::IsReachedDeadPos(void)
{
	// 一定の座標いったら
	if (transform_->pos_.y < DAMAGE_POS_Y)
	{
		info_.hp_--;
		return;
	}
}

void Player::DrawHP(void)
{
	// プレイヤーのhpの表示 緑
	DrawFormatString(
			STATUS_DRAW_POS_X,
			HP_DRAW_POS_Y - STATUS_DRAW_POS_OFFSET,
			0x00fa9a,
			"%d",
			info_.hp_);
}

void Player::DebugDraw(void)
{
	// プレイヤー座標表示
	DrawFormatString(20,
		300,
		0xff0000,
		"プレイヤー座標 : %.f,%.f,%.f",
		transform_->pos_.x, transform_->pos_.y, transform_->pos_.z);
}