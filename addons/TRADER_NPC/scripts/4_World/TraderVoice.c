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
static bool m_Loaded = false;

// <VoiceEnabled> yes|no в TraderNpcVariables.txt выключает голос торговцев
static void Load()
{
	if ( m_Loaded )
		return;
	m_Loaded = true;
	FileHandle fh = OpenFile( "$profile:Trader_NPC_Prof/TraderNpcVariables.txt", FileMode.READ );
	if ( fh == 0 )
		return;
	string line = "";
	while ( FGets( fh, line ) != -1 )
	{
		line = TraderNpcText.Clean( line );
		if ( line.Contains( "<VoiceEnabled>" ) )
		{
			int i = line.IndexOf( ">" );
			if ( i >= 0 )
				m_Enabled = line.Substring( i + 1, line.Length() - i - 1 ).Trim() == "yes";
		}
	}
	CloseFile( fh );
	TraderMessage.ServerLog( "[Voice] enabled=" + m_Enabled );
}

	static void Play( PlayerBase player, string phrase )
	{
		Load();
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
