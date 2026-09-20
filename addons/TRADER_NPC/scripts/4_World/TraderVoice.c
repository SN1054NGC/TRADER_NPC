// ============================================================
// Голос торговца: играет заранее сгенерированные фразы (piper -> ogg).
// Источник звука - сам торговец (через TraderVoiceRegistry), поэтому
// голос идёт позиционно от NPC и слышен только рядом.
//
// Голос выбирается по имени NPC (SurvivorF_ / SurvivorM_) и порядку спавна
// с переопределением <Object> SurvivorF_Eva, male_2 в TraderNpcObjects.txt.
// ============================================================
class TraderVoice
{
	static bool m_Enabled = true;

	static void Play( PlayerBase player, string phrase )
	{
		if ( !m_Enabled || !player || phrase == "" )
			return;

		string voice = "";
		Object trader = TraderVoiceRegistry.Nearest( player, voice );

		string soundSet = "TRADER_NPC_VOICE_" + voice + "_" + phrase + "_SoundSet";
		if ( voice == "" )
			soundSet = "TRADER_NPC_VOICE_female_1_" + phrase + "_SoundSet";

		string from = "player";
		vector pos = player.GetPosition();
		if ( trader )
		{
			pos = trader.GetPosition();
			from = "trader";
		}

		// Проверенный путь мода: сервер рассылает RPC игрокам в радиусе,
		// каждый играет набор локально (SEffectManager.PlaySound).
		player.TraderPlaySoundForAll( soundSet, pos );
		TraderMessage.ServerLog( "[Voice] " + phrase + " voice=" + voice + " from=" + from );
	}
}
