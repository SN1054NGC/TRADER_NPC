// ============================================================
// ФАЙЛ: TraderNpcProfile.c   (серверная сторона)
//
// АВТОСОЗДАНИЕ ПРОФИЛЯ. При первом запуске, если профиля нет, создаём
// $profile:Trader_NPC_Prof/ и минимальные файлы настроек. Дальше мод сам
// читает db/types.xml и генерирует черновик цен (TraderNpcConfig_auto.txt) -
// это делает TraderAutoPrices при <AutoPrices> yes (стоит в дефолте).
//
// Папка называется Trader_NPC_Prof, а не Trader, чтобы НЕ пересекаться с
// оригинальным модом Trader (он читает $profile:Trader/).
//
// Только ванильные средства: FileExist / MakeDirectory / OpenFile / FPrintln.
// ============================================================
class TraderNpcProfile
{
	static const string DIR = "$profile:Trader_NPC_Prof";

	// --- содержимое файлов по умолчанию (одна строка = один файл, разделители \\n) ---
	static const string DEF_VARIABLES = "// TRADER_NPC - настройки (создано автоматически)\n<BuySellTimer> 0.5\n<RatingEnabled> yes\n<RatingMaxDiscount> 20\n<RatingFullHours> 100\n<RatingCurve> 1.35\n<RatingFullDistance> 20000\n<RatingFullKills> 250\n<RatingWeightTime> 0.5\n<RatingWeightDistance> 0.3\n<RatingWeightKills> 0.2\n<SoundEnabled> yes\n<SoundRange> 60\n<KillReward> 0\n<SafezoneTimeout> 30\n<SafezoneRemoveAnimals> no\n<SafezoneRemoveInfected> no\n<SafezoneRemoveEAI> no\n<SafezoneShowDebugShape> no\n<FeatureRating> yes\n<FeatureSound> yes\n<FeatureSafezoneTrade> yes\n<FeatureMapTools> yes\n<FeatureHud> yes\n<FeatureKillReward> yes\n<FeatureAutoPrices> yes\n<FeatureNotifications> yes\n<VoiceEnabled> yes\n<AiEnabled> no\n<AiUrl> 127.0.0.1:8010\n<AiPath> /dayz/asktext\n<AutoPricesApply> yes\n<AutoPrices> yes\n<AutoPricesMagFill> no\n<FileEnd>";

	static const string DEF_OBJECTS = "// TRADER_NPC - торговцы. Скопируйте блок и укажите свои координаты (X, Y, Z).\n// Пока файл пуст, торговцы не спавнятся - это нормально для чистой установки.\n//\n// <TraderMarker> 0\n// <TraderMarkerPosition> 13311.63, 9.65, 11007.78\n// <TraderMarkerSafezone> 500\n// <Object> SurvivorF_Eva\n// <ObjectPosition> 13311.63, 9.65, 11007.78\n// <ObjectOrientation> 0, 0, 0\n// <ObjectAttachment> NPC_DUMMY\n\n<FileEnd>";

	static const string DEF_CONFIG = "// TRADER_NPC - ассортимент: Classname, Quantity, BuyPrice[, SellPrice[, Ammo]]\n// Цены генерируются автоматически из db/types.xml (см. TraderNpcConfig_auto.txt)\n<CurrencyName> #tm_ruble\n\n// Валюту можно поменять на любой предмет: <Currency> ИмяКласса, номинал\n<Currency> TraderNpcMoney, 1\n\n<OpenFile>TraderNpcConfig_auto.txt\n\n<Trader> Weapon Trader\n<Category> AssaultRifles\nM4A1, 1, 40000, *\nAK101, 1, 35000, *\n<FileEnd>";

	static const string DEF_ADMINS = "// TRADER_NPC - UID администраторов торговца (по одному в строке)\n<FileEnd>";

	// Создаёт профиль и недостающие файлы. true = что-то создано.
	static bool EnsureDefaults()
	{
		bool created = false;

		if ( !FileExist( DIR ) )
		{
			MakeDirectory( DIR );
			created = true;
		}

		if ( !FileExist( DIR + "/TraderNpcVariables.txt" ) )
		{
			WriteText( DIR + "/TraderNpcVariables.txt", DEF_VARIABLES );
			created = true;
		}
		if ( !FileExist( DIR + "/TraderNpcObjects.txt" ) )
		{
			WriteText( DIR + "/TraderNpcObjects.txt", DEF_OBJECTS );
			created = true;
		}
		if ( !FileExist( DIR + "/TraderNpcConfig.txt" ) )
		{
			WriteText( DIR + "/TraderNpcConfig.txt", DEF_CONFIG );
			created = true;
		}
		if ( !FileExist( DIR + "/TraderNpcAdmins.txt" ) )
		{
			WriteText( DIR + "/TraderNpcAdmins.txt", DEF_ADMINS );
			created = true;
		}

		if ( created )
			TraderMessage.ServerLog( "[TRADER] profile created: " + DIR );

		return created;
	}

	// Пишем построчно ванильным FPrintln
	static void WriteText( string path, string content )
	{
		FileHandle f = OpenFile( path, FileMode.WRITE );
		if ( !f )
		{
			TraderMessage.ServerLog( "[TRADER] CANNOT CREATE " + path );
			return;
		}

		TStringArray lines = new TStringArray;
		content.Split( "\\n", lines );

		for ( int i = 0; i < lines.Count(); i++ )
			FPrintln( f, lines.Get( i ) );

		CloseFile( f );
	}
};
