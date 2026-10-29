#pragma once

#include<vector>

class Rat;
class CatRobot;

class EnemyCommander
{
public:
	EnemyCommander(void);

	void Init(std::vector<Rat*> rats, CatRobot* catRobot);	// 初期化
	void Update(void);										// 更新
	void Release(void);										// 解放

private:
	// ネズミのポインタ
	std::vector<Rat*>rats_;

	// 猫ロボットのポインタ
	CatRobot* catRobot_ = nullptr;

private:
	// 時間を空けて攻撃させるため用のカウント
	int ratAttackStartCount_ = 0;

private:
	// ネズミ司令処理
	void RatControl(void);

};
