#include "WaveManager.h"

WaveManager* WaveManager::instance_ = nullptr;

WaveManager::WaveManager(void)
{
}

WaveManager::~WaveManager(void)
{
}

void WaveManager::AddWave(std::unique_ptr<WaveBase> wave)
{
	// ウェーブの追加
	waves_.push_back(std::move(wave));
}

void WaveManager::Update()
{
	// 全てのウェーブが終わっていたら処理しない
	if (currentWaveIndex_ >= (int)waves_.size())
		return;

	// 現在のウェーブを取得して更新
	auto& wave = waves_[currentWaveIndex_];
	wave->Update();

	// 現在のウェーブがクリアしていたら
	if (wave->GetState() == WaveBase::WaveState::CLEARED)
	{
		// ウェーブを加算
		currentWaveIndex_++;
		if (currentWaveIndex_ < (int)waves_.size())
		{
			// ウェーブが終わっていなかったらクリア情報を保存する
			waveIsClear_ = true;
		}
	}
}

void WaveManager::Draw(void)
{
	// 全てのウェーブが終わっていたら処理しない
	if (currentWaveIndex_ >= (int)waves_.size())
		return;

	// 現在のウェーブを取得して更新
	auto& wave = waves_[currentWaveIndex_];
	wave->Draw();
}

WaveBase* WaveManager::GetCurrentWave()
{
	// 全てのウェーブが終わっていないなら
	if (currentWaveIndex_ < (int)waves_.size())
	{
		// 現在のウェーブを取得
		return waves_[currentWaveIndex_].get();
	}

	return nullptr;
}

WaveBase* WaveManager::GetNextWave(void)
{
	int nextWaveIndex = currentWaveIndex_ + 1;

	// まだ次のWAVEが残っていたら
	if (nextWaveIndex < (int)waves_.size())
	{
		// 次のウェーブを取得
		return waves_[nextWaveIndex].get();
	}

	// 次に控えているWAVEがなかったらnullptrを返す
	return nullptr;
}

bool WaveManager::AllCleared(void) const
{
	// 全てのウェーブが終わっていたらtrue
	return currentWaveIndex_ >= (int)waves_.size();
}

