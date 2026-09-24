#pragma once
#include <DxLib.h>

class Player; // 前方宣言

// 状態関数型
typedef void(*playerStateFunction)(Player&);

// プレイヤーの状態
enum PLAYER_STATE
{
	PLAYER_STATE_IDLE,		// 待機
	PLAYER_STATE_MOVE,		// 移動
	PLAYER_STATE_ATTACK,	// 攻撃
	PLAYER_STATE_DODGE,		// 回避
	PLAYER_STATE_HIT,		// ダメージを受けた
	PLAYER_STATE_DEAD,		// 死亡
	PLAYER_STATE_END,		// 終了

	PLAYER_STATE_MAX
};

// 状態遷移
struct playerStateCtrl
{
	// ステート
	PLAYER_STATE state_;

	// ステートテーブル
	playerStateFunction initTable_[PLAYER_STATE_MAX];
	playerStateFunction updateTable_[PLAYER_STATE_MAX];
};

struct playerInfo
{
	float velocityY_ = 0.0f;	// 現在の落下速度

	VECTOR moveDir_ = {};	// 移動方向

	float moveSpeed_ = 0;	// 移動速度

	int hp_;	// 今現在のHP

	int invincibleTime_ = 0;	// 無敵時間

	bool jumpFlg = false;		// ジャンプ中か	true / ジャンプ中 , flase / ジャンプ中でない

	int hitStopCounter_ = 0;	// ヒットストップ用のカウンター
};