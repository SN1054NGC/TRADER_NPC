// ============================================================
// Голос торговца: проигрывает заранее сгенерированные фразы
// (piper -> ogg), зарегистрированные в config.cpp как CfgSoundSets.
//
// MVP: источник звука - сам игрок, поэтому фразу слышат все рядом.
// Следующий шаг - подставлять объект торговца, когда рядом есть NPC.
// ============================================================
class TraderVoice
{
	static bool m_Enabled = true;

	static void Play( PlayerBase player, string phrase )
	{
		if ( !m_Enabled || !player || phrase == "" )
			return;

		string soundSet = "TRADER_NPC_VOICE_" + phrase + "_SoundSet";
		GetGame().CreateSoundOnObject( player, soundSet, 25, false, true );
		TraderMessage.ServerLog( "[Voice] " + phrase );
	}
}