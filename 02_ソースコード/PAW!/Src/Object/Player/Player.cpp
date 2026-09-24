#include "../Object.h"

#include "../../Math/Math.h"
#include "../Component/Transform/Transform.h"
#include "../../Input/InputManager.h"
#include "../../Scene/SceneManager.h"

#include "Player.h"

Player::Player(void)
{
	// テーブルに関数のポインタを格納
	// 初期化関数
	stateCtrl_.initTable_[PLAYER_STATE_IDLE] = IdleInit;
	stateCtrl_.initTable_[PLAYER_STATE_MOVE] = MoveInit;
	stateCtrl_.initTable_[PLAYER_STATE_ATTACK] = AttackInit;
	stateCtrl_.initTable_[PLAYER_STATE_DODGE] = DodgeInit;
	stateCtrl_.initTable_[PLAYER_STATE_HIT] = HitInit;
	stateCtrl_.initTable_[PLAYER_STATE_DEAD] = DeadInit;
	// 更新関数
	stateCtrl_.updateTable_[PLAYER_STATE_IDLE] = IdleUpdate;
	stateCtrl_.updateTable_[PLAYER_STATE_MOVE] = MoveUpdate;
	stateCtrl_.updateTable_[PLAYER_STATE_ATTACK] = AttackUpdate;
	stateCtrl_.updateTable_[PLAYER_STATE_DODGE] = DodgeUpdate;
	stateCtrl_.updateTable_[PLAYER_STATE_HIT] = HitUpdate;
	stateCtrl_.updateTable_[PLAYER_STATE_DEAD] = DeadUpdate;
}

void Player::Load()
{
}

// 初期化
void Player::Init()
{
	// オーナーからTransform取得
	transform_ = owner_->GetComponent<Transform>();

	// 移動速度の初期化
	info_.moveSpeed_ = MOVE_SPEED;

	// HPの初期化
	info_.hp_ = DEFAULT_HP;

	// プレイヤーの状態初期化
	ChangeState(PLAYER_STATE_IDLE);
}

// 更新
void Player::Update()
{
	// 終了していたら処理を行わない
	if (stateCtrl_.state_ == PLAYER_STATE::PLAYER_STATE_END)
	{
		return;
	}

	// 移動処理
	StateUpdate();

	// ジャンプ処理
	Jump();

	// 重力処理
	ApplyGravity();

	// 無敵時間を更新
	InvincibleUodate();

	// ヒットストップ更新
	HitStopUodate();

	// 死亡座標へ到達しているか
	IsReachedDeadPos();
}

void Player::Draw2D()
{
	// HP描画
	DrawHP();

#ifdef _DEBUG

	// デバッグ表示
	DebugDraw();

#endif // _DEBUG

}

Transform* Player::GetTransform()
{
	return transform_;
}

PLAYER_STATE Player::GetState(void)
{
	return stateCtrl_.state_;
}

int Player::GetInvincibleTime(void)
{
	return info_.invincibleTime_;
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

void Player::ChangeState(PLAYER_STATE state)
{
	// 指定されたステートへ変更
	stateCtrl_.state_ = state;

	// nullチェック
	if (stateCtrl_.initTable_[stateCtrl_.state_])
	{
		// 状態別初期化
		stateCtrl_.initTable_[stateCtrl_.state_](*this);
	}
}

void Player::StateUpdate(void)
{
	// Transformがなければ処理しない
	if (!transform_) return;

	// nullチェック
	if (stateCtrl_.updateTable_[stateCtrl_.state_])
	{
		// 状態別更新
		stateCtrl_.updateTable_[stateCtrl_.state_](*this);
	}
}

void Player::IdleInit(Player& player)
{
	// 移動速度を初期化
	player.info_.moveSpeed_ = 0.0f;
}

void Player::MoveInit(Player& player)
{
	// プレイヤーの移動速度を普通の移動速度にする
	player.info_.moveSpeed_ = MOVE_SPEED;
}

void Player::AttackInit(Player& player)
{
}

void Player::HitInit(Player& player)
{
}

void Player::DodgeInit(Player& player)
{
}

void Player::DeadInit(Player& player)
{
}

void Player::EndInit(Player& player)
{
	// ゲームオーバーフラグを立てる
	SceneManager::GetInstance()->TrueGameOver();
}

void Player::IdleUpdate(Player& player)
{
	// 移動していたら
	if (player.InputMove())
	{
		// 移動状態へ
		player.ChangeState(PLAYER_STATE_MOVE);
	}

	// 回避ボタンが押されていたら
	if (InputManager::GetInstance()->DodgeButton())
	{
		// 回避状態へ
		player.ChangeState(PLAYER_STATE_DODGE);
	}

	// 攻撃ボタンが押されていたら
	if (InputManager::GetInstance()->AttackButton())
	{
		// 攻撃状態へ
		player.ChangeState(PLAYER_STATE_ATTACK);
	}
}

void Player::MoveUpdate(Player& player)
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
	}
}

