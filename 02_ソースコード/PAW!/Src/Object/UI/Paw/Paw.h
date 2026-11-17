#pragma once

class Paw
{
public:
	// シングルトン（生成・取得・削除）
	static void  CreateInstance(void) { if (instance_ == nullptr) { instance_ = new Paw(); } }
	static Paw* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; } }

private:
	// 静的インスタンス
	static Paw* instance_;

	// コピー・ムーブ操作を禁止
	Paw(const Paw&) = delete;
	Paw& operator=(const Paw&) = delete;
	Paw(Paw&&) = delete;
	Paw& operator=(Paw&&) = delete;

public:
	Paw(void);						// コンストラクタ

	void Load(void);				// 読み込み
	void Update(void);				// 更新
	void Draw(int x, int y);		// 描画
	void DrawF(float x, float y);	// 描画
	void Release(void);				// 解放

private:
	// 肉球画像
	int pawImage_ = -1;
	
	// 上下ふわふわさせるsin派に渡す値
	float value_ = 1.0f;	
	bool isBig_ = true;
};
