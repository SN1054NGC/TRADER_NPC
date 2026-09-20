// ============================================================
// ФАЙЛ: TraderNpcProfile.c   (серверная сторона)
//
// АВТОСОЗДАНИЕ ПРОФИЛЯ. При первом запуске, если профиля нет, создаём
// `$profile:Trader_NPC_Prof/` и минимальные файлы настроек. Дальше мод
// сам читает db/types.xml и генерирует черновик цен (TraderConfig_auto.txt)
// - это делает TraderAutoPrices при <AutoPrices> yes (стоит в дефолте).
//
// Папка называется Trader_NPC_Prof, а не Trader, чтобы НЕ пересекаться с
// оригинальным модом Trader (он читает $profile:Trader/).
//
// Используются только ванильные средства: FileExist / MakeDirectory / OpenFile / FPrintln.
// ============================================================
class TraderNpcProfile
{
	static const string DIR = "$profile:Trader_NPC_Prof";

	// --- содержимое файлов по умолчанию ---
	static const string DEF_VARIABLES = "// TRADER_NPC - настройки (создано автоматически, правьте под себя)
<BuySellTimer> 0.5
<RatingEnabled> yes
<RatingMaxDiscount> 20
<RatingFullHours> 100
<RatingCurve> 1.35
<RatingFullDistance> 20000
<RatingFullKills> 250
<RatingWeightTime> 0.5
<RatingWeightDistance> 0.3
<RatingWeightKills> 0.2
<SoundEnabled> yes
<SoundRange> 60
<KillReward> 0
<SafezoneTimeout> 30
<SafezoneRemoveAnimals> no
<SafezoneRemoveInfected> no
<SafezoneRemoveEAI> no
<SafezoneShowDebugShape> no
<FeatureRating> yes
<FeatureSound> yes
<FeatureSafezoneTrade> yes
<FeatureMapTools> yes
<FeatureHud> yes
<FeatureKillReward> yes
<FeatureAutoPrices> yes
<FeatureNotifications> yes
<AutoPrices> yes
<AutoPricesMagFill> no
<FileEnd>";

	static const string DEF_OBJECTS = "// TRADER_NPC - торговцы. Скопируйте блок и укажите свои координаты (X, Y, Z).
// Пока файл пуст, торговцы не спавнятся - это нормально для чистой установки.
//
// <TraderMarker> 0
// <TraderMarkerPosition> 13311.63, 9.65, 11007.78
// <TraderMarkerSafezone> 500
// <Object> SurvivorF_Eva
// <ObjectPosition> 13311.63, 9.65, 11007.78
// <ObjectOrientation> 0, 0, 0
// <ObjectAttachment> NPC_DUMMY

<FileEnd>";

	static const string DEF_CONFIG = "// TRADER_NPC - ассортимент: Classname, Quantity, BuyPrice[, SellPrice[, Ammo]]
// Цены можно сгенерировать автоматически из db/types.xml: см. TraderConfig_auto.txt
<OpenFile>TraderConfig_auto.txt

<TraderName> Weapon Trader
<Category> AssaultRifles
M4A1, 1, 40000, *
AK101, 1, 35000, *
<FileEnd>";

	static const string DEF_ADMINS = "// TRADER_NPC - UID администраторов торговца (по одному в строке)
<FileEnd>";

	// Создаёт профиль и недостающие файлы. Возвращает true, если что-то создано.
	static bool EnsureDefaults()
	{
		bool created = false;

		if ( !FileExist( DIR ) )
		{
			MakeDirectory( DIR );
			created = true;
		}

		if ( !FileExist( DIR + "/TraderVariables.txt" ) )
		{
			WriteText( DIR + "/TraderVariables.txt", DEF_VARIABLES );
			created = true;
		}
		if ( !FileExist( DIR + "/TraderObjects.txt" ) )
		{
			WriteText( DIR + "/TraderObjects.txt", DEF_OBJECTS );
			created = true;
		}
		if ( !FileExist( DIR + "/TraderConfig.txt" ) )
		{
			WriteText( DIR + "/TraderConfig.txt", DEF_CONFIG );
			created = true;
		}
		if ( !FileExist( DIR + "/TraderAdmins.txt" ) )
		{
			WriteText( DIR + "/TraderAdmins.txt", DEF_ADMINS );
			created = true;
		}

		if ( created )
			TraderMessage.ServerLog( "[TRADER] profile created: " + DIR + " (проверьте TraderObjects.txt - торговцы появятся после указания координат)" );

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
		content.Split( "\n", lines );

		for ( int i = 0; i < lines.Count(); i++ )
			FPrintln( f, lines.Get( i ) );

		CloseFile( f );
	}
};
