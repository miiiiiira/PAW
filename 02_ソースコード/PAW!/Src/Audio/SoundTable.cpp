#include "SoundTable.h"

// サウンドID から サウンドパスを取得

// システム用
namespace SoundTable_System
{
	const std::unordered_map<SoundID, SoundData> Table =
	{
		{ SoundID::SYS_BUTTON,		{"Data/Sound/System/SYS_BUTTON.mp3",	SoundType::SE_2D, 255 } },
		{ SoundID::SYS_SELECT_ON,	{"Data/Sound/System/SYS_SELECT_ON.mp3",	SoundType::SE_2D, 255 } },
		{ SoundID::SYS_SELECT_CLICK,{"Data/Sound/System/system.mp3",		SoundType::SE_2D, 255 } },
		{ SoundID::SYS_ENTER,		{"Data/Sound/System/system.mp3",		SoundType::SE_2D, 255 } },
		{ SoundID::SYS_CANCEL,		{"Data/Sound/System/system.mp3",		SoundType::SE_2D, 255 } },
		{ SoundID::SYS_PAUSE_ON,	{"Data/Sound/System/SYS_PAUSE_ON.wav",	SoundType::SE_2D, 200 } },
		{ SoundID::SYS_PAUSE_OFF,	{"Data/Sound/System/SYS_PAUSE_OFF.wav",	SoundType::SE_2D, 200 } },
	};
}

// タイトル画面用
namespace SoundTable_Title
{
	const std::unordered_map<SoundID, SoundData> Table =
	{
		{ SoundID::BGM_TITLE,		{"Data/Sound/Title/BGM_TITLE.wav",			SoundType::BGM, 255} },
		{ SoundID::SE_CHIME,		{"Data/Sound/Title/SE_CHIME.mp3",			SoundType::SE_2D, 255} },
		{ SoundID::SE_CAT_VOICE_1,	{"Data/Sound/Title/SE_CAT_VOICE_1.mp3",		SoundType::SE_2D, 200 } },
	};
}

// ゲーム画面用
namespace SoundTable_Game
{
	const std::unordered_map<SoundID, SoundData> Table =
	{
		{ SoundID::BGM_GAME,		{"Data/Sound/Game/BGM_GAME.mp3",		SoundType::BGM, 200} },
		{ SoundID::BGM_RAT_EVENT,	{"Data/Sound/Game/BGM_RAT_EVENT.mp3",	SoundType::BGM, 200} },

		{ SoundID::SE_JUMP,			{"Data/Sound/Game/Player/SE_JUMP.mp3",			SoundType::SE_2D, 255} },
		{ SoundID::SE_AVOID,		{"Data/Sound/Game/Player/SE_AVOID.mp3",			SoundType::SE_2D, 255} },
		{ SoundID::SE_ATTACK_1,		{"Data/Sound/Game/Player/SE_ATTACK_1.mp3",		SoundType::SE_2D, 255} },
		{ SoundID::SE_ATTACK_2,		{"Data/Sound/Game/Player/SE_ATTACK_2.mp3",		SoundType::SE_2D, 255} },
		{ SoundID::SE_ATTACK_3,		{"Data/Sound/Game/Player/SE_ATTACK_3.mp3",		SoundType::SE_2D, 255} },
		{ SoundID::SE_MOVE,			{"Data/Sound/Game/Player/SE_MOVE.wav",			SoundType::SE_2D, 100} },
		{ SoundID::SE_PLAYER_DAMAGE,	{"Data/Sound/Game/Player/SE_PLAYER_DAMAGE.mp3",		SoundType::SE_2D, 255} },

		{ SoundID::SE_ROBOT_ATTACK_START,	{"Data/Sound/Game/Enemy/SE_ROBOT_ATTACK_START.mp3",	SoundType::SE_2D, 255} },
		{ SoundID::SE_ROBOT_ATTACK1,		{"Data/Sound/Game/Enemy/SE_ROBOT_ATTACK1.mp3",		SoundType::SE_2D, 200} },
		{ SoundID::SE_ROBOT_ATTACK2,		{"Data/Sound/Game/Enemy/SE_ROBOT_ATTACK2.mp3",		SoundType::SE_2D, 255} },
		{ SoundID::SE_RAT_DAMAGE,			{"Data/Sound/Game/Enemy/SE_RAT_DAMAGE.mp3",			SoundType::SE_2D, 255} },
		{ SoundID::SE_RAT_DEAD,				{"Data/Sound/Game/Enemy/SE_RAT_DEAD.mp3",			SoundType::SE_2D, 255} },
		{ SoundID::SE_ROBOT_DAMAGE,			{"Data/Sound/Game/Enemy/SE_ROBOT_DAMAGE.mp3",		SoundType::SE_2D, 255} },
		{ SoundID::SE_ROBOT_FALL_DOWN,		{"Data/Sound/Game/Enemy/SE_ROBOT_FALL_DOWN.mp3",	SoundType::SE_2D, 255} },
		{ SoundID::SE_ROBOT_GET_UP,			{"Data/Sound/Game/Enemy/SE_ROBOT_GET_UP.mp3",		SoundType::SE_2D, 255} },
	};
}

// ゲームオーバー画面用
namespace SoundTable_GameOver
{
	const std::unordered_map<SoundID, SoundData> Table =
	{
		{ SoundID::BGM_GAMEOVER, {"Data/Sound/GameOver/BGM_GAMEOVER.wav", SoundType::BGM, 127} },
	};
}

// ゲームクリア画面用
namespace SoundTable_GameClear
{
	const std::unordered_map<SoundID, SoundData> Table =
	{
		{ SoundID::BGM_GAMECLEAR, {"Data/Sound/GameClear/BGM_GAMECLEAR.mp3", SoundType::BGM, 120} },
	};
}
