#include "../Object.h"
#include "../../Math/Math.h"
#include "../../Input/InputManager.h"
#include "../../Scene/SceneManager.h"
#include "../Camera/CameraUtility.h"
#include "../Component/Graphics/Render3D.h"
#include "../Component/Animation/Animation.h"
#include "../../Collision/3DCollider/CapsuleCollider.h"
#include "../../Collision/3DCollider/SphereCollider.h"

#include "Player.h"

namespace
{
	// リミット設定
	constexpr float DAMAGE_POS_Y = -2000.0f;	// プレイヤーがダメージを受ける座標
	constexpr int INVINCIBLE_TIME = 120;		// 無敵時間

	// 移動設定
	constexpr float MOVE_SPEED = 15.0f;			// 移動速度
	constexpr float ATTACK_MOVE_SPEED = 12.0f;	// 攻撃時の移動速度
	constexpr float HIT_MOVE_SPEED = 15.0f;		// 吹っ飛びリアクション時の移動速度
	constexpr float FRICTION = 0.5f;			// 摩擦
	constexpr float AVOID_DISTANCE = 500.0f;	// 回避距離
	constexpr float COEFFICIENT = 0.2f;			// 線形補間の係数
	constexpr float AVOID_MIN_DISTANCE = 30.0f;	// 回避時の打ち切り距離

	// 体力設定
	constexpr int DEFAULT_HP = 5;	// 初期HP

	// 重力設定
	constexpr float JUMP_POW = 28.0f;		// ジャンプ力
	constexpr float HIT_JUMP_POW = 15.0f;	// 吹っ飛びリアクション時のジャンプ力
	constexpr float GRAVITY = -1.98f;		// 重力加速度
	constexpr float MAX_FALL = -15.0f;		// 最大落下速度

	// 方向設定
	constexpr float ANGLE_COEFFICIENT = 0.2f;	// 補間の係数

	// ダメージ設定
	constexpr int SHAKE_TIME = 20;		// 揺らす時間

	// 攻撃設定
	constexpr float HIT_ATTACK_FRAME = 3.0f;	// 攻撃が当たるアニメーションフレーム
	constexpr float ASSIST_RANGE = 300.0f;		// プレイヤー周辺の円半径
	constexpr float ASSIST_ANGLE = 120.0f;		// 補正可能角度

	// アニメーション設定
	constexpr float ANIM_SPEED_STANDING_IDLE = 0.7f;
	constexpr float ANIM_SPEED_SITTING_IDLE = 0.7f;
	constexpr float ANIM_SPEED_WALK = 0.7f;
	constexpr float ANIM_SPEED_RUN = 0.7f;
	constexpr float ANIM_SPEED_JUMP = 0.7f;
	constexpr float ANIM_SPEED_PUNCH = 0.6f;
	constexpr float ANIM_SPEED_PUNCH_2 = 0.6f;
	constexpr float ANIM_SPEED_PUNCH_3 = 0.5f;
	constexpr float ANIM_SPEED_SCRATCH = 0.7f;
	constexpr float ANIM_SPEED_LOAF = 0.7f;
	constexpr float ANIM_SPEED_GROOMING = 0.7f;

	// 描画設定
	constexpr int  STATUS_DRAW_POS_X = 10;		// ステータス描画を始める座標
	constexpr int  HP_DRAW_POS_Y = 50;			// HPの描画Y軸
	constexpr int STATUS_DRAW_POS_OFFSET = 10;	// オフセット

