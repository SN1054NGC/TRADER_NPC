// Голос торговца: piper -> ogg, играется через TraderPlaySoundForAll (RPC + SEffectManager).
// Голос - у ближайшего торговца (TraderVoiceRegistry): SurvivorF_* -> female_1..3, SurvivorM_* -> male_1..3.
// Варианты фраз выбираются случайно без повтора подряд; приветствие зависит от денег игрока;
// анти-флуд: не чаще одной фразы в 6 с на игрока и одной в 2 с на сервере.
// Вызов: TraderVoice.Play( игрок, "greet" | "buy" | "sell" | "no_money" | "empty" | "notrade" | "bye" | "funny" )
class TraderVoice
{
	static bool m_Enabled = true;
	static int  m_CooldownMs = 6000;
	static int  m_GlobalMs   = 2000;
	static int  m_LastGlobal = 0;
	static int  m_CallCounter = 0;
	static ref array<string> m_Ids;
	static ref array<string> m_Last;
	static ref array<int>    m_Times;

	static int PlayerSlot( string id )
	{
		if ( !m_Ids )
		{
			m_Ids = new array<string>;
			m_Last = new array<string>;
			m_Times = new array<int>;
		}

		for ( int i = 0; i < m_Ids.Count(); i++ )
		{
			if ( m_Ids.Get( i ) == id )
				return i;
		}

		m_Ids.Insert( id );
		m_Last.Insert( "" );
		m_Times.Insert( 0 );
		return m_Ids.Count() - 1;
	}

	static int VariantCount( string base )
	{
		if ( base == "buy" || base == "sell" )
			return 3;
		if ( base == "greet" || base == "greet_poor" || base == "greet_rich" )
			return 2;
		if ( base == "bye" )
			return 2;
		if ( base == "empty" || base == "notrade" || base == "no_money" || base == "funny" )
			return 2;
		return 1;
	}

	static string PickVariant( int slot, string base, int variants )
	{
		int v = Math.RandomInt( 1, variants + 1 );
		string candidate = base + "_" + v;
		if ( variants > 1 && m_Last.Get( slot ) == candidate )
		{
			int alt = v + 1;
			if ( alt > variants )
				alt = 1;
			candidate = base + "_" + alt;
		}
		return candidate;
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

		// ВАЖНО: GetGame().GetTime() в DayZ считает не миллисекунды, поэтому
		// временной анти-флуд здесь не использовать. Считаем попытки:
		// каждая вторая фраза игроку и каждая третья на сервере молчат.
		int slot = PlayerSlot( pid );
		int calls = m_Times.Get( slot ) + 1;
		m_Times.Set( slot, calls );
		m_CallCounter = m_CallCounter + 1;

		TraderMessage.ServerLog( "[Voice] try " + base + " #" + calls + " " + pid.Substring( 0, 6 ) );

		if ( calls > 1 && ( calls % 2 ) == 0 )
			return;
		if ( ( m_CallCounter % 3 ) == 2 )
			return;

		if ( base == "greet" )
		{
			int money = player.getPlayerCurrencyAmount();
			if ( money < 2000 )
				base = "greet_poor";
			else if ( money > 20000 )
				base = "greet_rich";
		}

		string phrase = PickVariant( slot, base, VariantCount( base ) );

		string voice = "";
		Object trader = TraderVoiceRegistry.Nearest( player, voice );
		if ( voice == "" )
			voice = "female_1";

		string soundSet = "TRADER_NPC_VOICE_" + voice + "_" + phrase + "_SoundSet";

		vector pos = player.GetPosition();
		string from = "player";
		if ( trader )
		{
			pos = trader.GetPosition();
			from = "trader";
		}

		player.TraderPlaySoundForAll( soundSet, pos );

		// счётчики уже обновлены выше
		m_Last.Set( slot, phrase );

		TraderMessage.ServerLog( "[Voice] " + phrase + " voice=" + voice + " from=" + from );
	}
}