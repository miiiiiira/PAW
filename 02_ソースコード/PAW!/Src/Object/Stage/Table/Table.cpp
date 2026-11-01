#include "Table.h"

#include "../../Object.h"
#include "../../Component/Graphics/Render3D.h"

Table::Table()
{
}

Table::~Table()
{
}

void Table::Init(void)
{
	// À•W‚ÌÝ’è
	transform_ = owner_->AddComponent<Transform>();

	// 3Dƒ‚ƒfƒ‹Ý’è
	render3D_ = owner_->AddComponent<Render3D>();
	render3D_->Init();
	render3D_->SetModel("Data/Model/Object/Table/Table.mv1");

}

void Table::Update(void)
{
}
