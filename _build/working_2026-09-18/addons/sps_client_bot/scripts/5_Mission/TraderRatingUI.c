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

		m_TraderRatingRoot = GetGame().GetWorkspace().CreateWidgets( "sps_client_bot/scripts/layouts/TraderRating.layout", layoutRoot );
		if ( !m_TraderRatingRoot )
			return;

		m_TraderRatingValue = TextWidget.Cast( m_TraderRatingRoot.FindAnyWidget( "RatingValue" ) );
		m_TraderRatingInfo  = TextWidget.Cast( m_TraderRatingRoot.FindAnyWidget( "RatingInfo" ) );
		m_TraderRatingBar   = m_TraderRatingRoot.FindAnyWidget( "RatingBarFill" );
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

		if ( m_TraderRatingValue )
			m_TraderRatingValue.SetText( "-" + player.m_Trader_RatingDiscount + "%" );

		if ( m_TraderRatingInfo )
			m_TraderRatingInfo.SetText( "#tm_rating_survived" + " " + TraderRating.FormatSurvived( player.m_Trader_LifeSeconds * 1.0 ) );

		if ( m_TraderRatingBar )
			m_TraderRatingBar.SetSize( player.m_Trader_RatingProgress, 1.0 );
	}
};
