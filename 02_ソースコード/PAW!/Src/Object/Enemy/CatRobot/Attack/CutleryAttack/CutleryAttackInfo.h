#pragma once
#include <DxLib.h>

class CutleryAttack; // 前方宣言

// 状態関数型
typedef void(*cutleryAttackStateFunction)(CutleryAttack&);

// 攻撃状態
enum CUTLERY_ATTACK_STATE
{
	CUTLERY_ATTACK_STATE_IDLE,	// 待機中
	CUTLERY_ATTACK_STATE_MOVE,	// 移動中
	CUTLERY_ATTACK_STATE_END,	// 終了
	
	CUTLERY_ATTACK_STATE_MAX
};

// 状態遷移
struct attack3StateCtrl
{
	// ステート
	CUTLERY_ATTACK_STATE state_;
	// ステートテーブル
	cutleryAttackStateFunction initStateTable_[CUTLERY_ATTACK_STATE_MAX];
	cutleryAttackStateFunction updateStateTable_[CUTLERY_ATTACK_STATE_MAX];
};

struct attack3Info
{
	VECTOR moveDir_ = {};	// 移動方向

	float moveSpeed_ = 0;	// 移動速度

	VECTOR goalPos_ = {};			// 目的地

	float moveDistance_ = 0.0f;		// 移動距離

	int idleCount_ = 0;		// 待機時カウント（待機時に攻撃範囲見せる用）
};