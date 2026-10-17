#pragma once
#include <vector>
#include "Collider.h"
#include "CollisionInfo.h"

class CollisionManager
{

public:
	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new CollisionManager(); } }
	static CollisionManager* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

private:
	// 静的インスタンス
	static CollisionManager* instance_;

	CollisionManager();		// コンストラクタ
	~CollisionManager();	// デストラクタ

	// コピー・ムーブ操作を禁止
	CollisionManager(const CollisionManager&) = delete;
	CollisionManager& operator=(const CollisionManager&) = delete;
	CollisionManager(CollisionManager&&) = delete;
	CollisionManager& operator=(CollisionManager&&) = delete;

public:

	// 衝突判定の対象となるコライダーを登録する
	void Register(Collider* col);

	// 登録されたコライダー同士の衝突判定を行う
	void CheckAll();

	// 登録されたコライダーを全て解除する
	void Clear();

private:
	// コライダーのタグに応じて衝突判定の対象を絞り込む
	bool ShouldCheckCollision(CollisionTag a, CollisionTag b) const;

	// 衝突情報をもとに、オブジェクトの位置を補正する
	void ResolveCollision(Collider* a, Collider* b,const CollisionInfo& info);

	// 衝突判定の対象となるコライダーを保持する
	std::vector<Collider*> colliders_;


#ifdef _DEBUG
public:
	// 登録されたコライダーの当たり判定をデバッグ表示する
	void DrawDebug() const;

	// デバッグ表示の ON / OFF を切り替える
	void SetDebugDrawEnabled(bool enabled);

	// デバッグ表示の有効状態
	bool debugDrawEnabled_ = true;

#endif // _DEBUG

};
