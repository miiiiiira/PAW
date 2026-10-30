#pragma once
#include <DxLib.h>

class PlateAttack; // 前方宣言

// 状態関数型
typedef void(*plateAttackStateFunction)(PlateAttack&);

// 攻撃状態
enum PLATE_ATTACK_STATE
{
	PLATE_ATTACK_STATE_IDLE,	// 待機中
	PLATE_ATTACK_STATE_MOVE,	// 移動中
	PLATE_ATTACK_STATE_END,		// 終了
	
	PLATE_ATTACK_STATE_MAX
};

// 状態遷移
struct attack2StateCtrl
{
	// ステート
	PLATE_ATTACK_STATE state_;
	// ステートテーブル
	plateAttackStateFunction initStateTable_[PLATE_ATTACK_STATE_MAX];
	plateAttackStateFunction updateStateTable_[PLATE_ATTACK_STATE_MAX];
};

struct attack2Info
{
	VECTOR moveDir_ = {};	// 移動方向

	float moveSpeed_ = 0;	// 移動速度

	VECTOR goalPos_ = {};			// 目的地

	float moveDistance_ = 0.0f;		// 移動距離

	int idleCount_ = 0;		// 待機時カウント（待機時に攻撃範囲見せる用）
};