	// HP
	constexpr int HP_UI_SIZE_X = 1024;
	constexpr int HP_UI_SIZE_Y = 256;
	constexpr int HP_UI_DEAW_SIZE_X = HP_UI_SIZE_X / 2;
	constexpr int HP_UI_DEAW_SIZE_Y = HP_UI_SIZE_Y / 2;
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
	stateCtrl_.initAttackTable_[PLAYER_ATTACK_END] = InitAttackEnd;
	// 更新関数
	stateCtrl_.updateAttackTable_[PLAYER_ATTACK_1] = UpdateAttack1;
	stateCtrl_.updateAttackTable_[PLAYER_ATTACK_2] = UpdateAttack2;
	stateCtrl_.updateAttackTable_[PLAYER_ATTACK_3] = UpdateAttack3;
	stateCtrl_.updateAttackTable_[PLAYER_ATTACK_END] = UpdateAttackEnd;

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

Player::~Player(void)
{
	// 画像読み込み
	for (int i = 0; i < HP_UI_DIV; i++)
	{
		DeleteGraph(uiInfo_.hpImage_[i]);
	}
}

void Player::Load(void)
{
	// HP画像読み込み
	LoadDivGraph(
		"Data/Image/Game/HP.png",
		HP_UI_DIV,
		1,
		HP_UI_DIV,
		HP_UI_SIZE_X,
		HP_UI_SIZE_Y,
		uiInfo_.hpImage_);
}

// 初期化
void Player::Init(void)
{
	// コンポーネントの追加
	AddComponets();

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

	// 状態別更新処理
	UpdateState();

	// 重力処理
	ApplyGravity();

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
	DebugDraw2D();
#endif // _DEBUG
}

void Player::Draw3D(void)
{
#ifdef _DEBUG
	// デバッグ描画
	DebugDraw3D();
#endif // _DEBUG
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

	// 待機アニメーションを設定
	player.animation_->Play(static_cast<int>(PLAYER_ANIMATION::STANDING_IDLE));
}

void Player::InitMove(Player& player)
{
	// プレイヤーの移動速度を設定
	player.info_.moveSpeed_ = MOVE_SPEED;

	// 走りアニメーションを設定
	player.animation_->Play(static_cast<int>(PLAYER_ANIMATION::RUN));
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
	player.animation_->Play(static_cast<int>(PLAYER_ANIMATION::LOAF), false);
}

void Player::InitEnd(Player& player)
{
	// ゲームオーバーフラグを立てる
	SceneManager::GetInstance()->TrueGameOver();
}

void Player::UpdateIdle(Player& player)
{
	// 角度処理
	player.RotAngle();

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
	// 角度処理
	player.RotAngle();

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
		return;
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
	// 角度処理
	player.RotAngle();

	// だんだん移動速度を遅くする
	player.info_.moveSpeed_ =
		Math::SubtractClampZero(player.info_.moveSpeed_, FRICTION);

	player.Move();

	// nullチェック
	if (player.stateCtrl_.updateAttackTable_[player.stateCtrl_.attackState_])
	{
		// 状態別更新
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
	// 角度処理
	player.RotAngle();

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
	// 角度処理
	player.RotAngle();

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
		// 状態別更新
		player.stateCtrl_.updateJumpTable_[player.stateCtrl_.jumpState_](player);
	}
}

void Player::UpdateHit(Player& player)
{
	// nullチェック
	if (player.stateCtrl_.updateHitTable_[player.stateCtrl_.hitState_])
	{
		// 状態別更新
		player.stateCtrl_.updateHitTable_[player.stateCtrl_.hitState_](player);
	}
}

void Player::UpdateDead(Player& player)
{
	// 死亡アニメーションが終了したら
	if (player.animation_->IsEnd())
	{
		// 完全に死亡へ
		player.ChangeState(PLAYER_STATE_END);
	}
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
	// 攻撃角度補正
	player.ApplyAttackAssist();

	// 攻撃開始処理
	player.StartAttack(PLAYER_ANIMATION::PUNCH);
}

void Player::InitAttack2(Player& player)
{
	// 攻撃角度補正
	player.ApplyAttackAssist();

	// 攻撃開始処理
	player.StartAttack(PLAYER_ANIMATION::PUNCH_2);
}

void Player::InitAttack3(Player& player)
{
	// 攻撃角度補正
	player.ApplyAttackAssist();

	// 攻撃開始処理
	player.StartAttack(PLAYER_ANIMATION::PUNCH_3);
}

void Player::InitAttackEnd(Player& player)
{
}

void Player::UpdateAttack1(Player& player)
{
	// 攻撃が当たった場合の後処理
	if (player.OnAttackHit())return;

	// 選んだ敵の方へ向く
	player.FaceTarget();

	// アニメーション進行度に応じて攻撃判定のON/OFF	次の攻撃予約を管理する
	player.UpdateAttackState(HIT_ATTACK_FRAME);
}

void Player::UpdateAttack2(Player& player)
{
	// 攻撃が当たった場合の後処理
	if (player.OnAttackHit())return;

	// 選んだ敵の方へ向く
	player.FaceTarget();

	// アニメーション進行度に応じて攻撃判定のON/OFF	次の攻撃予約を管理する
	player.UpdateAttackState(HIT_ATTACK_FRAME);
}

void Player::UpdateAttack3(Player& player)
{
	// 攻撃が当たった場合の後処理
	if (player.OnAttackHit())return;

	// 選んだ敵の方へ向く
	player.FaceTarget();

	// アニメーション進行度に応じて攻撃判定のON/OFF
	player.UpdateAttackState(HIT_ATTACK_FRAME);
}

void Player::UpdateAttackEnd(Player& player)
{
	// 次の攻撃入力を受け付ける
	player.AcceptNextAttackInput();

	// アニメーションを再生し終えていなかったら処理を行わない
	if (!player.animation_->IsEnd())return;

	// 次の攻撃が予約されていなかったら
	if (player.info_.nextAttack_ == PLAYER_ATTACK_MAX ||
		player.info_.nextAttack_ == PLAYER_ATTACK_END ||
		player.info_.currentAttack_ == player.info_.nextAttack_)
	{
		// 待機状態へ
		player.ChangeState(PLAYER_STATE_IDLE);
	}
	// 次の攻撃が予約されていたら
	else
	{
		// 予約している攻撃の種類へ
		player.ChangeAttackState(player.info_.nextAttack_);
	}
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
	player.animation_->Play(static_cast<int>(PLAYER_ANIMATION::JUMP));
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
	// プレイヤーの移動向きを元に戻す
	player.info_.moveDir_.x = player.info_.prevMoveDir_.x;
	player.info_.moveDir_.z = player.info_.prevMoveDir_.z;
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
		// だんだん移動量を少なくする
		player.info_.moveSpeed_ =
			Math::SubtractClampZero(player.info_.moveSpeed_, FRICTION);

		// 指定された方向と移動速度を使用し座標に反映
		player.Move();
	}
	else
	{
		// 地面に足が付いていなかったら処理を行わない
		if (player.transform_->pos_.y > 0.0f)return;

		// 吹き飛びリアクション終了へ
		player.ChangeHitState(PLAYER_HIT_END);
	}
}

