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
	transform_->scale_ = { 1.0f,1.0f,1.0f };

	// 3Dƒ‚ƒfƒ‹Ý’è
	render3D_ = owner_->AddComponent<Render3D>();
	render3D_->Init();
	render3D_->SetModel("Data/Model/Object/Table/Table.mv1");

}

void Table::Update(void)
{
}

#ifdef _DEBUG
void Table::Draw3D(void)
{
	// ‹…•`‰æ
	DrawSphere3D(transform_->pos_, TABLE_RADIUS, 12, 0xff0000, 0xff0000,false);
}
#endif // _DEBUG
