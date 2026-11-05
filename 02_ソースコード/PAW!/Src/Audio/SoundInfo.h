#pragma once
#include <string>

// サウンドID
enum class SoundID
{
	NON = -1,

	SYS_BUTTON,
	SYS_SELECT_ON,
	SYS_SELECT_CLICK,
	SYS_ENTER,
	SYS_CANCEL,
	SYS_PAUSE_ON,
	SYS_PAUSE_OFF,

	BGM_TITLE,
	SE_CHIME,
	SE_CAT_VOICE_1,

	BGM_GAME,
	BGM_RAT_EVENT,
	BGM_GAMEOVER,
	BGM_GAMECLEAR,

	SE_JUMP,
	SE_AVOID,
	SE_ATTACK_1,
	SE_ATTACK_2,
	SE_ATTACK_3,
	SE_MOVE,
	SE_PLAYER_DAMAGE,

	SE_ROBOT_ATTACK_START,
	SE_ROBOT_ATTACK1,
	SE_ROBOT_ATTACK2,
	SE_RAT_DAMAGE,
	SE_RAT_DEAD,
	SE_ROBOT_DAMAGE,
	SE_ROBOT_FALL_DOWN,
	SE_ROBOT_GET_UP,
};

// 読み込むシーン
enum class LoadScene
{
	SYSTEM,			// システム
	TITLE,			// タイトル
	MAIN_MENU,		// メインメニュー
	GAME,			// ゲーム
	GAME_OVER,		// ゲームオーバー
	GAME_CLEAR,		// ゲームクリア
};

// サウンドのタイプ
enum class SoundType
{
	BGM,
	SE_2D,
	SE_3D,
};

// サウンド属性
struct SoundData
{
	std::string path;	// サウンドのパス
	SoundType type;
	int volume;			// 音量 (0～255)
};

// サウンドの情報
struct SoundResource
{
	int handle;
	SoundType type;
	int volume;
};
