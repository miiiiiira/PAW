#pragma once

#include "../../Component/Component.h"
#include "../../Component/Transform/Transform.h"

// 前方宣言
class Transform;
class Render3D;

class Floor : public Component
{
public:
	Floor();
	~Floor();

	void Init(void) override;	// 初期化
	void Update(void) override;	// 更新

private:
	// コンポーネント
	Transform* transform_ = nullptr;
	Render3D* render3D_ = nullptr;
};
