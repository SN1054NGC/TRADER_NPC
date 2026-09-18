// ============================================================
// FILE: TraderAutoPrices.c   (module 5_Mission, SERVER only)
//
// WHAT IT DOES
//   Reads the mission Central Economy file <mission>/db/types.xml and computes
//   a buy/sell price for every TRADEABLE class that the manual TraderConfig.txt
//   does NOT list (onlyUnlisted = yes by default). The result is written as a
//   NORMAL config fragment into
//        $profile:Trader/TraderConfig_auto.txt
//   Nothing is applied automatically - the owner reviews the file and includes
//   it with the mechanism that already exists in the config:
//        <Trader> Misc Trader
//            <Category> Auto
//                <OpenFile>TraderConfig_auto.txt
//   The include inherits the current <Trader>/<Category>, so the fragment itself
//   contains only item lines (plus comments).
//
// PRICE MODEL
//   <cost> from types.xml is deliberately NOT the base: on many servers every
//   type carries the same cost (this mission: 100 for 2186 of 2243 types), so it
//   carries no information. The base comes from the CE category and is shaped by
//   usage, tier (value) and rarity (nominal):
//        buy  = clamp( round( base(category) * usageMul(usage) * tierMul(value)
//                     * rarityMul(nominal) ), MinBuy, MaxBuy )
//        sell = max( 1, round( buy * SellCoef ) )
//   Everything is configurable in TraderVariables.txt (see Configure()).
//
// SAFETY
//   * server only, runs once per mission start
//   * writes exactly one file, never touches the manual config
//   * disabled by default (<AutoPrices> no)
//   * classes that cannot be priced are skipped and counted
//
// Enforce notes: no ternary, no void-as-expression, single-line strings.
// ============================================================
class TraderAutoPrices
{
	static string m_VarsFile  = "$profile:Trader/TraderVariables.txt";
	static string m_OutFile   = "$profile:Trader/TraderConfig_auto.txt";
	static string m_TypesFile = "";

	static bool   m_Enabled    = false;
	static bool   m_OnlyListed = true;
	static float  m_SellCoef   = 0.10;
	static int    m_MinBuy     = 5;
	static int    m_MaxBuy     = 50000;
	static int    m_MaxRows    = 3000;
	static string m_BaseTrader = "Misc Trader";
	static string m_BaseCat    = "Auto";
	// новое: мощность калибра, вместимость, округление цены, точечные исключения
	static bool   m_UseCaliber = true;
	static bool   m_UseCapacity = true;
	static int    m_PriceStep  = 10;
	static string m_SkipClasses = "";

	// CE categories that must never be auto-priced (vehicles have their own file)
	static const string m_SkipCats   = "zombie,animal,building,vehicle,vehiclesparts,tree,bush,rock,static,misc";
	// class-name prefixes that are never tradeable loot ('#' = CE service groups)
	static const string m_SkipPrefix = "#,Zmb,Animal,Land,StaticObj,Car_,Boat_,Plane_,House_,Grave,Underground";

	// ------------------------------------------------------------------
	// knobs are read from TraderVariables.txt so this file stays
	// self-contained and missionServer needs no extra parsing
	// ------------------------------------------------------------------
	static void Configure()
	{
		FileHandle fh = OpenFile( m_VarsFile, FileMode.READ );
		if ( fh == 0 )
			return;

		string line = "";
		while ( FGets( fh, line ) != -1 )
		{
			line = FileReadHelper.TrimComment( line );
			if ( line == "" )
				continue;

			if ( line.Contains( "<AutoPrices>" ) )
				m_Enabled = TagValue( line ) == "yes";
			else if ( line.Contains( "<AutoPricesSellCoef>" ) )
				m_SellCoef = TagValue( line ).ToFloat();
			else if ( line.Contains( "<AutoPricesMinBuy>" ) )
				m_MinBuy = TagValue( line ).ToInt();
			else if ( line.Contains( "<AutoPricesMaxBuy>" ) )
				m_MaxBuy = TagValue( line ).ToInt();
			else if ( line.Contains( "<AutoPricesMaxRows>" ) )
				m_MaxRows = TagValue( line ).ToInt();
			else if ( line.Contains( "<AutoPricesTrader>" ) )
				m_BaseTrader = TagValue( line );
			else if ( line.Contains( "<AutoPricesCategory>" ) )
				m_BaseCat = TagValue( line );
			else if ( line.Contains( "<AutoPricesTypesFile>" ) )
				m_TypesFile = TagValue( line );
			else if ( line.Contains( "<AutoPricesOnlyUnlisted>" ) )
				m_OnlyListed = TagValue( line ) == "yes";
			else if ( line.Contains( "<AutoPricesUseCaliber>" ) )
				m_UseCaliber = TagValue( line ) == "yes";
			else if ( line.Contains( "<AutoPricesUseCapacity>" ) )
				m_UseCapacity = TagValue( line ) == "yes";
			else if ( line.Contains( "<AutoPricesPriceStep>" ) )
				m_PriceStep = TagValue( line ).ToInt();
			else if ( line.Contains( "<AutoPricesSkipClasses>" ) )
				m_SkipClasses = TagValue( line );
		}
		CloseFile( fh );
	}

