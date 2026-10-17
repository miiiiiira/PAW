#include "CollisionManager.h"
#include "../Object/Object.h"

#include <algorithm>

CollisionManager* CollisionManager::instance_ = nullptr;

CollisionManager::CollisionManager() {}
CollisionManager::~CollisionManager() {}

void CollisionManager::Register(Collider* col)
{
	// nullptrは登録しない
	if (!col) return;

	// 同じコライダーが既に登録されている場合は追加しない
	if (std::find(colliders_.begin(), colliders_.end(), col) != colliders_.end())
	{
		return;
	}

	// 登録リストに追加する
	colliders_.push_back(col);
}

void CollisionManager::CheckAll()
{
	// 登録されたコライダーを順番に取り出す
	for (size_t i = 0; i < colliders_.size(); ++i)
	{
		Collider* a = colliders_[i];

		// コライダーまたは所有オブジェクトが無効な場合はスキップする
		if (!a || !a->GetOwner())
			continue;

		// Aより後ろにあるコライダーとの組み合わせを調べる
		for (size_t j = i + 1; j < colliders_.size(); ++j)
		{
			Collider* b = colliders_[j];

			// コライダーまたは所有オブジェクトが無効な場合はスキップする
			if (!b || !b->GetOwner())
				continue;

			// 同じオブジェクトに属するコライダー同士は判定しない
			if (a->GetOwner() == b->GetOwner())
				continue;

			// タグの組み合わせから、衝突判定が必要か確認する
			if (!ShouldCheckCollision(a->GetCollisionTag(), b->GetCollisionTag()))
				continue;

			// 当たり判定をチェック
			if (!a->CheckCollision(b))
				continue;

			// 衝突情報を元に、重なったオブジェクトの位置を補正する
			ResolveCollision(a, b, a->GetCollisionInfo());

			// 衝突したことを双方のオブジェクトに通知する
			a->GetOwner()->SendCollision(b);
			b->GetOwner()->SendCollision(a);
		}
	}
}

void CollisionManager::Clear()
{
	// 登録リストを空にする
	colliders_.clear();
}

bool CollisionManager::ShouldCheckCollision(CollisionTag a, CollisionTag b) const
{
	// NONEが設定されているコライダーは判定対象外とする
	if (a == CollisionTag::NONE || b == CollisionTag::NONE)
	{
		return false;
	}

	// プレイヤー本体とプレイヤーの攻撃判定は判定対象外とする
	if ((a == CollisionTag::PLAYER && b == CollisionTag::PLAYER_ATTACK) ||
		(a == CollisionTag::PLAYER_ATTACK && b == CollisionTag::PLAYER))
	{
		return false;
	}

	// 敵本体と敵の攻撃判定は判定対象外とする
	if ((a == CollisionTag::ENEMY && b == CollisionTag::ENEMY_ATTACK) ||
		(a == CollisionTag::ENEMY_ATTACK && b == CollisionTag::ENEMY))
	{
		return false;
	}

	// 同じ種類の攻撃判定同氏は判定対象外とする
	if ((a == CollisionTag::PLAYER_ATTACK && b == CollisionTag::PLAYER_ATTACK) ||
		(a == CollisionTag::ENEMY_ATTACK && b == CollisionTag::ENEMY_ATTACK))
	{
		return false;
	}

	// 上記以外の組み合わせは衝突判定を行う
	return true;
}

void CollisionManager::ResolveCollision(Collider* a, Collider* b, const CollisionInfo& info)
{
	// 情報がなければ何もしない
	if (!a || !b || !info.hit)
		return;

	// 逆質量を取得する
	// 逆質量が0のオブジェクトは、衝突による位置補正を行わない
	float invA = a->GetInverseMass();
	float invB = b->GetInverseMass();
	float total = invA + invB;

	// 両方とも動かせない場合は補正する必要がない
	if (total <= 0.0f)
		return;

	// オブジェクトのTransformを取得する
	auto transformA = a->GetOwner()->GetComponent<Transform>();
	auto transformB = b->GetOwner()->GetComponent<Transform>();

	// Transformを取得できない場合は処理しない
	if (!transformA || !transformB)
		return;

	// わずかな重なりは許容し、位置補正による振動を抑える
	const float slop = 0.01f;

	// 重なりを解消する際に適用する補正割合
	const float percent = 0.8f;

	// 逆質量に応じて、それぞれのオブジェクトに適用する補正量を計算する
	float correction = max(info.penetration - slop, 0.0f) * percent / total;

	// 衝突法線に補正量を掛け、位置補正用のベクトルを求める
	VECTOR offset = VScale(info.normal, correction);

	// Aを衝突法線と反対方向へ移動させる
	transformA->pos_ = VSub(transformA->pos_, VScale(offset, invA));

	// Bを衝突法線と反対方向へ移動させる
	transformB->pos_ = VAdd(transformB->pos_, VScale(offset, invB));
}

#ifdef _DEBUG
void CollisionManager::DrawDebug() const
{
	if (!debugDrawEnabled_)
		return;

	for (auto* col : colliders_)
	{
		// 無効なコライダーはスキップする
		if (!col || !col->GetOwner())
			continue;

		// 各コライダーの形状に応じたデバッグ描画を行う
		col->DebugDraw();
	}
}

void CollisionManager::SetDebugDrawEnabled(bool enabled)
{
	debugDrawEnabled_ = enabled;
}
#endif // _DEBUG
