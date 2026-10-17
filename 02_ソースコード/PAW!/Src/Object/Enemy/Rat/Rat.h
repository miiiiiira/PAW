#pragma once

#include "../../Component/Component.h"
#include "../../Component/Transform/Transform.h"
#include "RatInfo.h"

// 前方宣言
class Transform;
class Render3D;
class Animation;
class CapsuleCollider;

class Rat : public Component
{
public:
	Rat(void);					// コンストラクタ

	void Load(void) override;	// 読み込み
	void Init(void) override;	// 初期化
	void Update(void) override;	// 更新
	void Draw2D(void) override;	// 2D描画
	void Draw3D(void) override;	// 3D描画

public:
	// Transformを返す
	Transform* GetTransform(void) { return transform_; }

	// 状態を取得
	RAT_STATE GetState(void)const { return stateCtrl_.state_; }

	// 無敵時間を渡す
	int GetInvincibleTime(void)const { return info_.invincibleTime_; }

	// プレイヤーのTransformポインタを取得
	void SetPlayerTransform(const Transform* transform) { playerTransform_ = transform; }

private:
	// 状態別更新処理
	void UpdateState(void);

	// 行動状態
	// 状態別初期化
	static void InitIdle(Rat& rat);			// 待機
	static void InitMove(Rat& rat);			// 移動
	static void InitRetreat(Rat& rat);		// 後退
	static void InitAttack(Rat& rat);		// 攻撃
	static void InitHit(Rat& rat);			// ダメージを受けた
	static void InitDead(Rat& rat);			// 死亡
	static void InitEnd(Rat& rat);			// 終了
	// 状態別更新
	static void UpdateIdle(Rat& rat);		// 待機
	static void UpdateMove(Rat& rat);		// 移動
	static void UpdateRetreat(Rat& rat);	// 後退
	static void UpdateAttack(Rat& rat);		// 攻撃
	static void UpdateHit(Rat& rat);		// ダメージを受けた
	static void UpdateDead(Rat& rat);		// 死亡
	static void UpdateEnd(Rat& rat);		// 終了
	// 状態を変更させる
	void ChangeState(RAT_STATE state);

	// 待機状態
	// 状態別初期化
	static void InitIdleWait(Rat& rat);			// 待機
	static void InitIdleSway(Rat& rat);			// 左右に揺れる
	static void InitIdleApproach(Rat& rat);		// ちょっと近づく
	// 状態別更新
	static void UpdateIdleWait(Rat& rat);		// 待機
	static void UpdateIdleSway(Rat& rat);		// 左右に揺れる
	static void UpdateIdleApproach(Rat& rat);	// ちょっと近づく
	// 待機状態を変更させる
	void ChangeIdleState(RAT_IDLE_STATE state);

	// 攻撃状態
	// 状態別初期化
	static void InitAttackStart(Rat& rat);		// 攻撃始め
	static void InitAttacking(Rat& rat);		// 攻撃中
	static void InitAttackAfter(Rat& rat);		// 攻撃終了後の隙
	static void InitAttackEnd(Rat& rat);		// 攻撃終了
	// 状態別更新
	static void UpdateAttackStart(Rat& rat);	// 攻撃始め
	static void UpdateAttacking(Rat& rat);		// 攻撃中
	static void UpdateAttackAfter(Rat& rat);	// 攻撃終了後の隙
	static void UpdateAttackEnd(Rat& rat);		// 攻撃終了
	// 攻撃状態を変更させる
	void ChangeAttackState(RAT_ATTACK_STATE state);

	// 被ダメージ状態
	// 状態別初期化
	static void InitHitStart(Rat& rat);		// ダメージ開始
	static void InitHitStun(Rat& rat);		// ダメージ中
	static void InitHitEnd(Rat& rat);		// 復帰
	// 状態別更新
	static void UpdateHitStart(Rat& rat);	// ダメージ開始
	static void UpdateHitStun(Rat& rat);	// ダメージ中
	static void UpdateHitEnd(Rat& rat);		// 復帰
	// 被ダメージ状態を変更させる
	void ChangeHitState(RAT_HIT_STATE state);

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

	// ネズミからの左右を算出
	// 引数をtrueにすると右方向を算出
	void GetDirectionLR(bool isRight = true);

	// プレイヤーに向く
	void GetAngleToPlayer(void);

	// プレイヤーとの距離を算出
	float GetDistanceToPlayer(void);

	// 待機時の揺れ処理用のランダム系の初期化
	void InitSwayRandom(void);

	// ちょっと近づく時の距離を算出
	void SetApproachDistance(void);

	// ダメージを与える
	void SetDamage(void);

	// 吹っ飛びリアクションをさせる
	void SetHitReact(VECTOR moveDir);

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
	// ネズミ情報
	ratInfo info_;

	// ネズミの状態情報
	ratStateCtrl stateCtrl_;
};

