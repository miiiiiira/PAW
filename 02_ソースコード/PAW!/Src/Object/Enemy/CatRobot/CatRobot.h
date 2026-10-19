#pragma once

#include "../../Component/Component.h"
#include "../../Component/Transform/Transform.h"
#include "CatRobotInfo.h"

// 前方宣言
class Transform;
class Render3D;
class Animation;
class CapsuleCollider;

class CatRobot : public Component
{
public:
	CatRobot(void);
	~CatRobot(void);

	void Load(void) override;	// 読み込み
	void Init(void) override;	// 初期化
	void Update(void) override;	// 更新
	void Draw2D(void) override;	// 2D描画
	void Draw3D(void) override;	// 3D描画

public:
	// Transformを返す
	Transform* GetTransform(void) { return transform_; }

	// 状態を取得
	CAT_ROBOT_STATE GetState(void)const { return stateCtrl_.state_; }

private:
	// 状態別更新処理
	void UpdateState(void);

	// 行動状態
	// 状態別初期化
	static void InitIdle(CatRobot& catRobot);			// 待機
	static void InitMove(CatRobot& catRobot);			// 移動
	static void InitRetreat(CatRobot& catRobot);		// 後退
	static void InitAttack(CatRobot& catRobot);		// 攻撃
	static void InitHit(CatRobot& catRobot);			// ダメージを受けた
	static void InitDead(CatRobot& catRobot);			// 死亡
	static void InitEnd(CatRobot& catRobot);			// 終了
	// 状態別更新
	static void UpdateIdle(CatRobot& catRobot);		// 待機
	static void UpdateMove(CatRobot& catRobot);		// 移動
	static void UpdateRetreat(CatRobot& catRobot);	// 後退
	static void UpdateAttack(CatRobot& catRobot);		// 攻撃
	static void UpdateHit(CatRobot& catRobot);		// ダメージを受けた
	static void UpdateDead(CatRobot& catRobot);		// 死亡
	static void UpdateEnd(CatRobot& catRobot);		// 終了
	// 状態を変更させる
	void ChangeState(CAT_ROBOT_STATE state);

	// 待機状態
	// 状態別初期化
	static void InitIdleWait(CatRobot& catRobot);			// 待機
	static void InitIdleSway(CatRobot& catRobot);			// 左右に揺れる
	static void InitIdleApproach(CatRobot& catRobot);		// ちょっと近づく
	// 状態別更新
	static void UpdateIdleWait(CatRobot& catRobot);		// 待機
	static void UpdateIdleSway(CatRobot& catRobot);		// 左右に揺れる
	static void UpdateIdleApproach(CatRobot& catRobot);	// ちょっと近づく
	// 待機状態を変更させる
	void ChangeIdleState(CAT_ROBOT_IDLE_STATE state);

	// 攻撃状態
	// 状態別初期化
	static void InitAttackStart(CatRobot& catRobot);		// 攻撃始め
	static void InitAttacking(CatRobot& catRobot);		// 攻撃中
	static void InitAttackAfter(CatRobot& catRobot);		// 攻撃終了後の隙
	static void InitAttackEnd(CatRobot& catRobot);		// 攻撃終了
	// 状態別更新
	static void UpdateAttackStart(CatRobot& catRobot);	// 攻撃始め
	static void UpdateAttacking(CatRobot& catRobot);		// 攻撃中
	static void UpdateAttackAfter(CatRobot& catRobot);	// 攻撃終了後の隙
	static void UpdateAttackEnd(CatRobot& catRobot);		// 攻撃終了
	// 攻撃状態を変更させる
	void ChangeAttackState(CAT_ROBOT_ATTACK_STATE state);

	// 被ダメージ状態
	// 状態別初期化
	static void InitHitStart(CatRobot& catRobot);		// ダメージ開始
	static void InitHitStun(CatRobot& catRobot);		// ダメージ中
	static void InitHitEnd(CatRobot& catRobot);		// 復帰
	// 状態別更新
	static void UpdateHitStart(CatRobot& catRobot);	// ダメージ開始
	static void UpdateHitStun(CatRobot& catRobot);	// ダメージ中
	static void UpdateHitEnd(CatRobot& catRobot);		// 復帰
	// 被ダメージ状態を変更させる
	void ChangeHitState(CAT_ROBOT_HIT_STATE state);

private:
	// コンポーネント追加
	void AddComponets(void);

	// 重力
	void ApplyGravity(void);

	// 無敵時間を更新
	void UpdateInvincible(void);

	// 移動処理
	void Move(void);

	// 距離に応じて移動状態を更新する
	void UpdateMovementState(void);

	// 移動方向を算出 
	// 引数をtrueにするとプレイヤーから離れる方向を算出
	void GetDirectionToPlayer(bool isRetreat = false);

	// ダメージを与える
	void SetDamage(void);

	// 衝突判定の検知
	void OnCollision(Collider* other) override;

private:
	// コンポーネント
	Transform* transform_ = nullptr;
	Render3D* render3D_ = nullptr;
	Animation* animation_ = nullptr;
	CapsuleCollider* capsuleCollder_ = nullptr;

	// プレイヤーTransform
	const Transform* playerTransform_ = nullptr;

private:
	// 猫ロボット情報
	catRobotInfo info_;

	// 猫ロボット状態情報
	catRobotStateCtrl stateCtrl_;
};

