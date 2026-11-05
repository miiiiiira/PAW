#include "SoundTable.h"

// サウンドID から サウンドパスを取得

// システム用
namespace SoundTable_System
{
	const std::unordered_map<SoundID, SoundData> Table =
	{
		{ SoundID::SYS_BUTTON,		{"Data/Sound/System/SYS_BUTTON_1.wav",	SoundType::SE_2D, 200 } },
		{ SoundID::SYS_SELECT_ON,	{"Data/Sound/System/SYS_SELECT_ON.ogg",	SoundType::SE_2D, 200 } },
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
		{ SoundID::BGM_TITLE,	{"Data/Sound/Title/BGM_TITLE.wav",	 SoundType::BGM, 255} },
		{ SoundID::SE_CHIME,	{"Data/Sound/Title/SE_CHIME.mp3",	 SoundType::SE_2D, 255} },
	};
}

// ゲーム画面用
namespace SoundTable_Game
{
	const std::unordered_map<SoundID, SoundData> Table =
	{
		{ SoundID::BGM_GAME,		{"Data/Sound/Game/BGM/BGM_GAME_1.wav", SoundType::BGM, 255} },

		{ SoundID::SE_JUMP,			{"Data/Sound/Game/Player/SE_JUMP.mp3",			SoundType::SE_2D, 200} },
		{ SoundID::SE_AVOID,		{"Data/Sound/Game/Player/SE_JUMP.mp3",			SoundType::SE_2D, 200} },
		{ SoundID::SE_ATTACK_1,		{"Data/Sound/Game/Player/SE_JUMP.mp3",			SoundType::SE_2D, 200} },
		{ SoundID::SE_ATTACK_2,		{"Data/Sound/Game/Player/SE_JUMP.mp3",			SoundType::SE_2D, 200} },
		{ SoundID::SE_ATTACK_3,		{"Data/Sound/Game/Player/SE_JUMP.mp3",			SoundType::SE_2D, 200} },
		{ SoundID::SE_DAMAGE,		{"Data/Sound/Game/Player/SE_DAMAGE.mp3",		SoundType::SE_2D, 200} },
		{ SoundID::SE_MOVE,			{"Data/Sound/Game/Player/SE_MOVE.wav",			SoundType::SE_2D, 100} },
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
