#include "Collider.h"
#include "CollisionManager.h"

#include "../Object/Object.h"

// ‰Šú‰»
void Collider::Init()
{
	// TransformŽæ“¾
	transform_ = owner_->GetComponent<Transform>();
}

void Collider::Update()
{
	// ƒRƒ‰ƒCƒ_[‚Ì“o˜^
	CollisionManager::GetInstance()->Register(this);
}