	// <AutoPrices> yes        -> yes
	static string TagValue( string line )
	{
		int a = line.IndexOf( ">" );
		if ( a < 0 )
			return "";
		string v = line.Substring( a + 1, line.Length() - a - 1 );
		v.Replace( "<", " " );
		v = FileReadHelper.TrimComment( v );
		return FileReadHelper.TrimSpaces( v );
	}

	// name="XXX" -> XXX
	// Implemented without a quote character in a string literal (unproven in
	// Enforce): take everything after '=', cut at '>' and '/', then drop the
	// surrounding quotes positionally.
	static string AttrValue( string line )
	{
		int a = line.IndexOf( "=" );
		if ( a < 0 )
			return "";
		string rest = line.Substring( a + 1, line.Length() - a - 1 );
		int b = rest.IndexOf( ">" );
		if ( b >= 0 )
			rest = rest.Substring( 0, b );
		int c = rest.IndexOf( "/" );
		if ( c >= 0 )
			rest = rest.Substring( 0, c );
		rest = FileReadHelper.TrimSpaces( rest );
		if ( rest.Length() >= 2 )
			rest = rest.Substring( 1, rest.Length() - 2 );
		return FileReadHelper.TrimSpaces( rest );
	}

	// <nominal>8</nominal> -> 8
	static int TagInt( string line )
	{
		int a = line.IndexOf( ">" );
		int b = line.IndexOf( "</" );
		if ( a < 0 || b < 0 || b <= a )
			return 0;
		string v = line.Substring( a + 1, b - a - 1 );
		return FileReadHelper.TrimSpaces( v ).ToInt();
	}

	static bool InList( string list, string value )
	{
		if ( value == "" )
			return false;
		TStringArray parts = new TStringArray;
		list.Split( ",", parts );
		for ( int i = 0; i < parts.Count(); i++ )
		{
			string p = FileReadHelper.TrimSpaces( parts.Get( i ) );
			if ( p == "" )
				continue;
			if ( p == "#" )
			{
				if ( value.Get( 0 ) == "#" )
					return true;
				continue;
			}
			if ( value.IndexOf( p ) == 0 )
				return true;
		}
		return false;
	}

	// ---- price shaping ------------------------------------------------
	static int BaseForCategory( string cat )
	{
		if ( cat == "weapons" )     return 2500;
		if ( cat == "clothes" )     return 300;
		if ( cat == "containers" )  return 400;
		if ( cat == "tools" )       return 200;
		if ( cat == "food" )        return 60;
		if ( cat == "explosives" )  return 900;
		if ( cat == "medical" )     return 120;
		return 150;
	}

	static float UsageMul( string usage )
	{
		if ( usage == "Military" )    return 1.60;
		if ( usage == "Police" )      return 1.30;
		if ( usage == "Medic" )       return 1.20;
		if ( usage == "Underground" ) return 1.20;
		if ( usage == "Hunting" )     return 1.10;
		if ( usage == "Town" )        return 1.00;
		if ( usage == "Office" )      return 0.95;
		if ( usage == "Industrial" )  return 0.95;
		if ( usage == "Village" )     return 0.90;
		if ( usage == "Coast" )       return 0.90;
		if ( usage == "Farm" )        return 0.85;
		return 1.00;
	}

	static float TierMul( string tier )
	{
		if ( tier == "Tier1" ) return 1.00;
		if ( tier == "Tier2" ) return 1.20;
		if ( tier == "Tier3" ) return 1.40;
		if ( tier == "Tier4" ) return 1.70;
		if ( tier == "Tier5" ) return 2.00;
		return 1.00;
	}

