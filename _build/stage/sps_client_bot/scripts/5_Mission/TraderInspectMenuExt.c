// ============================================================
// FILE: TraderInspectMenuExt.c
// Compact item-sell inspect window (ArmorStats / vanilla style).
// Loaded on MMB -> MENU_INSPECT -> InspectMenuNew.
//
// The window shows the named item (name / quality / description, filled by
// vanilla super.SetItem) plus a real sell price, an optional AMOUNT SLIDER
// for stacks and ammo piles, and a red SELL button. When nothing buyable is
// nearby the sell area is hidden and NoPriceWidget shows a localized message.
// The server-authoritative appraisal (RPC_APPRAISE_SELL / _REPLY) overwrites
// the locally computed figure so the shown value matches what a click pays.
//
// Enforce Script rules respected here: no ternary operator, no void method
// used as an expression, single-line string concatenations, override + super.
// ============================================================
modded class InspectMenuNew
{
	private EntityAI      m_sellInspectItem;
	private int           m_sellTraderIndex  = -1;
	private int           m_sellPriceRow     = -1;
	private int           m_sellRowQuantity  = 0;   // price-row Quantity; negative means W/M/S (whole item)
	private int           m_sellAvailable    = 1;   // units the player really holds
	private int           m_sellAmount       = 1;   // amount currently selected with the slider

	private Widget        m_sellRoot;
	private Widget        m_sellRow;
	private TextWidget    m_sellPriceText;
	private TextWidget    m_sellAmountText;
	private SliderWidget  m_sellSlider;
	private TextWidget    m_noPriceText;
	private ButtonWidget  m_btnSell;

	// ==================================================================
	// init / widget binding
	// ==================================================================
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets( "sps_client_bot/scripts/layouts/TraderSellPopup.layout" );
		TraderUI_BindWidgets( layoutRoot );
		return layoutRoot;
	}

	void TraderUI_BindWidgets( Widget root )
	{
		m_sellRoot = root;

		if ( !root )
			return;

		m_sellRow        = root.FindAnyWidget( "SellRow" );
		m_sellPriceText  = TextWidget.Cast( root.FindAnyWidget( "SellPriceWidget" ) );
		m_sellAmountText = TextWidget.Cast( root.FindAnyWidget( "SellAmountWidget" ) );
		m_sellSlider     = SliderWidget.Cast( root.FindAnyWidget( "SellSlider" ) );
		m_noPriceText    = TextWidget.Cast( root.FindAnyWidget( "NoPriceWidget" ) );
		m_btnSell        = ButtonWidget.Cast( root.FindAnyWidget( "SellButton" ) );

		// start hidden until the real sellability state is known
		if ( m_sellRow ) m_sellRow.Show( false );
		if ( m_btnSell ) m_btnSell.Show( false );
		if ( m_noPriceText ) m_noPriceText.Show( false );
		if ( m_sellSlider ) m_sellSlider.Show( false );
		if ( m_sellAmountText ) m_sellAmountText.Show( false );
	}

	// Safety net: when another mod wins the InspectMenuNew.Init() override
	// chain (it does not call super), our Init never runs. In that case we
	// attach our compact panel to whatever layout is active instead.
	void TraderUI_EnsureWidgets()
	{
		if ( m_btnSell )
			return;
		if ( !layoutRoot )
			return;

		Widget existing = layoutRoot.FindAnyWidget( "SellButton" );
		if ( existing )
		{
			TraderUI_BindWidgets( layoutRoot );
			return;
		}

		Widget overlay = GetGame().GetWorkspace().CreateWidgets( "sps_client_bot/scripts/layouts/TraderSellPopup.layout", layoutRoot );
		if ( !overlay )
			return;

		TraderUI_BindWidgets( overlay );
	}

	// ==================================================================
	// helpers
	// ==================================================================
	PlayerBase TraderUI_Player()
	{
		return PlayerBase.Cast( GetGame().GetPlayer() );
	}

	int TraderUI_NearestTraderIndex( PlayerBase player, float maxDist )
	{
		if ( !player || !player.m_Trader_TraderPositions )
			return -1;

		vector pos = player.GetPosition();
		int bestIdx = -1;
		float bestDist = maxDist + 1.0;

		for ( int i = 0; i < player.m_Trader_TraderPositions.Count(); i++ )
		{
			float d = vector.Distance( pos, player.m_Trader_TraderPositions.Get( i ) );
			if ( d < bestDist )
			{
				bestDist = d;
				bestIdx = i;
			}
		}

		if ( bestDist <= maxDist )
			return bestIdx;
		return -1;
	}

	int TraderUI_FindPriceRow( PlayerBase player, int traderIndex, string typeName )
	{
		if ( !player )
			return -1;
		if ( !player.m_Trader_ItemsClassnames || !player.m_Trader_ItemsTraderId )
			return -1;

		string lower = typeName;
		lower.ToLower();

		for ( int i = 0; i < player.m_Trader_ItemsClassnames.Count(); i++ )
		{
			if ( player.m_Trader_ItemsTraderId.Get( i ) != traderIndex )
				continue;

			string row = player.m_Trader_ItemsClassnames.Get( i );
			row.ToLower();
			if ( row == lower )
				return i;
		}
		return -1;
	}

	bool TraderUI_HasBuyRow()
	{
		PlayerBase player = TraderUI_Player();
		if ( !player || m_sellPriceRow < 0 )
			return false;
		if ( !player.m_Trader_ItemsSellValue )
			return false;
		if ( m_sellPriceRow >= player.m_Trader_ItemsSellValue.Count() )
			return false;
		return player.m_Trader_ItemsSellValue.Get( m_sellPriceRow ) >= 0;
	}

	// Figures out whether the closest trader in range actually buys this item
	// and fills m_sellTraderIndex / m_sellPriceRow / m_sellRowQuantity.
	void TraderUI_ResolveBuyEntry()
	{
		m_sellTraderIndex = -1;
		m_sellPriceRow = -1;
		m_sellRowQuantity = 0;

		if ( !m_sellInspectItem )
			return;

		PlayerBase player = TraderUI_Player();
		if ( !player )
			return;

		if ( !player.m_Trader_TraderPositions || !player.m_Trader_TraderIDs )
			return;

		float near = TR_Helper.GetTraderSellAllowedDistance();
		// Продажа разрешена из любой точки сейф-зоны: расширяем радиус поиска торговца
		if ( player.IsInSafeZone() )
			near = 100000.0;

		int traderIdx = TraderUI_NearestTraderIndex( player, near );
		if ( traderIdx < 0 || traderIdx >= player.m_Trader_TraderIDs.Count() )
			return;

		int rowIdx = TraderUI_FindPriceRow( player, traderIdx, m_sellInspectItem.GetType() );
		if ( rowIdx < 0 )
			return;

		if ( !player.m_Trader_ItemsSellValue || !player.m_Trader_ItemsQuantity )
			return;

		if ( rowIdx >= player.m_Trader_ItemsSellValue.Count() )
			return;
		if ( rowIdx >= player.m_Trader_ItemsQuantity.Count() )
			return;

		if ( player.m_Trader_ItemsSellValue.Get( rowIdx ) < 0 )
			return;

		m_sellTraderIndex = traderIdx;
		m_sellPriceRow = rowIdx;
		m_sellRowQuantity = player.m_Trader_ItemsQuantity.Get( rowIdx );
	}

	// How many units of the inspected item the player can sell at once.
	int TraderUI_AvailableAmount()
	{
		ItemBase ib = ItemBase.Cast( m_sellInspectItem );
		if ( !ib )
			return 1;
		return TraderSmartSellEngine.GetSellableAmount( ib );
	}

	// The slider is only meaningful for real stacks / ammo piles. Price rows
	// flagged W (weapon), M (magazine) or S (steak) are always whole items.
	bool TraderUI_CanUseSlider()
	{
		if ( m_sellRowQuantity == -3 || m_sellRowQuantity == -4 || m_sellRowQuantity == -5 )
			return false;
		if ( m_sellAvailable <= 1 )
			return false;
		return true;
	}

	bool TraderUI_CanSellNow()
	{
		if ( !m_sellInspectItem )
			return false;

		ItemBase ib = ItemBase.Cast( m_sellInspectItem );
		if ( ib && ib.IsRuined() )
			return false;

		if ( !TraderUI_HasBuyRow() )
			return false;

		return true;
	}

	string TraderUI_CurrencyName()
	{
		PlayerBase player = TraderUI_Player();
		if ( player && player.m_Trader_CurrencyName != "" )
			return player.m_Trader_CurrencyName;
		return "";
	}

	// ==================================================================
	// amount slider
	// ==================================================================
	void TraderUI_UpdateAmountText()
	{
		if ( !m_sellAmountText )
			return;

		if ( !TraderUI_CanUseSlider() )
		{
			m_sellAmountText.SetText( "" );
			return;
		}

		m_sellAmountText.SetText( "#tm_amount" + " " + m_sellAmount + " / " + m_sellAvailable );
	}

	// Clamp the chosen amount, push it into the slider and show/hide the row.
	void TraderUI_SyncSlider()
	{
		if ( m_sellAmount < 1 )
			m_sellAmount = m_sellAvailable;
		if ( m_sellAmount > m_sellAvailable )
			m_sellAmount = m_sellAvailable;
		if ( m_sellAmount < 1 )
			m_sellAmount = 1;

		if ( !TraderUI_CanUseSlider() )
		{
			if ( m_sellSlider ) m_sellSlider.Show( false );
			if ( m_sellAmountText ) m_sellAmountText.Show( false );
			return;
		}

		if ( m_sellSlider )
		{
			float sliderMax = m_sellAvailable;
			float sliderNow = m_sellAmount;
			m_sellSlider.SetMinMax( 1.0, sliderMax );
			m_sellSlider.SetStep( 1.0 );
			m_sellSlider.SetCurrent( sliderNow );
			m_sellSlider.Show( true );
		}

		if ( m_sellAmountText )
		{
			m_sellAmountText.Show( true );
			TraderUI_UpdateAmountText();
		}
	}

	// ==================================================================
	// price (local best effort; the server reply overwrites it)
	// ==================================================================
	int TraderUI_ComputePrice()
	{
		PlayerBase player = TraderUI_Player();
		ItemBase ib = ItemBase.Cast( m_sellInspectItem );
		if ( !player || !ib )
			return 0;
		if ( m_sellPriceRow < 0 || !player.m_Trader_ItemsSellValue )
			return 0;
		if ( m_sellPriceRow >= player.m_Trader_ItemsSellValue.Count() )
			return 0;

		int rowSell = player.m_Trader_ItemsSellValue.Get( m_sellPriceRow );

		// amount 0 tells the server to use the price-row quantity (whole item)
		int amount = 0;
		if ( TraderUI_CanUseSlider() )
			amount = m_sellAmount;

		int basePrice = rowSell;
		if ( amount > 0 )
			basePrice = TraderSmartSellEngine.ComputeStackBasePrice( player, m_sellTraderIndex, ib.GetType(), amount, rowSell );

		int total = TraderSmartSellEngine.Condition( ib, basePrice );

		// accessory / loaded rounds bonus only applies to a whole packed item
		if ( amount <= 0 )
			total = total + TraderSmartSellEngine.ComputeExtraBonus( player, m_sellTraderIndex, ib );

		return total;
	}

	// ==================================================================
	// rendering / state
	// ==================================================================
	// showMode: 0 -> hide the sell strip (fallback text), 1 -> show it
	void TraderUI_ApplyView( int showMode )
	{
		if ( m_sellRow ) m_sellRow.Show( showMode == 1 );
		if ( m_btnSell ) m_btnSell.Show( showMode == 1 );
		if ( m_noPriceText ) m_noPriceText.Show( showMode != 1 );

		if ( showMode != 1 )
		{
			if ( m_sellSlider ) m_sellSlider.Show( false );
			if ( m_sellAmountText ) m_sellAmountText.Show( false );
		}
	}

	void TraderUI_UpdateSellPriceView()
	{
		if ( !m_sellPriceText || !m_btnSell )
		{
			TraderUI_ApplyView( 0 );
			return;
		}

		if ( !TraderUI_HasBuyRow() )
		{
			m_sellPriceText.SetText( "" );
			TraderUI_ApplyView( 0 );
			if ( m_noPriceText ) m_noPriceText.SetText( "#tm_no_trader_nearby" );
			return;
		}

		int realTotal = TraderUI_ComputePrice();

		string txt = "#tm_trader_pays" + " " + realTotal;
		string cur = TraderUI_CurrencyName();
		if ( cur != "" )
			txt = txt + " " + cur;

		m_sellPriceText.SetText( txt );
		m_sellPriceText.SetColor( ARGB( 255, 130, 255, 130 ) );
		TraderUI_ApplyView( 1 );
		TraderUI_SyncSlider();

		TraderUI_RequestAppraisal();
	}

	void TraderUI_RefreshSellButton()
	{
		TraderUI_EnsureWidgets();

		if ( !m_sellInspectItem )
		{
			TraderUI_ApplyView( 0 );
			return;
		}

		TraderUI_ResolveBuyEntry();

		ItemBase ib = ItemBase.Cast( m_sellInspectItem );
		if ( ib && ib.IsRuined() )
		{
			if ( m_sellPriceText ) m_sellPriceText.SetText( "" );
			TraderUI_ApplyView( 0 );
			if ( m_noPriceText ) m_noPriceText.SetText( "#tm_ruined_not_sellable" );
			return;
		}

		if ( !TraderUI_HasBuyRow() )
		{
			if ( m_sellPriceText ) m_sellPriceText.SetText( "" );
			TraderUI_ApplyView( 0 );
			if ( m_noPriceText ) m_noPriceText.SetText( "#tm_no_trader_nearby" );
			return;
		}

		m_sellAvailable = TraderUI_AvailableAmount();
		if ( m_sellAmount < 1 )
			m_sellAmount = m_sellAvailable;

		TraderUI_UpdateSellPriceView();
	}

	// ==================================================================
	// server-authoritative appraisal
	// ==================================================================
	void TraderUI_RequestAppraisal()
	{
		PlayerBase player = TraderUI_Player();
		if ( !player ) return;
		if ( m_sellPriceRow < 0 || m_sellTraderIndex < 0 ) return;

		int amount = 0;
		if ( TraderUI_CanUseSlider() )
			amount = m_sellAmount;

		player.RequestSellAppraise( m_sellTraderIndex, m_sellPriceRow, amount );

		GetGame().GetCallQueue( CALL_CATEGORY_GUI ).Remove( this.ApplyAppraisedSellPrice );
		GetGame().GetCallQueue( CALL_CATEGORY_GUI ).CallLater( this.ApplyAppraisedSellPrice, 250, false );
	}

	// Overwrite the local figure with the server reply when it still matches
	// the item / row / amount the request was made for.
	void ApplyAppraisedSellPrice()
	{
		if ( !m_sellPriceText ) return;

		PlayerBase player = TraderUI_Player();
		if ( !player ) return;

		if ( player.m_Trader_ApprRow != m_sellPriceRow ) return;
		if ( player.m_Trader_ApprTotal < 0 ) return;

		int amount = 0;
		if ( TraderUI_CanUseSlider() )
			amount = m_sellAmount;
		if ( player.m_Trader_ApprAmount != amount ) return;

		string txt = "#tm_trader_pays" + " " + player.m_Trader_ApprTotal;
		if ( player.m_Trader_ApprBonus > 0 )
			txt = txt + " (#tm_bonus_short +" + player.m_Trader_ApprBonus + ")";

		string cur = TraderUI_CurrencyName();
		if ( cur != "" )
			txt = txt + " " + cur;

		m_sellPriceText.SetText( txt );
		m_sellPriceText.SetColor( ARGB( 255, 130, 255, 130 ) );
	}

	// ==================================================================
	// overrides
	// ==================================================================
	void TraderUI_ReturnToInventory()
	{
		// MENU_INSPECT lives above the inventory; closing only the overlay keeps
		// the player seated in the item inventory that opened it.
		Close();
	}

	override void SetItem( EntityAI item )
	{
		m_sellInspectItem = item;
		m_sellAmount = 0;                 // 0 -> "not chosen yet", defaults to the full stack
		super.SetItem( item );            // fills name / quality / description into our layout

		// Keep the 3D preview identical to the trader BUY window (TraderMenu):
		// same model position and orientation, so the model scales and centres the
		// same way in both windows (vanilla inspect uses SetModelPosition(0,0,1)).
		ItemPreviewWidget preview = ItemPreviewWidget.Cast( layoutRoot.FindAnyWidget( "ItemFrameWidget" ) );
		if ( preview )
		{
			preview.SetModelPosition( Vector( 1.0, 1.0, 0.5 ) );
			preview.SetModelOrientation( Vector( 0, 0, 0 ) );
		}

		TraderUI_RefreshSellButton();
	}

	// slider drag / release
	override bool OnChange( Widget w, int x, int y, bool finished )
	{
		if ( w && w == m_sellSlider )
		{
			int chosen = Math.Round( m_sellSlider.GetCurrent() );
			if ( chosen < 1 )
				chosen = 1;
			if ( chosen > m_sellAvailable )
				chosen = m_sellAvailable;

			if ( chosen != m_sellAmount )
			{
				m_sellAmount = chosen;
				TraderUI_UpdateAmountText();
				TraderUI_UpdateSellPriceView();
			}
			else if ( finished )
			{
				TraderUI_UpdateSellPriceView();
			}

			return true;
		}

		return super.OnChange( w, x, y, finished );
	}

	override bool OnClick( Widget w, int x, int y, int button )
	{
		if ( w && w == m_btnSell )
		{
			if ( TraderUI_HasBuyRow() && m_sellPriceRow >= 0 && m_sellTraderIndex >= 0 )
			{
				ItemBase ib = ItemBase.Cast( m_sellInspectItem );
				if ( ib && !ib.IsRuined() )
				{
					PlayerBase player = TraderUI_Player();

					int amount = 0;
					if ( TraderUI_CanUseSlider() )
						amount = m_sellAmount;

					GetGame().RPCSingleParam( player, TRPCs.RPC_SELL, new Param4<int, int, string, int>( m_sellTraderIndex, m_sellPriceRow, "", amount ), true );

					// after selling, step back out of the compact sell overlay
					// straight into the (already open) inventory menu
					TraderUI_ReturnToInventory();
				}
			}
			return true;
		}

		return super.OnClick( w, x, y, button );
	}
};
