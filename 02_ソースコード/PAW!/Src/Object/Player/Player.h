#pragma once

#include <DxLib.h>

#include "../Component/Component.h"
#include "../Component/Transform/Transform.h"
#include "PlayerInfo.h"

// 前方宣言
class Transform;
class Render3D;
class Animation;
class CapsuleCollider;
class SphereCollider;

// プレイヤー制御コンポーネント
class Player : public Component
{
public:
	// ステージの当たり判定用の法線
	static constexpr float FLOOR_NORMAL_Y = 0.85f;	// 床
	static constexpr float WALL_NORMAL_Y = 0.20f;	// 壁
	static constexpr float SLOPE_NORMAL_Y = 0.65f;	// 坂

	// 段差の登れる量
	static constexpr float STEP_HEIGHT = 25.0f;

public:
	Player(void);				// コンストラクタ

	void Load(void) override;	// 読み込み
	void Init(void) override;	// 初期化
	void Update(void) override;	// 更新
	void Draw2D(void) override;	// 2D描画

public:
	// Transformを返す
	Transform* GetTransform(void) { return transform_; }

	// プレイヤー状態を取得
	PLAYER_STATE GetState(void) { return stateCtrl_.state_; }

private:
	// 状態別更新処理
	void UpdateState(void);

	// 状態別初期化
	static void InitIdle(Player& player);		// 待機
	static void InitMove(Player& player);		// 移動
	static void InitAttack(Player& player);		// 攻撃
	static void InitAvoid(Player& player);		// 回避
	static void InitJump(Player& player);		// ジャンプ
	static void InitHit(Player& player);		// ダメージを受けた
	static void InitDead(Player& player);		// 死亡
	static void InitEnd(Player& player);		// 終了
	// 状態別更新
	static void UpdateIdle(Player& player);		// 待機
	static void UpdateMove(Player& player);		// 移動
	static void UpdateAttack(Player& player);	// 攻撃
	static void UpdateAvoid(Player& player);	// 回避
	static void UpdateJump(Player& player);		// ジャンプ
	static void UpdateHit(Player& player);		// ダメージを受けた
	static void UpdateDead(Player& player);		// 死亡
	static void UpdateEnd(Player& player);		// 終了
	// 状態を変更させる
	void ChangeState(PLAYER_STATE state);

	// 状態別初期化
	static void InitAttack1(Player& player);	// 攻撃1
	static void InitAttack2(Player& player);	// 攻撃2
	static void InitAttack3(Player& player);	// 攻撃3
	static void InitAttackEnd(Player& player);	// 攻撃修了
	// 状態別更新
	static void UpdateAttack1(Player& player);	// 攻撃1
	static void UpdateAttack2(Player& player);	// 攻撃2
	static void UpdateAttack3(Player& player);	// 攻撃3
	static void UpdateAttackEnd(Player& player);// 攻撃終了
	// 攻撃状態を変更させる
	void ChangeAttackState(PLAYER_ATTACK_STATE state);

	// 状態別初期化
	static void InitJumpStart(Player& player);		// ジャンプ開始
	static void InitJumping(Player& player);		// ジャンプ中
	static void InitJumpEnd(Player& player);		// ジャンプ終了
	// 状態別更新
	static void UpdateJumpStart(Player& player);	// ジャンプ開始
	static void UpdateJumping(Player& player);		// ジャンプ中
	static void UpdateJumpEnd(Player& player);		// ジャンプ終了
	// ジャンプ状態を変更させる
	void ChangeJumpState(PLAYER_JUMP_STATE state);

	// 状態別初期化
	static void InitHitStart(Player& player);	// ダメージ開始
	static void InitHitStun(Player& player);	// ダメージ中
	static void InitHitEnd(Player& player);		// 復帰
	// 状態別更新
	static void UpdateHitStart(Player& player);	// ダメージ開始
	static void UpdateHitStun(Player& player);	// ダメージ中
	static void UpdateHitEnd(Player& player);	// 復帰
	// 被ダメージ状態を変更させる
	void ChangeHitState(PLAYER_HIT_STATE state);

private:
	// コンポーネント追加
	void AddComponets(void);

	// 重力
	void ApplyGravity(void);

	// 無敵時間を更新
	void UpdateInvincible(void);

	// ヒットストップ更新
	void UpdateHitStop(void);

	// ヒットストップカウンタが0じゃない場合に揺らし量を計算
	void GetShakeOffset(int& offset);

	// 移動しているかを渡す		true / 移動している, false / 移動していない
	bool InputMove(void);

	// 方向×移動速度で移動量を作って、座標に足して移動させる
	void Move(void);

	// 死亡座標へ到達しているか
	void IsReachedDeadPos(void);

	// 角度処理
	void RotAngle(void);

	// ダメージを与える
	void SetDamage(int damage);

	// 吹っ飛びリアクションをさせる
	void SetHitReact(VECTOR moveDir);

	// 攻撃が当たった場合の後処理
	bool OnAttackHit(void);

	// アニメーション進行度に応じて攻撃判定のON/OFF
	// (攻撃の判定をスタートさせるフレーム)
	void UpdateAttackState(float StartAttackFramee);

	// 次の攻撃入力を受け付ける
	void AcceptNextAttackInput(void);

	// 攻撃状態を終了させる
	void EndAttaking(void);

	// 攻撃開始処理(アニメーション再生とフラグ初期化)
	void StartAttack(PLAYER_ANIMATION anim);

	// 衝突判定の検知
	void OnCollision(Collider* other) override;
	// 攻撃判定の検知
	void AttackOnCollision(void);

	// HP描画
	void DrawHP(void);

#ifdef _DEBUG

	// デバッグ用描画
	void DebugDraw2D(void);

#endif // _DEBUG

private:
	// コンポーネント
	Transform* transform_ = nullptr;
	Render3D* render3D_ = nullptr;
	Animation* animation_ = nullptr;
	CapsuleCollider* capsuleCollder_ = nullptr;
	SphereCollider* sphereCollder_ = nullptr;

private:
	// プレイヤー情報
	playerInfo info_;

	// プレイヤーの状態情報
	playerStateCtrl stateCtrl_;
};
