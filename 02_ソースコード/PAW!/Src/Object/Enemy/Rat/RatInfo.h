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

// ネズミ待機状態
enum RAT_IDLE_STATE
{
	RAT_IDLE_WAIT,		// 待機
	RAT_IDLE_SWAY,		// 左右に揺れる
	RAT_IDLE_APPROACH,	// ちょっと近づく

	RAT_IDLE_MAX
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
	// ネズミの状態
	// ステート
	RAT_STATE state_;
	// ステートテーブル
	ratStateFunction initStateTable_[RAT_STATE_MAX];
	ratStateFunction updateStateTable_[RAT_STATE_MAX];

	// ネズミ待機状態
	// ステート
	RAT_IDLE_STATE idleState_;
	// ステートテーブル
	ratStateFunction initIdleTable_[RAT_IDLE_MAX];
	ratStateFunction updateIdleTable_[RAT_IDLE_MAX];

	// ネズミ攻撃状態
	// ステート
	RAT_ATTACK_STATE attackState_;
	// ステートテーブル
	ratStateFunction initAttackTable_[RAT_ATTACK_MAX];
	ratStateFunction updateAttackTable_[RAT_ATTACK_MAX];

	// ネズミ被ダメージ状態
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

	int hp_ = 0;	// 今現在のHP

	int invincibleTime_ = 0;	// 無敵時間

	int waitTime_ = 0;	// 待機時間

	// 待機時の揺れ
	int swayTime_ = 0;			// 揺れ時間
	int swayTotalNum_ = 0;		// 実際に揺れた回数
	int swayNum_ = 0;			// 揺れ回数
	bool isRightSway_ = true;	// 右へ揺れるか　	true / 右へ揺れる, false / 左へ揺れる

	// 待機時のちょっと近づく
	float approachDistance_ = 0.0f;	// ちょっと近づく距離
	bool isApproaching_ = true;		// 近づき中か	　true / 近づく , false/ 遠ざかる
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