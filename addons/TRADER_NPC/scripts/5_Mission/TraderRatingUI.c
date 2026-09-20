// ============================================================
// FILE: TraderRatingUI.c  (module 5_Mission -> client UI)
//
// Shows the current trader discount (survival rating) inside the inventory.
// The badge is an ADDITIONAL overlay: it does not replace the vanilla
// inventory layout, it is created once as a child of the inventory main
// widget (layoutRoot) and refreshed once per second from the net-synced
// PlayerBase fields (m_Trader_RatingDiscount / Progress / LifeSeconds /
// RatingEnabled). Server side remains authoritative: the badge is display only.
//
// Enforce Script notes: override + super, no ternary, no void-as-expression.
// ============================================================
modded class InventoryMenu
{
	private Widget     m_TraderRatingRoot;
	private TextWidget m_TraderRatingValue;
	private TextWidget m_TraderRatingInfo;
	private Widget     m_TraderRatingBar;
	private ButtonWidget m_TraderRatingToggle;
	private Widget     m_TraderRatingPanel;
	private float      m_TraderRatingTimer;

	override void OnShow()
	{
		super.OnShow();
		TraderRatingUI_Ensure();
		TraderRatingUI_Refresh();
	}

	override void Update( float timeslice )
	{
		super.Update( timeslice );

		m_TraderRatingTimer = m_TraderRatingTimer + timeslice;
		if ( m_TraderRatingTimer < 1.0 )
			return;
		m_TraderRatingTimer = 0;

		TraderRatingUI_Refresh();
	}

	// creates the badge once; re-creates it if the inventory root was rebuilt
	void TraderRatingUI_Ensure()
	{
		if ( m_TraderRatingRoot && m_TraderRatingRoot.GetParent() == layoutRoot )
			return;
		if ( !layoutRoot )
			return;

		m_TraderRatingRoot = GetGame().GetWorkspace().CreateWidgets( "TRADER_NPC/scripts/layouts/TraderRating.layout", layoutRoot );
		if ( !m_TraderRatingRoot )
			return;

		m_TraderRatingValue = TextWidget.Cast( m_TraderRatingRoot.FindAnyWidget( "RatingValue" ) );
		m_TraderRatingInfo  = TextWidget.Cast( m_TraderRatingRoot.FindAnyWidget( "RatingInfo" ) );
		m_TraderRatingBar   = m_TraderRatingRoot.FindAnyWidget( "RatingBar" );
		m_TraderRatingToggle = ButtonWidget.Cast( m_TraderRatingRoot.FindAnyWidget( "RatingToggle" ) );
		m_TraderRatingPanel  = m_TraderRatingRoot.FindAnyWidget( "RatingPanel" );
	}

	void TraderRatingUI_Refresh()
	{
		TraderRatingUI_Ensure();
		if ( !m_TraderRatingRoot )
			return;

		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if ( !player || !player.m_Trader_RatingEnabled )
		{
			m_TraderRatingRoot.Show( false );
			return;
		}

		m_TraderRatingRoot.Show( true );

		bool collapsed = player.m_Trader_RatingCollapsed;

		if ( m_TraderRatingPanel )
			m_TraderRatingPanel.Show( !collapsed );

		if ( m_TraderRatingValue )
		{
			m_TraderRatingValue.Show( !collapsed );
			m_TraderRatingValue.SetText( "" + player.m_Trader_RatingDiscount + "%" );
		}

		if ( m_TraderRatingBar )
		{
			m_TraderRatingBar.Show( !collapsed );
			SimpleProgressBarWidget bar = SimpleProgressBarWidget.Cast( m_TraderRatingBar );
			if ( bar )
				bar.SetCurrent( player.m_Trader_RatingProgress * 100.0 );
		}

		if ( m_TraderRatingToggle )
		{
			if ( collapsed )
				m_TraderRatingToggle.SetText( "#tm_rating_show" );
			else
				m_TraderRatingToggle.SetText( "#tm_rating_hide" );
		}

		if ( m_TraderRatingInfo )
		{
			m_TraderRatingInfo.Show( !collapsed );

			string info = "#tm_rating_survived" + " " + TraderRating.FormatSurvived( player.m_Trader_LifeSeconds * 1.0 );
			info = info + ", " + "#tm_rating_distance" + " " + TraderRating.FormatDistance( player.m_Trader_RatingDistance ) + " " + "#tm_rating_km";
			info = info + ", " + "#tm_rating_kills" + " " + player.m_Trader_RatingKills;

			bool hasGps = TraderRatingUI_HasItem( player, "GPSReceiver" );

			// время в игре: с будильником (AlarmClock_*) или с GPS
			if ( hasGps || TraderRatingUI_HasItem( player, "AlarmClock" ) )
				info = info + "  " + TraderRatingUI_ClockText();

			// позиция: только с GPS
			if ( hasGps )
				info = info + "  " + TraderRatingUI_PosText( player );

			m_TraderRatingInfo.SetText( info );
		}
	}

	// есть ли у игрока предмет класса, содержащего подстроку (будильники, GPS)
	static bool TraderRatingUI_HasItem( PlayerBase player, string typePart )
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

	// текущее игровое время "ЧЧ:ММ"
	static string TraderRatingUI_ClockText()
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

	// позиция игрока "X: 13314 Z: 10999"
	static string TraderRatingUI_PosText( PlayerBase player )
	{
		vector pos = player.GetPosition();
		return "X: " + Math.Round( pos[0] ) + " Z: " + Math.Round( pos[2] );
	}

	override bool OnClick( Widget w, int x, int y, int button )
	{
		if ( w == m_TraderRatingToggle )
		{
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
			if ( player )
			{
				player.m_Trader_RatingCollapsed = !player.m_Trader_RatingCollapsed;
				TraderRatingUI_Refresh();
			}
			return true;
		}

		return super.OnClick( w, x, y, button );
	}
};
