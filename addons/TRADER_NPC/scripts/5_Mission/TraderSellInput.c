// ============================================================
// FILE: TraderSellInput.c  (module 5_Mission -> client UI)
//
// Opens the compact sell window with the RIGHT mouse button.
// Vanilla opens MENU_INSPECT from the item icon with the MIDDLE button
// (inventorynew/containeditems/icon.c:  case MouseState.MIDDLE: InspectItem(m_Item)).
// The sell window lives in that same inspect menu, so instead of touching seven
// vanilla MouseClick handlers we intercept the right button in Icon.MouseClick
// (see TraderSellRMB.c) and open the menu exactly like vanilla
// InventoryNew/layoutholder.c does:
//     InventoryMenu menu = ...FindMenu(MENU_INVENTORY);
//     InspectMenuNew inspect = InspectMenuNew.Cast( menu.EnterScriptedMenu(MENU_INSPECT) );
//     hud.ShowHudUI(false); hud.ShowQuickbarUI(false); inspect.SetItem(item);
//
// The window only takes over the right button when the closest trader really
// buys the item - otherwise the vanilla right-click context menu stays intact.
// ============================================================
class TraderSellInput
{
	// does any nearby trader buy this classname?
	static bool TraderBuys( PlayerBase player, string classname )
	{
		if ( !player )
			return false;
		if ( !player.m_Trader_ItemsClassnames || !player.m_Trader_ItemsSellValue )
			return false;
		if ( player.m_Trader_ItemsClassnames.Count() <= 0 )
			return false;

		string lower = classname;
		lower.ToLower();

		for ( int i = 0; i < player.m_Trader_ItemsClassnames.Count(); i++ )
		{
			string row = player.m_Trader_ItemsClassnames.Get( i );
			row.ToLower();
			if ( row != lower )
				continue;
			if ( player.m_Trader_ItemsSellValue.Get( i ) >= 0 )
				return true;
		}
		return false;
	}

	static bool TraderInRange( PlayerBase player )
	{
		if ( !player || !player.m_Trader_TraderPositions )
			return false;
		if ( player.m_Trader_TraderPositions.Count() <= 0 )
			return false;

		float allowed = TR_Helper.GetTraderSellAllowedDistance();
		vector pos = player.GetPosition();

		for ( int i = 0; i < player.m_Trader_TraderPositions.Count(); i++ )
		{
			// радиус сейф-зоны этого торговца, если он больше базового
			float limit = allowed;
			if ( player.m_Trader_TraderSafezones && i < player.m_Trader_TraderSafezones.Count() )
			{
				float zoneRadius = player.m_Trader_TraderSafezones.Get( i );
				if ( zoneRadius > limit )
					limit = zoneRadius;
			}

			float d = vector.Distance( pos, player.m_Trader_TraderPositions.Get( i ) );
			if ( d <= limit || player.IsInSafeZone() )
				return true;
		}
		return false;
	}

	static bool CanOpen( EntityAI item )
	{
		if ( !item )
			return false;

		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if ( !player )
			return false;
		if ( !player.HasReceivedAllTraderData() )
			return false;
		if ( !TraderInRange( player ) )
			return false;

		return TraderBuys( player, item.GetType() );
	}

	static bool Open( EntityAI item )
	{
		if ( !CanOpen( item ) )
			return false;

		InventoryMenu menu = InventoryMenu.Cast( GetGame().GetUIManager().FindMenu( MENU_INVENTORY ) );
		if ( !menu )
			return false;

		InspectMenuNew inspect = InspectMenuNew.Cast( menu.EnterScriptedMenu( MENU_INSPECT ) );
		if ( !inspect )
			return false;

		Hud hud = GetGame().GetMission().GetHud();
		if ( hud )
		{
			hud.ShowHudUI( false );
			hud.ShowQuickbarUI( false );
		}

		inspect.SetItem( item );
		return true;
	}
};
