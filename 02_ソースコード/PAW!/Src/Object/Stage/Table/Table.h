#pragma once

#include "../../Component/Component.h"
#include "../../Component/Transform/Transform.h"

// 前方宣言
class Transform;
class Render3D;

class Table : public Component
{
public:
	// テーブル半径
	static constexpr float TABLE_RADIUS = 1600.0f;

public:
	Table(void);				// コンストラクタ
	~Table(void);				// デストラクタ

	void Init(void) override;	// 初期化
	void Update(void) override;	// 更新

#ifdef _DEBUG
	void Draw3D(void) override;	// 3D描画
#endif // _DEBUG

public:
	// Transformを返す
	Transform* GetTransform(void) { return transform_; }

private:
	// コンポーネント
	Transform* transform_ = nullptr;
	Render3D* render3D_ = nullptr;
};
