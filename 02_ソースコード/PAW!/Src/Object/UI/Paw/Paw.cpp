#include<DxLib.h>

#include "Paw.h"

Paw* Paw::instance_ = nullptr;

Paw::Paw(void)
{
}

void Paw::Load(void)
{
	// “÷‹…‰æ‘œ‚Ì“Ç‚Ýž‚Ý
	pawImage_ = LoadGraph("Data/Image/paw.png");
}

void Paw::Update(void)
{
	if (isBig_)
	{
		value_ += 0.02f;
	}
	else
	{
		value_ -= 0.02f;
	}

	if (value_ > 1.0f)
	{
		isBig_ = false;
	}
	else if (value_ < 0.3f)
	{
		isBig_ = true;
	}
}

void Paw::Draw(int x, int y)
{
	// “÷‹…‰æ‘œ‚Ì•`‰æ
	DrawRotaGraph(x, y, value_, 0.0, pawImage_, true);
}

void Paw::DrawF(float x, float y)
{
	// “÷‹…‰æ‘œ‚Ì•`‰æ
	DrawRotaGraphF(x, y, value_, 0.0, pawImage_, true);
}

void Paw::Release(void)
{
	DeleteGraph(pawImage_);
}
