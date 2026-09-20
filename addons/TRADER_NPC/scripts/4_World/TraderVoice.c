// Голос торговца. Фразы генерируются через TTS Айлы (Silero + словарь ударений),
// хранятся в sounds/voice как <голос>_<действие>_<1..9>.ogg и играются проверенным
// путём мода: TraderPlaySoundForAll -> RPC -> SEffectManager.PlaySound на клиенте.
//
// У каждого торговца свой голос (SurvivorF_* -> female_*, SurvivorM_* -> male_*)
// и свой характер-блок: A нейтральный, B ворчливый, C балагур (1-3 свои тексты на действие).
// Вариант внутри блока выбирается случайно и не повторяет предыдущий.
// Анти-флуд: каждая вторая фраза игроку и каждая третья на сервере молчат (без таймеров:
// GetGame().GetTime() в DayZ не миллисекунды).
class TraderVoice
{
	static bool m_Enabled = true;
	static int  m_CallsGlobal = 0;
	static ref array<string> m_Ids;
	static ref array<string> m_Last;
	static ref array<int>    m_Calls;

	static int PlayerSlot( string id )
	{
		if ( !m_Ids )
		{
			m_Ids = new array<string>;
			m_Last = new array<string>;
			m_Calls = new array<int>;
		}

		for ( int i = 0; i < m_Ids.Count(); i++ )
		{
			if ( m_Ids.Get( i ) == id )
				return i;
		}

		m_Ids.Insert( id );
		m_Last.Insert( "" );
		m_Calls.Insert( 0 );
		return m_Ids.Count() - 1;
	}

	// у каждого действия 9 вариантов: 3 блока по 3 текста
	static int VariantCount( string base )
	{
		return 9;
	}

	static void Play( PlayerBase player, string base )
	{
		if ( !m_Enabled || !player || base == "" )
			return;
		if ( !player.GetIdentity() )
			return;

		string pid = player.GetIdentity().GetId();
		if ( pid == "" )
			return;

		int slot = PlayerSlot( pid );
		int calls = m_Calls.Get( slot ) + 1;
		m_Calls.Set( slot, calls );
		m_CallsGlobal = m_CallsGlobal + 1;

		TraderMessage.ServerLog( "[Voice] try " + base + " #" + calls );

		if ( calls > 1 && ( calls % 2 ) == 0 )
			return;
		if ( ( m_CallsGlobal % 3 ) == 2 )
			return;

		// приветствие зависит от денег игрока
		if ( base == "greet" )
		{
			int money = player.getPlayerCurrencyAmount();
			if ( money < 2000 )
				base = "greet_poor";
			else if ( money > 20000 )
				base = "greet_rich";
		}

		string voice = "";
		int block = 0;
		Object trader = TraderVoiceRegistry.Nearest( player, voice, block );
		if ( voice == "" )
			voice = "female_1";

		int variants = VariantCount( base );
		int third = variants / 3;
		if ( third < 1 )
			third = 1;

		int v = block * third + Math.RandomInt( 0, third ) + 1;
		if ( v > variants )
			v = variants;

		string phrase = base + "_" + v;
		if ( m_Last.Get( slot ) == phrase && third > 1 )
		{
			int off = v - block * third + 1;
			if ( off > third )
				off = 1;
			v = block * third + off;
			phrase = base + "_" + v;
		}

		string soundSet = "TRADER_NPC_VOICE_" + voice + "_" + phrase + "_SoundSet";

		vector pos = player.GetPosition();
		string from = "player";
		if ( trader )
		{
			pos = trader.GetPosition();
			from = "trader";
		}

		player.TraderPlaySoundForAll( soundSet, pos );

		m_Last.Set( slot, phrase );
		TraderMessage.ServerLog( "[Voice] " + phrase + " voice=" + voice + " block=" + block + " from=" + from );
	}
}