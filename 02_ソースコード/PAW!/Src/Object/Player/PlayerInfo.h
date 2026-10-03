#pragma once
#include <DxLib.h>

class Player; // 前方宣言

// 状態関数型
typedef void(*playerStateFunction)(Player&);

// プレイヤーの状態
enum PLAYER_STATE
{
	PLAYER_STATE_IDLE,	// 待機中
	PLAYER_STATE_MOVE,	// 移動中
	PLAYER_STATE_ATTACK,// 攻撃中
	PLAYER_STATE_AVOID,	// 回避中
	PLAYER_STATE_JUMP,	// ジャンプ中
	PLAYER_STATE_HIT,	// 被ダメージ
	PLAYER_STATE_DEAD,	// 死亡
	PLAYER_STATE_END,	// 終了

	PLAYER_STATE_MAX
};

// プレイヤー攻撃状態
enum PLAYER_ATTACK_STATE
{
	PLAYER_ATTACK_1,	// 攻撃1
	PLAYER_ATTACK_2,	// 攻撃2
	PLAYER_ATTACK_3,	// 攻撃3

	PLAYER_ATTACK_MAX
};

// プレイヤージャンプ状態
enum PLAYER_JUMP_STATE
{
	PLAYER_JUMP_START,	// ジャンプ開始
	PLAYER_JUMPING,		// ジャンプ中
	PLAYER_JUMP_END,	// ジャンプ終了

	PLAYER_JUMP_MAX
};

// プレイヤー被ダメージ状態
enum PLAYER_HIT_STATE
{
	PLAYER_HIT_START,	// ダメージ開始
	PLAYER_HIT_STUN,	// ダメージ中
	PLAYER_HIT_END,		// 復帰

	PLAYER_HIT_MAX
};

// 状態遷移
struct playerStateCtrl
{
	// プレイヤーの状態
	// ステート
	PLAYER_STATE state_;
	// ステートテーブル
	playerStateFunction initStateTable_[PLAYER_STATE_MAX];
	playerStateFunction updateStateTable_[PLAYER_STATE_MAX];

	// プレイヤー攻撃状態
	// ステート
	PLAYER_ATTACK_STATE attackState_;
	// ステートテーブル
	playerStateFunction initAttackTable_[PLAYER_ATTACK_MAX];
	playerStateFunction updateAttackTable_[PLAYER_ATTACK_MAX];

	// プレイヤージャンプ状態
	// ステート
	PLAYER_JUMP_STATE jumpState_;
	// ステートテーブル
	playerStateFunction initJumpTable_[PLAYER_JUMP_MAX];
	playerStateFunction updateJumpTable_[PLAYER_JUMP_MAX];

	// プレイヤー被ダメージ状態
	// ステート
	PLAYER_HIT_STATE hitState_;
	// ステートテーブル
	playerStateFunction initHitTable_[PLAYER_HIT_MAX];
	playerStateFunction updateHitTable_[PLAYER_HIT_MAX];
};

struct playerInfo
{
	float velocityY_ = 0.0f;	// 現在の落下速度

	VECTOR moveDir_ = {};	// 移動方向

	float moveSpeed_ = 0;	// 移動速度

	int hp_;	// 今現在のHP

	VECTOR avoidPos_ = {};	// 回避先の座標

	int invincibleTime_ = 0;	// 無敵時間

	int hitStopCounter_ = 0;	// ヒットストップ用のカウンター
};