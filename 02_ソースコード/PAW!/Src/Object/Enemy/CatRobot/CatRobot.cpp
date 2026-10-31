#include "../../Object.h"
#include "../../Component/Graphics/Render3D.h"
#include "../../Component/Animation/Animation.h"
#include "../../Camera/CameraUtility.h"
#include "../../../Collision/3DCollider/CapsuleCollider.h"
#include "../../../Math/Math.h"
#include "Attack/Attack1/Attack1.h"
#include "Attack/PlateAttack/PlateAttack.h"

#include "CatRobot.h"

namespace
{
	// 体力設定
	constexpr int DEFAULT_HP = 20;	// 初期HP

	// 重力設定
	constexpr float GRAVITY = -1.98f;	// 重力加速度
	constexpr float MAX_FALL = -20.0f;	// 最大落下速度

	// リミット設定
	constexpr int INVINCIBLE_TIME = 20;	// 無敵時間

	// 攻撃1の（食材攻撃）
	constexpr float ATTACK1_POS_Y = 1000.0f;	// Y座標
	constexpr int ATTACK1_NUM = 3;				// 攻撃を行う回数
	constexpr int ATTACK1_FALL_NUM = 10;		// 降ってくる個数
	constexpr int ATTACK1_RANGE = 2000;			// 攻撃範囲
	constexpr int ATTACK1_TRY_MAX = 10;			// 最大挑戦回数

	// 攻撃2,3の（お皿攻撃）
	constexpr float ATTACK2_POS_Y = 10.0f;			// Y座標
	constexpr int ATTACK2_NUM = 2;					// 攻撃を行う回数
	constexpr int ATTACK3_NUM = 2;					// 攻撃を行う回数
	constexpr int PLATE_FLYING_NUM = 4;				// 個数
	constexpr float PLATE_MOVE_DISTANCE = 2000.0f;	// 攻撃距離
	constexpr int PLATE_IDLE_COUNT = 60;			// 待機時間の基準値

	// 攻撃全体の回数
	constexpr int ATTACK_NUM_ALL = 1;

	// スタン時間
	constexpr int STUN_TIME = 300;

