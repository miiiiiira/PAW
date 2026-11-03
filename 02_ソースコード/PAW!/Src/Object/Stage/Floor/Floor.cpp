#include "Floor.h"

#include "../../Object.h"
#include "../../Component/Graphics/Render3D.h"

Floor::Floor()
{
}

Floor::~Floor()
{
}

void Floor::Init(void)
{
	// À•W‚ÌÝ’è
	transform_ = owner_->AddComponent<Transform>();
	transform_->scale_ = { 1.0f,1.0f,1.0f };

	// 3Dƒ‚ƒfƒ‹Ý’è
	render3D_ = owner_->AddComponent<Render3D>();
	render3D_->Init();
	render3D_->SetModel("Data/Model/Object/Floor/Floor.mv1");

}

void Floor::Update(void)
{
}
