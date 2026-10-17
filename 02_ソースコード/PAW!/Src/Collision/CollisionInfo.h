#pragma once

#include <DxLib.h>

// 当たり判定の分類
enum class CollisionTag
{
	NONE,			// 未分類
	PLAYER,			// プレイヤー本体
	ENEMY,			// 敵本体
	PLAYER_ATTACK,	// プレイヤーの攻撃
	ENEMY_ATTACK,	// 敵の攻撃
	STAGE,			// 地形
	OBJECT,			// 押せるオブジェクト
	ITEM			// アイテム
};

struct CollisionInfo
{
	bool hit = false;	// 衝突したかどうか

	// AからBに向かう接触方向
	VECTOR normal = VGet(0.0f, 0.0f, 0.0f);

	// 当たり判定同士の重なり量
	float penetration = 0.0f;

	// 接触位置
	VECTOR point = VGet(0.0f, 0.0f, 0.0f);
};
