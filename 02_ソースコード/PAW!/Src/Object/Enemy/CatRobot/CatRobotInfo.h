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
	CAT_ROBOT_STATE_CHANCE,		// プレイヤーの攻撃チャンス
	CAT_ROBOT_STATE_DEAD,		// 死亡
	CAT_ROBOT_STATE_END,		// 終了

	CAT_ROBOT_STATE_MAX
};

// 猫ロボット攻撃状態
enum CAT_ROBOT_ATTACK_STATE
{
	CAT_ROBOT_ATTACK_SELECT,	// 攻撃選択
	CAT_ROBOT_ATTACK_1,			// 食材爆撃
	CAT_ROBOT_ATTACK_2,			// お皿攻撃上下
	CAT_ROBOT_ATTACK_3,			// お皿攻撃左右
	CAT_ROBOT_ATTACK_4,			// ナイフ攻撃
	CAT_ROBOT_ATTACK_END,		// 攻撃終了

	CAT_ROBOT_ATTACK_MAX
};

// 猫ロボットチャンス状態
enum CAT_ROBOT_CHANCE_STATE
{
	CAT_ROBOT_CHANCE_START,	// 攻撃を受けられる位置まで移動する
	CAT_ROBOT_CHANCE_STUN,	// スタンして無防備になる(プレイヤーの攻撃チャンス)
	CAT_ROBOT_CHANCE_END,	// スタンから回復し、通常行動へ戻る

	CAT_ROBOT_CHANCE_MAX
};

// 状態遷移
struct catRobotStateCtrl
{
	// 猫ロボット状態
	// ステート
	CAT_ROBOT_STATE state_;
	// ステートテーブル
	catRobotStateFunction initStateTable_[CAT_ROBOT_STATE_MAX];
	catRobotStateFunction updateStateTable_[CAT_ROBOT_STATE_MAX];

	// 猫ロボット攻撃状態
	// ステート
	CAT_ROBOT_ATTACK_STATE attackState_;
	// ステートテーブル
	catRobotStateFunction initAttackTable_[CAT_ROBOT_ATTACK_MAX];
	catRobotStateFunction updateAttackTable_[CAT_ROBOT_ATTACK_MAX];

	// 猫ロボット被ダメージ状態
	// ステート
	CAT_ROBOT_CHANCE_STATE chanceState_;
	// ステートテーブル
	catRobotStateFunction initChanceTable_[CAT_ROBOT_CHANCE_MAX];
	catRobotStateFunction updateChanceTable_[CAT_ROBOT_CHANCE_MAX];
};

struct catRobotInfo
{
	float velocityY_ = 0.0f;	// 現在の落下速度

	float velocityRot_ = 0.0f;	// 現在の角速度

	VECTOR moveDir_ = {};	// 移動方向

	float moveSpeed_ = 0;	// 移動速度

	int hp_ = 0;	// 今現在のHP

	int invincibleTime_ = 0;	// 無敵時間

	int waitTime_ = 0;	// 待機時間
};
