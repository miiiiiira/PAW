#pragma once
#include <DxLib.h>

class Attack1; // 前方宣言

// 状態関数型
typedef void(*attack1StateFunction)(Attack1&);

// 攻撃状態
enum ATTACK1_STATE
{
	ATTACK1_STATE_IDLE,		// 待機中
	ATTACK1_STATE_MOVE,		// 移動中
	ATTACK1_STATE_END,		// 終了

	ATTACK1_STATE_MAX
};

// 状態遷移
struct attack1StateCtrl
{
	// 猫ロボット状態
	// ステート
	ATTACK1_STATE state_;
	// ステートテーブル
	attack1StateFunction initStateTable_[ATTACK1_STATE_MAX];
	attack1StateFunction updateStateTable_[ATTACK1_STATE_MAX];
};

struct attack1Info
{
	VECTOR moveDir_ = {};	// 移動方向

	float moveSpeed_ = 0;	// 移動速度

	int idleCount_ = 0;		// 待機時カウント（待機時に攻撃範囲見せる用）
};