void Player::UpdateHitEnd(Player& player)
{
	if (player.info_.hp_ <= 0)
	{
		// 死亡状態へ
		player.ChangeState(PLAYER_STATE_DEAD);
	}
	else
	{
		// 待機状態へ
		player.ChangeState(PLAYER_STATE_IDLE);
	}
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

void Player::AddComponets(void)
{
	// 座標の設定
	transform_ = owner_->AddComponent<Transform>();
	transform_->pos_ = {};
	transform_->angle_ = {};
	transform_->scale_ = { 4.0f,4.0f,4.0f };

	// 当たり判定の設定(カプセル)
	capsuleCollder_ = owner_->AddComponent<CapsuleCollider>();
	capsuleCollder_->Init();
	// カプセルの設定
	capsuleCollder_->AddCapsule(
		VGet(0.0f, 150.0f, 0.0f),
		VGet(0.0f, 30.0f, 0.0f),
		40.0f);
	// タグの設定
	capsuleCollder_->SetCollisionTag(CollisionTag::PLAYER);
	capsuleCollder_->SetInverseMass(1.0f);

	// 当たり判定の設定(球)
	sphereCollder_ = owner_->AddComponent<SphereCollider>();
	sphereCollder_->Init();
	// 球の設定
	sphereCollder_->SetCenterOffset(VGet(0.0f, 70.0f, -120.0f));
	sphereCollder_->SetRadius(70.0f);
	// タグの設定
	sphereCollder_->SetCollisionTag(CollisionTag::NONE);
	sphereCollder_->SetInverseMass(-1.0f);

	// 3Dモデル設定
	render3D_ = owner_->AddComponent<Render3D>();
	render3D_->Init();
	render3D_->SetModel("Data/Model/Player/Player.mv1");

	// アニメーション設定
	animation_ = owner_->AddComponent<Animation>();
	animation_->Init();
	// アニメーション登録
	animation_->AddInFbx(
		static_cast<int>(PLAYER_ANIMATION::STANDING_IDLE),
		ANIM_SPEED_STANDING_IDLE,
		static_cast<int>(PLAYER_ANIMATION::STANDING_IDLE));
	animation_->AddInFbx(
		static_cast<int>(PLAYER_ANIMATION::SITTING_IDLE),
		ANIM_SPEED_SITTING_IDLE,
		static_cast<int>(PLAYER_ANIMATION::SITTING_IDLE));
	animation_->AddInFbx(
		static_cast<int>(PLAYER_ANIMATION::WALK),
		ANIM_SPEED_WALK,
		static_cast<int>(PLAYER_ANIMATION::WALK));
	animation_->AddInFbx(
		static_cast<int>(PLAYER_ANIMATION::RUN),
		ANIM_SPEED_RUN,
		static_cast<int>(PLAYER_ANIMATION::RUN));
	animation_->AddInFbx(
		static_cast<int>(PLAYER_ANIMATION::JUMP),
		ANIM_SPEED_JUMP,
		static_cast<int>(PLAYER_ANIMATION::JUMP));
	animation_->AddInFbx(
		static_cast<int>(PLAYER_ANIMATION::PUNCH),
		ANIM_SPEED_PUNCH,
		static_cast<int>(PLAYER_ANIMATION::PUNCH));
	animation_->AddInFbx(
		static_cast<int>(PLAYER_ANIMATION::PUNCH_2),
		ANIM_SPEED_PUNCH_2,
		static_cast<int>(PLAYER_ANIMATION::PUNCH_2));
	animation_->AddInFbx(
		static_cast<int>(PLAYER_ANIMATION::PUNCH_3),
		ANIM_SPEED_PUNCH_3,
		static_cast<int>(PLAYER_ANIMATION::PUNCH_3));
	animation_->AddInFbx(
		static_cast<int>(PLAYER_ANIMATION::SCRATCH),
		ANIM_SPEED_SCRATCH,
		static_cast<int>(PLAYER_ANIMATION::SCRATCH));
	animation_->AddInFbx(
		static_cast<int>(PLAYER_ANIMATION::LOAF),
		ANIM_SPEED_LOAF,
		static_cast<int>(PLAYER_ANIMATION::LOAF));
	animation_->AddInFbx(
		static_cast<int>(PLAYER_ANIMATION::GROOMING),
		ANIM_SPEED_GROOMING,
		static_cast<int>(PLAYER_ANIMATION::GROOMING));
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
		// 0 or 2 中心を作る
		offset *= 2;
		// -1 or 1 0を中心にする
		offset -= 1;
		// -5 or 5 振れ幅を付ける
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
	if (InputManager::GetInstance()->MoveBackButton()) { dir = VAdd(dir, { 0.0f, 0.0f, -1.0f }); }
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
	// 死亡処理中は行わない
	if (stateCtrl_.state_ == PLAYER_STATE_DEAD ||
		stateCtrl_.state_ == PLAYER_STATE_END) return;

	if (transform_->pos_.y >= DAMAGE_POS_Y) return;

	// ダメージを受ける
	SetDamage(1);

	// 座標を初期化
	transform_->pos_ = {};

	// 重力も初期化
	info_.velocityY_ = 0;

	// HPが0なら
	if (info_.hp_ <= 0)
	{
		// 死亡状態へ
		ChangeState(PLAYER_STATE_DEAD);
	}
	else
	{
		// 待機状態へ
		ChangeState(PLAYER_STATE_IDLE);
	}
}

void Player::RotAngle(void)
{
	// 方向から角度を算出
	float targetAngle = atan2f(info_.moveDir_.x, info_.moveDir_.z);
	// 今回のモデルのY軸向きが逆なので向きを反転させる
	targetAngle += DX_PI_F;

	// 角度補間をかける
	transform_->angle_.y = Math::LerpAngle(
		transform_->angle_.y,
		targetAngle,
		ANGLE_COEFFICIENT);

	transform_->angle_.x = transform_->angle_.z = 0.0f;

	// モデルに向きを設定
	MV1SetRotationXYZ(render3D_->GetHandle(), transform_->angle_);
}

void Player::SetDamage(int damage)
{
	// HPにダメージを与える
	info_.hp_ -= damage;

	// HPが0以下になったら
	if (info_.hp_ <= 0)
	{
		// 0初期化しておく
		info_.hp_ = 0;
	}
	else
	{
		// 無敵時間を設ける
		info_.invincibleTime_ = INVINCIBLE_TIME;

		// HP描画を揺らす
		info_.hitStopCounter_ = SHAKE_TIME;
	}
}

void Player::SetHitReact(VECTOR moveDir)
{
	// 吹っ飛び方向を算出のため反転
	moveDir.x *= -1.0f;
	moveDir.z *= -1.0f;

	// 前回向きを保持しておく
	info_.prevMoveDir_ = info_.moveDir_;
	// 指定された移動向きを設定
	info_.moveDir_ = moveDir;

	// 指定された移動速度を設定
	info_.moveSpeed_ = HIT_MOVE_SPEED;

	// 指定されたジャンプ力を設定
	info_.velocityY_ = HIT_JUMP_POW;

	// ダメージを受けたステートへ
	ChangeState(PLAYER_STATE_HIT);
}

void Player::AcceptNextAttackInput(void)
{
	// 攻撃ボタンが押されたかつ、次の攻撃が予約されていなかったら
	if (InputManager::GetInstance()->AttackButton() && info_.currentAttack_ == info_.nextAttack_)
	{
		// 現在の攻撃から次の攻撃を算出
		int attack = static_cast<int>(info_.nextAttack_);

		attack++;

		// 上限
		if (attack >= static_cast<int>(PLAYER_ATTACK_END))
		{
			attack = static_cast<int>(PLAYER_ATTACK_END);
		}

		// 次の攻撃を予約する
		info_.nextAttack_ = static_cast<PLAYER_ATTACK_STATE>(attack);
	}
}

void Player::StartAttack(PLAYER_ANIMATION anim)
{
	// 移動先の向きを,
	InputMove();

	// プレイヤーの移動速度を設定
	info_.moveSpeed_ = ATTACK_MOVE_SPEED;

	// 攻撃アニメーションを再生
	animation_->Play(static_cast<int>(anim), false);

	// 攻撃が当たったかフラグ初期化
	info_.isAttackHit_ = false;

	// 攻撃予約フラグ
	info_.currentAttack_ = info_.nextAttack_ = stateCtrl_.attackState_;
}

void Player::UpdateAttackState(float StartAttackFrame)
{
	// アニメーションが攻撃終了フレーム以上になっていたら
	if (animation_->GetStep() >= animation_->GetTotalAnimTime())
	{
		// カプセルコライダーのタグがNONEになってたら処理を行わない
		if (sphereCollder_->GetCollisionTag() == CollisionTag::NONE)return;

		// 攻撃状態を終了
		EndAttaking();
	}
	// アニメーションが攻撃開始フレーム以上になっているかつ攻撃終了フレームより小さければ
	else if (animation_->GetStep() >= StartAttackFrame && animation_->GetStep() < animation_->GetTotalAnimTime())
	{
		// 次の攻撃入力を受け付ける
		AcceptNextAttackInput();

		// カプセルコライダーのタグがNONEではなかったら処理を行わない
		if (sphereCollder_->GetCollisionTag() != CollisionTag::NONE)return;

		// カプセルコライダーのタグをプレイヤー攻撃へ
		sphereCollder_->SetCollisionTag(CollisionTag::PLAYER_ATTACK);
	}
}

bool Player::OnAttackHit(void)
{
	// 攻撃が当たっていたら
	if (info_.isAttackHit_)
	{
		EndAttaking();
		// 攻撃フラグを折る
		info_.isAttackHit_ = false;
		return true;
	}

	return false;
}

void Player::EndAttaking(void)
{
	// カプセルコライダーのタグをNONEへ
	sphereCollder_->SetCollisionTag(CollisionTag::NONE);

	// 攻撃終了状態へ
	ChangeAttackState(PLAYER_ATTACK_STATE::PLAYER_ATTACK_END);
}

bool Player::IsInAssistRange(const Transform* ratTransform)
{
	VECTOR vec = VSub(ratTransform->pos_, transform_->pos_);
	float distanceXZ = vec.x * vec.x + vec.z * vec.z;
	float rad = ASSIST_RANGE * ASSIST_RANGE;

	return distanceXZ <= rad;
}

bool Player::IsWithinAssistAngle(const Transform* ratTransform)
{
	VECTOR vec = VSub(ratTransform->pos_, transform_->pos_);

	// XZ上の距離を算出
	float distance = std::sqrt(vec.x * vec.x + vec.z * vec.z);

	// 基準となる向き（XZのみ）
	float dirX = info_.moveDir_.x;
	float dirZ = info_.moveDir_.z;
	float dirLen = std::sqrt(dirX * dirX + dirZ * dirZ);

	if (dirLen < 0.0001f)
	{
		// 入力がないときはモデルの向きを使う
		// ※モデルのY軸向きが逆なので符号を反転している
		dirX = -std::sin(transform_->angle_.y);
		dirZ = -std::cos(transform_->angle_.y);
		dirLen = 1.0f;
	}

	dirX /= dirLen;
	dirZ /= dirLen;

	const float cosTheta =
		(vec.x * dirX + vec.z * dirZ) / distance;

	return cosTheta >= std::cos((ASSIST_ANGLE * DX_PI_F / 180.0f) * 0.5f);
}

void Player::SelectNearestTarget(void)
{
	// 最短距離
	float minDistance = FLT_MAX;
	// 候補の中から一番近いものを選ぶ
	for (const auto& transform : info_.attackAssistCandidates_)
	{
		// 自分から敵へのベクトルを求める
		VECTOR vec = VSub(transform->pos_, transform_->pos_);
		// 距離を求める
		float distance = vec.x * vec.x + vec.z * vec.z;

		// 一番距離が近いものを選ぶ
		if (distance < minDistance)
		{
			// 近かったら最短距離と決定対象を更新
			minDistance = distance;
			info_.finalCandidateTransform_ = transform;
		}
	}
}

void Player::FaceTarget(void)
{
	// 中身がなかったら処理を行わない
	if (info_.finalCandidateTransform_ == nullptr) return;

	// 相手へのベクトルを計算
	VECTOR vec = VSub(info_.finalCandidateTransform_->pos_, transform_->pos_);

	// ベクトルの正規化で単位ベクトル(方向)を取得する
	float length = sqrtf(vec.x * vec.x + vec.z * vec.z);

	if (length < 0.0001f)
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

void Player::ApplyAttackAssist(void)
{
	// 前回のものを引き継がないよう初期化
	info_.velocityRot_ = 0.0f;					// 角速度
	info_.attackAssistCandidates_.clear();		// 補正対象
	info_.finalCandidateTransform_ = nullptr;	// 決定対象

	for (const auto& transform : ratsTransform_)
	{
		// プレイヤー周辺の円内にいなければ次の処理へ
		if (!IsInAssistRange(transform))continue;

		// 角度判定内に入ってなければ次の処理へ
		if (!IsWithinAssistAngle(transform))continue;

		// 補正対象に追加
		info_.attackAssistCandidates_.emplace_back(transform);
	}

	// 中身がなければ処理終了
	if (info_.attackAssistCandidates_.empty())return;

	// 距離が近い対象を選ぶ
	SelectNearestTarget();
}

void Player::OnCollision(Collider* other)
{
	// 当たったオブジェクトが何の種類かを判断
	// 衝突したオブジェクトをタグごとに判別
	switch (other->GetCollisionTag())
	{
	case CollisionTag::ENEMY:
		// 敵本体と接触した
		break;

	case CollisionTag::ENEMY_ATTACK:
		// 敵の攻撃を受けた
		// 死亡処理に入っていたら処理を行わない
		if (stateCtrl_.state_ == PLAYER_STATE_DEAD ||
			stateCtrl_.state_ == PLAYER_STATE_END)return;

		// 無敵時間があれば処理を行わない
		if (info_.invincibleTime_ > 0)return;

		// ダメージ処理
		SetDamage(1);

		// 吹っ飛びリアクションをさせる
		SetHitReact(capsuleCollder_->GetCollisionInfo().normal);
		break;

	default:
		break;
	}

	// 攻撃判定の検知
	AttackOnCollision();
}

void Player::AttackOnCollision(void)
{
	// 自分の球が当たったかどうかを判断
	// 衝突したオブジェクトをタグごとに判別
	switch (sphereCollder_->GetCollisionTag())
	{
	case CollisionTag::PLAYER_ATTACK:
		// 敵本体と接触した
		// 攻撃が当たったフラグを立てる
		info_.isAttackHit_ = true;
		break;

	default:
		break;
	}
}

void Player::DrawHP(void)
{
	// HP描画
	DrawRotaGraph(
		HP_UI_DEAW_SIZE_X / 2,
		HP_UI_DEAW_SIZE_Y / 2,
		0.5,
		0.0,
		uiInfo_.hpImage_[info_.hp_],
		true);
}

#ifdef _DEBUG
void Player::DebugDraw2D(void)
{
	// プレイヤー座標表示
	DrawFormatString(20,
		300,
		0xff0000,
		"プレイヤー座標 : %.f,%.f,%.f",
		transform_->pos_.x, transform_->pos_.y, transform_->pos_.z);
}
void Player::DebugDraw3D(void)
{
	// 攻撃補正範囲の描画
	DrawSphere3D(
		transform_->pos_,
		ASSIST_RANGE,
		16,
		0x0000ff,
		0x0000ff,
		false
	);
}
#endif // _DEBUG
