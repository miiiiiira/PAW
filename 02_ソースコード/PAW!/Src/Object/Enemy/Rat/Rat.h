#pragma once

#include "../../Component/Component.h"
#include "../../Component/Transform/Transform.h"
#include "RatInfo.h"

// 前方宣言
class Transform;
class Render3D;
class Animation;

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
	RAT_STATE GetState(void) { return stateCtrl_.state_; }

	// 無敵時間を渡す
	int GetInvincibleTime(void) { return info_.invincibleTime_; }

	// プレイヤーの座標ポインタを取得
	void SetPlayerPos(const VECTOR* vec) { playerPos_ = vec; }

	// ダメージを与える
	void SetDamage(int damage);

	// 吹っ飛びリアクションをさせる
	void SetHitReact(VECTOR moveDir, float moveSpeed, float jumpPow);

private:
	// 状態別更新処理
	void UpdateState(void);

	// 行動状態
	// 状態別初期化
	static void InitIdle(Rat& rat);		// 待機
	static void InitMove(Rat& rat);		// 移動
	static void InitAttack(Rat& rat);	// 攻撃
	static void InitHit(Rat& rat);		// ダメージを受けた
	static void InitDead(Rat& rat);		// 死亡
	static void InitEnd(Rat& rat);		// 終了
	// 状態別更新
	static void UpdateIdle(Rat& rat);	// 待機
	static void UpdateMove(Rat& rat);	// 移動
	static void UpdateAttack(Rat& rat);	// 攻撃
	static void UpdateHit(Rat& rat);	// ダメージを受けた
	static void UpdateDead(Rat& rat);	// 死亡
	static void UpdateEnd(Rat& rat);	// 終了
	// 状態を変更させる
	void ChangeState(RAT_STATE state);

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
	// 重力
	void ApplyGravity(void);

	// 無敵時間を更新
	void UpdateInvincible(void);

	// 移動処理
	void Move(void);

	// 移動方向を算出 （引数をtrueにするとプレイヤーから離れる方向を算出）
	void GetDirectionToPlayer(bool isRetreat = false);

	// プレイヤーに向く
	void GetAngleToPlayer(void);

private:
	// コンポーネント
	Transform* transform_ = nullptr;	
	Render3D* render3D_ = nullptr;
	Animation* animation_ = nullptr;

	// プレイヤー座標
	const VECTOR* playerPos_ = nullptr;

private:
	// ねずみ情報
	ratInfo info_;

	// ねずみの状態情報
	ratStateCtrl stateCtrl_;
};

