#pragma once
#include <DxLib.h>

class CatRobot; // 前方宣言

// 状態関数型
typedef void(*catRobotStateFunction)(CatRobot&);

// 猫ロボット状態
enum CAT_ROBOT_STATE
{
	CAT_ROBOT_STATE_IDLE,		// 待機中
	CAT_ROBOT_STATE_MOVE,		// 移動中
	CAT_ROBOT_STATE_ATTACK,		// 攻撃中
	CAT_ROBOT_STATE_HIT,		// 被ダメージ
	CAT_ROBOT_STATE_DEAD,		// 死亡
	CAT_ROBOT_STATE_END,		// 終了

	CAT_ROBOT_STATE_MAX
};

// 猫ロボット待機状態
enum CAT_ROBOT_IDLE_STATE
{
	CAT_ROBOT_IDLE_WAIT,		// 待機
	CAT_ROBOT_IDLE_SWAY,		// 左右に揺れる
	CAT_ROBOT_IDLE_APPROACH,	// ちょっと近づく
	
	CAT_ROBOT_IDLE_MAX
};

// 猫ロボット攻撃状態
enum CAT_ROBOT_ATTACK_STATE
{
	CAT_ROBOT_ATTACK_START,		// 攻撃始め
	CAT_ROBOT_ATTACKING,		// 攻撃中
	CAT_ROBOT_ATTACK_AFTER,		// 攻撃終了後の隙
	CAT_ROBOT_ATTACK_END,		// 攻撃終了

	CAT_ROBOT_ATTACK_MAX
};

// 猫ロボット被ダメージ状態
enum CAT_ROBOT_HIT_STATE
{
	CAT_ROBOT_HIT_START,// ダメージ開始
	CAT_ROBOT_HIT_STUN,	// ダメージ中
	CAT_ROBOT_HIT_END,	// 復帰

	CAT_ROBOT_HIT_MAX
};

// 状態遷移
struct ratStateCtrl
{
	// 猫ロボット状態
	// ステート
	CAT_ROBOT_STATE state_;
	// ステートテーブル
	catRobotStateFunction initStateTable_[CAT_ROBOT_STATE_MAX];
	catRobotStateFunction updateStateTable_[CAT_ROBOT_STATE_MAX];

	// 猫ロボット待機状態
	// ステート
	CAT_ROBOT_IDLE_STATE idleState_;
	// ステートテーブル
	catRobotStateFunction initIdleTable_[CAT_ROBOT_IDLE_MAX];
	catRobotStateFunction updateIdleTable_[CAT_ROBOT_IDLE_MAX];

	// 猫ロボット攻撃状態
	// ステート
	CAT_ROBOT_ATTACK_STATE attackState_;
	// ステートテーブル
	catRobotStateFunction initAttackTable_[CAT_ROBOT_ATTACK_MAX];
	catRobotStateFunction updateAttackTable_[CAT_ROBOT_ATTACK_MAX];

	// 猫ロボット被ダメージ状態
	// ステート
	CAT_ROBOT_HIT_STATE hitState_;
	// ステートテーブル
	catRobotStateFunction initHitTable_[CAT_ROBOT_HIT_MAX];
	catRobotStateFunction updateHitTable_[CAT_ROBOT_HIT_MAX];
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
};