	// scarcer nominal -> more expensive
	static float RarityMul( int nominal )
	{
		if ( nominal <= 0 )  return 1.00;
		if ( nominal <= 2 )  return 1.50;
		if ( nominal <= 5 )  return 1.25;
		if ( nominal <= 12 ) return 1.05;
		return 0.95;
	}

	// ------------------------------------------------------------------
	// power of the round the item shoots (weapons) or the round itself (ammo)
	//   CfgWeapons <gun> chamberableFrom[] -> CfgAmmo <ammo> caliber
	//   vanilla calibers: 9x19 ~0.5, 5.56/7.62 ~1.0, AP ~1.2, bolts ~3.1
	// missing keys return 0 -> neutral 1.0, so a wrong key can never distort a price
	// ------------------------------------------------------------------
	static float CaliberMul( string cls )
	{
		float cal = 0.0;
		TStringArray ammo = new TStringArray;
		GetGame().ConfigGetTextArray( CFG_WEAPONSPATH + " " + cls + " chamberableFrom", ammo );
		for ( int i = 0; i < ammo.Count() && i < 8; i++ )
		{
			float c = GetGame().ConfigGetFloat( "cfgAmmo " + ammo.Get( i ) + " caliber" );
			if ( c > cal )
				cal = c;
		}
		if ( cal <= 0.0 )
			cal = GetGame().ConfigGetFloat( "cfgAmmo " + cls + " caliber" );

		if ( cal <= 0.0 )
		{
			// fallback for weapons without a readable chamberableFrom (older or
			// heavily modded configs): use the magazine capacity as a power proxy
			TStringArray mags = new TStringArray;
			GetGame().ConfigGetTextArray( CFG_WEAPONSPATH + " " + cls + " magazines", mags );
			int cap = 0;
			for ( int j = 0; j < mags.Count() && j < 8; j++ )
			{
				int c = GetGame().ConfigGetInt( CFG_VEHICLESPATH + " " + mags.Get( j ) + " count" );
				if ( c > cap )
					cap = c;
			}
			if ( cap <= 0 )
				return 1.0;
			float byCap = 0.80 + cap / 40.0;   // 10 -> 1.05 ; 30 -> 1.55 ; 75 -> 2.68
			if ( byCap > 3.00 )
				byCap = 3.00;
			return byCap;
		}

		float mul = 0.70 + cal * 0.60;   // 0.5 -> 1.00 ; 1.0 -> 1.30 ; 3.1 -> 2.56
		if ( mul < 0.70 )
			mul = 0.70;
		if ( mul > 3.00 )
			mul = 3.00;
		return mul;
	}

	// ------------------------------------------------------------------
	// capacity: magazine rounds (count), stack size (varQuantityMax) or
	// container grid (itemsCargoSize[] = {width,height} -> slots)
	// ------------------------------------------------------------------
	static float CapacityMul( string cls )
	{
		int cap = GetGame().ConfigGetInt( CFG_VEHICLESPATH + " " + cls + " count" );
		if ( cap <= 1 )
			cap = GetGame().ConfigGetInt( CFG_VEHICLESPATH + " " + cls + " varQuantityMax" );
		if ( cap <= 1 )
		{
			TIntArray grid = new TIntArray;
			GetGame().ConfigGetIntArray( CFG_VEHICLESPATH + " " + cls + " itemsCargoSize", grid );
			if ( grid.Count() >= 2 )
				cap = grid.Get( 0 ) * grid.Get( 1 );
		}
		if ( cap <= 1 )
			return 1.0;

		float mul = 0.80 + cap / 60.0;   // 10 -> 0.97 ; 30 -> 1.30 ; 64 -> 1.87 ; 200 -> 2.50
		if ( mul < 0.80 )
			mul = 0.80;
		if ( mul > 2.50 )
			mul = 2.50;
		return mul;
	}

	// how many units one row means: magazines M, weapons W, everything else
	// is a stack (the loader turns '*' into the item's max quantity)
	static string QtyToken( string cls )
	{
		if ( GetGame().ConfigIsExisting( CFG_MAGAZINESPATH + " " + cls ) )
			return "M";
		if ( GetGame().ConfigIsExisting( CFG_WEAPONSPATH + " " + cls ) )
			return "W";
		return "*";
	}

