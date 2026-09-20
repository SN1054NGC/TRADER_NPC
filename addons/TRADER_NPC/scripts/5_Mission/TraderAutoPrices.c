// ============================================================
// FILE: TraderAutoPrices.c   (module 5_Mission, SERVER only)
//
// WHAT IT DOES
//   Reads the mission Central Economy file <mission>/db/types.xml and computes
//   a buy/sell price for every TRADEABLE class that the manual TraderNpcConfig.txt
//   does NOT list (onlyUnlisted = yes by default). The result is written as a
//   NORMAL config fragment into
//        $profile:Trader_NPC_Prof/TraderNpcConfig_auto.txt
//   Nothing is applied automatically - the owner reviews the file and includes
//   it with the mechanism that already exists in the config:
//        <Trader> Misc Trader
//            <Category> Auto
//                <OpenFile>TraderNpcConfig_auto.txt
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
//   Everything is configurable in TraderNpcVariables.txt (see Configure()).
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
	static string m_VarsFile  = "$profile:Trader_NPC_Prof/TraderNpcVariables.txt";
	static string m_OutFile   = "$profile:Trader_NPC_Prof/TraderNpcConfig_auto.txt";
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
	static bool   m_UseArmor = true;
	static bool   m_UseWeight = false;
	static float  m_CaliberScale = 1.20;   // множитель = 0.50 + caliber * scale
	static float  m_CapacityScale = 0.04;  // множитель = 0.50 + capacity * scale
	static float  m_ArmorScale = 2.50;     // множитель = 1 + защита * scale
	static float  m_WeightScale = 0.00003; // множитель = 1 + вес(г) * scale
	// одежда (включаются отдельно)
	static bool   m_UseWarmth = true;     // heatIsolation: 0..1
	static bool   m_UseCamo = true;       // visibilityModifier: меньше = лучше
	static bool   m_UseItemSize = true;   // itemSize[] = {w,h} -> габарит
	static float  m_WarmthScale = 0.60;   // множитель = 1 + тепло * scale
	static float  m_CamoScale = 0.40;     // множитель = 1 + max(0, 1.5 - visibility) * scale
	static float  m_SizeScale = 0.030;    // множитель = 1 + (w*h) * scale
	// прочность / еда / жидкости / утилизация
	static int    m_RuinedPrice = 0;      // цена «хлама» для уничтоженного (0 = утилизация бесплатно)
	static int    m_RottenPrice = 1;      // ROTTEN-еда: сколько коинов даёт торговец
	static float  m_FoodStageBurn = 0.20; // BURNED
	static float  m_FoodStageRaw = 0.50;  // RAW
	static float  m_LiquidWater = 0.30;   // множители по типу жидкости
	static float  m_LiquidRiver = 0.20;
	static float  m_LiquidDisinfect = 3.00;
	static float  m_LiquidFuel = 1.50;
	static int    m_PriceStep  = 10;
	static string m_SkipClasses = "";

	// ---- результаты для missionServer.ApplyAutoPrices() ----
	static bool m_Apply = true;              // <AutoPricesApply> yes|no
	static ref array<string> m_OutClassnames;
	static ref array<string> m_OutCategories;
	static ref array<string> m_OutQuantity;
	static ref array<int>    m_OutBuy;
	static ref array<int>    m_OutSell;

	// CE categories that must never be auto-priced (vehicles have their own file)
	static const string m_SkipCats   = "zombie,animal,building,vehicle,vehiclesparts,tree,bush,rock,static,misc";
	// class-name prefixes that are never tradeable loot ('#' = CE service groups)
	static const string m_SkipPrefix = "#,Zmb,Animal,Land,StaticObj,Car_,Boat_,Plane_,House_,Grave,Underground";

	// ------------------------------------------------------------------
	// knobs are read from TraderNpcVariables.txt so this file stays
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
			line = TraderNpcText.Clean( line );
			if ( line == "" )
				continue;

			if ( line.Contains( "<AutoPricesApply>" ) )
				m_Apply = TagValue( line ) == "yes";
			else 
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
			else if ( line.Contains( "<AutoPricesUseArmor>" ) )
				m_UseArmor = TagValue( line ) == "yes";
			else if ( line.Contains( "<AutoPricesUseWeight>" ) )
				m_UseWeight = TagValue( line ) == "yes";
			else if ( line.Contains( "<AutoPricesCaliberScale>" ) )
				m_CaliberScale = TagValue( line ).ToFloat();
			else if ( line.Contains( "<AutoPricesCapacityScale>" ) )
				m_CapacityScale = TagValue( line ).ToFloat();
			else if ( line.Contains( "<AutoPricesArmorScale>" ) )
				m_ArmorScale = TagValue( line ).ToFloat();
			else if ( line.Contains( "<AutoPricesWeightScale>" ) )
				m_WeightScale = TagValue( line ).ToFloat();
			else if ( line.Contains( "<AutoPricesUseWarmth>" ) )
				m_UseWarmth = TagValue( line ) == "yes";
			else if ( line.Contains( "<AutoPricesUseCamo>" ) )
				m_UseCamo = TagValue( line ) == "yes";
			else if ( line.Contains( "<AutoPricesUseItemSize>" ) )
				m_UseItemSize = TagValue( line ) == "yes";
			else if ( line.Contains( "<AutoPricesWarmthScale>" ) )
				m_WarmthScale = TagValue( line ).ToFloat();
			else if ( line.Contains( "<AutoPricesCamoScale>" ) )
				m_CamoScale = TagValue( line ).ToFloat();
			else if ( line.Contains( "<AutoPricesSizeScale>" ) )
				m_SizeScale = TagValue( line ).ToFloat();
			else if ( line.Contains( "<AutoPricesRuinedPrice>" ) )
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
		v = TraderNpcText.Clean( v );
		return TraderNpcText.Tidy( v );
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
		rest = TraderNpcText.Tidy( rest );
		if ( rest.Length() >= 2 )
			rest = rest.Substring( 1, rest.Length() - 2 );
		return TraderNpcText.Tidy( rest );
	}

	// <nominal>8</nominal> -> 8
	static int TagInt( string line )
	{
		int a = line.IndexOf( ">" );
		int b = line.IndexOf( "</" );
		if ( a < 0 || b < 0 || b <= a )
			return 0;
		string v = line.Substring( a + 1, b - a - 1 );
		return TraderNpcText.Tidy( v ).ToInt();
	}

	static bool InList( string list, string value )
	{
		if ( value == "" )
			return false;
		TStringArray parts = new TStringArray;
		list.Split( ",", parts );
		for ( int i = 0; i < parts.Count(); i++ )
		{
			string p = TraderNpcText.Tidy( parts.Get( i ) );
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
				int magRounds = GetGame().ConfigGetInt( CFG_VEHICLESPATH + " " + mags.Get( j ) + " count" );
				if ( magRounds > cap )
					cap = magRounds;
			}
			if ( cap <= 0 )
				return 1.0;
			float byCap = 0.80 + cap / 40.0;   // 10 -> 1.05 ; 30 -> 1.55 ; 75 -> 2.68
			if ( byCap > 3.00 )
				byCap = 3.00;
			return byCap;
		}

		// harsher curve than a flat ratio: small rounds stay cheap, big ones climb fast
		//   0.5 -> 1.10 ; 1.0 -> 1.70 ; 1.2 -> 1.94 ; 3.1 -> 4.00 (clamped)
		float mul = 0.50 + cal * m_CaliberScale;
		if ( mul < 0.40 )
			mul = 0.40;
		if ( mul > 4.00 )
			mul = 4.00;
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

		// harsher: 10 -> 0.90 ; 30 -> 1.70 ; 64 -> 3.06 ; 100+ -> 4.00 (clamped)
		float mul = 0.50 + cap * m_CapacityScale;
		if ( mul < 0.50 )
			mul = 0.50;
		if ( mul > 4.00 )
			mul = 4.00;
		return mul;
	}

	// ------------------------------------------------------------------
	// armor: DamageSystem GlobalArmor <type> <zone> damage is the FRACTION of
	// incoming damage that still gets through, so protection = 1 - damage.
	// We take the BEST protection over the queried cells (a vest that stops
	// 60 % of a bullet is worth more, not three times more because Health,
	// Blood and Shock are listed separately).
	//   gloves: 0.9 -> 10 % ; plate carrier: usually 0.4 .. 0.6 -> 40 .. 60 %
	// ------------------------------------------------------------------
	static float ArmorProtection( string cls )
	{
		float best = 0.0;
		string base = CFG_VEHICLESPATH + " " + cls + " DamageSystem GlobalArmor ";
		best = ProtectionAt( base + "Projectile Health damage", best );
		best = ProtectionAt( base + "Projectile Blood damage", best );
		best = ProtectionAt( base + "Melee Health damage", best );
		best = ProtectionAt( base + "Melee Blood damage", best );
		best = ProtectionAt( base + "Explosion Health damage", best );
		best = ProtectionAt( base + "Infected Health damage", best );
		return best;
	}

	static float ProtectionAt( string path, float best )
	{
		float d = GetGame().ConfigGetFloat( path );
		if ( d <= 0.0 || d > 1.0 )
			return best;
		float prot = 1.0 - d;
		if ( prot > best )
			best = prot;
		return best;
	}

	// одежда: тепло / маскировка / габарит (значения из CfgVehicles)
	static float WarmthMul( string cls )
	{
		float h = GetGame().ConfigGetFloat( CFG_VEHICLESPATH + " " + cls + " heatIsolation" );
		if ( h <= 0.0 )
			return 1.0;
		float mul = 1.0 + h * m_WarmthScale;
		if ( mul > 2.50 )
			mul = 2.50;
		return mul;
	}

	static float CamoMul( string cls )
	{
		float v = GetGame().ConfigGetFloat( CFG_VEHICLESPATH + " " + cls + " visibilityModifier" );
		if ( v <= 0.0 )
			return 1.0;
		float gain = 1.5 - v;
		if ( gain < 0.0 )
			gain = 0.0;
		float mul = 1.0 + gain * m_CamoScale;
		if ( mul > 2.00 )
			mul = 2.00;
		return mul;
	}

	static float SizeMul( string cls )
	{
		TIntArray size = new TIntArray;
		GetGame().ConfigGetIntArray( CFG_VEHICLESPATH + " " + cls + " itemSize", size );
		if ( size.Count() < 2 )
			return 1.0;
		int slots = size.Get( 0 ) * size.Get( 1 );
		if ( slots <= 1 )
			return 1.0;
		float mul = 1.0 + slots * m_SizeScale;
		if ( mul > 2.00 )
			mul = 2.00;
		return mul;
	}

	static float ArmorMul( string cls )
	{
		float prot = ArmorProtection( cls );
		if ( prot <= 0.0 )
			return 1.0;
		float mul = 1.0 + prot * m_ArmorScale;   // 10 % -> 1.25 ; 60 % -> 2.50
		if ( mul > 4.00 )
			mul = 4.00;
		return mul;
	}

	// weight in grams -> logistics surcharge (off by default, weak by design)
	static float WeightMul( string cls )
	{
		float w = GetGame().ConfigGetFloat( CFG_VEHICLESPATH + " " + cls + " weight" );
		if ( w <= 0.0 )
			return 1.0;
		float mul = 1.0 + w * m_WeightScale;   // 1.3 kg -> 1.04 ; 20 kg -> 1.60
		if ( mul > 2.00 )
			mul = 2.00;
		return mul;
	}

	// ------------------------------------------------------------------
	// Trade UNIT of one row. The engine sells a whole row or, with the amount
	// slider, "unit price * amount" - so every stackable class gets a UNIT row
	// (quantity 1) and the slider does the rest. No "1 / 10 / 30" rows needed.
	//   weapon    -> W  (one weapon is the unit)
	//   magazine  -> M  (one magazine is the unit; its rounds are inside it)
	//   ammo pile -> 1  (one ROUND is the unit; the pile is the amount)
	//   anything else stackable / simple item -> 1
	// ------------------------------------------------------------------
	static string QtyToken( string cls )
	{
		// ammo piles (Ammo_556x45 ...) are magazines too, but their trade unit
		// is a single ROUND; AmmoBox_* is a sealed box, so it stays "M"
		if ( cls.IndexOf( "Ammo_" ) == 0 )
			return "1";
		if ( GetGame().ConfigIsExisting( CFG_MAGAZINESPATH + " " + cls ) )
			return "M";
		if ( GetGame().ConfigIsExisting( CFG_WEAPONSPATH + " " + cls ) )
			return "W";
		return "1";
	}

	// ------------------------------------------------------------------
	// main entry - called once after readTraderData(), server only
	// ------------------------------------------------------------------
	// Понятное имя категории: для файла-справочника и для списка категорий в UI
	static string DisplayCat( string ceCat )
	{
		if ( ceCat == "" )          return "Other";
		if ( ceCat == "ammo" )      return "Ammo";
		if ( ceCat == "magazines" ) return "Magazines";
		if ( ceCat == "weapons" )   return "Weapons";
		if ( ceCat == "clothes" )   return "Clothes";
		if ( ceCat == "food" )      return "Food";
		if ( ceCat == "medical" )   return "Medical";
		if ( ceCat == "tools" )     return "Tools";
		if ( ceCat == "containers" ) return "Containers";
		if ( ceCat == "explosives" ) return "Explosives";
		return "Auto " + ceCat;
	}

	static void Run( array<string> listedClasses )
	{
	// результаты для missionServer.ApplyAutoPrices() + заголовки категорий в файле
	m_OutClassnames = new array<string>;
	m_OutCategories = new array<string>;
	m_OutQuantity   = new array<string>;
	m_OutBuy        = new array<int>;
	m_OutSell       = new array<int>;
	string lastOutCat = "";
		Configure();
		if ( !m_Enabled )
			return;

		// GetMissionPath() отдаёт путь к ФАЙЛУ миссии (mission.c), а нужна ПАПКА.
		// Перебираем варианты: папка миссии, потом стандартные имена по миру.
		string types = m_TypesFile;
		if ( types == "" || !FileExist( types ) )
		{
			types = g_Game.GetMissionFolderPath() + "/db/types.xml";
		}
		if ( !FileExist( types ) )
		{
			string worldName = "";
			g_Game.GetWorldName( worldName );
			string byWorld = "$CurrentDir:mpmissions/dayzOffline." + worldName + "/db/types.xml";
			if ( FileExist( byWorld ) )
				types = byWorld;
			else
				types = "$CurrentDir:mpmissions/" + worldName + "/db/types.xml";
		}

		if ( !FileExist( types ) )
		{
			string worldDbg = "";
			g_Game.GetWorldName( worldDbg );
			TraderMessage.ServerLog( "[AutoPrices] types.xml not found. folder=" + g_Game.GetMissionFolderPath() + " world=" + worldDbg + " tried=" + types );
			return;
		}
		TraderMessage.ServerLog( "[AutoPrices] types.xml -> " + types );

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
			line = TraderNpcText.Clean( line );
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
				if ( cat == "clothes" )
				{
					if ( m_UseArmor )
						f = f * ArmorMul( cls );
					if ( m_UseWarmth )
						f = f * WarmthMul( cls );
					if ( m_UseCamo )
						f = f * CamoMul( cls );
					if ( m_UseItemSize )
						f = f * SizeMul( cls );
				}
				if ( m_UseWeight )
					f = f * WeightMul( cls );

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
					string dispCat = DisplayCat( cat );
					if ( dispCat != lastOutCat )
					{
						outLines.Insert( "" );
						outLines.Insert( "<Category> " + dispCat );
						lastOutCat = dispCat;
					}

					outLines.Insert( "        " + cls + ", " + QtyToken( cls ) + ", " + buy + ", " + sell + ",   // " + cat + " / " + usage + " / " + tier );
					m_OutClassnames.Insert( cls );
					m_OutCategories.Insert( dispCat );
					m_OutQuantity.Insert( QtyToken( cls ) );
					m_OutBuy.Insert( buy );
					m_OutSell.Insert( sell );
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
		FPrintln( fo, "// ФАЙЛ-СПРАВОЧНИК. Подключать через <OpenFile> НЕ нужно:" );
		FPrintln( fo, "// ассортимент применяется автоматически (missionServer.ApplyAutoPrices)" );
		FPrintln( fo, "// торговец: " + m_BaseTrader + "   выключить: <AutoPricesApply> no" );
		FPrintln( fo, "" );
		FPrintln( fo, "<Trader> " + m_BaseTrader );
		FPrintln( fo, "// suggested target: <Trader> " + m_BaseTrader + "   <Category> " + m_BaseCat );
		for ( int i = 0; i < outLines.Count(); i++ )
		{
			FPrintln( fo, outLines.Get( i ) );
		}
		FPrintln( fo, "<FileEnd>" );
		CloseFile( fo );

		TraderMessage.ServerLog( "[AutoPrices] written " + m_OutFile + ": rows " + generated + "/" + total + ", apply=" + m_Apply );
	}
};
