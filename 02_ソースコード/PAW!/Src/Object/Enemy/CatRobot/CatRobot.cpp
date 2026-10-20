#include "../../Object.h"
#include "../../Component/Graphics/Render3D.h"
#include "../../Component/Animation/Animation.h"
#include "../../Camera/CameraUtility.h"
#include "../../../Collision/3DCollider/CapsuleCollider.h"
#include "../../../Math/Math.h"

#include "CatRobot.h"

namespace
{
	// 体力設定
	constexpr int DEFAULT_HP = 20;	// 初期HP

	// 重力設定
	constexpr float GRAVITY = -1.98f;	// 重力加速度
	constexpr float MAX_FALL = -10.0f;	// 最大落下速度

	// リミット設定
	constexpr int INVINCIBLE_TIME = 60;	// 無敵時間
}

CatRobot::CatRobot(void)
{
	// 猫ロボット状態
	// 初期化関数
	stateCtrl_.initStateTable_[CAT_ROBOT_STATE_IDLE] = InitIdle;
	stateCtrl_.initStateTable_[CAT_ROBOT_STATE_MOVE] = InitMove;
	stateCtrl_.initStateTable_[CAT_ROBOT_STATE_ATTACK] = InitAttack;
	stateCtrl_.initStateTable_[CAT_ROBOT_STATE_CHANCE] = InitChance;
	stateCtrl_.initStateTable_[CAT_ROBOT_STATE_DEAD] = InitDead;
	stateCtrl_.initStateTable_[CAT_ROBOT_STATE_END] = InitEnd;
	// 更新関数
	stateCtrl_.updateStateTable_[CAT_ROBOT_STATE_IDLE] = UpdateIdle;
	stateCtrl_.updateStateTable_[CAT_ROBOT_STATE_MOVE] = UpdateMove;
	stateCtrl_.updateStateTable_[CAT_ROBOT_STATE_ATTACK] = UpdateAttack;
	stateCtrl_.updateStateTable_[CAT_ROBOT_STATE_CHANCE] = UpdateChance;
	stateCtrl_.updateStateTable_[CAT_ROBOT_STATE_DEAD] = UpdateDead;
	stateCtrl_.updateStateTable_[CAT_ROBOT_STATE_END] = UpdateEnd;

	// 猫ロボット攻撃状態
	// 初期化関数
	stateCtrl_.initAttackTable_[CAT_ROBOT_ATTACK_SELECT] = InitAttackSelect;
	stateCtrl_.initAttackTable_[CAT_ROBOT_ATTACK_1] = InitAttack1;
	stateCtrl_.initAttackTable_[CAT_ROBOT_ATTACK_2] = InitAttack2;
	stateCtrl_.initAttackTable_[CAT_ROBOT_ATTACK_3] = InitAttack3;
	stateCtrl_.initAttackTable_[CAT_ROBOT_ATTACK_4] = InitAttack4;
	stateCtrl_.initAttackTable_[CAT_ROBOT_ATTACK_END] = InitAttackEnd;
	// 更新関数
	stateCtrl_.updateAttackTable_[CAT_ROBOT_ATTACK_SELECT] = UpdateAttackSelect;
	stateCtrl_.updateAttackTable_[CAT_ROBOT_ATTACK_1] = UpdateAttack1;
	stateCtrl_.updateAttackTable_[CAT_ROBOT_ATTACK_2] = UpdateAttack2;
	stateCtrl_.updateAttackTable_[CAT_ROBOT_ATTACK_3] = UpdateAttack3;
	stateCtrl_.updateAttackTable_[CAT_ROBOT_ATTACK_4] = UpdateAttack4;
	stateCtrl_.updateAttackTable_[CAT_ROBOT_ATTACK_END] = UpdateAttackEnd;

	// プレイヤーの攻撃チャンス状態
	// 初期化関数
	stateCtrl_.initChanceTable_[CAT_ROBOT_CHANCE_START] = InitChanceStart;
	stateCtrl_.initChanceTable_[CAT_ROBOT_CHANCE_STUN] = InitChanceStun;
	stateCtrl_.initChanceTable_[CAT_ROBOT_CHANCE_END] = InitChanceEnd;
	// 更新関数
	stateCtrl_.updateChanceTable_[CAT_ROBOT_CHANCE_START] = UpdateChanceStart;
	stateCtrl_.updateChanceTable_[CAT_ROBOT_CHANCE_STUN] = UpdateChanceStun;
	stateCtrl_.updateChanceTable_[CAT_ROBOT_CHANCE_END] = UpdateChanceEnd;
}

void CatRobot::Init(void)
{
	// コンポーネントの追加
	AddComponets();

	// HPの初期化
	info_.hp_ = DEFAULT_HP;

	// 猫ロボットの状態初期化
	ChangeState(CAT_ROBOT_STATE_IDLE);
}

void CatRobot::Update(void)
{
	// 完全に死亡していたら処理を行わない
	if (stateCtrl_.state_ == CAT_ROBOT_STATE_END)	return;

	// 状態別更新処理
	UpdateState();

	// 重力処理
	ApplyGravity();

	// 無敵時間を更新
	UpdateInvincible();
}

