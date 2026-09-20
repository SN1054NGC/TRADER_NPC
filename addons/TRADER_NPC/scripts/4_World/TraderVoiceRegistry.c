// Реестр торговцев: объект, голос и характер-блок. Заполняется миссией при спавне,
// используется TraderVoice, чтобы фраза звучала от самого торговца его голосом и текстами.
//
// Пол и характер определяются по имени NPC из TraderNpcObjects.txt:
//   SurvivorF_* -> женские (female_1..3), SurvivorM_* -> мужские (male_1..3);
//   блок: 1-й торговец своего пола -> A (0), 2-й -> B (1), 3-й -> C (2), дальше по кругу.
// Переопределение в строке объекта: <Object> SurvivorF_Eva, female_2, B
class TraderVoiceRegistry
{
	static ref array<Object> m_Objects;
	static ref array<string> m_Voices;
	static ref array<int>    m_Blocks;
	static int m_FemaleCount;
	static int m_MaleCount;
	static int m_FemaleBlock;
	static int m_MaleBlock;

	static void Reset()
	{
		m_Objects = new array<Object>;
		m_Voices = new array<string>;
		m_Blocks = new array<int>;
		m_FemaleCount = 0;
		m_MaleCount = 0;
		m_FemaleBlock = 0;
		m_MaleBlock = 0;
	}

	static bool IsFemale( string objectType )
	{
		if ( objectType.Contains( "SurvivorM" ) )
			return false;
		return true;
	}

	static string PickVoice( string objectType, string voiceOverride )
	{
		if ( voiceOverride != "" )
			return voiceOverride;

		bool female = IsFemale( objectType );
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

		int idx = n % 3;
		if ( idx == 0 )
			return prefix + "1";
		if ( idx == 1 )
			return prefix + "2";
		return prefix + "3";
	}

	// характер: A - нейтральный, B - ворчливый, C - балагур
	static int PickBlock( string objectType, string blockOverride )
	{
		if ( blockOverride == "A" || blockOverride == "a" )
			return 0;
		if ( blockOverride == "B" || blockOverride == "b" )
			return 1;
		if ( blockOverride == "C" || blockOverride == "c" )
			return 2;

		bool female = IsFemale( objectType );
		int b = m_MaleBlock;
		if ( female )
		{
			b = m_FemaleBlock;
			m_FemaleBlock = m_FemaleBlock + 1;
		}
		else
		{
			m_MaleBlock = m_MaleBlock + 1;
		}

		return b % 3;
	}

	static void Register( Object trader, string voice, int block )
	{
		if ( !trader )
			return;
		if ( !m_Objects )
			Reset();

		m_Objects.Insert( trader );
		m_Voices.Insert( voice );
		m_Blocks.Insert( block );

		TraderMessage.ServerLog( "[Voice] " + trader.GetType() + " -> " + voice + " block=" + block );
	}

	// ближайший торговец к игроку (до 60 м); голос и блок возвращаем через out
	static Object Nearest( PlayerBase player, out string voice, out int block )
	{
		voice = "";
		block = 0;
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
				block = m_Blocks.Get( i );
			}
		}

		if ( bestDist > 60.0 )
			return null;
		return best;
	}
}