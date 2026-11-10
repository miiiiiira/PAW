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
	// 攻撃させるネズミを保存
	std::vector<Rat*> ratAttackQueue_;

	// 時間を空けて攻撃させるため用のカウント
	int ratAttackStartCount_ = 0;

private:
	// 敵司令処理
	void EnemyControl(void);

	// ネズミ司令処理
	void RatControl(void);
	void RatAttackOrder(void);		// 攻撃指示
	void SetRatAttackIndex(void);	// 誰を攻撃させるか決める

	// 猫ロボット司令処理
	void CatRobotControl(void);
};
