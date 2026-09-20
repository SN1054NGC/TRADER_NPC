// ============================================================
// ФАЙЛ: TraderNpcMapGps.c
// Метка игрока на карте при наличии GPS (GPSReceiver).
// Используется ванильный API: MapWidget.AddUserMark(pos, text, color, icon).
// Метка ставится один раз при открытии карты (позиция на момент открытия).
// ============================================================
modded class MapMenu
{
	override void OnShow()
	{
		super.OnShow();
		TraderNpcMapGps_Mark();
	}

	void TraderNpcMapGps_Mark()
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if ( !player )
			return;

		if ( !TraderNpcMapGps_HasItem( player, "GPSReceiver" ) )
			return;

		MapWidget mapWidget = MapWidget.Cast( layoutRoot.FindAnyWidget( "Map" ) );
		if ( !mapWidget )
			return;

		mapWidget.AddUserMark( player.GetPosition(), "Я", COLOR_RED, "\\dz\\gear\\navigation\\data\\map_tree_ca.paa" );
	}

	static bool TraderNpcMapGps_HasItem( PlayerBase player, string typePart )
	{
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
};
