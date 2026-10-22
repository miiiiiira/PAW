#pragma once
#include <map>
#include <string>

#include "../Component.h"

class Render3D;

class Animation : public Component
{
public:
	struct AnimationState
	{
		int model = -1;
		int attachNo = -1;
		int animIndex = 0;
		float speed = 0.0f;
		float totalTime = 0.0f;
		float step = 0.0f;
	};

public:
	void Init(void) override;	// 初期化
	void Update(void) override;	// 更新
	void Release(void);			// 解放

public:
	// アニメーション登録
	// モデル自体にアニメーションが入っていない時に使用
	void Add(int type, float speed, const std::string& path);
	// モデル自体にアニメーションが入っている時に使用
	void AddInFbx(int type, float speed, int animIndex);

	// アニメーションを再生する
	// blendFrame : ブレンドにかけるフレーム数 (0以下でブレンドなし)
	void Play(int type, bool loop = true, float blendFrame = 10.0f);

	// 現在のアニメーションの種類を返す
	int GetPlayType(void) const { return playType_; }

	// アニメーションが終了していたらtrueを返す
	bool IsEnd(void) const;

	// ブレンド中ならtrueを返す
	bool IsBlending(void) const { return isBlending_; }

	// アニメーションの再生時間を返す
	float GetStep(void) const { return playAnim_.step; }

	// アニメーションの総再生時間を返す
	float GetTotalAnimTime(void)const { return playAnim_.totalTime; }

private:
	// Render3D
	Render3D* render_ = nullptr;

private:
	// アニメーション情報をまとめておく
	std::map<int, AnimationState> animations_;

	// 再生中のアニメーション情報
	AnimationState playAnim_;

	// 再生中の種類
	int playType_ = -1;

	// ループ再生か　true / ループ, false / 1回のみ
	bool isLoop_ = true;

	// ブレンド元(直前まで再生していた)アニメーション情報
	AnimationState prevAnim_;
	bool prevLoop_ = true;

	// ブレンド中か
	bool isBlending_ = false;
	// ブレンドにかけるフレーム数
	float blendFrame_ = 0.0f;
	// ブレンド経過フレーム数
	float blendCount_ = 0.0f;

private:
	// アニメーション再生速度設定
	void AddInternal(int type, float speed, AnimationState& anim);

	// アニメーションを1フレーム進める
	void StepAnim(AnimationState& anim, bool loop);

	// ブレンド元アニメーションを外してブレンドを終了する
	void EndBlend(void);
};
