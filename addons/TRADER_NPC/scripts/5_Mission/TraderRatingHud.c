// ============================================================
// ФАЙЛ: TraderRatingHud.c
// Панель рейтинга выживания: показ В ИГРЕ (HUD) + общие помощники.
//  * HUD-копия layout создаётся на уровне workspace (видна поверх игры),
//    скрывается, когда открыто любое меню (там показывается своя копия);
//  * обновление раз в секунду через CallLater(..., 1000, true) - в моде нет
//    пофреймового клиентского хука;
//  * в HUD интерактивные элементы (сворачивание, чекбокс "В игре") скрыты -
//    ими управляют из инвентаря, где клики обрабатывает InventoryMenu;
//  * включение/отключение и сворачивание хранятся у игрока:
//    m_Trader_RatingHud / m_Trader_RatingCollapsed.
// ============================================================
class TraderRatingHud
{
	static ref TraderRatingHud s_Instance;

	Widget     m_Root;
	TextWidget m_Value;
	TextWidget m_Info;
	Widget     m_Bar;
	Widget     m_Panel;
	Widget     m_Toggle;
	Widget     m_Check;
	bool       m_Started;

	static TraderRatingHud Get()
	{
		if ( !s_Instance )
			s_Instance = new TraderRatingHud();
		return s_Instance;
	}

	// Запускается один раз (из инвентаря или окна торговца)
	void Bootstrap()
	{
		Ensure();
		if ( m_Started )
			return;
		m_Started = true;
		GetGame().GetCallQueue( CALL_CATEGORY_GUI ).CallLater( Tick, 1000, true );
	}

	void Ensure()
	{
		if ( m_Root )
			return;

		m_Root = GetGame().GetWorkspace().CreateWidgets( "TRADER_NPC/scripts/layouts/TraderRating.layout" );
		if ( !m_Root )
			return;

		m_Value  = TextWidget.Cast( m_Root.FindAnyWidget( "RatingValue" ) );
		m_Info   = TextWidget.Cast( m_Root.FindAnyWidget( "RatingInfo" ) );
		m_Bar    = m_Root.FindAnyWidget( "RatingBar" );
		m_Panel  = m_Root.FindAnyWidget( "RatingPanel" );
		m_Toggle = m_Root.FindAnyWidget( "RatingToggle" );
		m_Check  = m_Root.FindAnyWidget( "RatingHudCheck" );
	}

	// Раз в секунду: обновить и показать/скрыть HUD-копию
	void Tick()
	{
		Ensure();
		if ( !m_Root )
			return;

		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if ( !player || !player.m_Trader_RatingEnabled )
		{
			m_Root.Show( false );
			return;
		}

		// в меню (инвентарь/карта/трейдер) HUD-копия не нужна - там своя панель
		bool menuOpen = ( GetGame().GetUIManager() && GetGame().GetUIManager().GetMenu() );

		m_Root.Show( player.m_Trader_RatingHud && !menuOpen );
		if ( !player.m_Trader_RatingHud || menuOpen )
			return;

		// в HUD интерактивные элементы не нужны
		if ( m_Toggle )
			m_Toggle.Show( false );
		if ( m_Check )
			m_Check.Show( false );

		Fill( player, m_Root, m_Value, m_Info, m_Bar, m_Panel );
	}

	// ============================================================
	// Общие помощники (использует и инвентарь, и HUD)
	// ============================================================
	static void Fill( PlayerBase player, Widget root, TextWidget value, TextWidget info, Widget bar, Widget panel )
	{
		bool collapsed = player.m_Trader_RatingCollapsed;

		if ( panel )
			panel.Show( !collapsed );

		if ( value )
		{
			value.Show( !collapsed );
			value.SetText( "" + player.m_Trader_RatingDiscount + "%" );
		}

		if ( bar )
		{
			bar.Show( !collapsed );
			SimpleProgressBarWidget pb = SimpleProgressBarWidget.Cast( bar );
			if ( pb )
				pb.SetCurrent( player.m_Trader_RatingProgress * 100.0 );
		}

		if ( info )
		{
			info.Show( !collapsed );
			info.SetText( BuildInfo( player ) );

			int textW, textH;
			info.GetTextSize( textW, textH );

			float panelW = ( textW + 150 ) / 0.70;
			if ( panelW < 380 )
				panelW = 380;
			if ( panelW > 1250 )
				panelW = 1250;

			root.SetSize( panelW / 1920.0, 0.038 );
		}
	}

	static string BuildInfo( PlayerBase player )
	{
		string info = "#tm_rating_survived" + " " + TraderRating.FormatSurvived( player.m_Trader_LifeSeconds * 1.0 );
		info = info + ", " + "#tm_rating_distance" + " " + TraderRating.FormatDistance( player.m_Trader_RatingDistance ) + " " + "#tm_rating_km";
		info = info + ", " + "#tm_rating_kills" + " " + player.m_Trader_RatingKills;

		bool hasGps = HasItem( player, "GPSReceiver" );

		if ( hasGps || HasItem( player, "AlarmClock" ) )
			info = info + "  " + ClockText();

		if ( hasGps )
			info = info + "  " + PosText( player );

		return info;
	}

	static bool HasItem( PlayerBase player, string typePart )
	{
		if ( !player )
			return false;

		array<EntityAI> items = new array<EntityAI>;
		player.GetInventory().EnumerateInventory( InventoryTraversalType.PREORDER, items );

		for ( int i = 0; i < items.Count(); i++ )
		{
			EntityAI e = items.Get( i );
			if ( e && e.GetType().Contains( typePart ) )
				return true;
		}

		return false;
	}

	static string ClockText()
	{
		int year, month, day, hour, minute;
		GetGame().GetWorld().GetDate( year, month, day, hour, minute );

		string hh = "" + hour;
		if ( hour < 10 )
			hh = "0" + hh;

		string mm = "" + minute;
		if ( minute < 10 )
			mm = "0" + mm;

		return hh + ":" + mm;
	}

	static string PosText( PlayerBase player )
	{
		vector pos = player.GetPosition();
		return "X: " + Math.Round( pos[0] ) + " Z: " + Math.Round( pos[2] );
	}
};