void Player::AttackUpdate(Player& player)
{
	// 待機状態へ
	player.ChangeState(PLAYER_STATE_IDLE);
}

void Player::DodgeUpdate(Player& player)
{
	// 待機状態へ
	player.ChangeState(PLAYER_STATE_IDLE);
}

void Player::HitUpdate(Player& player)
{
	// 移動速度が0より大きく移動している場合
	if (player.info_.moveSpeed_ > 0.0f)
	{
		// 移動速度を減算
		player.info_.moveSpeed_ -= HIT_REACT_FRICTION;

		// 指定された方向と移動速度を使用し座標に反映
		player.Move();
	}
}

void Player::DeadUpdate(Player& player)
{
	player.ChangeState(PLAYER_STATE_END);
}

void Player::EndUpdate(Player& player)
{
}

void Player::ApplyGravity()
{
	// ステージコライダが無ければ処理を行わない
	//if (!stageColl_) return;

	// Y座標へ反映
	transform_->pos_.y += info_.velocityY_;

	// 接地判定
	
	// 空中
	//if (!stageColl_->IsGround())
	if (transform_->pos_.y > 0)
	{
		// 重力加算
		info_.velocityY_ += GRAVITY;
		
		// 最大落下速度
		if (info_.velocityY_ < MAX_FALL)
			info_.velocityY_ = MAX_FALL;
	}
	else
	{
		transform_->pos_.y = 0;

		// 地面上なら少し下方向に押す
		// 0だと浮く場合があるため
		info_.velocityY_ = -0.1f;

		// ジャンプ中フラグを折る
		info_.jumpFlg = false;
	}
}

void Player::Jump(void)
{
	// ダメージ時は処理を行わない
	if (stateCtrl_.state_ == PLAYER_STATE_HIT)return;

	// ジャンプボタンを押されたかつ、ジャンプ中ではなかったら
	if (InputManager::GetInstance()->JumpButton()
		&& !info_.jumpFlg)
	{
		// ジャンプ力を設定
		info_.velocityY_ = JUMP_POW;

		// ジャンプ中にする
		info_.jumpFlg = true;
	}
}

void Player::InvincibleUodate(void)
{
	// 無敵時間を減らす
	if (info_.invincibleTime_ > 0)
	{
		--info_.invincibleTime_;
	}
}

void Player::HitStopUodate(void)
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
	if (InputManager::GetInstance()->MoveBackButton()) { dir = VAdd(dir, { -1.0f, 0.0f, 0.0f }); }
	if (InputManager::GetInstance()->MoveLeftButton()) { dir = VAdd(dir, { 0.0f, 0.0f, -1.0f }); }
	if (InputManager::GetInstance()->MoveRightButton()) { dir = VAdd(dir, { 1.0f, 0.0f, 0.0f }); }

	if (!Math::EqualsVZero(dir))
	{
		// 正規化
		dir = VNorm(dir);

		// XYZの回転行列
		// XZ平面移動にする場合は、XZの回転を考慮しないようにする
		MATRIX mat = MGetIdent();

		// カメラのY軸角度回転行列を出す
		//mat = MMult(mat, MGetRotY(CameraUtility::GetCameraAngle().y));

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
	DrawFormatString(20,
		300,
		0xff0000,
		"プレイヤー座標 : %.f,%.f,%.f",
		transform_->pos_.x, transform_->pos_.y, transform_->pos_.z);
}