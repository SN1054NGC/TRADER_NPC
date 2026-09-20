// ============================================================
// Реестр торговцев: объект + его голос. Заполняется миссией при спавне,
// используется TraderVoice, чтобы фраза звучала ОТ САМОГО ТОРГОВЦА.
//
// Пол определяется по имени NPC из TraderNpcObjects.txt:
//   SurvivorF_* -> женский (female_1 / female_2 по кругу)
//   SurvivorM_* -> мужской (male_1 / male_2 по кругу)
// Переопределение: <Object> SurvivorF_Eva, male_2
// ============================================================
class TraderVoiceRegistry
{
	static ref array<Object> m_Objects;
	static ref array<string> m_Voices;
	static int m_FemaleCount;
	static int m_MaleCount;

	static void Reset()
	{
		m_Objects = new array<Object>;
		m_Voices = new array<string>;
		m_FemaleCount = 0;
		m_MaleCount = 0;
	}

	static string PickVoice( string objectType, string voiceOverride )
	{
		if ( voiceOverride != "" )
			return voiceOverride;

		bool female = true;
		if ( objectType.Contains( "SurvivorM" ) )
			female = false;

		int n = m_MaleCount;
		string prefix = "male_";
		if ( female )
		{
			n = m_FemaleCount;
			prefix = "female_";
			m_FemaleCount = m_FemaleCount + 1;
		}
		else
		{
			m_MaleCount = m_MaleCount + 1;
		}

		int v = n % 3;
		if ( v == 0 )
			return prefix + "1";
		if ( v == 1 )
			return prefix + "2";
		return prefix + "3";
	}

	static void Register( Object trader, string voice )
	{
		if ( !trader )
			return;
		if ( !m_Objects )
		{
			m_Objects = new array<Object>;
			m_Voices = new array<string>;
		}
		m_Objects.Insert( trader );
		m_Voices.Insert( voice );
		TraderMessage.ServerLog( "[Voice] " + trader.GetType() + " -> " + voice );
	}

	// Ближайший торговец к игроку (в радиусе 60 м), голос возвращаем через voice
	static Object Nearest( PlayerBase player, out string voice )
	{
		voice = "";
		if ( !player || !m_Objects )
			return null;

		Object best = null;
		float bestDist = 999999.0;
		vector ppos = player.GetPosition();

		for ( int i = 0; i < m_Objects.Count(); i++ )
		{
			Object o = m_Objects.Get( i );
			if ( !o )
				continue;
			float d = vector.Distance( ppos, o.GetPosition() );
			if ( d < bestDist )
			{
				bestDist = d;
				best = o;
				voice = m_Voices.Get( i );
			}
		}

		if ( bestDist > 60.0 )
			return null;
		return best;
	}
}