void CatRobot::UpdateState(void)
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

void CatRobot::InitIdle(CatRobot& catRobot)
{
}

void CatRobot::InitMove(CatRobot& catRobot)
{
}

void CatRobot::InitAttack(CatRobot& catRobot)
{
}

void CatRobot::InitChance(CatRobot& catRobot)
{
}

void CatRobot::InitDead(CatRobot& catRobot)
{
}

void CatRobot::InitEnd(CatRobot& catRobot)
{
}

void CatRobot::UpdateIdle(CatRobot& catRobot)
{
}

void CatRobot::UpdateMove(CatRobot& catRobot)
{
}

void CatRobot::UpdateAttack(CatRobot& catRobot)
{
	// nullチェック
	if (catRobot.stateCtrl_.updateAttackTable_[catRobot.stateCtrl_.attackState_])
	{
		// 状態別更新
		catRobot.stateCtrl_.updateAttackTable_[catRobot.stateCtrl_.attackState_](catRobot);
	}
}

void CatRobot::UpdateChance(CatRobot& catRobot)
{
	// nullチェック
	if (catRobot.stateCtrl_.updateChanceTable_[catRobot.stateCtrl_.chanceState_])
	{
		// 状態別更新
		catRobot.stateCtrl_.updateChanceTable_[catRobot.stateCtrl_.chanceState_](catRobot);
	}
}

void CatRobot::UpdateDead(CatRobot& catRobot)
{
}

void CatRobot::UpdateEnd(CatRobot& catRobot)
{
}

void CatRobot::ChangeState(CAT_ROBOT_STATE state)
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

void CatRobot::InitAttackSelect(CatRobot& catRobot)
{
}

void CatRobot::InitAttack1(CatRobot& catRobot)
{
}

void CatRobot::InitAttack2(CatRobot& catRobot)
{
}

void CatRobot::InitAttack3(CatRobot& catRobot)
{
}

void CatRobot::InitAttack4(CatRobot& catRobot)
{
}

void CatRobot::InitAttackEnd(CatRobot& catRobot)
{
}

void CatRobot::UpdateAttackSelect(CatRobot& catRobot)
{
}

void CatRobot::UpdateAttack1(CatRobot& catRobot)
{
}

void CatRobot::UpdateAttack2(CatRobot& catRobot)
{
}

void CatRobot::UpdateAttack3(CatRobot& catRobot)
{
}

void CatRobot::UpdateAttack4(CatRobot& catRobot)
{
}

void CatRobot::UpdateAttackEnd(CatRobot& catRobot)
{
}

void CatRobot::ChangeAttackState(CAT_ROBOT_ATTACK_STATE state)
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

void CatRobot::InitChanceStart(CatRobot& catRobot)
{
}

void CatRobot::InitChanceStun(CatRobot& catRobot)
{
}

void CatRobot::InitChanceEnd(CatRobot& catRobot)
{
}

void CatRobot::UpdateChanceStart(CatRobot& catRobot)
{
}

void CatRobot::UpdateChanceStun(CatRobot& catRobot)
{
}

void CatRobot::UpdateChanceEnd(CatRobot& catRobot)
{
}

void CatRobot::ChangeChanceState(CAT_ROBOT_CHANCE_STATE state)
{
	// 指定されたステートへ変更
	stateCtrl_.chanceState_ = state;

	// nullチェック
	if (stateCtrl_.initChanceTable_[stateCtrl_.chanceState_])
	{
		// 状態別初期化
		stateCtrl_.initChanceTable_[stateCtrl_.chanceState_](*this);
	}
}

void CatRobot::AddComponets(void)
{
	// 座標の設定
	transform_ = owner_->AddComponent<Transform>();
	transform_->pos_ = { 0.0f,0.0f,500.0f };
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
	render3D_->SetModel("Data/Model/Enemy/CatRobot.mv1");
}

void CatRobot::ApplyGravity(void)
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

void CatRobot::UpdateInvincible(void)
{
	// 無敵時間を減らす
	if (info_.invincibleTime_ > 0)
	{
		--info_.invincibleTime_;
	}
}

void CatRobot::Move(void)
{
	// 移動速度が0以下であれば処理を行わない
	if (info_.moveSpeed_ <= 0.0f)return;

	// 方向×スピードで移動量を作って、座標に足して移動
	transform_->pos_ =
		VAdd(transform_->pos_,
			VScale(info_.moveDir_, info_.moveSpeed_));
}

void CatRobot::GetDirectionToPlayer(bool isRetreat)
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

void CatRobot::SetDamage(void)
{
	// 無敵時間があればダメージを与えない
	if (info_.invincibleTime_ > 0)return;

	// HPにダメージを与える
	--info_.hp_;

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
	}
}

void CatRobot::OnCollision(Collider* other)
{
	// 衝突したオブジェクトをタグごとに判別
	switch (other->GetCollisionTag())
	{
	case CollisionTag::PLAYER:
		break;

	case CollisionTag::PLAYER_ATTACK:
		break;

	default:
		break;
	}
}