#pragma once

#include <DxLib.h>

#include "../Component/Component.h"
#include "../Component/Transform/Transform.h"
#include "PlayerInfo.h"

// 前方宣言
class Transform;

// プレイヤー制御コンポーネント
class Player : public Component
{
public:
	// プレイヤーのカプセルオフセット
	static constexpr VECTOR STANDING_CAP_END_OFFSET = { 0.0f,30.0f,0.0f };		// エンド位置
	static constexpr VECTOR STANDING_CAP_START_OFFSET = { 0.0f,150.0f,0.0f };	// 立ち状態スタート位置
	static constexpr VECTOR MOVE_CAP_START_OFFSET = { 0.0f,135.0f,0.0f };		// 移動状態スタート位置
	static constexpr VECTOR CROUCHING_CAP_START_OFFSET = { 0.0f,60.0f,0.0f };	// しゃがみ状態スタート位置

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
	void Draw3D(void) override;	// 3D描画

public:
	// Transformを返す
	Transform* GetTransform(void) { return transform_; }
	
	// プレイヤーの頭座標を返す
	VECTOR GetPlayerHeadPos(void) { return { transform_->pos_.x,transform_->pos_.y + HEAD_POS_OFFSET ,transform_->pos_.z }; }

	// プレイヤー状態を取得
	PLAYER_STATE GetState(void) { return stateCtrl_.state_; }
	
	// 無敵時間を渡す
	int GetInvincibleTime(void) { return info_.invincibleTime_; }
	
	// ダメージを与える
	void SetDamage(int damage);	
	
	// 吹っ飛びリアクションをさせる
	void SetHitReact(VECTOR moveDir,float moveSpeed,float jumpPow);	

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
	// 状態別更新
	static void UpdateAttack1(Player& player);	// 攻撃1
	static void UpdateAttack2(Player& player);	// 攻撃2
	static void UpdateAttack3(Player& player);	// 攻撃3
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
	// 頭座標
	static constexpr float HEAD_POS_OFFSET = 100.0f;

private:
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

	// HP描画
	void DrawHP(void);		

	// デバッグ用描画
	void DebugDraw(void);	

private:
	// Transform
	Transform* transform_ = nullptr;		

private:
	// プレイヤー情報
	playerInfo info_;	

	// プレイヤーの状態情報
	playerStateCtrl stateCtrl_;			
};
