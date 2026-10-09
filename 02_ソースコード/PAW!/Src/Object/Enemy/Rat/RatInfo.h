#pragma once

#include <DxLib.h>

class Rat; // 前方宣言

// 状態関数型
typedef void(*ratStateFunction)(Rat&);

// ネズミの状態
enum RAT_STATE
{
	RAT_STATE_IDLE,		// 待機中
	RAT_STATE_MOVE,		// 移動中
	RAT_STATE_RETREAT,	// 後退中
	RAT_STATE_ATTACK,	// 攻撃中
	RAT_STATE_HIT,		// 被ダメージ
	RAT_STATE_DEAD,		// 死亡
	RAT_STATE_END,		// 終了
	
	RAT_STATE_MAX
};

// ネズミ攻撃状態
enum RAT_ATTACK_STATE
{
	RAT_ATTACK_START,	// 攻撃始め
	RAT_ATTACKING,		// 攻撃中
	RAT_ATTACK_AFTER,	// 攻撃終了後の隙
	RAT_ATTACK_END,		// 攻撃終了

	RAT_ATTACK_MAX
};

// ネズミ被ダメージ状態
enum RAT_HIT_STATE
{
	RAT_HIT_START,	// ダメージ開始
	RAT_HIT_STUN,	// ダメージ中
	RAT_HIT_END,	// 復帰

	RAT_HIT_MAX
};

// 状態遷移
struct ratStateCtrl
{
	// プレイヤーの状態
	// ステート
	RAT_STATE state_;
	// ステートテーブル
	ratStateFunction initStateTable_[RAT_STATE_MAX];
	ratStateFunction updateStateTable_[RAT_STATE_MAX];

	// プレイヤー攻撃状態
	// ステート
	RAT_ATTACK_STATE attackState_;
	// ステートテーブル
	ratStateFunction initAttackTable_[RAT_ATTACK_MAX];
	ratStateFunction updateAttackTable_[RAT_ATTACK_MAX];

	// プレイヤー被ダメージ状態
	// ステート
	RAT_HIT_STATE hitState_;
	// ステートテーブル
	ratStateFunction initHitTable_[RAT_HIT_MAX];
	ratStateFunction updateHitTable_[RAT_HIT_MAX];
};

struct ratInfo
{
	float velocityY_ = 0.0f;	// 現在の落下速度

	float velocityRot_ = 0.0f;	// 現在の角速度

	VECTOR moveDir_ = {};	// 移動方向

	float moveSpeed_ = 0;	// 移動速度

	int hp_;	// 今現在のHP

	int invincibleTime_ = 0;	// 無敵時間
};

enum class RAT_ANIMATION
{
	ATTACK,
	ATTACK_START,
	DEAD,
	IDLE,
	MOVE,
	MOVE_CLOSER,

	MAX
};