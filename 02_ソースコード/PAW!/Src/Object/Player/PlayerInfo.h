#pragma once

#include <DxLib.h>

class Player; // 前方宣言

// 状態関数型
typedef void(*playerStateFunction)(Player&);

static constexpr int HP_UI_DIV = 6;

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
	PLAYER_ATTACK_END,	// 攻撃終了

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
	PLAYER_STATE state_ = PLAYER_STATE_MAX;
	// ステートテーブル
	playerStateFunction initStateTable_[PLAYER_STATE_MAX];
	playerStateFunction updateStateTable_[PLAYER_STATE_MAX];

	// プレイヤー攻撃状態
	// ステート
	PLAYER_ATTACK_STATE attackState_ = PLAYER_ATTACK_MAX;
	// ステートテーブル
	playerStateFunction initAttackTable_[PLAYER_ATTACK_MAX];
	playerStateFunction updateAttackTable_[PLAYER_ATTACK_MAX];

	// プレイヤージャンプ状態
	// ステート
	PLAYER_JUMP_STATE jumpState_ = PLAYER_JUMP_MAX;
	// ステートテーブル
	playerStateFunction initJumpTable_[PLAYER_JUMP_MAX];
	playerStateFunction updateJumpTable_[PLAYER_JUMP_MAX];

	// プレイヤー被ダメージ状態
	// ステート
	PLAYER_HIT_STATE hitState_ = PLAYER_HIT_MAX;
	// ステートテーブル
	playerStateFunction initHitTable_[PLAYER_HIT_MAX];
	playerStateFunction updateHitTable_[PLAYER_HIT_MAX];
};

struct playerInfo
{
	float velocityY_ = 0.0f;	// 現在の落下速度

	float velocityRot_ = 0.0f;	// 現在の角速度

	VECTOR moveDir_ = {0.0f,0.0f,0000000.1f};	// 移動方向
	VECTOR prevMoveDir_ = {};					// 前の移動方向

	float moveSpeed_ = 0;	// 移動速度

	int hp_ = 0;	// 今現在のHP

	VECTOR avoidPos_ = {};	// 回避先の座標

	int invincibleTime_ = 0;	// 無敵時間

	int hitStopCounter_ = 0;	// ヒットストップ用のカウンター

	bool isAttackHit_ = false;	// 攻撃が当たったか		true / 当たった , false / 当たっていない
	
	PLAYER_ATTACK_STATE currentAttack_ = PLAYER_ATTACK_MAX;	// 攻撃中の種類を入れる
	PLAYER_ATTACK_STATE nextAttack_ = PLAYER_ATTACK_MAX;	// 攻撃予約された種類を入れる

	std::vector<const Transform*> attackAssistCandidates_;	// 補正対象の候補
	const Transform* finalCandidateTransform_;				// 補正対象の最終候補

};

struct playerUiInfo
{
	// 画像ハンドル
	int hpImage_[HP_UI_DIV];	// HP
};

enum class PLAYER_ANIMATION
{
	STANDING_IDLE,
	SITTING_IDLE,
	WALK,
	RUN,
	JUMP,
	PUNCH,
	PUNCH_2,
	PUNCH_3,
	SCRATCH,
	LOAF,
	GROOMING,

	MAX
};