	// ------------------------------------------------------------------
	// main entry - called once after readTraderData(), server only
	// ------------------------------------------------------------------
	static void Run( array<string> listedClasses )
	{
		Configure();
		if ( !m_Enabled )
			return;

		string types = m_TypesFile;
		if ( types == "" )
			types = g_Game.GetMissionPath() + "/db/types.xml";

		if ( !FileExist( types ) )
		{
			TraderMessage.ServerLog( "[AutoPrices] types.xml not found: " + types );
			return;
		}

		FileHandle fi = OpenFile( types, FileMode.READ );
		if ( fi == 0 )
		{
			TraderMessage.ServerLog( "[AutoPrices] cannot open " + types );
			return;
		}

		TStringArray outLines = new TStringArray;
		int total = 0;
		int skipped = 0;
		int generated = 0;
		int minPrice = 0;
		int maxPrice = 0;

		string cls = "";
		string cat = "";
		string usage = "";
		string tier = "";
		int nominal = 0;
		bool inType = false;

		string line = "";
		while ( FGets( fi, line ) != -1 )
		{
			line = FileReadHelper.TrimComment( line );
			if ( line == "" )
				continue;

			if ( line.IndexOf( "<type " ) >= 0 )
			{
				cls = AttrValue( line );
				cat = "";
				usage = "";
				tier = "";
				nominal = 0;
				inType = cls != "";
				if ( inType )
					total++;
				continue;
			}
			if ( !inType )
				continue;

			if ( line.IndexOf( "</type>" ) >= 0 )
			{
				inType = false;
				if ( cls == "" )
					continue;
				if ( InList( m_SkipCats, cat ) || InList( m_SkipPrefix, cls ) || InList( m_SkipClasses, cls ) )
				{
					skipped++;
					continue;
				}
				if ( m_OnlyListed && listedClasses.Find( cls ) != -1 )
				{
					skipped++;
					continue;
				}

				float f = BaseForCategory( cat ) * UsageMul( usage ) * TierMul( tier ) * RarityMul( nominal );
				if ( m_UseCaliber )
					f = f * CaliberMul( cls );
				if ( m_UseCapacity )
					f = f * CapacityMul( cls );

				int buy = Math.Round( f );
				if ( m_PriceStep > 1 )
				{
					buy = Math.Round( buy / m_PriceStep ) * m_PriceStep;
				}
				if ( buy < m_MinBuy )
					buy = m_MinBuy;
				if ( buy > m_MaxBuy )
					buy = m_MaxBuy;
				int sell = Math.Round( buy * m_SellCoef );
				if ( sell < 1 )
					sell = 1;

				if ( generated < m_MaxRows )
				{
					outLines.Insert( "        " + cls + ", " + QtyToken( cls ) + ", " + buy + ", " + sell + ",   // " + cat + " / " + usage + " / " + tier );
					if ( minPrice == 0 || buy < minPrice )
						minPrice = buy;
					if ( buy > maxPrice )
						maxPrice = buy;
					generated++;
				}
				continue;
			}

			if ( line.IndexOf( "<nominal>" ) >= 0 )
				nominal = TagInt( line );
			else if ( line.IndexOf( "<category" ) >= 0 )
				cat = AttrValue( line );
			else if ( line.IndexOf( "<usage" ) >= 0 )
				usage = AttrValue( line );
			else if ( line.IndexOf( "<value" ) >= 0 )
				tier = AttrValue( line );
		}
		CloseFile( fi );

		FileHandle fo = OpenFile( m_OutFile, FileMode.WRITE );
		if ( fo == 0 )
		{
			TraderMessage.ServerLog( "[AutoPrices] cannot write " + m_OutFile );
			return;
		}
		FPrintln( fo, "// ===== AUTO-GENERATED by TraderAutoPrices - do not edit by hand =====" );
		FPrintln( fo, "// source: " + types );
		FPrintln( fo, "// types: " + total + "   generated: " + generated + "   skipped: " + skipped );
		FPrintln( fo, "// price range: " + minPrice + " .. " + maxPrice );
		FPrintln( fo, "// usage: put  <OpenFile>TraderConfig_auto.txt  under the wanted <Trader>/<Category>" );
		FPrintln( fo, "// suggested target: <Trader> " + m_BaseTrader + "   <Category> " + m_BaseCat );
		for ( int i = 0; i < outLines.Count(); i++ )
		{
			FPrintln( fo, outLines.Get( i ) );
		}
		CloseFile( fo );

		TraderMessage.ServerLog( "[AutoPrices] " + m_OutFile + " written: generated " + generated + " of " + total + " types (skipped " + skipped + ", price " + minPrice + ".." + maxPrice + ")" );
	}
};
