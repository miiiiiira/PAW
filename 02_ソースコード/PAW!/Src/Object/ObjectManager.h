#pragma once
#include <vector>
#include <memory>
#include "Object.h"
#include "Tag.h"

// Objectをまとめて管理
class ObjectManager
{
public:
	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new ObjectManager(); } }
	static ObjectManager* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

private:
	// 静的インスタンス
	static ObjectManager* instance_;

	ObjectManager();		// コンストラクタ
	~ObjectManager() {};	// デストラクタ

	// コピー・ムーブ操作を禁止
	ObjectManager(const ObjectManager&) = delete;
	ObjectManager& operator=(const ObjectManager&) = delete;
	ObjectManager(ObjectManager&&) = delete;
	ObjectManager& operator=(ObjectManager&&) = delete;

public:

	// オブジェクトの生成
	Object* CreateObject(void);

	void Load(void);	// 読み込み(全オブジェクト)
	void Init(void);	// 初期化(全オブジェクト)
	void Update(void);	// 更新(全オブジェクト)
	void PreDraw(void);	// 描画前(全オブジェクト)
	void Draw2D(void);	// 2D描画(全オブジェクト)
	void Draw3D(void);	// 3D描画(全オブジェクト)

	Object* FindWithTag(TAG_3D tag);

	template<class T>
	T* FindComponentWithTag(TAG_3D tag);

	// 指定のクラス、タグにヒットするポインタをすべて渡す
	template<class T>
	std::vector<T*>FindComponentWithTagAll(TAG_3D tag);

private:

	// 管理しているObject
	std::vector<std::unique_ptr<Object>> objects_;
};

template<class T>
inline T* ObjectManager::FindComponentWithTag(TAG_3D tag)
{
	for (auto& obj : objects_)
	{
		if (obj->GetTag() != tag) continue;

		T* comp = obj->GetComponent<T>();
		if (comp) return comp;
	}

	return nullptr;
}

template<class T>
inline std::vector <T*> ObjectManager::FindComponentWithTagAll(TAG_3D tag)
{
	std::vector<T*> result;

	for (auto& obj : objects_)
	{
		if (obj->GetTag() != tag) continue;

		T* comp = obj->GetComponent<T>();
		if (comp) result.push_back(comp);
	}

	return result;
}
