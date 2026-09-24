#pragma once
#include "../Component/Component.h"
#include "PlayerInfo.h"
#include <DxLib.h>

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

public:

	Player(void);		// コンストラクタ

	void Load() override;		// 読み込み
	void Init() override;		// 初期化
	void Update() override;		// 更新
	void Draw2D() override;		// 2D描画

public:

	Transform* GetTransform();	// Transformを返す
	
	PLAYER_STATE GetState(void);	// プレイヤー状態を取得
	
	int GetInvincibleTime(void);	// 無敵時間を渡す
	
	void SetDamage(int damage);	// ダメージを与える
	
	void SetHitReact(VECTOR moveDir,float moveSpeed,float jumpPow);	// 吹っ飛びリアクションをさせる

private:
	
	void StateUpdate(void);	// 状態別更新処理

	// 状態別初期化
	static void IdleInit(Player& player);			// 待機
	static void MoveInit(Player& player);			// 移動
	static void AttackInit(Player& player);			// 攻撃
	static void DodgeInit(Player& player);			// 回避
	static void HitInit(Player& player);			// ダメージを受けた
	static void DeadInit(Player& player);			// 死亡
	static void EndInit(Player& player);			// 終了

	// 状態別更新
	static void IdleUpdate(Player& player);			// 待機
	static void MoveUpdate(Player& player);			// 移動
	static void AttackUpdate(Player& player);		// 攻撃
	static void DodgeUpdate(Player& player);		// 回避
	static void HitUpdate(Player& player);			// ダメージを受けた
	static void DeadUpdate(Player& player);			// 死亡
	static void EndUpdate(Player& player);			// 終了

	void ChangeState(PLAYER_STATE state);	// 状態を変更させる

	void ApplyGravity(void);	// 重力

	void Jump(void);	// ジャンプ

	void InvincibleUodate(void);	// 無敵時間を更新

	void HitStopUodate(void);	// ヒットストップ更新
	
	void GetShakeOffset(int& offset);	// ヒットストップカウンタが0じゃない場合に揺らし量を計算

	bool InputMove(void);	// 移動しているかを渡す		true / 移動している, false / 移動していない

	void Move(void);	// 方向×移動速度で移動量を作って、座標に足して移動させる

	void IsReachedDeadPos(void);	// 死亡座標へ到達しているか

	void DrawHP(void);		// HP描画

	void DebugDraw(void);	// デバッグ用描画

private:

	// リミット設定
	static constexpr float DAMAGE_POS_Y = -2000.0f;	// プレイヤーがダメージを受ける座標
	static constexpr int INVINCIBLE_TIME = 120;	// 無敵時間

	// 移動設定
	static constexpr float MOVE_SPEED = 7.0f;	// 移動速度

	// 体力設定
	static constexpr int DEFAULT_HP = 5;	// 初期HP

	// 重力
	static constexpr float JUMP_POW = 25.0f;	// ジャンプ力
	static constexpr float GRAVITY = -1.98f;	// 重力加速度
	static constexpr float MAX_FALL = -40.0f;	// 最大落下速度

	// ダメージ設定
	static constexpr float HIT_REACT_FRICTION = 0.5f;	// ダメージ時のリアクション時の摩擦
	static constexpr int SHAKE_TIME = 20;				// 揺らす時間

	// 描画設定
	static constexpr int  STATUS_DRAW_POS_X = 10;		// ステータス描画を始める座標
	static constexpr int  HP_DRAW_POS_Y = 50;			// HPの描画Y軸
	static constexpr int STATUS_DRAW_POS_OFFSET = 10;	// オフセット
	static constexpr float COEFFICIENT = 0.2f;			// 線形補間の係数

private:

	// コンポーネント
	Transform* transform_ = nullptr;		// Transform

private:

	// プレイヤー情報
	playerInfo info_;	

	playerStateCtrl stateCtrl_;			// プレイヤーの状態情報
};
