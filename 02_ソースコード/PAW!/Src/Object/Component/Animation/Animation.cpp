#include <DxLib.h>

#include "../Graphics/Render3D.h"
#include "../../Object.h"

#include "Animation.h"

void Animation::Init(void)
{
	render_ = owner_->GetComponent<Render3D>();
}

void Animation::Update(void)
{
	if (!render_) return;

	// アニメーションが再生されていなかったら処理を行わない
	if (playType_ == -1) return;

	int handle = render_->GetHandle();

	// 再生中アニメーションを進める
	StepAnim(playAnim_, isLoop_);
	MV1SetAttachAnimTime(handle, playAnim_.attachNo, playAnim_.step);

	// ブレンド中の処理
	if (isBlending_)
	{
		// ブレンド元アニメーションも進める
		StepAnim(prevAnim_, prevLoop_);
		MV1SetAttachAnimTime(handle, prevAnim_.attachNo, prevAnim_.step);

		// ブレンド率を更新
		blendCount_ += 1.0f;
		float rate = blendCount_ / blendFrame_;
		if (rate > 1.0f) rate = 1.0f;

		MV1SetAttachAnimBlendRate(handle, playAnim_.attachNo, rate);
		MV1SetAttachAnimBlendRate(handle, prevAnim_.attachNo, 1.0f - rate);

		// ブレンド完了
		if (rate >= 1.0f)
		{
			EndBlend();
		}
	}
}

void Animation::Release(void)
{
	// ブレンド元が残っていれば外す
	if (render_ && isBlending_)
	{
		EndBlend();
	}

	// アニメーションの解放
	for (auto& pair : animations_)
	{
		if (pair.second.model != -1)
		{
			MV1DeleteModel(pair.second.model);
		}
	}

	// 拡張オブジェクトをクリア
	animations_.clear();
}

void Animation::Add(int type, float speed, const std::string& path)
{
	AnimationState anim;
	// アニメーション読み込み
	anim.model = MV1LoadModel(path.c_str());
	anim.animIndex = -1;

	// アニメーションの情報を設定
	AddInternal(type, speed, anim);
}

void Animation::AddInFbx(int type, float speed, int animIndex)
{
	AnimationState anim;
	anim.model = -1;
	anim.animIndex = animIndex;

	// アニメーションの情報を設定
	AddInternal(type, speed, anim);
}

void Animation::Play(int type, bool loop, float blendFrame)
{
	if (!render_)
	{
		playType_ = type;
		isLoop_ = loop;
		return;
	}

	// 既に同じアニメーションが再生中の場合は何もしない
	if (playType_ == type) return;

	// モデルハンドルを取得
	int handle = render_->GetHandle();

	// ブレンド中に切り替わった場合は、古いブレンド元を外す
	if (isBlending_)
	{
		EndBlend();
	}

	// 以前に再生していたアニメーションがある場合
	bool doBlend = false;
	if (playType_ != -1)
	{
		if (blendFrame > 0.0f)
		{
			// ブレンド元として保持しておく (Detachしない)
			prevAnim_ = playAnim_;
			prevLoop_ = isLoop_;
			blendFrame_ = blendFrame;
			blendCount_ = 0.0f;
			isBlending_ = true;
			doBlend = true;
		}
		else
		{
			// ブレンドなしの場合はそのままアタッチを外す
			MV1DetachAnim(handle, playAnim_.attachNo);
		}
	}

	// 再生するアニメーション情報を更新し、再生時間を初期化
	playType_ = type;
	playAnim_ = animations_[type];
	playAnim_.step = 0.0f;

	// アニメーションをモデルにアタッチ
	if (playAnim_.model == -1)
	{
		// 単一モデル内のアニメーションインデックスからアタッチする場合
		playAnim_.attachNo = MV1AttachAnim(handle, playAnim_.animIndex);
	}
	else
	{
		// 外部のモデルハンドルからアタッチする場合
		playAnim_.attachNo = MV1AttachAnim(handle, 0, playAnim_.model);
	}

	// アタッチしたアニメーションの総再生時間を取得
	playAnim_.totalTime = MV1GetAttachAnimTotalTime(handle, playAnim_.attachNo);

	// ブレンド率の初期設定
	if (doBlend)
	{
		MV1SetAttachAnimBlendRate(handle, playAnim_.attachNo, 0.0f);
		MV1SetAttachAnimBlendRate(handle, prevAnim_.attachNo, 1.0f);
	}
	else
	{
		MV1SetAttachAnimBlendRate(handle, playAnim_.attachNo, 1.0f);
	}

	// ループフラグの更新
	isLoop_ = loop;
}

bool Animation::IsEnd(void) const
{
	// ループ再生だったら処理を行わない
	if (isLoop_) return false;

	// 再生時間が総再生時間を超えていたら
	return playAnim_.step >= playAnim_.totalTime;
}

void Animation::AddInternal(int type, float speed, AnimationState& anim)
{
	// アニメーションの速度を設定
	anim.speed = speed;

	if (animations_.count(type) == 0)
	{
		// アニメーション情報を追加
		animations_.emplace(type, anim);
	}
}

void Animation::StepAnim(AnimationState& anim, bool loop)
{
	// アニメーションを進める
	anim.step += anim.speed;

	// 再生時間が総再生時間を超えていたら
	if (anim.step >= anim.totalTime)
	{
		if (loop)
		{
			// ループ再生の場合は再生時間を初期化
			anim.step = 0.0f;
		}
		else
		{
			// ループ再生ではない場合は総再生時間に設定
			anim.step = anim.totalTime;
		}
	}
}

void Animation::EndBlend(void)
{
	if (render_)
	{
		int handle = render_->GetHandle();
		MV1DetachAnim(handle, prevAnim_.attachNo);
		// 再生中アニメーションのブレンド率を1.0に確定
		MV1SetAttachAnimBlendRate(handle, playAnim_.attachNo, 1.0f);
	}
	isBlending_ = false;
	blendCount_ = 0.0f;
}
