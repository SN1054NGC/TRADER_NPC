// ============================================================
// FILE: TraderTradeRules.c   (module 3_Game - shared by World and Mission)
//
// Trade rules that BOTH sides need:
//   * 4_World  (DayZPlayerImplement.handleSellRPC) - what the trader pays
//   * 5_Mission (TraderAutoPrices)                - the generated price list
//
// Module note: 4_World may not reference 5_Mission classes, so these rules live
// in 3_Game and are loaded once on the server from TraderVariables.txt.
//
// What is here:
//   * ruined items: the trader takes destroyed gear as scrap ("утилизация")
//     for RuinedPrice coins (default 0) and DELETES it, instead of refusing.
//   * food stages: ROTTEN pays a flat RottenPrice (default 1), BURNED/RAW are
//     scaled down, BAKED/BOILED/DRIED keep the full price.
//   * liquids: the value of a container is its millilitres times a per-liquid
//     factor (water, river water, disinfectant, fuel).
//   * wear: damaged-but-alive items are paid proportionally to health (this is
//     done by TraderSmartSellEngine.Condition, the minimum stays 1).
// ============================================================
class TraderTradeRules
{
	static int   m_RuinedPrice   = 0;
	static int   m_RottenPrice   = 1;
	static float m_FoodStageBurn = 0.20;
	static float m_FoodStageRaw  = 0.50;
	static float m_LiquidWater   = 0.30;
	static float m_LiquidRiver   = 0.20;
	static float m_LiquidDisinfect = 3.00;
	static float m_LiquidFuel    = 1.50;
	static bool  m_Loaded = false;
	// снаряжать магазин указанным патроном (экспериментально: SetCartridgeAtIndex
	// объявлен с out-параметрами и в ванильных скриптах не используется)
	static bool  m_MagFill = false;

	static string TagValue( string line )
	{
		int a = line.IndexOf( ">" );
		if ( a < 0 )
			return "";
		string v = line.Substring( a + 1, line.Length() - a - 1 );
		v.Replace( "<", " " );
		v = TraderNpcText.Clean( v );
		return TraderNpcText.Tidy( v );
	}

	static void Load()
	{
		if ( m_Loaded )
			return;
		m_Loaded = true;

		FileHandle fh = OpenFile( "$profile:Trader/TraderVariables.txt", FileMode.READ );
		if ( fh == 0 )
			return;

		string line = "";
		while ( FGets( fh, line ) != -1 )
		{
			line = TraderNpcText.Clean( line );
			if ( line == "" )
				continue;

			if ( line.Contains( "<AutoPricesRuinedPrice>" ) )
				m_RuinedPrice = TagValue( line ).ToInt();
			else if ( line.Contains( "<AutoPricesRottenPrice>" ) )
				m_RottenPrice = TagValue( line ).ToInt();
			else if ( line.Contains( "<AutoPricesFoodStageBurn>" ) )
				m_FoodStageBurn = TagValue( line ).ToFloat();
			else if ( line.Contains( "<AutoPricesFoodStageRaw>" ) )
				m_FoodStageRaw = TagValue( line ).ToFloat();
			else if ( line.Contains( "<AutoPricesLiquidWater>" ) )
				m_LiquidWater = TagValue( line ).ToFloat();
			else if ( line.Contains( "<AutoPricesLiquidRiver>" ) )
				m_LiquidRiver = TagValue( line ).ToFloat();
			else if ( line.Contains( "<AutoPricesLiquidDisinfect>" ) )
				m_LiquidDisinfect = TagValue( line ).ToFloat();
			else if ( line.Contains( "<AutoPricesLiquidFuel>" ) )
				m_LiquidFuel = TagValue( line ).ToFloat();
			else if ( line.Contains( "<AutoPricesMagFill>" ) )
				m_MagFill = TagValue( line ) == "yes";
		}
		CloseFile( fh );
	}

	// ----------------------------------------------------------------
	// price factor for the food stage of an edible (1.0 when not edible)
	// ROTTEN returns 0 here - the caller pays the flat RottenPrice instead
	// ----------------------------------------------------------------
	// Стадии еды живут в TraderSmartSellEngine (4_World): Edible_Base и
	// FoodStageType объявлены в модуле 4_World, а этот класс - в 3_Game, и
	// ссылаться вверх по модулям нельзя.
	//
	// Жидкости: объём (мл) уже учитывается штатной логикой стака
	// (GetSellableAmount -> GetQuantity), поэтому полупустая фляга стоит
	// ровно половину. Множители по ТИПУ жидкости требуют API, которого в
	// ванильных скриптах нет - включаем, когда будет проверенный доступ.

	// ----------------------------------------------------------------
	// liquids: ОТЛОЖЕНО до проверенного API типа жидкости.
	// Сейчас объём (мл) уже учитывается: GetSellableAmount -> GetQuantity(),
	// цена стака = цена за единицу * количество, поэтому полупустая фляга
	// стоит ровно половину. Множители ниже заготовлены и не используются.
	// ----------------------------------------------------------------
};
