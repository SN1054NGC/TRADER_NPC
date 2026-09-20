// ============================================================
// ИИ-торговец (СЕРВЕРНАЯ часть). Вопрос из чата ("!вопрос") уходит
// в локальную модель, ответ возвращается игроку в чат.
//
// Приватно: адрес модели и сборка фактов живут на сервере,
// в клиентский аддон ничего не уходит.
//
// Настройки в TraderNpcVariables.txt:
//   <AiEnabled> yes
//   <AiUrl> 127.0.0.1:8010
//   <AiPath> /dayz/asktext
// ============================================================
class TraderAiChat
{
	static bool   m_Enabled = false;
	static string m_Url     = "127.0.0.1:8010";
	static string m_Path    = "/dayz/asktext";
	static string m_Config  = "$profile:Trader_NPC_Prof/TraderNpcConfig.txt";
	static string m_Vars    = "$profile:Trader_NPC_Prof/TraderNpcVariables.txt";
	static bool   m_Loaded  = false;
	static int    m_LastMs  = 0;

	static string TagValue( string line )
	{
		int i = line.IndexOf( ">" );
		if ( i < 0 )
			return "";
		return line.Substring( i + 1, line.Length() - i - 1 ).Trim();
	}

	static void Load()
	{
		if ( m_Loaded )
			return;
		m_Loaded = true;

		FileHandle fh = OpenFile( m_Vars, FileMode.READ );
		if ( fh == 0 )
			return;

		string line = "";
		while ( FGets( fh, line ) != -1 )
		{
			line = TraderNpcText.Clean( line );
			if ( line.Contains( "<AiEnabled>" ) )
				m_Enabled = TagValue( line ) == "yes";
			else if ( line.Contains( "<AiUrl>" ) )
				m_Url = TagValue( line );
			else if ( line.Contains( "<AiPath>" ) )
				m_Path = TagValue( line );
		}
		CloseFile( fh );

		TraderMessage.ServerLog( "[AI] enabled=" + m_Enabled + " url=" + m_Url + m_Path );
	}

	// JSON-строка: кавычки и переводы строк недопустимы
	static string Sanitize( string s )
	{
		s.Replace( "\"", "'" );
		s.Replace( "\n", " " );
		s.Replace( "\r", " " );
		s.Replace( "\t", " " );
		return s;
	}

	// Факты для модели: валюта, скидка и первые позиции из TraderNpcConfig.txt
	static string BuildFacts()
	{
		string facts = "Валюта: бумага TRADER_NPC (TraderNpcMoney), 1 штука = 1 рубль, складывается в пачку до 99999 штук. ";
		facts = facts + "Цены указаны за одну штуку. Скидка зависит от рейтинга игрока (максимум около 20 процентов). Товары: ";

		FileHandle fh = OpenFile( m_Config, FileMode.READ );
		if ( fh == 0 )
			return facts;

		string line = "";
		int n = 0;
		while ( FGets( fh, line ) != -1 && n < 25 )
		{
			line = TraderNpcText.Clean( line );
			if ( line == "" || line.Contains( "<" ) )
				continue;
			if ( !line.Contains( "," ) )
				continue;

			TStringArray parts = new TStringArray;
			line.Split( ",", parts );
			if ( parts.Count() < 3 )
				continue;

			string cls = parts.Get( 0 ).Trim();
			string buy = parts.Get( 2 ).Trim();
			if ( buy == "nobuy" || buy == "NOBUY" || buy == "-1" )
				continue;

			facts = facts + cls + " - " + buy + " руб.; ";
			n++;
		}
		CloseFile( fh );

		facts = facts + "Если предмета нет в списке - честно скажи, что не знаешь цену.";
		return facts;
	}

	static void Ask( PlayerBase player, string question )
	{
		if ( !player )
			return;

		Load();

		if ( !m_Enabled )
		{
			TraderMessage.PlayerWhite( "ИИ-торговец выключен (<AiEnabled> no)", player );
			return;
		}

		// анти-спам: не чаще одного вопроса в 3 секунды
		int now = GetGame().GetTime();
		if ( now - m_LastMs < 3000 )
		{
			TraderMessage.PlayerWhite( "Торговец ещё думает над прошлым вопросом...", player );
			return;
		}
		m_LastMs = now;

		string body = "{\"q\":\"" + Sanitize( question ) + "\",\"trader\":\"Торговец\",\"facts\":\"" + Sanitize( BuildFacts() ) + "\",\"max_tokens\":96}";

		TraderMessage.ServerLog( "[AI] ask: " + question );

		RestContext ctx = GetRestApi().GetRestContext( "http://" + m_Url );
		ctx.SetHeader( "application/json" );

		TraderAiRestCallback cb = new TraderAiRestCallback;
		cb.Setup( player );
		ctx.POST( cb, m_Path, body );
	}

	static void Answer( PlayerBase player, string text )
	{
		if ( !player )
			return;
		if ( !player.GetIdentity() )
			return;

		text = text.Trim();
		if ( text.Length() > 400 )
			text = text.Substring( 0, 400 );

		TraderMessage.ServerLog( "[AI] answer: " + text );
		GetGame().RPCSingleParam( player, TRPCs.RPC_AI_ANSWER, new Param1<string>( text ), true, player.GetIdentity() );
	}
}

class TraderAiRestCallback extends RestCallback
{
	PlayerBase m_Player;

	void Setup( PlayerBase player )
	{
		m_Player = player;
	}

	override void OnSuccess( string data, int dataSize )
	{
		TraderAiChat.Answer( m_Player, data );
	}

	override void OnError( int errorCode )
	{
		TraderAiChat.Answer( m_Player, "Торговец молчит: модель недоступна (код " + errorCode + ")." );
	}

	override void OnTimeout()
	{
		TraderAiChat.Answer( m_Player, "Торговец задумался и не ответил. Спроси ещё раз." );
	}
}