	constexpr int ATTACK_RAND_NUM = 5000;		// ランダム用数
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

void CatRobot::Draw2D(void)
{
	unsigned int col = 0xff0000;

	if (stateCtrl_.state_ == CAT_ROBOT_STATE_CHANCE &&
		stateCtrl_.chanceState_ == CAT_ROBOT_CHANCE_STUN)
	{
		col = 0x0000ff;
	}

	// HP描画
	DrawFormatString(0, 200, col, "%d", info_.hp_);
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
	catRobot.info_.attackNumAll_ = ATTACK_NUM_ALL;

	// 攻撃種類選択へ
	catRobot.ChangeAttackState(CAT_ROBOT_ATTACK_SELECT);
}

void CatRobot::InitChance(CatRobot& catRobot)
{
	// 攻撃種類選択へ
	catRobot.ChangeChanceState(CAT_ROBOT_CHANCE_START);
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
	// 完全終了へ
	catRobot.ChangeState(CAT_ROBOT_STATE_END);
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
	// 0より大きければ初期化を行わない
	if (catRobot.info_.attackNum_ <= 0)
	{
		// 食材攻撃の回数初期化
		catRobot.info_.attackNum_ = ATTACK1_NUM;
	}

	// 攻撃1の座標決め
	catRobot.ResetAttack1Pos();
}

void CatRobot::InitAttack2(CatRobot& catRobot)
{
	// 0より大きければ初期化を行わない
	if (catRobot.info_.attackNum_ <= 0)
	{
		// 食材攻撃の回数初期化
		catRobot.info_.attackNum_ = ATTACK2_NUM;
	}

	// 攻撃2の座標決め
	catRobot.ResetPlateAttackPos(true);
}

void CatRobot::InitAttack3(CatRobot& catRobot)
{
	// 0より大きければ初期化を行わない
	if (catRobot.info_.attackNum_ <= 0)
	{
		// 食材攻撃の回数初期化
		catRobot.info_.attackNum_ = ATTACK3_NUM;
	}

	// 攻撃2の座標決め
	catRobot.ResetPlateAttackPos(false);
}

void CatRobot::InitAttack4(CatRobot& catRobot)
{
}

void CatRobot::InitAttackEnd(CatRobot& catRobot)
{
}

void CatRobot::UpdateAttackSelect(CatRobot& catRobot)
{
	catRobot.info_.attackNumAll_--;

	int rand = GetRand(ATTACK_RAND_NUM);
	// 0～2にする
	rand %= 3;

	switch (rand)
	{
	case 0:
		// 攻撃1へ
		catRobot.ChangeAttackState(CAT_ROBOT_ATTACK_1);
		break;
	case 1:
		// 攻撃2へ
		catRobot.ChangeAttackState(CAT_ROBOT_ATTACK_2);
		break;
	case 2:
		// 攻撃3へ
		catRobot.ChangeAttackState(CAT_ROBOT_ATTACK_3);
		break;
	default:
		break;
	}
}

void CatRobot::UpdateAttack1(CatRobot& catRobot)
{
	bool isAllEnd = true;

	// 攻撃数分回す
	for (const auto& attack : catRobot.attack1_)
	{
		// 終了していなかったら
		if (attack->GetState() != ATTACK1_STATE_END)
		{
			// フラグを折る
			isAllEnd = false;
			break;
		}
	}

	// すべて下へ落ち切っていたら
	if (isAllEnd)
	{
		// 攻撃回数を減らす
		catRobot.info_.attackNum_--;

		// 回数が残っていたら攻撃終了しない
		if (catRobot.info_.attackNum_ > 0)
		{
			// もう一度攻撃させる
			catRobot.ChangeAttackState(catRobot.stateCtrl_.attackState_);
			return;
		}

		// 攻撃終了へ
		catRobot.ChangeAttackState(CAT_ROBOT_ATTACK_END);
	}
}

void CatRobot::UpdateAttack2(CatRobot& catRobot)
{
	bool isAllEnd = true;

	// 攻撃数分回す
	for (const auto& attack : catRobot.plateAttack_)
	{
		// 終了していなかったら
		if (attack->GetState() != ATTACK1_STATE_END)
		{
			// フラグを折る
			isAllEnd = false;
			break;
		}
	}

	// すべて下へ落ち切っていたら
	if (isAllEnd)
	{
		// 攻撃回数を減らす
		catRobot.info_.attackNum_--;

		// 回数が残っていたら攻撃終了しない
		if (catRobot.info_.attackNum_ > 0)
		{
			// もう一度攻撃させる
			catRobot.ChangeAttackState(catRobot.stateCtrl_.attackState_);
			return;
		}

		// 攻撃終了へ
		catRobot.ChangeAttackState(CAT_ROBOT_ATTACK_END);
	}
}

void CatRobot::UpdateAttack3(CatRobot& catRobot)
{
	bool isAllEnd = true;

	// 攻撃数分回す
	for (const auto& attack : catRobot.plateAttack_)
	{
		// 終了していなかったら
		if (attack->GetState() != ATTACK1_STATE_END)
		{
			// フラグを折る
			isAllEnd = false;
			break;
		}
	}

	// すべて下へ落ち切っていたら
	if (isAllEnd)
	{
		// 攻撃回数を減らす
		catRobot.info_.attackNum_--;

		// 回数が残っていたら攻撃終了しない
		if (catRobot.info_.attackNum_ > 0)
		{
			// もう一度攻撃させる
			catRobot.ChangeAttackState(catRobot.stateCtrl_.attackState_);
			return;
		}

		// 攻撃終了へ
		catRobot.ChangeAttackState(CAT_ROBOT_ATTACK_END);
	}
}

void CatRobot::UpdateAttack4(CatRobot& catRobot)
{
}

void CatRobot::UpdateAttackEnd(CatRobot& catRobot)
{
	// 攻撃回数が残っていたら
	if (catRobot.info_.attackNumAll_ > 0)
	{
		// 攻撃選択へ
		catRobot.ChangeAttackState(CAT_ROBOT_ATTACK_SELECT);
	}
	else
	{
		// 攻撃チャンス状態へ
		catRobot.ChangeState(CAT_ROBOT_STATE_CHANCE);
	}
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
	catRobot.info_.stunTime_ = STUN_TIME;
}

void CatRobot::InitChanceEnd(CatRobot& catRobot)
{
}

void CatRobot::UpdateChanceStart(CatRobot& catRobot)
{
	// スタン状態へ
	catRobot.ChangeChanceState(CAT_ROBOT_CHANCE_STUN);
}

void CatRobot::UpdateChanceStun(CatRobot& catRobot)
{
	catRobot.info_.stunTime_--;

	if (catRobot.info_.stunTime_ <= 0)
	{
		// スタン終了状態へ
		catRobot.ChangeChanceState(CAT_ROBOT_CHANCE_END);
	}
}

void CatRobot::UpdateChanceEnd(CatRobot& catRobot)
{
	// 待機状態へ
	catRobot.ChangeState(CAT_ROBOT_STATE_IDLE);
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
	transform_->pos_ = { 0.0f,500.0f,2000.0f };
	transform_->angle_ = {};
	transform_->scale_ = {};

	// 当たり判定の設定(カプセル)
	capsuleCollder_ = owner_->AddComponent<CapsuleCollider>();
	capsuleCollder_->Init();

	// カプセルの設定
	capsuleCollder_->AddCapsule(
		VGet(0.0f, 800.0f, 0.0f),
		VGet(0.0f, 30.0f, 0.0f),
		350.0f);

	// タグの設定
	capsuleCollder_->SetCollisionTag(CollisionTag::BOSS);
	capsuleCollder_->SetInverseMass(0.0f);

	// 3Dモデル設定
	render3D_ = owner_->AddComponent<Render3D>();
	render3D_->Init();
	render3D_->SetModel("Data/Model/Enemy/CatRobot.mv1");

	// アニメーション設定
	animation_ = owner_->AddComponent<Animation>();
	animation_->Init();
	// アニメーション登録
	animation_->AddInFbx(
		static_cast<int>(CAT_ROBOT_ANIMATION::STANDING_IDLE),
		10,
		static_cast<int>(CAT_ROBOT_ANIMATION::STANDING_IDLE));

	attack1_.clear();
	plateAttack_.clear();

	// 攻撃1(食材攻撃)個数分回す
	for (int i = 0; i < ATTACK1_FALL_NUM; i++)
	{
		Attack1* attack = attack1_.emplace_back(owner_->AddComponent<Attack1>());
		attack->Init();
	}

	// 攻撃2,3(お皿攻撃)個数分回す
	for (int i = 0; i < PLATE_FLYING_NUM; i++)
	{
		PlateAttack* attack = plateAttack_.emplace_back(owner_->AddComponent<PlateAttack>());
		attack->Init();
	}
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

void CatRobot::ResetAttack1Pos(void)
{
	// 一つだけ、プレイヤーの頭上に設定
	attack1_.front()->
		StartAttack({ playerTransform_->pos_.x,ATTACK1_POS_Y,playerTransform_->pos_.z });

	// ランダム座標
	int randX, randZ = 0;
	VECTOR pos = {};

	// 攻撃数分回す
	for (int i = 1; i < attack1_.size(); i++)
	{
		for (int tryNum = 0; tryNum < ATTACK1_TRY_MAX; tryNum++)
		{
			// ランダムで座標を決める
			randX = GetRand(ATTACK1_RANGE);
			randZ = GetRand(ATTACK1_RANGE);
			randX -= ATTACK1_RANGE / 2;
			randZ -= ATTACK1_RANGE / 2;

			pos = { static_cast<float>(randX),ATTACK1_POS_Y,static_cast<float>(randZ) };

			// 判定に自分が含まれないよう-1
			if (IsFarEnough(pos, Attack1::ATTACK1_RADIUS, i - 1))
			{
				// 座標を設定
				attack1_[i]->StartAttack(pos);
				break;
			}

			// 10回しても重なっていたらあきらめる
			if (tryNum >= ATTACK1_TRY_MAX - 1)
			{
				// 座標を設定
				attack1_[i]->StartAttack(pos);
			}
		}
	}
}

bool CatRobot::IsFarEnough(VECTOR pos, float rad, int i)
{
	VECTOR vec = {};
	for (int j = 0; j < i; j++)
	{
		vec = VSub(attack1_[j]->GetTransform()->pos_, pos);
		// 距離を求める
		float distance = VSize(vec);
		// それぞれの半径の和
		float radius = rad + rad;

		// 当たっていたらfalseを返す
		if (distance < radius)return false;
	}

	// 当たっていないためtrueを返す
	return true;
}

void CatRobot::ResetPlateAttackPos(bool leftRight)
{
	// 左右指定で向きを出す
	VECTOR dir = SelectPlateAttackDir(leftRight);
	VECTOR pos = { 0.0f,ATTACK2_POS_Y,0.0f };

	// 円柱の直径
	float diameter = PlateAttack::PLATE_RADIUS * 2;

	// 攻撃数分回す
	for (int i = 0; i < plateAttack_.size(); i++)
	{
		// 左右移動なため
		if (dir.x != 0.0f)
		{
			// 移動方向の反対側に配置
			pos.x = PLATE_MOVE_DISTANCE * -dir.x;
			// 重ならないように配置
			pos.z = PLATE_MOVE_DISTANCE - ((i + 1) * diameter);
		}
		// 上下移動なため
		else if (dir.z != 0.0f)
		{
			// 重ならないように配置
			pos.x = PLATE_MOVE_DISTANCE - ((i + 1) * diameter);
			// 移動方向の反対側に配置
			pos.z = PLATE_MOVE_DISTANCE * -dir.z;
		}

		// 座標を設定
		plateAttack_[i]->StartAttack(pos, dir, PLATE_MOVE_DISTANCE, PLATE_IDLE_COUNT * i);
	}
}

VECTOR CatRobot::SelectPlateAttackDir(bool leftRight)
{
	// ランダム用
	int rand = GetRand(ATTACK_RAND_NUM);
	// 0か1にする
	rand %= 2;;

	VECTOR dir = {};

	// 左右指定であれば
	if (leftRight)
	{
		// X軸に向きを入れる
		if (rand <= 0)
		{
			dir.x = -1.0f;
		}
		else
		{
			dir.x = 1.0f;
		}
	}
	// 上下指定であれば
	else
	{
		// Z軸に向きを入れる
		if (rand <= 0)
		{
			dir.z = -1.0f;
		}
		else
		{
			dir.z = 1.0f;
		}
	}

	return dir;
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

		// 死亡状態へ
		ChangeState(CAT_ROBOT_STATE_DEAD);
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
		// スタン状態だったらダメージを与える
		if (stateCtrl_.state_ == CAT_ROBOT_STATE_CHANCE &&
			stateCtrl_.chanceState_ == CAT_ROBOT_CHANCE_STUN)
		{
			// ダメージを与える
			SetDamage();
		}

		break;

	default:
		break;
	}
}