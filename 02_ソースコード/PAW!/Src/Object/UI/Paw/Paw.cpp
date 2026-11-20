#include<DxLib.h>

#include "Paw.h"

Paw* Paw::instance_ = nullptr;

namespace
{
	// 肉球画像のサイズ変化用の値
	constexpr float PAW_SIZE_CHANGE_VALUE = 0.015f;

	// 肉球画像のサイズ変化の最小値と最大値
	constexpr float PAW_SIZE_CHANGE_MIN = 0.6f;
	constexpr float PAW_SIZE_CHANGE_MAX = 1.0f;
}

Paw::Paw(void)
{
}

void Paw::Load(void)
{
	// 肉球画像の読み込み
	pawImage_ = LoadGraph("Data/Image/paw.png");
}

void Paw::Update(void)
{
	// 肉球画像のサイズ変化
	if (isBig_)
	{
		value_ += PAW_SIZE_CHANGE_VALUE;
	}
	else
	{
		value_ -= PAW_SIZE_CHANGE_VALUE;
	}

	// サイズ変化の最大値と最小値を超えたら反転させる
	if (value_ > PAW_SIZE_CHANGE_MAX)
	{
		isBig_ = false;
	}
	else if (value_ < PAW_SIZE_CHANGE_MIN)
	{
		isBig_ = true;
	}
}

void Paw::Draw(int x, int y)
{
	// 肉球画像の描画
	DrawRotaGraph(x, y, value_, 0.0, pawImage_, true);
}

void Paw::DrawF(float x, float y)
{
	// 肉球画像の描画
	DrawRotaGraphF(x, y, value_, 0.0, pawImage_, true);
}

void Paw::Release(void)
{
	DeleteGraph(pawImage_);
}
