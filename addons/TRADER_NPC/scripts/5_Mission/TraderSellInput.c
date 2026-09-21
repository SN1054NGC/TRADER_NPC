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

	// Диагностика правой кнопки: объясняет, почему окно продажи не открылось.
	// Молчит, если игрок вообще не у торговца (там причина очевидна), иначе
	// пишет в чат одну строку: нет данных / нет в списке / цена < 0.
	static void Explain( EntityAI item )
	{
		if ( !item )
			return;

		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if ( !player )
			return;
		if ( !TraderInRange( player ) )
			return;

		if ( !player.HasReceivedAllTraderData() )
		{
			g_Game.Chat( "[TRADER] данные торговца ещё не получены", "colorAction" );
			return;
		}

		string type = item.GetType();
		string lowered = type;
		lowered.ToLower();

		if ( player.m_Trader_ItemsClassnames && player.m_Trader_ItemsSellValue )
		{
			for ( int i = 0; i < player.m_Trader_ItemsClassnames.Count(); i++ )
			{
				string row = player.m_Trader_ItemsClassnames.Get( i );
				row.ToLower();
				if ( row != lowered )
					continue;

				if ( i >= player.m_Trader_ItemsSellValue.Count() )
					break;

				int price = player.m_Trader_ItemsSellValue.Get( i );
				if ( price < 0 )
					g_Game.Chat( "[TRADER] " + type + ": торговец не покупает (цена " + price + ")", "colorAction" );
				else
					g_Game.Chat( "[TRADER] " + type + ": цена " + price + ", окно должно было открыться", "colorAction" );
				return;
			}
		}

		g_Game.Chat( "[TRADER] " + type + ": нет ни у одного торговца в списке", "colorAction" );
	}

	static bool Open( EntityAI item )
	{
		if ( !CanOpen( item ) )
		{
			Explain( item );
			return false;
		}

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
